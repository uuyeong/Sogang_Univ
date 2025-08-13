#define _CRT_SECURE_NO_WARNINGS

#include "Camera.h"
#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f


//추가 내용
#define SPEED 60.0f
#define ROTATION 4.0f * TO_RADIAN
#define FOVY_STEP (1.0f * TO_RADIAN)

void Perspective_Camera::update_ViewMatrix() {
	glm::vec3 center = cam_view.pos - cam_view.naxis;
	ViewMatrix = glm::lookAt(cam_view.pos, center, cam_view.vaxis);
}

void Perspective_Camera::update_ProjectionMatrix() {
	ProjectionMatrix = glm::perspective(
		cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
		cam_proj.params.pers.n, cam_proj.params.pers.f
	);
}

// 이동
void Perspective_Camera::move_forward() { cam_view.pos -= SPEED * cam_view.naxis; }
void Perspective_Camera::move_backward() { cam_view.pos += SPEED * cam_view.naxis; }
void Perspective_Camera::move_left() { cam_view.pos -= SPEED * cam_view.uaxis; }
void Perspective_Camera::move_right() { cam_view.pos += SPEED * cam_view.uaxis; }
void Perspective_Camera::move_up() { cam_view.pos += SPEED * cam_view.vaxis; }
void Perspective_Camera::move_down() { cam_view.pos -= SPEED * cam_view.vaxis; }

// 회전
void Perspective_Camera::yaw_left() {
	glm::mat4 R = glm::rotate(glm::mat4(1.0f), ROTATION, cam_view.vaxis);
	cam_view.naxis = glm::normalize(glm::vec3(R * glm::vec4(cam_view.naxis, 0.0f)));
	cam_view.uaxis = glm::normalize(glm::cross(cam_view.vaxis, cam_view.naxis));
	update_ViewMatrix();
}

void Perspective_Camera::yaw_right() {
	glm::mat4 R = glm::rotate(glm::mat4(1.0f), -ROTATION, cam_view.vaxis);
	cam_view.naxis = glm::normalize(glm::vec3(R * glm::vec4(cam_view.naxis, 0.0f)));
	cam_view.uaxis = glm::normalize(glm::cross(cam_view.vaxis, cam_view.naxis));
	update_ViewMatrix();
}

void Perspective_Camera::pitch_up() {
	glm::mat4 R = glm::rotate(glm::mat4(1.0f), ROTATION, cam_view.uaxis);
	cam_view.naxis = glm::normalize(glm::vec3(R * glm::vec4(cam_view.naxis, 0.0f)));
	cam_view.vaxis = glm::normalize(glm::cross(cam_view.naxis, cam_view.uaxis));
	update_ViewMatrix();
}

void Perspective_Camera::pitch_down() {
	glm::mat4 R = glm::rotate(glm::mat4(1.0f), -ROTATION, cam_view.uaxis);
	cam_view.naxis = glm::normalize(glm::vec3(R * glm::vec4(cam_view.naxis, 0.0f)));
	cam_view.vaxis = glm::normalize(glm::cross(cam_view.naxis, cam_view.uaxis));
	update_ViewMatrix();
}

void Perspective_Camera::roll_left() {
	glm::mat4 R = glm::rotate(glm::mat4(1.0f), ROTATION, cam_view.naxis);
	cam_view.uaxis = glm::normalize(glm::vec3(R * glm::vec4(cam_view.uaxis, 0.0f)));
	cam_view.vaxis = glm::normalize(glm::cross(cam_view.naxis, cam_view.uaxis));
	update_ViewMatrix();
}

void Perspective_Camera::roll_right() {
	glm::mat4 R = glm::rotate(glm::mat4(1.0f), -ROTATION, cam_view.naxis);
	cam_view.uaxis = glm::normalize(glm::vec3(R * glm::vec4(cam_view.uaxis, 0.0f)));
	cam_view.vaxis = glm::normalize(glm::cross(cam_view.naxis, cam_view.uaxis));
	update_ViewMatrix();
}


// 줌
void Perspective_Camera::zoom_in() {
	cam_proj.params.pers.fovy -= FOVY_STEP;
}
void Perspective_Camera::zoom_out() {
	cam_proj.params.pers.fovy += FOVY_STEP;
}








void Perspective_Camera::define_camera(int win_width, int win_height, float win_aspect_ratio) {
	glm::mat3 R33_t;
	glm::mat4 T;

	switch (camera_id) {
	case CAMERA_MAIN:
		flag_valid = true;
		flag_move = true; // yes. the main camera is permitted to move

		// let's use glm funtions to set up the initial camera pose
		ViewMatrix = glm::lookAt(glm::vec3(-600.0f, -600.0f, 400.0f), glm::vec3(125.0f, 80.0f, 25.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)); // initial pose for main camera
		cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
		R33_t = glm::transpose(glm::mat3(ViewMatrix));
		T = glm::mat4(R33_t) * ViewMatrix;
		cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]); // why does this work?

		cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
		cam_proj.params.pers.fovy = 15.0f * TO_RADIAN;
		cam_proj.params.pers.aspect = win_aspect_ratio;
		cam_proj.params.pers.n = 1.0f;
		cam_proj.params.pers.f = 50000.0f;

		ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);
		view_port.x = 200; view_port.y = 200; view_port.w = win_width - 200; view_port.h = win_height - 200;
		break;
	case CAMERA_CC_0:
		break;
	}
}

