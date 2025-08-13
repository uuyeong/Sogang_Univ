#define _CRT_SECURE_NO_WARNINGS

#include "Scene_Definitions.h"
extern Scene scene;

void Static_Object::read_geometry(int bytes_per_primitive) {
	FILE* fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Error: cannot open the object file %s ...\n", filename);
		exit(EXIT_FAILURE);
	}
	fread(&n_triangles, sizeof(int), 1, fp);
	vertices = (float*)malloc(n_triangles * bytes_per_primitive);
	if (vertices == NULL) {
		fprintf(stderr, "Error: cannot allocate memory for the geometry file %s ...\n", filename);
		exit(EXIT_FAILURE);
	}
	fread(vertices, bytes_per_primitive, n_triangles, fp); // assume the data file has no faults.
	// fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);
}

void Static_Object::prepare_geom_of_static_object() {
	int i, n_bytes_per_vertex, n_bytes_per_triangle;
	char filename[512];

	n_bytes_per_vertex = n_fields * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	read_geometry(n_bytes_per_triangle);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, n_triangles * n_bytes_per_triangle, vertices, GL_STATIC_DRAW);

	free(vertices);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,n_bytes_per_vertex,BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Building::define_object() {
	Material* cur_material;
	strcpy(filename, "Data/Building1_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	instances.back().ModelMatrix = glm::mat4(1.0f);

	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.135f, 0.2225f, 0.1575f, 1.0f);
	cur_material->diffuse = glm::vec4(0.54f, 0.89f, 0.63f, 1.0f);
	cur_material->specular = glm::vec4(0.316228f, 0.316228f, 0.316228f, 1.0f);
	cur_material->exponent = 128.0f * 0.1f;
}

void WoodTower::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;

	strcpy(filename, "Data/static_objects/woodtower_vnt.geom");	
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(157.0f, 76.5f, 0.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(2.5f, 2.5f, 2.5f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.1f, 0.3f, 0.1f, 1.0f);
	cur_material->diffuse = glm::vec4(0.4f, 0.6f, 0.3f, 1.0f);
	cur_material->specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	cur_material->exponent = 15.0f;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(198.0f, 120.0f, 0.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(4.0f, 4.0f, 4.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
	cur_material->diffuse = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
	cur_material->specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	cur_material->exponent = 128.0f * 0.078125f;
}

void Cat::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/static_objects/cat_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;


	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 145.5f, 10.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(15.0f, 15.0f, 15.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	cur_material->diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	cur_material->specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	cur_material->exponent = 128.0f * 0.4f;


}

void Dragon::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/static_objects/dragon_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(155.0f, 70.0f, 30.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(0.7f, 0.7f, 0.7f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.1745f, 0.01175f, 0.01175f, 1.0f);
	cur_material->diffuse = glm::vec4(0.61424f, 0.04136f, 0.04136f, 1.0f);
	cur_material->specular = glm::vec4(0.727811f, 0.626959f, 0.626959f, 1.0f);
	cur_material->exponent = 128.0f * 0.6;
}

void Bus::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/static_objects/bus_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 145.0f, 0.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(0.035f, 0.035f, 0.035f));
	*cur_MM = glm::rotate(*cur_MM, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.05f, 0.05f, 0.0f, 1.0f);
	cur_material->diffuse = glm::vec4(0.5f, 0.5f, 0.4f, 1.0f);
	cur_material->specular = glm::vec4(0.7f, 0.7f, 0.04f, 1.0f);
	cur_material->exponent = 128.0f * 0.078125f;
}

void Optimus::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/static_objects/optimus_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, 40.0f, 0.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(0.04f, 0.04f, 0.04f));\
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.19125f, 0.0735f, 0.0225f, 1.0f);
	cur_material->diffuse = glm::vec4(0.7038f, 0.27048f, 0.0828f, 1.0f);
	cur_material->specular = glm::vec4(0.256777f, 0.137622f, 0.086014f, 1.0f);
	cur_material->exponent = 128.0f * 0.1f;
}


void print_mat4(const char* string, glm::mat4 M) {
	fprintf(stdout, "\n***** %s ******\n", string);
	for (int i = 0; i < 4; i++)
		fprintf(stdout, "*** COL[%d] (%f, %f, %f, %f)\n", i, M[i].x, M[i].y, M[i].z, M[i].w);
	fprintf(stdout, "**************\n\n");
}


void Static_Object::draw_object(glm::mat4& ViewMatrix,
	glm::mat4& ProjectionMatrix,
	SHADER_ID shader_kind,
	std::vector<std::reference_wrapper<Shader>>& shader_list)
{
	glFrontFace(front_face_mode);

	for (auto& inst : instances) {
		// 모델 행렬 및 MVP 계산
		glm::mat4 ModelMatrix = inst.ModelMatrix;
		if (object_id == STATIC_OBJECT_OPTIMUS) {
			       // scene.time_stamp 에 따라 각도 계산 (0.5° per tick)
				float angle = scene.time_stamp * 0.5f;
			ModelMatrix = glm::rotate(
				ModelMatrix,
				glm::radians(angle),
				glm::vec3(0.0f, 0.0f, 1.0f)
				 );
			
		}
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// 법선 변환 행렬 계산
		glm::mat3 normalMatrix = glm::transpose(
			glm::inverse(glm::mat3(ViewMatrix * ModelMatrix))
		);

		// 카메라(눈) 위치 계산
		glm::mat4 invView = glm::inverse(ViewMatrix);
		glm::vec3 cameraEyePos(invView[3]);

		// 셰이더에 넘길 lightPos 결정
		glm::vec3 lightPos(0.0f);
		if (scene.world_light_on) {
			lightPos = scene.world_light_pos;
		}
		else if (scene.eye_light_on) {
			lightPos = cameraEyePos;
		}

		// 텍스처 셰이더 처리 (우선순위)
		if (shader_kind == SHADER_TEXTURE) {
			auto* sh = static_cast<Shader_Texture*>(
				&shader_list[shader_ID_mapper[shader_kind]].get());
			glUseProgram(sh->h_ShaderProgram);
			glm::mat4 MVP_tex = ProjectionMatrix * ViewMatrix * inst.ModelMatrix;
			glUniformMatrix4fv(sh->loc_ModelViewProjectionMatrix, 1, GL_FALSE, &MVP_tex[0][0]);
			glUniform1i(sh->loc_sampler, 0);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3 * n_triangles);
			glBindVertexArray(0);
			glUseProgram(0);
			return;
		}

		switch (shader_kind) {
		case SHADER_SIMPLE: {
			auto* sh = static_cast<Shader_Simple*>(
				&shader_list[shader_ID_mapper[shader_kind]].get());
			glUseProgram(sh->h_ShaderProgram);
			// u_ModelViewProjectionMatrix
			glUniformMatrix4fv(sh->loc_ModelViewProjectionMatrix, 1, GL_FALSE, &MVP[0][0]);
			// u_primitive_color
			glUniform3f(sh->loc_primitive_color,
				inst.material.diffuse.r,
				inst.material.diffuse.g,
				inst.material.diffuse.b);
			float a = 1.0f;
			if (object_id == STATIC_OBJECT_OPTIMUS && scene.transparent_mode) {
				a = scene.transparent_alpha;
				
			}
			glUniform1f(sh->loc_alpha, a);
			break;
		}

		case SHADER_GOURAUD: {
			auto* sh = static_cast<Shader_Gouraud*>(&shader_list[shader_ID_mapper[shader_kind]].get());
			glUseProgram(sh->h_ShaderProgram);
			glUniformMatrix4fv(sh->loc_MVP, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix3fv(sh->loc_NormalMat, 1, GL_FALSE, &normalMatrix[0][0]);
			// 월드광원 위치 + on/off
			glUniform3fv(sh->loc_worldLightPos, 1, &scene.world_light_pos[0]);
			glUniform1i(sh->loc_worldLightOn, scene.world_light_on);
			// 눈광원 위치 + on/off
			glUniform3fv(sh->loc_eyeLightPos, 1, &cameraEyePos[0]);
			glUniform1i(sh->loc_eyeLightOn, scene.eye_light_on);
			glUniform4fv(sh->loc_material, 1, &inst.material.diffuse[0]);
			break;
		}

		case SHADER_PHONG: {
			auto* sh = static_cast<Shader_Phong*>(&shader_list[shader_ID_mapper[shader_kind]].get());
			glUseProgram(sh->h_ShaderProgram);
			glUniformMatrix4fv(sh->loc_MVP, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(sh->loc_ModelMat, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix3fv(sh->loc_NormalMat, 1, GL_FALSE, &normalMatrix[0][0]);
			// 월드광원
			glUniform3fv(sh->loc_worldLightPos, 1, &scene.world_light_pos[0]);
			glUniform1i(sh->loc_worldLightOn, scene.world_light_on);
			// 눈광원
			glUniform3fv(sh->loc_eyeLightPos, 1, &cameraEyePos[0]);
			glUniform1i(sh->loc_eyeLightOn, scene.eye_light_on);
			// 뷰포지션 (반사 계산용)
			glUniform3fv(sh->loc_eyePos, 1, &cameraEyePos[0]);
			glUniform4fv(sh->loc_material, 1, &inst.material.diffuse[0]);
			break;
		}
		}

		// VAO 바인딩 및 그리기
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3 * n_triangles);
		glBindVertexArray(0);
		glUseProgram(0);
	}
}
