#version 330

layout(location = 0) in vec3 av4position;
layout(location = 1) in vec2 in_tex_coord;

uniform mat4 MVP;

out vec2 vs_tex_coord;

void main()
{
    vs_tex_coord = in_tex_coord;
    gl_Position = MVP * vec4(av4position, 1.0);
}

