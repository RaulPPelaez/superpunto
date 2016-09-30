#version 450

layout (points) in;
layout (line_strip, max_vertices = 5) out;

uniform mat4 MVP;

in vec3 dir[];
in vec3 c[];
in int axis[];

out vec3 C;

void main(){
     int ax = axis[0];

     vec4 arrowdir = vec4(ax==1?1:0,ax==2?1:0,ax==0?1:0,0)*0.618*0.618;

     vec4 d = vec4(dir[0],0);

     gl_Position = gl_in[0].gl_Position;
     C= c[0]; 
     EmitVertex();
     
     gl_Position = gl_in[0].gl_Position+MVP*(d);
     C= c[0];
     EmitVertex();


     gl_Position = gl_in[0].gl_Position+MVP*(d*0.618+arrowdir);
     C = c[0];
     EmitVertex();
     
     gl_Position = gl_in[0].gl_Position+MVP*(d);
     C = c[0];
     EmitVertex();
     
     gl_Position = gl_in[0].gl_Position+MVP*(d*0.618-arrowdir);
     C = c[0];
     EmitVertex();

     EndPrimitive();	
}