#include "tracer_objects.h"

Sphere::Sphere(vec3 const &center, GLfloat radius, Material const &material)
    : center{center}, radius{radius}, material{material} {};

Material& Material::lambertian(vec3 const& albedo, GLfloat reflectance) {
    assert(reflectance > 0.0 && "What kind of black hole are you trying to create?");
    this->albedo = albedo;
    this->reflectance = reflectance;
    return *this;
}

Material& Material::metal(vec3 const& albedo, GLfloat specular, GLfloat fuzz) {
    assert(0.0 < specular && specular <= 1.0);
    assert(0.0 <= fuzz && fuzz <= 1.0);
    this->albedo = albedo;
    this->specular = specular;
    this->fuzz = fuzz;
    return *this;
}
