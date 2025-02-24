#include "math_utils.h"

struct Material {
    vec3 albedo;
    GLfloat reflectance;
    GLfloat specular;
    GLfloat pad1, pad2, pad3;

    Material(vec3 const &albedo, GLfloat reflectance, GLfloat specular)
    : albedo{albedo}, reflectance{reflectance}, specular{specular} {};
};

struct Sphere {
    vec3 center;
    GLfloat radius;
    Material material;

    Sphere(vec3 const &center, GLfloat radius, Material const &material)
    : center{center}, radius{radius}, material{material} {};
};
