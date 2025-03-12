#include "tracer_objects.h"

Sphere::Sphere(vec3 const& center, GLfloat radius, Material const& material)
    : center{center}, radius{radius}, material{material} {};

Quad::Quad(vec3 const& Q, vec3 const& u, vec3 const& v, Material const& material)
    : Q{Q}, u{u}, v{v}, material{material} {};

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

Material& Material::dielectric(vec3 const& albedo, GLfloat ri) {
    // TODO: assert
    this->albedo = albedo;
    this->ri = ri;
    this->material = DIELECTRIC;
    return *this;
}
