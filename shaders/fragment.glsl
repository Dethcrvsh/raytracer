#version 330 core

in vec2 frag_coord;
in vec2 tex_coord;

out vec4 out_color;

uniform mat4 to_world;

uniform vec2 resolution;
uniform float time;
uniform int frame;

uniform sampler2D tex;

// Camera
const int FOV = 70;

// Ray
const float MIN_DIST = 0.001;
const float MAX_DIST = 100;
const int SAMPLES_PER_PIXEL = 10;
const int MAX_BOUNCE = 100;

// Constants
const float PI = 3.141592;
const float FAR = 999999999;

vec3 rand_seed = vec3(frag_coord.xy, time);

struct Material {
    vec3 albedo;
    float reflectance;
    float specular;
};

struct HitInfo {
    vec3 p;
    vec3 normal;
    float t;
    bool front_face;
    Material material;
};

float random() {
    rand_seed = vec3(rand_seed.xy, rand_seed.z + 3.837);
    vec3 scale = vec3(12.9898, 78.233, 45.164);
    float dot_product = dot(rand_seed, scale);
    return fract(sin(dot_product) * 43758.5453);
}

float random(const float min, const float max) {
    return min + (max-min) * random();
}

vec2 sample_square() {
    return vec2(random() - 0.5, random() - 0.5);
}

vec3 vec3_random() {
    vec3 rand = vec3(random(-1, 1), random(-1, 1), random(-1, 1));
    while (length(rand) < 0.001) {
        rand = vec3(random(-1, 1), random(-1, 1), random(-1, 1));
    }
    return normalize(rand);
}

vec3 random_on_hemisphere(const vec3 normal) {
    vec3 dir = vec3_random();
    bool on_hemisphere = dot(dir, normal) > 0.0;
    return on_hemisphere ? dir : -dir;
}

vec4 to_gamma(const vec4 color) {
    return vec4(sqrt(color.r), sqrt(color.g), sqrt(color.b), color.b);
}

/**************************************
*           RAY FUNCTIONS
***************************************/

struct Ray {
    vec3 origin;
    vec3 dir;
};

Ray ray_create() {
    vec2 offset = sample_square();
    offset.x /= resolution.x;
    offset.y /= resolution.x;
    float aspect_ratio = resolution.x / resolution.y;
    float dist = 1.0 / tan(FOV * 0.5 * PI / 180.0);
    vec3 ray_pos = vec3(0.0, 0.0, 0.0);
    vec3 ray_target = vec3(frag_coord.x * aspect_ratio + offset.x, frag_coord.y + offset.y, -dist);
    vec3 ray_dir = normalize(ray_target - ray_pos);

    return Ray (
        ray_pos,
        ray_dir
    );
}

vec3 ray_at(Ray ray, float t) {
    return ray.origin + t * ray.dir;
}

/**************************************
*           SPHERE FUNCTIONS
***************************************/

struct Sphere {
    vec3 center;
    float radius;
    Material material;
};

Sphere spheres[16];
const int SPHERES_NUM = 6;

Sphere get_sphere(vec3 center, float radius, Material material) {
    vec4 new_center = vec4(center, 1.0) * to_world;
    return Sphere(new_center.xyz, radius, material);
}

float sphere_hit(Sphere sphere, Ray ray) {
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

HitInfo sphere_hit_data(Sphere sphere, Ray ray, float t) {
    vec3 p = ray_at(ray, t);
    vec3 outward_normal = normalize(p - sphere.center);
    bool front_face = dot(ray.dir, outward_normal) < 0;
    vec3 normal = front_face ? outward_normal : -outward_normal;

    return HitInfo(p, normal, t, front_face, sphere.material);
}

/**************************************
*           PLANE FUNCTIONS
***************************************/

struct Plane {
    vec3 normal;
    vec3 point;
    Material material;
};

Plane plane;

Plane get_plane(vec3 normal, vec3 point, Material material) {
    vec4 new_point = vec4(point, 1.0) * to_world;
    return Plane(normal, new_point.xyz, material);
}

float plane_hit(Plane plane, Ray ray) {
    float denom = dot(plane.normal, ray.dir);

    if (abs(denom) < 1e-6) {
        return -1.0;
    }

    float t = dot(plane.point - ray.origin, plane.normal) / denom;
    return t > 0.0 ? t : -1.0;
}

HitInfo plane_hit_data(Plane plane, Ray ray, float t) {
    vec3 p = ray_at(ray, t);
    bool front_face = dot(ray.dir, plane.normal) < 0;
    vec3 normal = front_face ? plane.normal : -plane.normal;

    return HitInfo(p, plane.normal, t, front_face, plane.material);
}

void trace_scene(Ray ray, inout HitInfo hit_info) {
    float dist = MAX_DIST;

    float t = plane_hit(plane, ray);
    if (MIN_DIST <= t && t < dist) {
        hit_info = plane_hit_data(plane, ray, t);
        dist = hit_info.t;
    }

    for (int i = 0; i < SPHERES_NUM; i++) {
        Sphere sphere = spheres[i];
        float t = sphere_hit(sphere, ray);

        // Sphere hit
        if (MIN_DIST <= t && t < dist) {
            hit_info = sphere_hit_data(sphere, ray, t);
            dist = hit_info.t;
        }
    }

}

HitInfo get_hit(Ray ray) {
    HitInfo hit_info;
    hit_info.t = MAX_DIST;
    trace_scene(ray, hit_info);
    return hit_info;
}

vec4 get_ray_color(Ray ray) {
    vec3 new_color = vec3(1.0);
    float probability = 1.0;

    for (int i = 0; i < MAX_BOUNCE; i++) {
        HitInfo hit_info = get_hit(ray);

        // Hit!
        if (hit_info.t < MAX_DIST) {
            Material mat = hit_info.material;
            if (mat.reflectance > 0.0) {
                vec3 dir = normalize(hit_info.normal + random_on_hemisphere(hit_info.normal));
                ray = Ray(hit_info.p, dir);
                new_color *= hit_info.material.albedo * hit_info.material.reflectance;
            } else if (mat.specular > 0.0) {
                vec3 dir = normalize(ray.dir - 2 * dot(ray.dir, hit_info.normal) * hit_info.normal);
                ray = Ray(hit_info.p, dir);
                new_color *= hit_info.material.albedo * hit_info.material.specular;
            }
        } else {
            float a = 0.5*(ray.dir.y + 1.0);
            vec3 color = (1.0-a)*vec3(1.0, 1.0, 1.0) + a*vec3(0.5, 0.7, 1.0);
            new_color *= color;
            return vec4(new_color, 1.0);
        }
    }

    return vec4(new_color, 1.0);
}

void main() {
    plane = get_plane(vec3(0.0, 1.0, 0.0), vec3(0.0, -0.001, 0.0), Material(vec3(0.3, 0.7, 0.2), 1.0, 0));
    spheres[0] = get_sphere(vec3(-1.0, 0.5, -2.0), 0.5, Material(vec3(1.0, 0.2, 1.0), 1.0, 0));
    spheres[1] = get_sphere(vec3(-0.5, 0.5, -6.0), 0.5, Material(vec3(1.0), 1, 0));
    spheres[2] = get_sphere(vec3(1.0, 0.5, -2.0), 0.5, Material(vec3(1.0, 1.0, 1.0), 0.0, 1.0));
    spheres[3] = get_sphere(vec3(-0.3, 0.1, -1.0), 0.1, Material(vec3(0.7, 0.3, 0.7), 0.0, 1.0));
    spheres[4] = get_sphere(vec3(0.15, 0.1, -1.3), 0.1, Material(vec3(0.8, 0.2, 0.1), 1.0, 0.0));
    spheres[5] = get_sphere(vec3(0.55, 0.1, -1.55), 0.1, Material(vec3(0.1, 0.1, 0.8), 1.0, 0.0));

    vec3 color = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < SAMPLES_PER_PIXEL; i++) {
        Ray ray = ray_create();
        color += get_ray_color(ray).xyz;
    }

    float weight = 1.0 / (frame + 1);
    vec4 tex_color = texture(tex, tex_coord);
    vec4 ray_color = to_gamma(vec4(color / SAMPLES_PER_PIXEL, 1.0));
    out_color = mix(tex_color, ray_color, weight);
}
