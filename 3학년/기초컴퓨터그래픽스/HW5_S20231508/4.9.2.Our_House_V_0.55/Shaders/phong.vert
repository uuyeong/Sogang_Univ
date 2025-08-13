// Shaders/phong.vert
#version 330 core

uniform vec3  u_worldLightPos;   // 월드 좌표계 광원 위치
uniform vec3  u_eyeLightPos;     // 카메라(눈) 좌표계 광원 위치
uniform bool  u_worldLightOn;    // 월드광원 on/off
uniform bool  u_eyeLightOn;      // 눈광원 on/off

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4  u_ModelViewProjectionMatrix;
uniform mat4  u_ModelMatrix;
uniform mat3  u_NormalMatrix;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    // 위치 변환
    gl_Position = u_ModelViewProjectionMatrix * vec4(aPos, 1.0);

    // 월드 공간에서의 프래그먼트 위치
    FragPos = vec3(u_ModelMatrix * vec4(aPos, 1.0));

    // 법선 벡터 변환
    Normal  = normalize(u_NormalMatrix * aNormal);
}
