#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal_vec;

out vec3 mid_color;
out vec3 mid_position;
out vec3 mid_normal_vec;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  gl_Position = projection * view * model * vec4(pos, 1.0);

  mid_position = vec3(model * vec4(pos, 1.0));
  mid_color = color;
  mid_normal_vec = normal_vec;
}
