#version 450

layout (points) in;
layout (line_strip, max_vertices = 5) out;

uniform mat4 MVP;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform vec3 campos;

//uniform float depth_limit;
uniform float dirscale = 1.0f;

in vec3 p[];
in vec3 c[];
in vec3 Dir[];
out vec3 Color;
out float vdir;

//flat out int discard_flag;

void main(){
//     float kk = length(campos-vec3(model*(vec4(p[0],0))));
//     if(kk>depth_limit){
//     discard_flag = 1;
//     EndPrimitive();
//     return;
//     }
//     discard_flag = 0;

       float app= 0.618;

     vec3 v = (p[0]-campos);
     
     vec4 d = vec4(Dir[0], 0)*dirscale;		  

     vec3 nv = normalize(cross(Dir[0]*dirscale, v));
     
     gl_Position = gl_in[0].gl_Position;
     Color = c[0];
     vdir = dot(v,Dir[0]);
     EmitVertex();
     
     gl_Position = gl_in[0].gl_Position+MVP*(d);
     Color = c[0];
          vdir = dot(normalize(v),normalize(Dir[0]));
     EmitVertex();
     
     gl_Position = gl_in[0].gl_Position+MVP*(d*app) + 0.5*proj*view*vec4(nv,0)*app*app*dirscale;
     Color = c[0];
           vdir = dot(normalize(v),normalize(Dir[0]));
     EmitVertex();
     
     gl_Position = gl_in[0].gl_Position+MVP*(d);
     Color = c[0];
          vdir = dot(normalize(v),normalize(Dir[0]));
     EmitVertex();
     
     gl_Position = gl_in[0].gl_Position+MVP*(d*app) - 0.5*proj*view*vec4(nv,0)*app*app*dirscale;
     Color = c[0];
          vdir = dot(normalize(v),normalize(Dir[0]));
     EmitVertex();


     EndPrimitive();	
}
