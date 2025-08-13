// Shaders/texture.vert
#version 330 core
layout(location=0) in vec3 aPos;
layout(location=2) in vec2 aTexCoord;

uniform mat4 u_ModelViewProjectionMatrix;
out vec2 v_TexCoord;

void main() {
    gl_Position = u_ModelViewProjectionMatrix * vec4(aPos,1);
    v_TexCoord  = aTexCoord;
}
