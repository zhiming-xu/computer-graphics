#include "bsdf.h"

#include <iostream>
#include <algorithm>
#include <utility>

using std::min;
using std::max;
using std::swap;

namespace CGL {

void make_coord_space(Matrix3x3& o2w, const Vector3D& n) {
  Vector3D z = Vector3D(n.x, n.y, n.z);
  Vector3D h = z;
  if (fabs(h.x) <= fabs(h.y) && fabs(h.x) <= fabs(h.z)) h.x = 1.0;
  else if (fabs(h.y) <= fabs(h.x) && fabs(h.y) <= fabs(h.z)) h.y = 1.0;
  else h.z = 1.0;

  z.normalize();
  Vector3D y = cross(h, z);
  y.normalize();
  Vector3D x = cross(z, y);
  x.normalize();

  o2w[0] = x;
  o2w[1] = y;
  o2w[2] = z;
}

// Mirror BSDF //

Spectrum MirrorBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum MirrorBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO: 1.2
  // Using BSDF::reflect(), implement sample_f for a mirror surface
    reflect(wo, wi);
    *pdf = 1.0;
    return this->reflectance / abs_cos_theta(*wi);
}

// Microfacet BSDF //

double MicrofacetBSDF::G(const Vector3D& wo, const Vector3D& wi) {
    return 1.0 / (1.0 + Lambda(wi) + Lambda(wo));
}

double MicrofacetBSDF::D(const Vector3D& h) {
  // TODO: 2.2
  // Compute Beckmann normal distribution function (NDF) here.
  // You will need the roughness alpha.
    auto theta = acos(h.z);
    return exp(-pow(tan(theta)/alpha, 2.0))/(PI*pow(alpha, 2.0)*pow(h.z, 4.0));
}

Spectrum MicrofacetBSDF::F(const Vector3D& wi) {
  // TODO: 2.3
  // Compute Fresnel term for reflection on dielectric-conductor interface.
  // You will need both eta and etaK, both of which are Spectrum.
    auto eta_2 = this->eta * this->eta + this->k * this->k;
    auto eta_cos = 2.0*eta*wi.z;
    auto cos_2 = Spectrum(wi.z * wi.z, wi.z * wi.z, wi.z * wi.z);
    auto Rs = (eta_2 - eta_cos + cos_2)/(eta_2 + eta_cos + cos_2);
    auto Rp = (eta_2*cos_2 - eta_cos + Spectrum(1.0, 1.0, 1.0))/(eta_2*cos_2 + eta_cos + Spectrum(1.0, 1.0, 1.0));
    return (Rs + Rp)/2.0;
}

Spectrum MicrofacetBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  // TODO: 2.1
  // Implement microfacet model here
    auto h = wo + wi;
    h.normalize();
    return cos_theta(wo) <= 0 || cos_theta(wi) <= 0 ?
           Spectrum(0, 0, 0) : F(wi) * G(wo, wi) * D(h) / (4 * cos_theta(wo) * cos_theta(wi));
}

Spectrum MicrofacetBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO: 2.4
  // *Importance* sample Beckmann normal distribution function (NDF) here.
  // Note: You should fill in the sampled direction *wi and the corresponding *pdf,
  //       and return the sampled BRDF value.
    Vector2D sample = this->sampler.get_sample();
    auto rand1 = sample.x, rand2 = sample.y;
    auto theta = atan(sqrt(-this->alpha * this->alpha * log(1.0 - rand1)));
    auto phi = 2.0 * PI * rand2;
    auto h = Vector3D(sin(theta) * sin(phi), sin(theta) * cos(phi), cos(theta));
    *wi = 2 * h * dot(h, wo) - wo;
    if (wi->z <= 0)
    {
        *pdf = 0;
        return Spectrum();
    }
    auto p_theta = 2 * sin(theta) / this->alpha / this->alpha / pow(cos(theta), 3) / exp(pow(tan(theta) / this->alpha, 2));
    auto p_phi  = 1 / (2 * PI);
    auto ph = (p_theta * p_phi) / sin(theta);
    *pdf = ph / 4 / dot(*wi, h);
    return MicrofacetBSDF::f(wo, *wi);
}

// Refraction BSDF //

Spectrum RefractionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum RefractionBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  return Spectrum();
}

// Glass BSDF //

Spectrum GlassBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum GlassBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {

  // TODO: 1.4
  // Compute Fresnel coefficient and either reflect or refract based on it.
    if (!refract(wo, wi, this->ior))
    {
        reflect(wo, wi);
        *pdf = 1.0;
        return this->reflectance / abs_cos_theta(*wi);
    }
    else
    {
        auto R = (1.0 - this->ior) / (1.0 + this->ior);
        auto eta = wo.z > 0 ? 1.0 / this->ior : this->ior;
        R *= R;
        auto tmp = 1 - abs_cos_theta(wo);
        R = R + (1 - R) * tmp * tmp * tmp * tmp * tmp;
        if (coin_flip(R))
        {
            reflect(wo, wi);
            *pdf = R;
            return R * this->reflectance / abs_cos_theta(*wi);
        }
        else
        {
            *pdf = 1 - R;
            return *pdf * this->transmittance / (eta * eta) / abs_cos_theta(*wi);
        }
    }
}

void BSDF::reflect(const Vector3D& wo, Vector3D* wi) {

  // TODO: 1.1
  // Implement reflection of wo about normal (0,0,1) and store result in wi.
  *wi = Vector3D(-wo.x, -wo.y, wo.z);
}

bool BSDF::refract(const Vector3D& wo, Vector3D* wi, float ior) {

  // TODO: 1.3
  // Use Snell's Law to refract wo surface and store result ray in wi.
  // Return false if refraction does not occur due to total internal reflection
  // and true otherwise. When dot(wo,n) is positive, then wo corresponds to a
  // ray entering the surface through vacuum.
    auto sign = 1;
    auto n = ior;
    if (wo.z > 0)
    {
        n = 1.0 / ior;
        sign = -1;
    }
    auto tmp = 1 - n * n * sin_theta2(wo);
    if (tmp < 0)
        return false;
    *wi = Vector3D(-n * wo.x, -n * wo.y, sign * sqrt(tmp));
    return true;
}

// Emission BSDF //

Spectrum EmissionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum EmissionBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  *pdf = 1.0 / PI;
  *wi  = sampler.get_sample(pdf);
  return Spectrum();
}

} // namespace CGL
