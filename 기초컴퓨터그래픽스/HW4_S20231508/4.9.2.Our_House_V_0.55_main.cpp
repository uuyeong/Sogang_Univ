#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Shaders/LoadShaders.h"
#include "Scene_Definitions.h"

Scene scene;
extern Scene scene;
static int currentCam = 0;
static bool showCamFrame = false;


// 동적카메라
static const glm::vec3 dynEye(100.0f, 100.0f, 40.0f);
static float dynYaw = glm::radians(0.0f);
static float dynPitch = glm::radians(0.0f);
static float dynFov = 45.0f;  // degrees


static void drawCameraFrame(const glm::mat4& VP,
	const glm::vec3& pos,
	const glm::vec3& xAxis,
	const glm::vec3& yAxis,
	const glm::vec3& zAxis)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&VP[0][0]);
	glBegin(GL_LINES);
	// X축 (빨강)
	glColor3f(1, 0, 0);
	glVertex3fv(&pos.x);
	{ glm::vec3 q = pos + xAxis * 20.0f; glVertex3fv(&q.x); }
	// Y축 (초록)
	glColor3f(0, 1, 0);
	glVertex3fv(&pos.x);
	{ glm::vec3 q = pos + yAxis * 20.0f; glVertex3fv(&q.x); }
	// Z축 (파랑)
	glColor3f(0, 0, 1);
	glVertex3fv(&pos.x);
	{ glm::vec3 q = pos + zAxis * 20.0f; glVertex3fv(&q.x); }
	glEnd();
	glColor3f(1, 1, 1);
}



void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, scene.window.width, scene.window.height);

	if (currentCam == 0) {
		// 주 카메라
		scene.ViewMatrix = scene.camera_data.cam_main.ViewMatrix;
		scene.ProjectionMatrix = scene.camera_data.cam_main.ProjectionMatrix;
	} else if(1<=currentCam && currentCam < 4) {
		// CCTV 뷰
		static const glm::vec3 eye[3] = {
			{145.0f,  90.0f, 50.0f},
			{225.0f,  75.0f, 50.0f},
			{ 40.0f, 110.0f, 50.0f}
		};
		static const glm::vec3 center[3] = {
			{140.0f, 105.0f, 10.0f},
			{210.0f,  75.0f, 10.0f},
			{ 50.0f,  80.0f, 10.0f}
		};
		const glm::vec3 up(0.0f, 0.0f, 1.0f);

		int idx = currentCam - 1;
		scene.ViewMatrix = glm::lookAt(eye[idx], center[idx], up);

		float aspect = float(scene.window.width) / float(scene.window.height);
		scene.ProjectionMatrix = glm::perspective( glm::radians(60.0f), aspect, 1.0f, 500.0f );
	} else if (currentCam == 4) {
		glm::vec3 dir;
		dir.x = cos(dynPitch) * cos(dynYaw);
		dir.y = cos(dynPitch) * sin(dynYaw);
		dir.z = sin(dynPitch);
		scene.ViewMatrix = glm::lookAt(dynEye, dynEye + dir, { 0,0,1 });
		float aspect = float(scene.window.width) / scene.window.height;
		scene.ProjectionMatrix = glm::perspective( glm::radians(dynFov), aspect, 1.0f, 500.0f);
	} else {
		// 5: 상면도, 6: 측면도, 7: 정면도
		glm::vec3 eye, center(0.0f), up;
		float aspect = float(scene.window.width) / scene.window.height;
		float viewSize = 200.0f;
		float left = -viewSize * aspect;
		float right = viewSize * aspect;
		float bottom = -viewSize;
		float top = viewSize;
		float zNear = -500.0f;
		float zFar = 500.0f;

		switch (currentCam) {
			case 5:
				eye = glm::vec3(0.0f, 0.0f, 300.0f); 
				up = glm::vec3(0.0f, 1.0f, 0.0f);
				break;
			case 6:
				eye = glm::vec3(300.0f, 0.0f, 0.0f);
				up = glm::vec3(0.0f, 0.0f, 1.0f);
				break;
			case 7:
				eye = glm::vec3(0.0f, 300.0f, 0.0f);
				up = glm::vec3(0.0f, 0.0f, 1.0f);
				break;
		}

		scene.ViewMatrix = glm::lookAt(eye, center, up);
		scene.ProjectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
	}


	scene.draw_world();


	if (showCamFrame) {
		glm::mat4 VP = scene.ProjectionMatrix * scene.ViewMatrix;

		// 주 카메라 
		{
			auto& cam = scene.camera_data.cam_main;
			glm::mat4 invV = glm::inverse(cam.ViewMatrix);
			glm::vec3 camPos = glm::vec3(invV[3]);

			glm::vec3 right = cam.cam_view.uaxis;
			glm::vec3 up = cam.cam_view.vaxis;
			glm::vec3 forward = -cam.cam_view.naxis;

			drawCameraFrame(VP, camPos, right, up, forward);
		}

		static const glm::vec3 eyeC[3] = {
			{145.0f,  90.0f, 40.0f},
			{225.0f,  75.0f, 40.0f},
			{ 40.0f, 110.0f, 40.0f}
		};
		static const glm::vec3 cenC[3] = {
			{140.0f, 105.0f, 10.0f},
			{210.0f,  75.0f, 10.0f},
			{ 50.0f,  80.0f, 10.0f}
		};

		for (int i = 0; i < 3; ++i) {
			glm::vec3 forward = glm::normalize(cenC[i] - eyeC[i]);
			glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 0, 1)));
			glm::vec3 up = glm::cross(right, forward);
			drawCameraFrame(VP, eyeC[i], right, up, forward);
		}
	}

	glutSwapBuffers();
}


void keyboard(unsigned char key, int x, int y) {
	static int flag_cull_face = 0, polygon_fill_on = 0, depth_test_on = 0;
	switch (key) {
		case 27: // ESC
			glutLeaveMainLoop();
			break;

		// 이동
		case 'w': scene.camera_data.cam_main.move_forward(); break;
		case 's': scene.camera_data.cam_main.move_backward(); break;
		case 'a': scene.camera_data.cam_main.move_left(); break;
		case 'd': scene.camera_data.cam_main.move_right(); break;

		case 'q': scene.camera_data.cam_main.move_up(); break;
		case 'e': scene.camera_data.cam_main.move_down(); break;

		// 회전
		case 'j': scene.camera_data.cam_main.yaw_left(); break;
		case 'l': scene.camera_data.cam_main.yaw_right(); break;

		case 'i': scene.camera_data.cam_main.pitch_up(); break;
		case 'k': scene.camera_data.cam_main.pitch_down(); break;

		case 'u': scene.camera_data.cam_main.roll_left(); break;
		case 'o': scene.camera_data.cam_main.roll_right(); break;


		//기존 코드 (임의로 수정함)
		case 'z':
			flag_cull_face = (flag_cull_face + 1) % 3;
			switch (flag_cull_face) {
			case 0:
				glDisable(GL_CULL_FACE);
				glutPostRedisplay();
				fprintf(stdout, "^^^ No faces are culled.\n");
				break;
			case 1: // cull back faces;
				glCullFace(GL_BACK);
				glEnable(GL_CULL_FACE);
				glutPostRedisplay();
				fprintf(stdout, "^^^ Back faces are culled.\n");
				break;
			case 2: // cull front faces;
				glCullFace(GL_FRONT);
				glEnable(GL_CULL_FACE);
				glutPostRedisplay();
				fprintf(stdout, "^^^ Front faces are culled.\n");
				break;
			}
			break;
		case 'x':
			polygon_fill_on = 1 - polygon_fill_on;
			if (polygon_fill_on) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				fprintf(stdout, "^^^ Polygon filling enabled.\n");
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				fprintf(stdout, "^^^ Line drawing enabled.\n");
			}
			glutPostRedisplay();
			break;
		case 'c':
			depth_test_on = 1 - depth_test_on;
			if (depth_test_on) {
				glEnable(GL_DEPTH_TEST);
				fprintf(stdout, "^^^ Depth test enabled.\n");
			}
			else {
				glDisable(GL_DEPTH_TEST);
				fprintf(stdout, "^^^ Depth test disabled.\n");
			}
			glutPostRedisplay();
			break;

			// CCTV 전환
		case '0': currentCam = 0; break; //기본카메라
		case '1': currentCam = 1; break; //CCTV 1
		case '2': currentCam = 2; break; //CCTV 2
		case '3': currentCam = 3; break; //CCTV 3
		case '4': currentCam = 4; break; //동적카메라
		case '5': currentCam = 5; break;  // 상면도
		case '6': currentCam = 6; break;  // 측면도
		case '7': currentCam = 7; break;  // 정면도

		case 'f':   // F키로 카메라 프레임 토글
			showCamFrame = !showCamFrame;
			glutPostRedisplay();
			break;

	}

	// 갱신
	scene.camera_data.cam_main.update_ViewMatrix();
	scene.camera_data.cam_main.update_ProjectionMatrix();
	glutPostRedisplay();
	glutIgnoreKeyRepeat(1);
}



void mouse(int button, int state, int x, int y) {
	if (currentCam != 4 && state == GLUT_DOWN) {
		switch (button) {
		case 3:
			scene.camera_data.cam_main.zoom_in();
			break;
		case 4: 
			scene.camera_data.cam_main.zoom_out();
			break;
		}

		scene.camera_data.cam_main.update_ProjectionMatrix();
		glutPostRedisplay();
	}
	else if(currentCam == 4 && state == GLUT_DOWN) {
		if (button == 3) {     
			dynFov = glm::clamp(dynFov + 1.0f, 15.0f, 90.0f);
			glutPostRedisplay();
		}
		else if (button == 4) { 
			dynFov = glm::clamp(dynFov - 1.0f, 15.0f, 90.0f);
			glutPostRedisplay();
		}
	}
}

void special(int key, int x, int y) {
	if (currentCam == 4) {
		const float dAng = glm::radians(2.0f);
		switch (key) {
		case GLUT_KEY_LEFT:  dynYaw -= dAng; break;
		case GLUT_KEY_RIGHT: dynYaw += dAng; break;
		case GLUT_KEY_UP:    dynPitch = glm::clamp(dynPitch + dAng, -glm::half_pi<float>(), glm::half_pi<float>()); break;
		case GLUT_KEY_DOWN:  dynPitch = glm::clamp(dynPitch - dAng, -glm::half_pi<float>(), glm::half_pi<float>()); break;
		}
		glutPostRedisplay();
	}
}




void reshape(int width, int height) {
	scene.window.width = width;
	scene.window.height = height;
	scene.window.aspect_ratio = (float)width / height;
	scene.create_camera_list(scene.window.width, scene.window.height, scene.window.aspect_ratio);
	glutPostRedisplay();
}

void timer_scene(int index) {
	scene.clock(0);
	glutPostRedisplay();
	glutTimerFunc(100, timer_scene, 0);
}


void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutMouseFunc(mouse);
	//	glutCloseFunc(cleanup_OpenGL_stuffs or else); // Do it yourself!!!
}

void initialize_OpenGL(void) {
	glEnable(GL_DEPTH_TEST); 

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glClearColor(0.12f, 0.18f, 0.12f, 1.0f);
}

void initialize_renderer(void) {
	register_callbacks();
	initialize_OpenGL();
	scene.initialize();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void print_message(const char* m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char* program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170/AIE4012 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}


#define N_MESSAGE_LINES 1
void main(int argc, char* argv[]) {
	char program_name[256] = "Sogang CSE4170/AIE4120 Our_House_GLSL_V_0.55";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: fill it yourself!" };

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}

