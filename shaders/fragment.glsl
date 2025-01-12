#version 330 core

in vec2 frag_coord;
out vec4 color;

uniform vec2 resolution;

struct Sphere {
    vec3 center;
    float radius;
};

struct Ray {
    vec3 origin;
    vec3 target;
    vec3 dir;
};

Ray get_ray() {
    float aspect_ratio = resolution.x / resolution.y;
    vec3 ray_pos = vec3(0.0, 0.0, 0.0);
    vec3 ray_target = vec3(frag_coord.x * aspect_ratio, frag_coord.y, -1.0);
    vec3 ray_dir = normalize(ray_target - ray_pos);

    return Ray (
        ray_pos,
        ray_target,
        ray_dir
    );
}

float hit_sphere(Sphere sphere, Ray ray) {
    vec3 oc = sphere.center - ray.origin;
    float a = dot(ray.dir, ray.dir);
    float b = -2.0 * dot(ray.dir, oc);
    float c = dot(oc, oc) - pow(sphere.radius, 2);
    float discriminant = b*b - 4*a*c;

    if (discriminant < 0) {
        return -1.0;
    } else {
        return (-b - sqrt(discriminant)) / (2.0*a);
    }
}

void main() {
    Ray ray = get_ray();
    Sphere sphere = Sphere(vec3(0.0, 0.0, -1.0), 0.5);

    if (hit_sphere(sphere, ray) >= 0.0) {
        color = vec4(1.0, 0.0, 0.0, 1.0);
    } else {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
