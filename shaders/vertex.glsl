#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coords;

out vec4 interpolated_pos;
out vec4 interpolated_normal;
out vec4 interpolated_color;
out vec2 interpolated_tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normal_mat;


void main()
{
    interpolated_pos = model * vec4(position, 1);
    interpolated_normal = normal_mat * vec4(normal, 1);


    gl_Position = projection * view * interpolated_pos;

    interpolated_tex_coords = tex_coords;
}
