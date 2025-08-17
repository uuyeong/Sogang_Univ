#define _CRT_SECURE_NO_WARNINGS

#include "Scene_Definitions.h"

void Tiger_D::define_object() {
#define N_TIGER_FRAMES 12
	glm::mat4* cur_MM;
	Material* cur_material;
	flag_valid = true;

	for (int i = 0; i < N_TIGER_FRAMES; i++) {
		object_frames.emplace_back();
		sprintf(object_frames[i].filename, "Data/Tiger_%d%d_triangles_vnt.geom", i / 10, i % 10);
		object_frames[i].n_fields = 8;
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

void Cow_D::define_object() {
    const int N_FRAMES_COW_1 = 1;
    glm::mat4* cur_MM;
    Material* cur_material;

    flag_valid = true;

    if (object_id != DYNAMIC_OBJECT_COW_1) {
        return;
    }

    for (int i = 0; i < N_FRAMES_COW_1; i++) {
        object_frames.emplace_back();
        auto& frame = object_frames.back();

        strcpy(frame.filename, "Data/cow_vn.geom");
        frame.n_fields = 6;
        frame.front_face_mode = GL_CCW;
        frame.prepare_geom_of_static_object();

        frame.instances.emplace_back();
        cur_MM = &frame.instances.back().ModelMatrix;
        *cur_MM = glm::scale(glm::mat4(1.0f), glm::vec3(30.2f, 30.2f, 30.2f));
        *cur_MM = glm::rotate(*cur_MM, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));

        cur_material = &frame.instances.back().material;

        cur_material->emission = glm::vec4(0.0f);
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
    int time_stamp) {
    if (object_frames.empty() || object_id == DYNAMIC_OBJECT_COW_2) {
        return;
    }

    int cur_object_index = time_stamp % object_frames.size();
    Static_Object& cur_object = object_frames[cur_object_index];
    glFrontFace(cur_object.front_face_mode);

    glm::mat4 ModelMatrix = glm::mat4(1.0f);

    switch (object_id) {
    case DYNAMIC_OBJECT_TIGER: {
        const float t1 = 20.0f;   // (190,20) → (190,70)
        const float t2 = 15.0f;   // (190,70) → (210,70)
        const float t3 = 22.0f;   // (210,70) → (210,100)
        const float t4 = 23.0f;   // (210,100) → (80,100)
        const float t5 = 16.0f;   // (80,100) → (80,75)
        const float t6 = 17.0f;   // (80,75) → (25,75)
        const float forwardTotal = t1 + t2 + t3 + t4 + t5 + t6;
        const float cycle = forwardTotal * 2.0f;

        float t = fmod(static_cast<float>(time_stamp), cycle);

        glm::vec2 A(190.0f, 20.0f);
        glm::vec2 B(190.0f, 70.0f);
        glm::vec2 C(210.0f, 70.0f);
        glm::vec2 D(210.0f, 100.0f);
        glm::vec2 E(80.0f, 100.0f);
        glm::vec2 F(80.0f, 75.0f);
        glm::vec2 G(25.0f, 75.0f);

        glm::vec2 pos;
        float     heading;

        if (t < forwardTotal) {
            if (t < t1) {
                float u = t / t1;
                pos = glm::mix(A, B, u);
                heading = atan2(B.y - A.y, B.x - A.x);
            }
            else if (t < t1 + t2) {
                float u = (t - t1) / t2;
                pos = glm::mix(B, C, u);
                heading = atan2(C.y - B.y, C.x - B.x);
            }
            else if (t < t1 + t2 + t3) {
                float u = (t - t1 - t2) / t3;
                pos = glm::mix(C, D, u);
                heading = atan2(D.y - C.y, D.x - C.x);
            }
            else if (t < t1 + t2 + t3 + t4) {
                float u = (t - t1 - t2 - t3) / t4;
                pos = glm::mix(D, E, u);
                heading = atan2(E.y - D.y, E.x - D.x);
            }
            else if (t < t1 + t2 + t3 + t4 + t5) {
                float u = (t - t1 - t2 - t3 - t4) / t5;
                pos = glm::mix(E, F, u);
                heading = atan2(F.y - E.y, F.x - E.x);
            }
            else {
                float u = (t - (t1 + t2 + t3 + t4 + t5)) / t6;
                pos = glm::mix(F, G, u);
                heading = atan2(G.y - F.y, G.x - F.x);
            }
        }
        else {
            float tr = t - forwardTotal;
            if (tr < t6) {
                float u = tr / t6;
                pos = glm::mix(G, F, u);
                heading = atan2(F.y - G.y, F.x - G.x);
            }
            else if (tr < t6 + t5) {
                float u = (tr - t6) / t5;
                pos = glm::mix(F, E, u);
                heading = atan2(E.y - F.y, E.x - F.x);
            }
            else if (tr < t6 + t5 + t4) {
                float u = (tr - t6 - t5) / t4;
                pos = glm::mix(E, D, u);
                heading = atan2(D.y - E.y, D.x - E.x);
            }
            else if (tr < t6 + t5 + t4 + t3) {
                float u = (tr - t6 - t5 - t4) / t3;
                pos = glm::mix(D, C, u);
                heading = atan2(C.y - D.y, C.x - D.x);
            }
            else if (tr < t6 + t5 + t4 + t3 + t2) {
                float u = (tr - t6 - t5 - t4 - t3) / t2;
                pos = glm::mix(C, B, u);
                heading = atan2(B.y - C.y, B.x - C.x);
            }
            else {
                float u = (tr - (t6 + t5 + t4 + t3 + t2)) / t1;
                pos = glm::mix(B, A, u);
                heading = atan2(A.y - B.y, A.x - B.x);
            }
        }

        glm::mat4 M(1.0f);
        M = glm::translate(M, glm::vec3(pos.x, pos.y, 0.0f));
        M = glm::rotate(M, heading + glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
        ModelMatrix = M;
        break;
    }





    case DYNAMIC_OBJECT_COW_1: {
        const float t1 = 20.0f, t2 = 8.0f, t3 = 6.0f, t4 = 10.0f;
        const float cycle = 2 * (t1 + t2 + t3 + t4);

        float t = fmod(static_cast<float>(time_stamp), cycle);

        // 위치 정의
        glm::vec2 A(170.0f, 150.0f);
        glm::vec2 B(170.0f, 100.0f);
        glm::vec2 C(80.0f, 100.0f);
        glm::vec2 D(80.0f, 20.0f);

        glm::vec2 pos;
        float     heading; // 바라보는 방향

        if (t < t1) {
            float u = t / t1;
            pos = glm::mix(A, B, u);
            heading = atan2(B.y - A.y, B.x - A.x);
        }
        else if (t < t1 + t2) {
            float u = (t - t1) / t2;
            pos = glm::mix(B, C, u);
            heading = atan2(C.y - B.y, C.x - B.x);
        }
        else if (t < t1 + t2 + t3) {
            float u = (t - t1 - t2) / t3;  
            pos = C;
            heading = u * glm::two_pi<float>(); 
        }
        else if (t < t1 + t2 + t3 + t4) {
            float u = (t - t1 - t2 - t3) / t4;
            pos = glm::mix(C, D, u);
            heading = atan2(D.y - C.y, D.x - C.x);
        }
        else {
            float tr = t - (t1 + t2 + t3 + t4);
            if (tr < t1) {
                float u = tr / t1;
                pos = glm::mix(D, C, u);
                heading = atan2(C.y - D.y, C.x - D.x);
            }
            else if (tr < t1 + t2) {
                float u = (tr - t1) / t2;
                pos = glm::mix(C, B, u);
                heading = atan2(B.y - C.y, B.x - C.x);
            }
            else if (tr < t1 + t2 + t3) {
                float u = (tr - t1 - t2) / t3;
                pos = B;
                heading = -u * glm::two_pi<float>();
            }
            else {
                float u = (tr - t1 - t2 - t3) / t4;
                pos = glm::mix(B, A, u);
                heading = atan2(A.y - B.y, A.x - B.x);
            }
        }

        glm::mat4 M(1.0f);
        M = glm::translate(M, glm::vec3(pos.x, pos.y, 10.0f));
        M = glm::rotate(M, heading, glm::vec3(0, 0, 1));   // 방향 맞추기

        ModelMatrix = M;
        break;
    }


    default:
        return;
    }

    for (auto& inst : cur_object.instances) {
        glm::mat4 M = ProjectionMatrix
            * ViewMatrix
            * ModelMatrix
            * inst.ModelMatrix;

        Shader_Simple* shader = static_cast<Shader_Simple*>(
            &shader_list[shader_ID_mapper[shader_kind]].get());
        glUseProgram(shader->h_ShaderProgram);
        glUniformMatrix4fv(shader->loc_ModelViewProjectionMatrix,
            1, GL_FALSE, &M[0][0]);
        glUniform3f(shader->loc_primitive_color,
            inst.material.diffuse.r,
            inst.material.diffuse.g,
            inst.material.diffuse.b);

        glBindVertexArray(cur_object.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3 * cur_object.n_triangles);
        glBindVertexArray(0);
        glUseProgram(0);
    }
}