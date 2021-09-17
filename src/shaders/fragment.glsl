#version 330 core

uniform vec3 light_pos;

in vec3 mid_normal_vec;
in vec3 mid_color;
in vec3 mid_position;

out vec4 out_color;

void main()
{
  vec3 light_color = vec3(1.0, 1.0, 1.0);

  // Ambient light
  float ambient_strength = 0.1;
  vec3 ambient = ambient_strength * light_color;

  // Diffuse light
  vec3 norm = normalize(mid_normal_vec);
  vec3 light_dir = normalize(light_pos - mid_position);

  float diff = max(dot(norm, light_dir), 0.0);
  vec3 diffuse = diff * light_color;

  // Specular light

  vec3 result = (ambient + diffuse) * mid_color;
  //vec3 result = vec3(diffuse);
  //vec3 result = norm;
  
  out_color = vec4(result, 1.0f);
}

