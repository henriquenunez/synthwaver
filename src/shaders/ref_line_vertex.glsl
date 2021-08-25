#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

out vec3 mid_color;

uniform mat4 mvp_trans;

void main()
{
  gl_Position = mvp_trans * vec4(pos, 1.0);
  mid_color = color;
}
