// Shaders/gouraud.vert
#version 330 core

uniform vec4  u_material;    // vec4(diffuse.rgb, diffuse.a)

uniform mat4  u_ModelViewProjectionMatrix;
uniform mat3  u_NormalMatrix;
uniform vec3  u_worldLightPos;
uniform vec3  u_eyeLightPos;
uniform bool  u_worldLightOn;
uniform bool  u_eyeLightOn;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;



out vec4 v_Color;

void main()
{
    gl_Position = u_ModelViewProjectionMatrix * vec4(aPos, 1.0);
     vec3 FragPos = vec3(gl_Position);
     vec3 N       = normalize(u_NormalMatrix * aNormal);
     vec3 V       = normalize(-FragPos);

     vec3 ambient = 0.1 * u_material.rgb;
    vec3 result  = ambient;

    // world ±¤¿ø
    if (u_worldLightOn) {
        vec3 Lw     = normalize(u_worldLightPos - FragPos);
        float dw    = max(dot(N, Lw), 0.0);
        vec3 diffW = dw * u_material.rgb;
        vec3 Rw    = reflect(-Lw, N);
        float sw   = pow(max(dot(V, Rw), 0.0), u_material.a);
        vec3 specW = sw * vec3(1.0);
        result    += diffW + specW;
    }

    // eye ±¤¿ø
    if (u_eyeLightOn) {
        vec3 Le     = normalize(u_eyeLightPos - FragPos);
        float de    = max(dot(N, Le), 0.0);
        vec3 diffE = de * u_material.rgb;
        vec3 Re    = reflect(-Le, N);
        float se   = pow(max(dot(V, Re), 0.0), u_material.a);
        vec3 specE = se * vec3(1.0);
        result    += diffE + specE;
    }

    v_Color = vec4(result, u_material.a);
}
