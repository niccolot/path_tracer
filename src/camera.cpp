#include <thread>

#include "camera.h"
#include "utils.h"
#include "material.h"
#include "pdf.h"


Camera::Camera(
    int width, 
    double asp_rat, 
    Vec3&& lookfrom,
    Vec3&& lookat,
    double vfov,
    double focus_dist,
    double defocus_angle,
    int samples, 
    int depth) : 

        img_width_val(width),
        aspect_ratio_val(asp_rat),
        lookfrom(std::move(lookfrom)),
        lookat(std::move(lookat)),
        vfov(vfov),
        focus_dist(focus_dist),
        defocus_angle(defocus_angle),
        samples_per_pixel(samples),
        max_depth(depth) {

    img_height_val = int(img_width_val/aspect_ratio_val);
    img_height_val = (img_height_val > 1) ? img_height_val : 1; // img_height must be at leat 1 pixel

    pixel_samples_scale = 1.0 / samples_per_pixel;

    samples_sqrt = int(std::sqrt(samples));
    inv_samples_sqrt = 1.0/samples_sqrt;

    center = lookfrom;

    depth_cutoff = int(max_depth/5);

    image.reserve(img_height_val * img_width_val);

    // camera
    double theta = degs_to_rads(vfov);
    double h = std::tan(theta * 0.5);
    
    // up direction in the camera frame, change from global y direction for tiltable camera
    vup = Vec3(0,1,0); 

    // antiparallel to the viewing direction
    w = unit_vector(lookfrom - lookat); 

    // perpendicular to viewing dir and vertical versor
    u = unit_vector(cross(vup, w));

    // camera up direction in the global frame of reference
    v = cross(w, u);

    // viewport
    double viewport_height = 2*h*focus_dist;
    double viewport_width = viewport_height * (double(img_width_val)/img_height_val);
    auto viewport_u = viewport_width * u;
    auto viewport_v = viewport_height * -v;

    pixel_delta_u = viewport_u / img_width_val;
    pixel_delta_v = viewport_v / img_height_val;

    auto viewport_upper_left = center - 
                                focus_dist * w
                                - 0.5*(viewport_u + viewport_v);

    pixel00_loc = viewport_upper_left + 0.5*(pixel_delta_u + pixel_delta_v);

    // defocus blur
    auto defocus_radius = focus_dist * std::tan(degs_to_rads(defocus_angle * 0.5));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;

    // arbitrary default backgorund
    background = Color(0.40, 0.50, 1.00);
} 

Ray Camera::get_ray(int i, int j, int s_i, int s_j) const {
    /**
     * @brief construct a camera ray originated from
     * the camera center and directed at a randomly 
     * sampled point around pixel (i, j) for stratified
     * sample square (s_i, s_j)
     */
    auto offset = sample_square_stratified(s_i, s_j);
    auto pixel_sample = pixel00_loc 
                        + ((i + offset.x()) * pixel_delta_u)
                        + ((j + offset.y()) * pixel_delta_v);
    
    auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
    auto ray_direction = pixel_sample - ray_origin;
    
    /*
    for blurred 'moving' spheres that moves from t=0 to t=1 
    the rays are generated at random times in [0,1) 
    */
    auto ray_time = RandomUtils::random_double();

    return Ray(ray_origin, ray_direction, ray_time);
}

Color Camera::ray_color(const Ray& r, int depth, const Hittable& world, const Hittable& lights) const {
    if (depth <= 0) {
        // no light returned after too many bounces
        return Color(0, 0, 0);
    }

    HitRecord rec;
    double shadow_acne_offset = 0.001;

    // no hits
    if (!world.hit(r, Interval(shadow_acne_offset, infinity), rec)) {
        return background;
    }

    ScatterRecord srec;
    Color color_from_emission = 
        rec.material()->emitted(r, rec, rec.u(), rec.v(), rec.point());

    if (!rec.material()->scatter(r,rec,srec)) {
        return color_from_emission;
    }
 
    Color color_from_scatter = Color();
    for (const auto& ray_t : srec.scattered_rays) {
        if (ray_t.skip_pdf || ray_t.pdf == nullptr) {
            color_from_scatter += ray_t.attenuation * ray_color(ray_t.skip_pdf_ray, depth-1, world, lights);
        } else {
            auto light_ptr = std::make_shared<HittablePDF>(lights, rec.point());
            MixturePDF p(light_ptr, ray_t.pdf);
            Ray scattered = Ray(rec.point(), p.generate(), r.time());
            double pdf_value = p.value(scattered.direction(), r.direction(), w);
            double scattering_pdf = rec.material()->scattering_pdf(r, rec, scattered, w);
            Color sample_color = ray_color(scattered, depth-1, world, lights);
                
            color_from_scatter += (ray_t.attenuation * scattering_pdf * sample_color) / pdf_value;
        }
    }

    return color_from_emission + color_from_scatter;
}

Vec3 Camera::defocus_disk_sample() const {
    /**
     * @brief returns a random point in the defocus disk
     */

    auto p = random_in_unit_disk();
    return center + (p.x() * defocus_disk_u) + (p.y() * defocus_disk_v);
}

Vec3 Camera::sample_square_stratified(int s_i, int s_j) const {
    /**
     * @brief returns the vector to a random point in the square
     * subpixel specified by grid indices (s_i, s_j) for an 
     * idealized unit square pixel [-0.5, -0.5] x [0.5, 0.5] 
     */

    auto px = ((s_i + RandomUtils::random_double()) * inv_samples_sqrt) - 0.5;
    auto py = ((s_j + RandomUtils::random_double()) * inv_samples_sqrt) - 0.5;

    return Vec3(px,py,0);
}

void Camera::color_per_job(const Hittable& world, const Hittable& lights, job_block_t& job) {
    job.colors.reserve((job.row_end - job.row_start) * job.row_size);
    job.indices.reserve((job.row_end - job.row_start) * job.row_size);
    for (int j=job.row_start; j<job.row_end; ++j) {
        for (int i = 0; i < job.row_size; ++i) {
            Color pixel_color(0,0,0);
            for (int s_j = 0; s_j < samples_sqrt; ++s_j) {
                for (int s_i = 0; s_i < samples_sqrt; ++s_i) {
                    Ray r = get_ray(i, j, s_i, s_j);
                    pixel_color += ray_color(r, max_depth, world, lights);
                }
            }

            pixel_color *= pixel_samples_scale;
            pixel_color.set_x(linear_to_gamma(pixel_color.x()));
            pixel_color.set_y(linear_to_gamma(pixel_color.y()));
            pixel_color.set_z(linear_to_gamma(pixel_color.z()));

            const int index = j * job.row_size + i;
            job.indices.push_back(index);
            job.colors.push_back(pixel_color);
        }
    }

    {
        std::lock_guard<std::mutex> lock(mutex);
        image_blocks.push_back(job);
        cv.notify_one();
    }
}

void Camera::reconstruct_image(std::ostream& out) {
    for (auto& job : image_blocks) {
        auto len = job.indices.size();
        for (size_t i = 0; i<len; ++i) {
            int col_index = job.indices[i];
            image[col_index] = job.colors[i];
            ++col_index;
        }
    }

    out << "P3\n" << img_width_val << ' ' << img_height_val << "\n255\n";

    static const Interval intensity(0.000, 0.999);
    for (int i = 0; i < img_height_val * img_width_val; ++i) {
        auto r = image[i].x();
        auto g = image[i].y();
        auto b = image[i].z();

        if (std::isnan(r)) r = 0.;
        if (std::isnan(g)) g = 0.;
        if (std::isnan(b)) b = 0.;
        
        int r_byte = int(256 * intensity.clamp(r));
        int g_byte = int(256 * intensity.clamp(g));
        int b_byte = int(256 * intensity.clamp(b));

        out << r_byte << ' ' << g_byte << ' ' << b_byte << '\n';
    }
}

void Camera::render(const Hittable& world, const Hittable& lights) {
    unsigned n_threads = std::thread::hardware_concurrency();
    unsigned rows_per_job = img_height_val / n_threads;
    unsigned leftover = img_height_val % n_threads;
    std::vector<job_block_t> jobs;
    std::vector<std::thread> threads;
    image_blocks.reserve(n_threads);

    for (unsigned i = 0; i < n_threads; ++i) {
        job_block_t job;
        job.row_start = i * rows_per_job;
        job.row_end = job.row_start + rows_per_job;
        if (i == (n_threads - 1)) {
            job.row_end += leftover;
        }

        job.row_size = img_width_val;
        jobs.push_back(job);
    }

    for (unsigned i = 0; i < (n_threads - 1); ++i) {
        threads.emplace_back([this, &world, &lights, &jobs, i] {
            color_per_job(world, lights, jobs[i]);
        });
    }

    color_per_job(world, lights, jobs[n_threads - 1]);

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    reconstruct_image(std::cout);
    std::clog << "\nDone.\n";
}