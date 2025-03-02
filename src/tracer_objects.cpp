#include "tracer_objects.h"

Sphere::Sphere(vec3 const &center, GLfloat radius, Material const &material)
    : center{center}, radius{radius}, material{material} {};

Material& Material::lambertian(vec3 const& albedo) {
    this->albedo = albedo;
    this->material = LAMBERTIAN;
    return *this;
}

Material& Material::metal(vec3 const& albedo, GLfloat fuzz) {
    assert(0.0 <= fuzz && fuzz <= 1.0);
    this->albedo = albedo;
    this->fuzz = fuzz;
    this->material = METAL;
    return *this;
}
