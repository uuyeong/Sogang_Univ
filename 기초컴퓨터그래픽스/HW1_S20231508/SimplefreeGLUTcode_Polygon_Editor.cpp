#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Definitions.h"
#include <math.h>

Window wd;
Status st;
My_Polygon pg;


void move_points(My_Polygon* pg, float delx, float dely) {
	for (int i = 0; i < pg->n_points; i++) {
		pg->point[i][0] += delx;  // x ��ǥ �̵�
		pg->point[i][1] += dely;  // y ��ǥ �̵�
	}

	// �ٰ��� �̵� �� �����߽ɵ� ���
	update_center_of_gravity(pg);
}


// GLUT callbacks
void timer(int value) {
	rotate_points_around_center_of_grivity(&pg);
	glutPostRedisplay();
	if (st.rotation_mode)
		glutTimerFunc(ROTATION_STEP, timer, 0);
}

void draw_lines_by_points(My_Polygon* pg) {
	glColor3f(POINT_COLOR);
	for (int i = 0; i < pg->n_points; i++) {
		glBegin(GL_POINTS);
		glVertex2f(pg->point[i][0], pg->point[i][1]);
		glEnd();
	}
	if (st.leftbuttonpressed) {
		glColor3f(CHANGE_LINE_COLOR); //�̵������ ���� �� ����
	}
	else {
		glColor3f(LINE_COLOR);  // ���� ���� �� ����
	}
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < pg->n_points; i++)
		glVertex2f(pg->point[i][0], pg->point[i][1]);
	glEnd();
}


void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);

	if (pg.n_points > 0) {
		draw_lines_by_points(&pg);
	}

	// �ٰ����� �ϼ��Ǿ��� ���� �����߽��� ǥ��
	if (!st.polygon_mode && pg.n_points >= 3) {
		glColor3f(CENTER_POINT_COLOR);
		glBegin(GL_POINTS);
		glVertex2f(pg.center_x, pg.center_y);
		glEnd();
	}

	glFlush();
}




void keyboard(unsigned char key, int x, int y) {
	bool temp = false;
	switch (key) {
	case 'c':
		if (!st.rotation_mode) {
			pg.n_points = 0, st.polygon_mode = 0;
			fprintf(stderr, "*** Screen reset.\n");
		}
		glutPostRedisplay();
		break;
	case 'p':
		if (st.polygon_mode) {  // �ٰ��� ��忡�� p�� ������ ��
			if (pg.n_points >= 3) {  // �ٰ����� �ϼ��� ���
				st.polygon_mode = 0;
				fprintf(stderr, "*** Polygon completed!\n");
				update_center_of_gravity(&pg);
				glutPostRedisplay();
			}
			else {
				fprintf(stderr, "*** Choose at least three points!\n");
			}
		}
		else {  // �ٰ��� ��尡 �ƴ� ���¿��� p�� ������ ��
			if (pg.n_points > 0) {
				fprintf(stderr, "*** Press 'C' to clear the polygon, then press 'P' to restart polygon mode.\n");
			}
			else {
				st.polygon_mode = 1;
				fprintf(stderr, "*** Polygon mode started!\n");
				glutPostRedisplay();
			}
		}
		break;
	case 'r':
		if (!st.polygon_mode && pg.n_points >= 3) { //�ٰ��� ��尡 ���� �ٰ����� �����ϴ� ������ ��
			if (!st.rotation_mode) { //ȸ������ �ʰ� ���� �� rŰ �Է¹���
				update_center_of_gravity(&pg);
				fprintf(stderr, "*** Rotation mode started!\n");
				temp = true;
			}
			st.rotation_mode = 1 - st.rotation_mode;

			if (st.rotation_mode) //ȸ������ �� rŰ �Է¹���
				glutTimerFunc(ROTATION_STEP, timer, 0);
			if (!temp) fprintf(stderr, "*** Rotation mode ended.\n");

		}
		break;
	case 'f':
		glutLeaveMainLoop();
		break;
	}
}


void special(int key, int x, int y) {
	if (st.rotation_mode || !st.polygon_mode) return;
	switch (key) {
	case GLUT_KEY_LEFT:
		move_points(&pg, -TRANSLATION_OFFSET, 0.0f);
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		move_points(&pg, TRANSLATION_OFFSET, 0.0f);
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		move_points(&pg, 0.0f, -TRANSLATION_OFFSET);
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		move_points(&pg, 0.0f, TRANSLATION_OFFSET);
		glutPostRedisplay();
		break;
	}
}


static int prev_x, prev_y;


void mousepress(int button, int state, int x, int y) {
	if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {  // ��Ŭ���� ������ ��
		int key_state = glutGetModifiers();
		if (key_state & GLUT_ACTIVE_CTRL) {  // CTRL+��Ŭ��
			if (!st.polygon_mode) {  // �ٰ��� ���� ��尡 �ƴ� ��
				fprintf(stderr, "*** Press 'P' to start polygon mode, then draw points.\n");
			}
			else {  // �ٰ��� ���� ����� ��
				add_point(&pg, &wd, x, y);  // �� �߰�
				glutPostRedisplay();
			}
		}
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {  // ��Ŭ�� ������ ��
		float norm_x = 2.0f * x / wd.width - 1.0f;  // x ��ǥ�� OpenGL�� �°� ��ȯ
		float norm_y = 1.0f - 2.0f * y / wd.height;  // y ��ǥ�� OpenGL�� �°� ��ȯ

		float dist = sqrt(pow(norm_x - pg.center_x, 2) + pow(norm_y - pg.center_y, 2));

		if (dist <= 0.03f) {  // �����߽��� Ŭ���� ���
			st.leftbuttonpressed = 1;
			prev_x = x;
			prev_y = y;
			fprintf(stderr, "*** Move mode started(Mouse clicked)\n");
		}
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {  // ��Ŭ���� ������ ��
		if (st.leftbuttonpressed) {
			st.leftbuttonpressed = 0;
			fprintf(stderr, "*** Move mode ended\n");
			glutPostRedisplay();
		}
	}
}



void mousemove(int x, int y) {
	if (st.leftbuttonpressed) {
		float delx, dely;
		delx = 2.0f * ((float)x - prev_x) / wd.width;
		dely = 2.0f * ((float)prev_y - y) / wd.height;
		prev_x = x, prev_y = y;
		move_points(&pg, delx, dely);
		glutPostRedisplay();
	}
}


// �ٰ����� �����߽��� �������� Ȯ��, ���
void zoom_polygon(My_Polygon* pg, float zoom_factor) {
	for (int i = 0; i < pg->n_points; i++) {
		pg->point[i][0] = pg->center_x + (pg->point[i][0] - pg->center_x) * zoom_factor;
		pg->point[i][1] = pg->center_y + (pg->point[i][1] - pg->center_y) * zoom_factor;
	}
	glutPostRedisplay();
}


void mouse_wheel(int wheel, int direction, int x, int y) {
	float zoom_factor = (direction < 0) ? 1.1f : 0.9f;
	zoom_polygon(&pg, zoom_factor);
}




void reshape(int width, int height) {
	fprintf(stdout, "### The new window size is %dx%d.\n", width, height);
	wd.width = width, wd.height = height;
	glViewport(0, 0, wd.width, wd.height);
}

void close(void) {
	fprintf(stdout, "\n^^^ The control is at the close callback function now.\n\n");
}
// End of GLUT callbacks

void initialize_polygon_editor(void) {
	wd.width = 800, wd.height = 600, wd.initial_anchor_x = 500, wd.initial_anchor_y = 200;
	st.rightbuttonpressed = 0, st.rotation_mode = 0, st.polygon_mode = 0;
	pg.n_points = 0; pg.center_x = 0.0f; pg.center_y = 0.0f;
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mousepress);
	glutMotionFunc(mousemove);
	glutReshapeFunc(reshape);
	glutCloseFunc(close);
	glutMouseWheelFunc(mouse_wheel);  // ���콺 �� �̺�Ʈ ó�� �߰�
}


void initialize_renderer(void) {
	register_callbacks();

	glPointSize(5.0);
	glClearColor(BACKGROUND_COLOR, 1.0f);
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = TRUE;
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

void greetings(char* program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 4
int main(int argc, char* argv[]) {
	char program_name[64] = "Sogang CSE4170 SimplefreeGLUTcode_Polygon_Editor";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'p', 'c', 'r', 'f'",
		"    - Special keys used: LEFT, RIGHT, UP, DOWN",
		"    - Mouse used: L-click, R-click and move",
		"    - Other operations: window reshape"
	};

	glutInit(&argc, argv);
	initialize_polygon_editor();

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE); // <-- Be sure to use this profile for this example code!
	//	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_RGBA);

	glutInitWindowSize(wd.width, wd.height);
	glutInitWindowPosition(wd.initial_anchor_x, wd.initial_anchor_y);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	// glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_EXIT); // default
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutMainLoop();
	fprintf(stdout, "^^^ The control is at the end of main function now.\n\n");
	return 0;
}