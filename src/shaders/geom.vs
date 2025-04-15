#version 450
layout (location = 0) in vec3 in_vertex;
layout(std430, binding = 1) readonly buffer Positions {
  vec4 positions[];
};
layout(std430, binding = 2) readonly buffer Colors {
  vec4 colors[];
};
layout(std430, binding = 3) readonly buffer Ids {
  uint ids[];
};

uniform mat4 normal_model;
uniform mat4 MVP;
uniform float pickscale;
uniform float gscale;
uniform int lod_base;
uniform bool drawing_picked;
uniform int pick_id;
out vec3 Normal;
out vec3 Color;
out vec3 Pos;

flat out int id;

void main () {
  const uint real_id = drawing_picked?pick_id:ids[gl_InstanceID+lod_base];
  const vec3 pos = positions[real_id].xyz;
  const vec3 color = colors[real_id].rgb;
  const float scale = positions[real_id].w;
  id = int(real_id)+1;
  const vec3 vpos = pickscale*in_vertex*gscale*scale;
  const vec4 temp = vec4(vpos+pos*gscale, 1.0f);
  Color = color;
  gl_Position =  MVP*temp;
  const vec3 local_normal = normalize(vpos);
  Normal = vec3(mat3(normal_model)*(local_normal));
}
