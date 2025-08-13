#include "Scene_Definitions.h"

void Shader_Simple::prepare_shader() {
	shader_info[0] = { GL_VERTEX_SHADER, "Shaders/simple.vert" };
	shader_info[1] = { GL_FRAGMENT_SHADER, "Shaders/simple.frag" };
	shader_info[2] = { GL_NONE, NULL };

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
    loc_alpha = glGetUniformLocation(h_ShaderProgram, "u_alpha");
    loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
    loc_alpha = glGetUniformLocation(h_ShaderProgram, "u_alpha");
    glUseProgram(0);
}

void Shader_Gouraud::prepare_shader() {
    shader_info[0] = { GL_VERTEX_SHADER,   "Shaders/gouraud.vert" };
    shader_info[1] = { GL_FRAGMENT_SHADER, "Shaders/gouraud.frag" };
    shader_info[2] = { GL_NONE, NULL };
    h_ShaderProgram = LoadShaders(shader_info);
    glUseProgram(h_ShaderProgram);
    // uniform 위치 가져오기 (예: u_MVP, u_NormalMatrix, u_lightPos 등)
    loc_MVP = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
    loc_NormalMat = glGetUniformLocation(h_ShaderProgram, "u_NormalMatrix");
    loc_worldLightPos = glGetUniformLocation(h_ShaderProgram, "u_worldLightPos");
    loc_eyeLightPos = glGetUniformLocation(h_ShaderProgram, "u_eyeLightPos");
    loc_worldLightOn = glGetUniformLocation(h_ShaderProgram, "u_worldLightOn");
    loc_eyeLightOn = glGetUniformLocation(h_ShaderProgram, "u_eyeLightOn");
    loc_material = glGetUniformLocation(h_ShaderProgram, "u_material"); // struct로 받는 경우
    loc_modelLightPos = glGetUniformLocation(h_ShaderProgram, "u_modelLightPos");
    glUseProgram(0);
}

void Shader_Phong::prepare_shader() {
    shader_info[0] = { GL_VERTEX_SHADER,   "Shaders/phong.vert" };
    shader_info[1] = { GL_FRAGMENT_SHADER, "Shaders/phong.frag" };
    shader_info[2] = { GL_NONE, NULL };
    h_ShaderProgram = LoadShaders(shader_info);
    glUseProgram(h_ShaderProgram);
    // uniform 위치 가져오기
    loc_MVP = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
    loc_ModelMat = glGetUniformLocation(h_ShaderProgram, "u_ModelMatrix");
    loc_NormalMat = glGetUniformLocation(h_ShaderProgram, "u_NormalMatrix");
    loc_eyePos = glGetUniformLocation(h_ShaderProgram, "u_eyePos");
    loc_worldLightPos = glGetUniformLocation(h_ShaderProgram, "u_worldLightPos");
    loc_eyeLightPos = glGetUniformLocation(h_ShaderProgram, "u_eyeLightPos");
    loc_worldLightOn = glGetUniformLocation(h_ShaderProgram, "u_worldLightOn");
    loc_eyeLightOn = glGetUniformLocation(h_ShaderProgram, "u_eyeLightOn");
    loc_material = glGetUniformLocation(h_ShaderProgram, "u_material");
    loc_modelLightPos = glGetUniformLocation(h_ShaderProgram, "u_modelLightPos");
    glUseProgram(0);
}

void Shader_Texture::prepare_shader() {
    shader_info[0] = { GL_VERTEX_SHADER,   "Shaders/texture.vert" };
    shader_info[1] = { GL_FRAGMENT_SHADER, "Shaders/texture.frag" };
    shader_info[2] = { GL_NONE, NULL };
    h_ShaderProgram = LoadShaders(shader_info);
    glUseProgram(h_ShaderProgram);
    loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
    loc_sampler = glGetUniformLocation(h_ShaderProgram, "u_texture");
    glUseProgram(0);
    
}