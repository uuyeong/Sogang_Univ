#version 330
uniform float u_alpha;
in vec4 v_color;

layout (location = 0) out vec4 final_color;

void main(void) {
    final_color = vec4(v_color.rgb, v_color.a * u_alpha);
}