#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D tex;
uniform vec4 color;

void main()
{
  if(TexCoords.x >= 0 && TexCoords.y >= 0) {
    FragColor = texture(tex, TexCoords);
  } else {
    FragColor = color;
  }
}
