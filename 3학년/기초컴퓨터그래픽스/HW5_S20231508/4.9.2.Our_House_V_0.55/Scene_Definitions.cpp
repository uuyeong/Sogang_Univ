#define _CRT_SECURE_NO_WARNINGS

#include "Scene_Definitions.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
extern Scene scene;

unsigned int static_object_ID_mapper[N_MAX_STATIC_OBJECTS];
unsigned int dynamic_object_ID_mapper[N_MAX_DYNAMIC_OBJECTS];
unsigned int camera_ID_mapper[N_MAX_CAMERAS];
unsigned int shader_ID_mapper[N_MAX_SHADERS];

void Axis_Object::define_axis() {
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_axes), &vertices_axes[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Axis_Object::draw_axis(Shader_Simple* shader_simple, glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix) {
#define WC_AXIS_LENGTH		60.0f
	glm::mat4 ModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(WC_AXIS_LENGTH, WC_AXIS_LENGTH, WC_AXIS_LENGTH));
	glm::mat4 ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;

	glUseProgram(shader_simple->h_ShaderProgram);
	glUniformMatrix4fv(shader_simple->loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);


	glBindVertexArray(VAO);
	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Scene::clock(int clock_id) { // currently one clock
	time_stamp = ++time_stamp % UINT_MAX;
}

void Scene::build_static_world() {
	static_geometry_data.building.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.building);

	static_geometry_data.woodtower.define_object();
	static_objects.push_back(static_geometry_data.woodtower);

	static_geometry_data.cat.define_object();
	static_objects.push_back(static_geometry_data.cat);

	static_geometry_data.dragon.define_object();
	static_objects.push_back(static_geometry_data.dragon);

	static_geometry_data.optimus.define_object();
	static_objects.push_back(static_geometry_data.optimus);

	static_geometry_data.bus.define_object();
	static_objects.push_back(static_geometry_data.bus);
}


void Scene::build_dynamic_world() {
	dynamic_geometry_data.tiger_d.define_object();
	dynamic_object_ID_mapper[DYNAMIC_OBJECT_TIGER] = dynamic_objects.size();
	dynamic_objects.push_back(dynamic_geometry_data.tiger_d);

	dynamic_geometry_data.wolf_d_1.define_object();
	dynamic_object_ID_mapper[DYNAMIC_OBJECT_WOLF_1] = dynamic_objects.size();
	dynamic_objects.push_back(dynamic_geometry_data.wolf_d_1);

	dynamic_geometry_data.wolf_d_2.define_object();
	dynamic_object_ID_mapper[DYNAMIC_OBJECT_WOLF_2] = dynamic_objects.size();
	dynamic_objects.push_back(dynamic_geometry_data.wolf_d_2);
}

void Scene::create_camera_list(int win_width, int win_height, float win_aspect_ratio) {
	camera_list.clear();
	// main camera
	camera_data.cam_main.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_MAIN] = camera_list.size();
	camera_list.push_back(camera_data.cam_main);

	// define others here
}

void Scene::build_shader_list() {
	// Simple
	shader_data.shader_simple.prepare_shader();
	shader_ID_mapper[SHADER_SIMPLE] = shader_list.size();
	shader_list.push_back(shader_data.shader_simple);
	
	// Gouraud
	shader_data.shader_gouraud.prepare_shader();
	shader_ID_mapper[SHADER_GOURAUD] = shader_list.size();
	shader_list.push_back(shader_data.shader_gouraud);
	
   // Phong
	shader_data.shader_phong.prepare_shader();
	shader_ID_mapper[SHADER_PHONG] = shader_list.size();
	shader_list.push_back(shader_data.shader_phong);

	// Texture
	shader_data.shader_texture.prepare_shader();
	shader_ID_mapper[SHADER_TEXTURE] = shader_list.size();
	shader_list.push_back(shader_data.shader_texture);
}

void Scene::initialize() {
	axis_object.define_axis();
	build_static_world();
	build_dynamic_world();
	create_camera_list(window.width, window.height, window.aspect_ratio);
	build_shader_list();

	world_light_on = GL_TRUE;
	world_light_pos = glm::vec3(125.0f, 100.0f, 30.0f);
	
	eye_light_on = GL_FALSE;
	model_light_on = GL_FALSE;

	glGenTextures(1, &tex_bus);
	glBindTexture(GL_TEXTURE_2D, tex_bus);
	int w, h, n;
	unsigned char* data = stbi_load("Textures/bus_diffuse.png", &w, &h, &n, 4);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		
	}
	stbi_image_free(data);
	
	glGenTextures(1, &tex_wolf);
	glBindTexture(GL_TEXTURE_2D, tex_wolf);
	data = stbi_load("Textures/wolf_diffuse.png", &w, &h, &n, 4);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		
	}
	 stbi_image_free(data);
	
	 // bus
	 glBindTexture(GL_TEXTURE_2D, tex_bus);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	 // wolf
	 glBindTexture(GL_TEXTURE_2D, tex_wolf);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}

void Scene::draw_static_world() {
	glm::mat4 ModelViewProjectionMatrix;

	for (auto it = static_objects.begin(); it != static_objects.end(); ++it) {
		auto& so = it->get();
		if (!so.flag_valid) continue;

		if (mode_v_flag) {
			if (so.object_id == STATIC_OBJECT_BUILDING) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				
			}
		}

		// 1) 어느 쉐이더를 사용할지 결정
		SHADER_ID use_shader = SHADER_SIMPLE;
		if (so.object_id == STATIC_OBJECT_DRAGON) {
			use_shader = shader_kind;              // ‘1’·‘2’키 결과
		}
		else if (so.object_id == STATIC_OBJECT_BUS) {
			use_shader = SHADER_TEXTURE;           // Bus → Texture
			// 텍스처 바인딩
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex_bus);
		}

		// 2) 실제 그리기
		so.draw_object(
			ViewMatrix,
			ProjectionMatrix,
			use_shader,
			shader_list);

		// 3) 텍스처 언바인딩 (필요 시)
		if (so.object_id == STATIC_OBJECT_BUS) {
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}


void Scene::draw_dynamic_world() {
	glm::mat4 ModelViewProjectionMatrix;

	for (auto it = dynamic_objects.begin(); it != dynamic_objects.end(); ++it) {
		auto& dobj = it->get();
		if (!dobj.flag_valid) continue;

		if (mode_v_flag) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// 무조건 기본(Simple) 이지만, Wolf만 Texture
		SHADER_ID use_shader = SHADER_SIMPLE;
		if (dobj.object_id == DYNAMIC_OBJECT_TIGER && scene.model_light_on) {
			use_shader = shader_kind;  // '1'·'2' 키로 선택된 Gouraud/Phong
			
		}
		else if (dobj.object_id == DYNAMIC_OBJECT_WOLF_1) {
			use_shader = SHADER_TEXTURE;  // wolf texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex_wolf);
			
		}

		// 그리기
		dobj.draw_object(ViewMatrix, ProjectionMatrix, use_shader, shader_list, time_stamp);


		// 언바인딩
		if (dobj.object_id == DYNAMIC_OBJECT_WOLF_1) 
			glBindTexture(GL_TEXTURE_2D, 0);
	}
}


void Scene::draw_axis() {
	axis_object.draw_axis(static_cast<Shader_Simple*>(&shader_list[shader_ID_mapper[SHADER_SIMPLE]].get()),
		ViewMatrix, ProjectionMatrix);
}

void Scene::draw_world() {
	draw_axis();
	draw_static_world();
	draw_dynamic_world();
}