#version 330

uniform sampler2D tex;
in vec2 vs_tex_coord;

layout (location = 0) out vec4 color;

void main()
{
    //gl_FragColor = vec4(vv3color, 1.0);
    color = texture(tex, vs_tex_coord);
    //color = vec4(1.0, 0, 0, 1.0);
}
