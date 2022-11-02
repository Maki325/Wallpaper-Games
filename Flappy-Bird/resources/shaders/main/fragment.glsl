#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;

void main()
{
  // linearly interpolate between both textures (80% container, 20% awesomeface)
  vec4 color = texture(texture1, TexCoord);
  if (color.a < 0.005) //for example, change to any value suitable
   discard;

  FragColor = color;
}
