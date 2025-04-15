#version 450
layout(local_size_x = 256) in;

layout(std430, binding = 0) readonly buffer ParticlePositions {
    vec4 positions[]; // xyz = world position, w = radius (in world units)
};

layout(std430, binding = 1) buffer AllLODIndices {
    uint lod_indices[];
};

layout(std430, binding = 2) buffer LODCounts {
    uint counts[];
};

uniform mat4 modelview;
uniform mat4 projection;
uniform int num_lods;
uniform int lod_stride;
uniform ivec2 screen_resolution; // in pixels
uniform float lod_pixel_min;    // pixel diameter for highest LOD
uniform float lod_pixel_max;    // pixel diameter for lowest LOD
uniform float lod_bias = 2.0f;  // bias for LOD mapping
uniform float gscale;           // Global scale of the positions

// Compute pixel diameter of a sphere projected onto the screen
float compute_pixel_diameter(vec3 world_pos, float radius) {
    const vec4 view_center = modelview * vec4(world_pos, 1.0);
    const vec4 view_edge   = modelview * vec4(world_pos + vec3(radius, 0.0, 0.0), 1.0);
    const vec4 clip_center = projection * view_center;
    const vec4 clip_edge   = projection * view_edge;
    const vec2 ndc_center = clip_center.xy / clip_center.w;
    const vec2 ndc_edge   = clip_edge.xy / clip_edge.w;
    const float screen_dist = 2*length(ndc_edge - ndc_center);
    return screen_dist * screen_resolution.x; // X = horizontal pixel width
}

int mapLogLOD(float pixel_size, int N){
    if(pixel_size<=17) return 0;
    if(pixel_size<=20) return 1;
    if(pixel_size<=80) return 2;
    const float logMin = log2(lod_pixel_min);
    const float logMax = log2(lod_pixel_max);
    const float logD   = log2(clamp(pixel_size, lod_pixel_min, lod_pixel_max));
    const float t = clamp((logD - logMin) / (logMax - logMin), 0.0, 1.0);
    const float biased_t = pow(t, lod_bias);
    return int(clamp(floor(biased_t * float(N)), 2.0, float(N-1)));
}

void main() {
    const uint id = gl_GlobalInvocationID.x;
    if (id >= positions.length()) return;
    const vec4 global_pos = positions[id] * gscale;
    const vec3 world_pos = global_pos.xyz;
    const float radius = global_pos.w;
    // Project to view space
    vec4 view_pos = modelview * vec4(world_pos, 1.0);
    if (view_pos.z > 0.0) return; // behind the camera
    // Project to clip space
    vec4 clip_pos = projection * view_pos;
    if (clip_pos.w <= 0.0) return; // invalid projection
    // Convert to NDC
    vec3 ndc = clip_pos.xyz / clip_pos.w;
    // Convert to screen space
    vec2 screen_center = (ndc.xy * 0.5 + 0.5) * vec2(screen_resolution);
    // Compute screen-space projected radius
    float pixel_diameter = compute_pixel_diameter(world_pos, radius);
    float pixel_radius = 0.5 * pixel_diameter;
    // Check if the sphere is in screen space
    if (screen_center.x + pixel_radius < 0.0 ||
        screen_center.y + pixel_radius < 0.0 ||
        screen_center.x - pixel_radius > float(screen_resolution.x) ||
        screen_center.y - pixel_radius > float(screen_resolution.y))
        return;
    // Compute LOD and store
    const float pixel_size = pixel_diameter;
    const int lod = mapLogLOD(pixel_size, num_lods);
    const uint offset = lod * lod_stride;
    const uint index = atomicAdd(counts[lod], 1);
    lod_indices[offset + index] = id;
}
