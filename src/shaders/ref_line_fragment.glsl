#version 330 core

in vec3 mid_color;

out vec4 out_color;

void main()
{
  out_color = vec4(mid_color, 1.0f);
}

