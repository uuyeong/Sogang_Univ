#version 330 core

in  vec3 FragPos;
in  vec3 Normal;

uniform vec3  u_worldLightPos;   // ¿ùµå ÁÂÇ¥°è ±¤¿ø À§Ä¡
uniform vec3  u_eyeLightPos;     // Ä«¸Þ¶ó(´«) ÁÂÇ¥°è ±¤¿ø À§Ä¡
uniform bool  u_worldLightOn;    // ¿ùµå±¤¿ø on/off
uniform bool  u_eyeLightOn;      // ´«±¤¿ø on/off


uniform vec3 u_eyePos;
uniform vec4 u_material;   // vec4(diffuse.rgb, diffuse.a)

out vec4 FragColor;

void main()
{
    vec3 norm    = normalize(Normal);
    vec3 viewDir = normalize(u_eyePos - FragPos);

    // ±âº» ambient ÇÑ ¹ø¸¸
    vec3 ambient = 0.1 * u_material.rgb;
    vec3 result  = ambient;

    // --- ¿ùµå ÁÂÇ¥°è ±¤¿ø ±â¿© ---
    if (u_worldLightOn) {
        vec3 Lw        = normalize(u_worldLightPos - FragPos);
        float diffW    = max(dot(norm, Lw), 0.0);
        vec3 diffuseW = diffW * u_material.rgb;
        vec3 Rw       = reflect(-Lw, norm);
        float specW   = pow(max(dot(viewDir, Rw), 0.0), u_material.a);
        vec3 specularW= specW * vec3(1.0);
        result       += diffuseW + specularW;
    }

    // --- Ä«¸Þ¶ó ÁÂÇ¥°è ±¤¿ø ±â¿© ---
    if (u_eyeLightOn) {
        vec3 Le        = normalize(u_eyeLightPos - FragPos);
        float diffE    = max(dot(norm, Le), 0.0);
        vec3 diffuseE = diffE * u_material.rgb;
        vec3 Re       = reflect(-Le, norm);
        float specE   = pow(max(dot(viewDir, Re), 0.0), u_material.a);
        vec3 specularE= specE * vec3(1.0);
        result       += diffuseE + specularE;
    }

    FragColor = vec4(result, u_material.a);
}
