#version 130                 
in vec2 TexCoords;
out vec4 color;
uniform sampler2D scTex;
const float offset = 1.0/200.0;
const vec2 offsets[9] = vec2[](
			       vec2(-offset, offset),  // top-left
			       vec2(0.0f,    offset),  // top-center
			       vec2(offset,  offset),  // top-right
			       vec2(-offset, 0.0f),    // center-left
			       vec2(0.0f,    0.0f),    // center-center
			       vec2(offset,  0.0f),    // center-right
			       vec2(-offset, -offset), // bottom-left
			       vec2(0.0f,    -offset), // bottom-center
			       vec2(offset,  -offset)  // bottom-right    
			       );

const float kernel[9] = float[](
				1, 2, -1,
				0, 0, 0,
			        4, -3, 0
				);

const float kernel_mod = 3.0;
void main() {                                               
  
  color = texture(scTex, TexCoords);
  return;
  
  vec3 sampleTex[9];
  for(int i = 0; i < 9; i++)
    {
      sampleTex[i] = vec3(texture(scTex, TexCoords.st + offsets[i]));
    }
  vec3 col;
  for(int i = 0; i < 9; i++)
    col += sampleTex[i] * kernel[i]/kernel_mod;
  
  color = vec4(col,1.0);
}        
