#version 330 core

in vec2 frag_coord;
in vec2 tex_coord;

out vec4 out_color;

uniform vec2 resolution; // The screen resolution
uniform float time; // Time elapsed since program start
uniform int frame; // Current frame count, used to blend frames

uniform sampler2D prev_frame_tex; // The previous frame as a texture

// Camera
uniform int FOV;
uniform mat4 view_matrix; // Transform the camera

// Ray
const float MIN_DIST = 0.001;
const float MAX_DIST = 100;
const int SAMPLES_PER_PIXEL = 10;
const int MAX_BOUNCE = 100;

// Constants
const float PI = 3.141592;
const float FAR = 999999999;

// Materials
const int LAMBERTIAN = 0;
const int METAL = 1;
const int DIELECTRIC = 2;

/* ================================================================ *
                        UTILITY FUNCTIONS
 * ================================================================ */ 

vec3 rand_seed = vec3(frag_coord.xy, time);

/*
 * random - Generate a random float
 *
 * Returns: A floating point value within [0.0, 1.0)
 */
float random() {
    rand_seed = vec3(rand_seed.xy, rand_seed.z + 3.837);
    vec3 scale = vec3(12.9898, 78.233, 45.164);
    float dot_product = dot(rand_seed, scale);
    return fract(sin(dot_product) * 43758.5453);
}

/*
 * random - Generate a random float
 *
 * Returns: A floating point value within [min, max)
 */
float random(const float min, const float max) {
    return min + (max - min) * random();
}

/*
 * sample_square - Generate a random point in a square
 *
 * Returns: A vec2 within [-0.5, 0.5)
 */
vec2 sample_square() {
    return vec2(random() - 0.5, random() - 0.5);
}

/*
 * vec3_random - Generate a random unit vector
 *
 * Returns: A vec3 unit vector
 */
vec3 vec3_random() {
    vec3 rand = vec3(random(-1, 1), random(-1, 1), random(-1, 1));
    while (length(rand) < 0.001) {
        rand = vec3(random(-1, 1), random(-1, 1), random(-1, 1));
    }
    return normalize(rand);
}

/*
 * random_on_hemisphere - Generate a random unit vector from a hemisphere
 *
 * @normal: The hemisphere's normal vector
 *
 * Returns: A vec3 unit vector
 */
vec3 random_on_hemisphere(const vec3 normal) {
    vec3 dir = vec3_random();
    bool on_hemisphere = dot(dir, normal) > 0.0;
    return on_hemisphere ? dir : -dir;
}

/*
 * to_gamma - Translate color into gamma space
 *
 * Returns: vec4 color
 */
vec4 to_gamma(const vec4 color) {
    return vec4(sqrt(color.r), sqrt(color.g), sqrt(color.b), color.b);
}

/* ================================================================ *
 *                        TRACING STRUCTS                           *
 * ================================================================ */

struct Material {
    vec3 albedo;
    int material;
    float fuzz;
    float ri;
};

struct HitInfo {
    vec3 p;
    vec3 normal;
    float t;
    bool front_face;
    Material material;
};

/* ================================================================ *
 *                         RAY FUNCTIONS                            *
 * ================================================================ */

struct Ray {
    vec3 origin;
    vec3 dir;
};

/* ray_create - Create a random ray from the camera to the fragment
 *              position in the tracing plane
 *
 * Returns: 3D Ray
 */
Ray ray_create() {
    vec2 offset = sample_square();
    offset.x /= resolution.x;
    offset.y /= resolution.x;
    float aspect_ratio = resolution.x / resolution.y;
    float dist = 1.0 / tan(radians(FOV) * 0.5);
    vec3 ray_pos = vec3(vec4(0.0, 0.0, 0.0, 1.0) * view_matrix);
    vec3 ray_target = vec3(frag_coord.x * aspect_ratio + offset.x, frag_coord.y + offset.y, -dist);
    ray_target = vec3(vec4(ray_target, 1.0) * view_matrix);
    vec3 ray_dir = normalize(ray_target - ray_pos);

    return Ray(
        ray_pos,
        ray_dir
    );
}

/*
 * ray_at - Calculate position of the ray at a specific distance
 *
 * @ray: The ray
 * @t: The distance
 *
 * Returns: vec3 position
 */
vec3 ray_at(Ray ray, float t) {
    return ray.origin + t * ray.dir;
}

/* ================================================================ *
 *                      SPHERE FUNCTIONS                            *
 * ================================================================ */

struct Sphere {
    vec3 center;
    float radius;
    Material material;
};

// Buffer for spheres uploaded from CPU
const int MAX_SPHERES = 256;
uniform int SPHERES_NUM;
layout(std140) uniform sphere_buffer {
    Sphere spheres[MAX_SPHERES];
};

/*
 * get_sphere - Create a struct Sphere
 *
 * @center: The center point in 3D space
 * @radius: Sphere radius
 * @Material: Material of the sphere
 *
 * Returns: A struct Sphere
 */
Sphere get_sphere(vec3 center, float radius, Material material) {
    return Sphere(center, radius, material);
}

/*
 * sphere_hit - Calculate the intersection between a sphere and ray
 *
 * @sphere
 * @ray
 *
 * Returns: Floating point distance from ray origin
 */
float sphere_hit(Sphere sphere, Ray ray) {
    vec3 oc = sphere.center - ray.origin;
    float a = dot(ray.dir, ray.dir);
    float b = -2.0 * dot(ray.dir, oc);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return -1.0;
    } else {
        return (-b - sqrt(discriminant)) / (2.0 * a);
    }
}

/*
 * sphere_hit_data - Get the hit related information from an intersecting sphere and ray
 *
 * @sphere
 * @ray
 * @t: The intersection distance
 *
 * Returns: A struct Hitdata
 */
HitInfo sphere_hit_data(Sphere sphere, Ray ray, float t) {
    vec3 p = ray_at(ray, t);
    vec3 outward_normal = normalize(p - sphere.center);
    bool front_face = dot(ray.dir, outward_normal) < 0;
    vec3 normal = front_face ? outward_normal : -outward_normal;

    return HitInfo(p, normal, t, front_face, sphere.material);
}

/* ================================================================ *
 *                      PLANE FUNCTIONS                             *
 * ================================================================ */

struct Plane {
    vec3 normal;
    vec3 point;
    Material material;
};

Plane plane;

/*
 * get_plane - Create a struct Plane
 *
 * @normal: The plane normal vector
 * @point: A point on the plane
 * @Material: Material of the plane
 *
 * Returns: A struct Plane
 */
Plane get_plane(vec3 normal, vec3 point, Material material) {
    return Plane(normal, point, material);
}

/*
 * plane_hit - Get the hit related information from an intersecting sphere and ray
 *
 * @sphere
 * @ray
 * @t: The intersection distance
 *
 * Returns: A struct Hitdata
 */
float plane_hit(Plane plane, Ray ray) {
    float denom = dot(plane.normal, ray.dir);

    if (abs(denom) < 1e-6) {
        return -1.0;
    }

    float t = dot(plane.point - ray.origin, plane.normal) / denom;
    return t > 0.0 ? t : -1.0;
}

/*
 * plane_hit_data - Get the hit related information from an intersecting plane and ray
 *
 * @plane
 * @ray
 * @t: The intersection distance
 *
 * Returns: A struct Hitdata
 */
HitInfo plane_hit_data(Plane plane, Ray ray, float t) {
    vec3 p = ray_at(ray, t);
    bool front_face = dot(ray.dir, plane.normal) < 0;
    vec3 normal = front_face ? plane.normal : -plane.normal;

    return HitInfo(p, plane.normal, t, front_face, plane.material);
}

/* ================================================================ *
 *                      TRACING FUNCTIONS                           *
 * ================================================================ */

/*
 * trace_scene - Trace a scene along a ray and return what it hit
 *
 * @ray
 * @hit_info: Information about what the ray hit
 *
 * Return: struct HitInfo (returned through argument)
 */
void trace_scene(Ray ray, inout HitInfo hit_info) {
    float dist = MAX_DIST;

    // Check if the ray intersects the plane
    float t = plane_hit(plane, ray);
    if (MIN_DIST <= t && t < dist) {
        hit_info = plane_hit_data(plane, ray, t);
        dist = hit_info.t;
    }

    // Check if the ray intersects any sphere
    for (int i = 0; i < SPHERES_NUM; i++) {
        Sphere sphere = spheres[i];
        float t = sphere_hit(sphere, ray);

        // Keep the closest hit sphere
        if (MIN_DIST <= t && t < dist) {
            hit_info = sphere_hit_data(sphere, ray, t);
            dist = hit_info.t;
        }
    }
}

/*
 * get_hit - Get the hit where a ray intersects an object
 *
 * @ray
 *
 * Returns: A struct HitInfo
 */
HitInfo get_hit(Ray ray) {
    HitInfo hit_info;
    hit_info.t = MAX_DIST;
    trace_scene(ray, hit_info);
    return hit_info;
}

vec3 lambertian_reflectance(HitInfo hit_info) {
    return normalize(hit_info.normal + random_on_hemisphere(hit_info.normal));
}

vec3 metal_reflectance(HitInfo hit_info, Ray ray) {
    return reflect(ray.dir, hit_info.normal) + hit_info.material.fuzz * vec3_random();
}

float reflectance(float angle, float ri) {
    float r0 = (1.0 - ri) / (1.0 + ri);
    r0 = r0*r0;
    return r0 + (1.0 - r0) * pow(1.0 - angle, 5);
}

vec3 dielectric_reflectance(HitInfo hit_info, Ray ray) {
    float cos_theta = min(dot(-ray.dir, hit_info.normal), 1.0);
    float sin_theta = sqrt(1.0 - cos_theta*cos_theta);

    // Set refraction index according to face
    float hit_ri = hit_info.material.ri;
    float ri = hit_info.front_face ? (1.0 / hit_ri) : hit_ri;

    bool can_refract = ri * sin_theta <= 1.0;

    if (can_refract && reflectance(cos_theta, ri) < random()) {
        return refract(ray.dir, hit_info.normal, ri);
    } else {
        return reflect(ray.dir, hit_info.normal);
    }
}

/*
 * get_ray_color - Get the color of an intersecting ray
 *
 * @ray
 *
 * Returns: vec4 color
 */
vec4 get_ray_color(Ray ray) {
    vec3 new_color = vec3(1.0);

    // Iterate for each bounce of light
    for (int i = 0; i < MAX_BOUNCE; i++) {
        HitInfo hit_info = get_hit(ray);

        // Check if the ray hit
        if (hit_info.t < MAX_DIST) {
            int mat_type = hit_info.material.material;
            vec3 scatter;

            if (mat_type== 0)
                scatter = lambertian_reflectance(hit_info);
            if (mat_type == 1)
                scatter = metal_reflectance(hit_info, ray);
            if (mat_type == 2)
                scatter = dielectric_reflectance(hit_info, ray);

            ray = Ray(hit_info.p, scatter);
            new_color *= hit_info.material.albedo;

        } else {
            float a = 0.5 * (ray.dir.y + 1.0);
            vec3 color = (1.0 - a) * vec3(1.0, 1.0, 1.0) + a * vec3(0.4, 0.6, 1.0);
            new_color *= color;
            return vec4(new_color, 1.0);
        }
    }

    return vec4(new_color, 1.0);
}

void main() {
    plane = get_plane(vec3(0.0, 1.0, 0.0), vec3(0.0, -0.000, 0.0), Material(vec3(0.86, 0.95, 0.99) * 0.8, 1, 0.05, 0));

    vec3 color = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < SAMPLES_PER_PIXEL; i++) {
        Ray ray = ray_create();
        color += get_ray_color(ray).xyz;
    }

    float weight = 1.0 / (frame + 1);
    vec4 tex_color = texture(prev_frame_tex, tex_coord);
    vec4 ray_color = to_gamma(vec4(color / SAMPLES_PER_PIXEL, 1.0));
    out_color = mix(tex_color, ray_color, weight);
}
