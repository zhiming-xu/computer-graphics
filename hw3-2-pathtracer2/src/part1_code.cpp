//
// TODO: Copy over 3-1 code after turning on BUILD_3-1 flag
//

#include "part1_code.h"
#include <time.h>

using namespace CGL::StaticScene;

using std::min;
using std::max;

namespace CGL {

    Spectrum PathTracer::estimate_direct_lighting_hemisphere(const Ray& r, const Intersection& isect) {
        // Estimate the lighting from this intersection coming directly from a light.
        // For this function, sample uniformly in a hemisphere.

        // make a coordinate system for a hit point
        // with N aligned with the Z direction.

        // We store the transformation from 'local object space' to 'world space' in the matrix o2w
        Matrix3x3 o2w;
        make_coord_space(o2w, isect.n);
        Matrix3x3 w2o = o2w.T();

        // a local coordinate space for the object hit point
        const Vector3D& hit_p = r.o + r.d * isect.t;
        // w_out points towards the source of the ray (e.g.,toward the camera if this is a primary ray)
        // Remember that this should be opposite to the direction that the ray was traveling.
        const Vector3D& w_out = w2o * (-r.d);

        // This is the same number of total samples as estimate_direct_lighting_importance (outside of delta lights).
        // We keep the same number of samples for clarity of comparison.
        int num_samples = scene->lights.size() * ns_area_light;
        Spectrum L_out;

        // TODO (Part 3.2):
        // Write your sampling loop here
        // COMMENT OUT `normal_shading` IN `est_radiance_global_illumination` BEFORE YOU BEGIN
        for (int i = 0; i < num_samples; i++)
        {
            //direction in world space
            Vector3D w_in = this->hemisphereSampler->get_sample();

            Vector3D direction = o2w * w_in;
            Vector3D origin = hit_p + (EPS_D * direction);
            Ray ray = Ray(origin, direction);
            Intersection intersec;
            bool intersected = this->bvh->intersect(ray, &intersec);

            if (intersected)
            {
                Spectrum b = isect.bsdf->f(w_out, w_in);
                Spectrum e = intersec.bsdf->get_emission();
                L_out += b * e * cos_theta(w_in);
            }
        }

        L_out = (L_out * 2.0 * PI) / num_samples;

        return L_out;
    }

    Spectrum PathTracer::estimate_direct_lighting_importance(const Ray& r, const Intersection& isect) {
        // Estimate the lighting from this intersection coming directly from a light.
        // To implement importance sampling, sample only from lights, not uniformly in a hemisphere.

        // make a coordinate system for a hit point
        // with N aligned with the Z direction.
        Matrix3x3 o2w;
        make_coord_space(o2w, isect.n);
        Matrix3x3 w2o = o2w.T();

        // w_out points towards the source of the ray (e.g.,
        // toward the camera if this is a primary ray)
        const Vector3D& hit_p = r.o + r.d * isect.t;
        const Vector3D& w_out = w2o * (-r.d);
        Spectrum L_out;

        // TODO (Part 3.2):
        // Here is where your code for looping over scene lights goes
        // COMMENT OUT `normal_shading` IN `est_radiance_global_illumination` BEFORE YOU BEGIN


        for (SceneLight* light : this->scene->lights)
        {
            int num_samples;
            if(light->is_delta_light())
                num_samples = 1;
            else
                num_samples = this->ns_area_light;

            Vector3D wi;
            float dist_to_light;
            float pdf;

            for (int i = 0; i < num_samples; i++)
            {
                Spectrum radiance = light->sample_L(hit_p, &wi, &dist_to_light, &pdf);
                Vector3D w_in = w2o * wi;
                if (w_in.z >= 0)
                {
                    Vector3D orgin = hit_p + (EPS_D * wi);
                    Ray r2 = Ray(orgin, wi);
                    r2.max_t = dist_to_light;
                    Intersection intersec;
                    bool intersected = this->bvh->intersect(r2, &intersec);
                    if (!intersected)
                    {
                        Spectrum b = isect.bsdf->f(w_out, w_in);
                        L_out += (radiance * b * cos_theta(w_in)) / pdf;
                    }
                }
            }
            L_out /= num_samples;
        }
        return L_out;
    }

    Spectrum PathTracer::zero_bounce_radiance(const Ray&r, const Intersection& isect) {

        // TODO (Part 4.2):
        // Returns the light that results from no bounces of light
        return isect.bsdf->get_emission();
    }

    Spectrum PathTracer::one_bounce_radiance(const Ray&r, const Intersection& isect) {

        // TODO (Part 4.2):
        // Returns either the direct illumination by hemisphere or importance sampling
        // depending on `direct_hemisphere_sample`
        // (you implemented these functions in Part 3)


        if(this->direct_hemisphere_sample)
            return this->estimate_direct_lighting_hemisphere(r, isect);
        else
            return this->estimate_direct_lighting_importance(r, isect);
    }

    Spectrum PathTracer::at_least_one_bounce_radiance(const Ray&r, const Intersection& isect) {
        Matrix3x3 o2w;
        make_coord_space(o2w, isect.n);
        Matrix3x3 w2o = o2w.T();

        Vector3D hit_p = r.o + r.d * isect.t;
        Vector3D w_out = w2o * (-r.d);

        auto L_out = Spectrum();
        if(r.depth!=0 && !isect.bsdf->is_delta())
            L_out += one_bounce_radiance(r, isect);

        // TODO (Part 4.2):
        // Here is where your code for sampling the BSDF,
        // performing Russian roulette step, and returning a recursively
        // traced ray (when applicable) goes

        auto russian_roulette = coin_flip(.5)?0.6:0.7;
        Vector3D w_in;
        float pdf;
        Spectrum sample = isect.bsdf->sample_f(w_out, &w_in, &pdf);
        bool flag = (coin_flip(1 - russian_roulette) && (this->max_ray_depth <= 1 ||
                    r.depth != this->max_ray_depth)) || (r.depth <= 1);
        if (!flag && pdf > 0)
        {
            Vector3D wi_world = o2w * w_in;
            auto new_ray = Ray((EPS_D * wi_world) + hit_p, wi_world, INF_D, r.depth - 1);
            Intersection new_isec;
            if (this->bvh->intersect(new_ray, &new_isec))
            {
                Spectrum bounce = this->at_least_one_bounce_radiance(new_ray, new_isec);
                if(isect.bsdf->is_delta())
                    bounce += zero_bounce_radiance(new_ray, new_isec);
                if (r.depth == this->max_ray_depth)
                    L_out += (abs_cos_theta(w_in.z) * sample * bounce) / pdf;
                else
                    L_out += (abs_cos_theta(w_in.z) * sample * bounce) / pdf / russian_roulette;
            }
        }
        return L_out;
    }

    Spectrum PathTracer::est_radiance_global_illumination(const Ray &r) {
        Intersection isect;
        Spectrum L_out;

        if (!bvh->intersect(r, &isect))
            return L_out;

        return this->zero_bounce_radiance(r, isect) + this->at_least_one_bounce_radiance(r, isect);

    }

    Spectrum PathTracer::raytrace_pixel(size_t x, size_t y, bool flag) {

        // TODO (Part 1.1):
        // Make a loop that generates num_samples camera rays and traces them
        // through the scene. Return the average Spectrum.
        // You should call est_radiance_global_illumination in this function.

        // TODO (Part 5):
        // Modify your implementation to include adaptive sampling.
        // Use the command line parameters "samplesPerBatch" and "maxTolerance"

        auto num_samples = this->ns_aa;            // total samples to evaluate
        double width = this->sampleBuffer.w;
        double height = this->sampleBuffer.h;
        double sigma = 0;
        double sigma_2 = 0;

        if (num_samples == 1)
        {
            Ray ray = this->camera->generate_ray((x + 0.5)/width, (y + 0.5)/height);
            ray.depth = this->max_ray_depth;
            return this->est_radiance_global_illumination(ray);
        }
        else
        {
            Spectrum result = Spectrum();
            int i = 0;
            for (i = 0; i < num_samples; i++)
            {
                if (i % samplesPerBatch == 0 && i)
                {
                    double mean = sigma/double(i);
                    double var = (sigma_2 - (sigma*sigma)/double(i))/(i - 1.0);
                    if (1.96*sqrt(var/double(i)) <= maxTolerance*mean)
                    {
                        break;
                    }
                }
                Vector2D sample_v = this->gridSampler->get_sample();
                Ray sample_r = this->camera->generate_ray((x + sample_v.x)/width, (y + sample_v.y)/height);
                sample_r.depth = this->max_ray_depth;
                Spectrum tmp = this->est_radiance_global_illumination(sample_r);
                result += tmp;
                sigma += tmp.illum();
                sigma_2 += tmp.illum() * tmp.illum();
            }
            this->sampleCountBuffer[x + y*width] = i;
            return result/double(i);
        }
    }

  // Diffuse BSDF //

  Spectrum DiffuseBSDF::f(const Vector3D& wo, const Vector3D& wi)
  {
        return this->reflectance / PI;
  }

  Spectrum DiffuseBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf)
  {
      *wi = this->sampler.get_sample(pdf);
      return this->reflectance / PI;
  }

  // Camera //
  Ray Camera::generate_ray(double x, double y) const
  {
      auto x_origin = tan(radians(hFov)) * .5;
      auto y_origin = tan(radians(vFov)) * .5;
      auto top_right = Vector3D(x_origin, y_origin, -1), bottom_left = Vector3D(-x_origin, -y_origin, -1);
      auto X = bottom_left.x + (top_right.x - bottom_left.x) * x;
      auto Y = bottom_left.y + (top_right.y - bottom_left.y) * y;
      auto Z = -1;
      Vector3D origin = this->pos;
      Vector3D direction = this->c2w * Vector3D(X, Y, Z);
      direction.normalize();
      return Ray(origin, direction, this->fClip);
  }
}
