#include "math_utils.h"

struct Material {
    vec3 albedo;
    GLfloat reflectance;
    GLfloat specular;
    GLfloat fuzz;
    GLfloat pad1, pad2;

    Material() = default;
    Material& lambertian(vec3 const& albedo, GLfloat reflectance);
    Material& metal(vec3 const& albedo, GLfloat specular, GLfloat fuzz);
};

struct Sphere {
    vec3 center;
    GLfloat radius;
    Material material;

    Sphere(vec3 const& center, GLfloat radius, Material const& material);
};
