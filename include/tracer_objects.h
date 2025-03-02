#include "math_utils.h"


struct Material {
    GLuint static const LAMBERTIAN {0};
    GLuint static const METAL {1};
    GLuint static const DIELECTRIC {2};

    vec3 albedo;
    GLint material;
    GLfloat fuzz;

    GLfloat pad1, pad2, pad3;

    Material() = default;
    Material& lambertian(vec3 const& albedo);
    Material& metal(vec3 const& albedo, GLfloat fuzz);
};

struct Sphere {
    vec3 center;
    GLfloat radius;
    Material material;
    
    Sphere() = default;
    Sphere(vec3 const& center, GLfloat radius, Material const& material);
};
