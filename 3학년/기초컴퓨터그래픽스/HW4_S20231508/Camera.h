#pragma once

#include <glm/gtc/matrix_transform.hpp>

enum Camera_Projection_TYPE {
	CAMERA_PROJECTION_PERSPECTIVE = 0, CAMERA_PROJECTION_ORTHOGRAPHIC
};

enum Camera_ID {
	CAMERA_MAIN = 0, CAMERA_SIDE_FRONT, CAMERA_TOP, CAMERA_SIDE,
	CAMERA_CC_0, CAMERA_CC_1
};

struct Camera_View {
	glm::vec3 pos;
	glm::vec3 uaxis, vaxis, naxis;
};

struct Camera_Projection {
	Camera_Projection_TYPE projection_type;
	union {
		struct {
			float fovy, aspect, n, f;
		} pers;
		struct {
			float left, right, botton, top, n, f;
		} ortho;
	} params;
};

struct View_Port {
	float x, y, w, h;
};

struct Camera {
	Camera_ID camera_id;
	Camera_View cam_view;
	glm::mat4 ViewMatrix;
	Camera_Projection cam_proj;
	glm::mat4 ProjectionMatrix;
	View_Port view_port;

	Camera(Camera_ID _camera_id) : camera_id(_camera_id) {}
	bool flag_valid;
	bool flag_move; // what else?
};


//과제
struct Perspective_Camera : public Camera {
	Perspective_Camera(Camera_ID _camera_id) : Camera(_camera_id) {}

	void define_camera(int win_width, int win_height, float win_aspect_ratio);

	// 이동
	void move_forward();
	void move_backward();
	void move_left();
	void move_right();
	void move_up();
	void move_down();

	// 회전
	void yaw_left();
	void yaw_right();
	void pitch_up();
	void pitch_down();
	void roll_left();
	void roll_right();

	// 줌
	void zoom_in();
	void zoom_out();

	// 행렬 갱신
	void update_ViewMatrix();
	void update_ProjectionMatrix();
};


struct Orthographic_Camera : public Camera {
	Orthographic_Camera(Camera_ID _camera_id) : Camera(_camera_id) {}
	void define_camera(int win_width, int win_height, float win_aspect_ratio);
};

struct Camera_Data {
	Perspective_Camera cam_main{ CAMERA_MAIN };
};

