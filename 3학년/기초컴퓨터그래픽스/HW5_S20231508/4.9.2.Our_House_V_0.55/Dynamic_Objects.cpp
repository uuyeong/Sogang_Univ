#define _CRT_SECURE_NO_WARNINGS

#include "Scene_Definitions.h"
extern Scene scene;

void Tiger_D::define_object() {
#define N_TIGER_FRAMES 12
	glm::mat4* cur_MM;
	Material* cur_material;
	flag_valid = true;

	for (int i = 0; i < N_TIGER_FRAMES; i++) {
		object_frames.emplace_back();
        sprintf(object_frames[i].filename, "Data/dynamic_objects/tiger/Tiger_%d%d_triangles_vnt.geom", i / 10, i % 10);		object_frames[i].n_fields = 8;
		object_frames[i].front_face_mode = GL_CW;
		object_frames[i].prepare_geom_of_static_object();

		object_frames[i].instances.emplace_back();
		cur_MM = &(object_frames[i].instances.back().ModelMatrix);
		*cur_MM = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
		cur_material = &(object_frames[i].instances.back().material);
		cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
		cur_material->diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
		cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
		cur_material->exponent = 128.0f * 0.21794872f;
	}
}




void Wolf_D::define_object() {
#define N_TIGER_FRAMES 12
    glm::mat4* cur_MM;
    Material* cur_material;
    flag_valid = true;

    for (int i = 0; i < N_TIGER_FRAMES; i++) {
        object_frames.emplace_back();
        sprintf(object_frames[i].filename, "Data/dynamic_objects/wolf/wolf_%d%d_vnt.geom", i / 10, i % 10);		object_frames[i].n_fields = 8;
        object_frames[i].front_face_mode = GL_CW;
        object_frames[i].prepare_geom_of_static_object();

        object_frames[i].instances.emplace_back();
        cur_MM = &(object_frames[i].instances.back().ModelMatrix);
        *cur_MM = glm::scale(glm::mat4(1.0f), glm::vec3(35.2f, 35.2f, 35.2f));
        *cur_MM = glm::rotate(*cur_MM, glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
        *cur_MM = glm::rotate(*cur_MM, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));

        cur_material = &(object_frames[i].instances.back().material);
        cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
        cur_material->diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
        cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
        cur_material->exponent = 128.0f * 0.21794872f;
    }
}



void Dynamic_Object::draw_object(glm::mat4& ViewMatrix,
    glm::mat4& ProjectionMatrix,
    SHADER_ID shader_kind,
    std::vector<std::reference_wrapper<Shader>>& shader_list,
    int time_stamp)
{
    // 0) 프레임이 없거나 Wolf_2인 경우 건너뜀
    if (object_frames.empty() || object_id == DYNAMIC_OBJECT_WOLF_2) {
        return;
    }

    // 1) 현재 보여줄 스태틱 프레임 선택
    int cur_object_index = time_stamp % object_frames.size();
    Static_Object& cur_object = object_frames[cur_object_index];
    glFrontFace(cur_object.front_face_mode);

    // 2) 이 오브젝트 전체의 ModelMatrix 계산 (Tiger/Wolf 공통)
    glm::mat4 ModelMatrix(1.0f);
    switch (object_id) {
    case DYNAMIC_OBJECT_TIGER: {
        // ? Tiger 이동/회전 로직 (원본 그대로) :contentReference[oaicite:0]{index=0}
        const float t1 = 20.0f, t2 = 15.0f, t3 = 22.0f,
            t4 = 23.0f, t5 = 16.0f, t6 = 17.0f;
        const float forwardTotal = t1 + t2 + t3 + t4 + t5 + t6;
        const float cycle = forwardTotal * 2.0f;
        float t = fmod(static_cast<float>(time_stamp), cycle);

        glm::vec2 A(190, 20), B(190, 70), C(210, 70), D(210, 100),
            E(80, 100), F(80, 75), G(25, 75);
        glm::vec2 pos;
        float heading;

        if (t < forwardTotal) {
            if (t < t1) {
                float u = t / t1; pos = glm::mix(A, B, u); heading = atan2(B.y - A.y, B.x - A.x);
            }
            else if (t < t1 + t2) {
                float u = (t - t1) / t2; pos = glm::mix(B, C, u); heading = atan2(C.y - B.y, C.x - B.x);
            }
            else if (t < t1 + t2 + t3) {
                float u = (t - t1 - t2) / t3; pos = glm::mix(C, D, u); heading = atan2(D.y - C.y, D.x - C.x);
            }
            else if (t < t1 + t2 + t3 + t4) {
                float u = (t - t1 - t2 - t3) / t4; pos = glm::mix(D, E, u); heading = atan2(E.y - D.y, E.x - D.x);
            }
            else if (t < t1 + t2 + t3 + t4 + t5) {
                float u = (t - (t1 + t2 + t3 + t4)) / t5; pos = glm::mix(E, F, u); heading = atan2(F.y - E.y, F.x - E.x);
            }
            else {
                float u = (t - (t1 + t2 + t3 + t4 + t5)) / t6; pos = glm::mix(F, G, u); heading = atan2(G.y - F.y, G.x - F.x);
            }
        }
        else {
            float tr = t - forwardTotal;
            if (tr < t6) { float u = tr / t6; pos = glm::mix(G, F, u); heading = atan2(F.y - G.y, F.x - G.x); }
            else if (tr < t6 + t5) { float u = (tr - t6) / t5; pos = glm::mix(F, E, u); heading = atan2(E.y - F.y, E.x - F.x); }
            else if (tr < t6 + t5 + t4) { float u = (tr - t6 - t5) / t4; pos = glm::mix(E, D, u); heading = atan2(D.y - E.y, D.x - E.x); }
            else if (tr < t6 + t5 + t4 + t3) { float u = (tr - t6 - t5 - t4) / t3; pos = glm::mix(D, C, u); heading = atan2(C.y - D.y, C.x - D.x); }
            else if (tr < t6 + t5 + t4 + t3 + t2) { float u = (tr - t6 - t5 - t4 - t3) / t2; pos = glm::mix(C, B, u); heading = atan2(B.y - C.y, B.x - C.x); }
            else { float u = (tr - (t6 + t5 + t4 + t3 + t2)) / t1; pos = glm::mix(B, A, u); heading = atan2(A.y - B.y, A.x - B.x); }
        }

        glm::mat4 M(1.0f);
        M = glm::translate(M, glm::vec3(pos, 0.0f));
        M = glm::rotate(M, heading + glm::half_pi<float>(), glm::vec3(0, 0, 1));
        ModelMatrix = M;
        break;
    }

    case DYNAMIC_OBJECT_WOLF_1: {
        // ? Wolf 이동/회전 로직 (원본 그대로) :contentReference[oaicite:1]{index=1}
        const float t1 = 20.0f, t2 = 8.0f, t3 = 6.0f, t4 = 10.0f;
        const float cycle = 2 * (t1 + t2 + t3 + t4);
        float t = fmod(static_cast<float>(time_stamp), cycle);

        glm::vec2 A(170, 140), B(170, 105), C(85, 105), D(85, 30);
        glm::vec2 pos; float heading;

        if (t < t1) {
            float u = t / t1; pos = glm::mix(A, B, u); heading = atan2(B.y - A.y, B.x - A.x);
        }
        else if (t < t1 + t2) {
            float u = (t - t1) / t2; pos = glm::mix(B, C, u); heading = atan2(C.y - B.y, C.x - B.x);
        }
        else if (t < t1 + t2 + t3) {
            float u = (t - t1 - t2) / t3; pos = C; heading = u * glm::two_pi<float>();
        }
        else if (t < t1 + t2 + t3 + t4) {
            float u = (t - t1 - t2 - t3) / t4; pos = glm::mix(C, D, u); heading = atan2(D.y - C.y, D.x - C.x);
        }
        else {
            float tr = t - (t1 + t2 + t3 + t4);
            if (tr < t1) { float u = tr / t1; pos = glm::mix(D, C, u); heading = atan2(C.y - D.y, C.x - D.x); }
            else if (tr < t1 + t2) { float u = (tr - t1) / t2; pos = glm::mix(C, B, u); heading = atan2(B.y - C.y, B.x - C.x); }
            else if (tr < t1 + t2 + t3) { float u = (tr - t1 - t2) / t3; pos = B; heading = -u * glm::two_pi<float>(); }
            else { float u = (tr - (t1 + t2 + t3)) / t4; pos = glm::mix(B, A, u); heading = atan2(A.y - B.y, A.x - B.x); }
        }

        glm::mat4 M(1.0f);
        M = glm::translate(M, glm::vec3(pos, 0.0f));
        M = glm::rotate(M, heading, glm::vec3(0, 0, 1));
        ModelMatrix = M;
        break;
    }

    default:
        return;
    }

    // 3) 타이거 모델 좌표계 광원 위치 계산
    glm::vec3 modelLightPos(0.0f);
    if (object_id == DYNAMIC_OBJECT_TIGER && scene.model_light_on) {
        glm::vec4 lp = ModelMatrix * glm::vec4(1, 0, 0, 1);
        modelLightPos = glm::vec3(lp);
    }

    glm::mat4 invView = glm::inverse(ViewMatrix);
    glm::vec3 cameraEyePos(invView[3]);

    // 4) 인스턴스별 드로우
    for (auto& inst : cur_object.instances) {
        // (A) MVP, NormalMatrix 계산
        glm::mat4 M = ProjectionMatrix * ViewMatrix * ModelMatrix * inst.ModelMatrix;
        glm::mat3 N = glm::transpose(
            glm::inverse(glm::mat3(ViewMatrix * ModelMatrix * inst.ModelMatrix))
        );

        // (B) 셰이더 분기
        if (shader_kind == SHADER_TEXTURE && object_id == DYNAMIC_OBJECT_WOLF_1) {
            // Wolf → Texture Shader
            auto* sh = static_cast<Shader_Texture*>(
                &shader_list[shader_ID_mapper[SHADER_TEXTURE]].get());
            glUseProgram(sh->h_ShaderProgram);
            glUniformMatrix4fv(sh->loc_ModelViewProjectionMatrix, 1, GL_FALSE, &M[0][0]);
            glUniform1i(sh->loc_sampler, 0);
        }
        else if ((shader_kind == SHADER_GOURAUD || shader_kind == SHADER_PHONG)
            && object_id == DYNAMIC_OBJECT_TIGER)
        {
            // Tiger → Gouraud/Phong with model-light
            if (shader_kind == SHADER_GOURAUD) {
                auto* sh = static_cast<Shader_Gouraud*>(
                    &shader_list[shader_ID_mapper[SHADER_GOURAUD]].get());
                glUseProgram(sh->h_ShaderProgram);
                glUniformMatrix4fv(sh->loc_MVP, 1, GL_FALSE, &M[0][0]);
                glUniformMatrix3fv(sh->loc_NormalMat, 1, GL_FALSE, &N[0][0]);
                // 월드 광원
                glUniform3fv(sh->loc_worldLightPos, 1, &scene.world_light_pos[0]);
                glUniform1i(sh->loc_worldLightOn, scene.world_light_on);
                       // 눈(카메라) 광원
                glUniform3fv(sh->loc_eyeLightPos, 1, &cameraEyePos[0]);
                glUniform1i(sh->loc_eyeLightOn, scene.eye_light_on);
                       // (기존) 모델 좌표계 광원
                glUniform3fv(sh->loc_modelLightPos, 1, &modelLightPos[0]);
                       // 재질
                glUniform4fv(sh->loc_material, 1, &inst.material.diffuse[0]);
            }
            else {
                auto* sh = static_cast<Shader_Phong*>(
                    &shader_list[shader_ID_mapper[SHADER_PHONG]].get());
                glUseProgram(sh->h_ShaderProgram);
                glUniformMatrix4fv(sh->loc_MVP, 1, GL_FALSE, &M[0][0]);
                glUniformMatrix4fv(sh->loc_ModelMat, 1, GL_FALSE, &ModelMatrix[0][0]);
                glUniformMatrix3fv(sh->loc_NormalMat, 1, GL_FALSE, &N[0][0]);
                glUniform3fv(sh->loc_worldLightPos, 1, &scene.world_light_pos[0]);
                glUniform1i(sh->loc_worldLightOn, scene.world_light_on);
                       // 눈(카메라) 광원
                glUniform3fv(sh->loc_eyeLightPos, 1, &cameraEyePos[0]);
                glUniform1i(sh->loc_eyeLightOn, scene.eye_light_on);
                       // 카메라 위치(for specular)
                glUniform3fv(sh->loc_eyePos, 1, &cameraEyePos[0]);
                       // (기존) 모델 좌표계 광원
                glUniform3fv(sh->loc_modelLightPos, 1, &modelLightPos[0]);
                       // 재질
                glUniform4fv(sh->loc_material, 1, &inst.material.diffuse[0]);
            }
        }
        else {
            // 기본 Simple Shader
            auto* sh = static_cast<Shader_Simple*>(
                &shader_list[shader_ID_mapper[SHADER_SIMPLE]].get());
            glUseProgram(sh->h_ShaderProgram);
            glUniformMatrix4fv(sh->loc_ModelViewProjectionMatrix, 1, GL_FALSE, &M[0][0]);
            glUniform3f(sh->loc_primitive_color,
                inst.material.diffuse.r,
                inst.material.diffuse.g,
                inst.material.diffuse.b);
            glUniform1f(sh->loc_alpha, 1.0f);
        }

        // (C) 실제 그리기
        glBindVertexArray(cur_object.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3 * cur_object.n_triangles);
        glBindVertexArray(0);
        glUseProgram(0);
    }
}