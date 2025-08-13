#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include <cstdlib>

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; 
GLint loc_ModelViewProjectionMatrix, loc_primitive_color;

#include <glm/gtc/matrix_transform.hpp> 
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewMatrix, ProjectionMatrix, ViewProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))
#define M_PI 3.14159265358979323846
#define LOC_VERTEX 0




int win_width = 0, win_height = 0;
float centerx = 0.0f, centery = 0.0f, rotate_angle = 0.0f;

GLfloat axes[4][2];
GLfloat axes_color[3] = { 0.313f, 0.453f, 0.680f };
GLuint VBO_axes, VAO_axes;

void prepare_axes(void) { 
	axes[0][0] = -win_width / 2.5f; axes[0][1] = 0.0f;
	axes[1][0] = win_width / 2.5f; axes[1][1] = 0.0f;
	axes[2][0] = 0.0f; axes[2][1] = -win_height / 2.5f;
	axes[3][0] = 0.0f; axes[3][1] = win_height / 2.5f;

	glGenBuffers(1, &VBO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO_axes);
	glBindVertexArray(VAO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_axes(void) {
	axes[0][0] = -win_width / 2.25f; axes[1][0] = win_width / 2.25f;
	axes[2][1] = -win_height / 2.25f;
	axes[3][1] = win_height / 2.25f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_axes(void) {
	glUniform3fv(loc_primitive_color, 1, axes_color);
	glBindVertexArray(VAO_axes);
	glDrawArrays(GL_LINES, 0, 4);
	glBindVertexArray(0);
}

GLfloat line[2][2];
GLfloat line_color[3] = { 1.0f, 0.0f, 0.0f };
GLuint VBO_line, VAO_line;

void prepare_line(void) { 
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f) * win_height;
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f) * win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f;
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	glGenBuffers(1, &VBO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO_line);
	glBindVertexArray(VAO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_line(void) { 	
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f) * win_height;
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f) * win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f;
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_line(void) {
	glUniform3fv(loc_primitive_color, 1, line_color);
	glBindVertexArray(VAO_line);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

//비행기 -------------------------------------------------------------------------
#define AIRPLANE_BIG_WING 0
#define AIRPLANE_SMALL_WING 1
#define AIRPLANE_BODY 2
#define AIRPLANE_BACK 3
#define AIRPLANE_SIDEWINDER1 4
#define AIRPLANE_SIDEWINDER2 5
#define AIRPLANE_CENTER 6

float airplane_scale = 1.0f;
float airplane_center_x = -500.0f, airplane_center_y = 300.0f;


GLfloat big_wing[6][2] = { { 0.0, 0.0 },{ -20.0, 15.0 },{ -20.0, 20.0 },{ 0.0, 23.0 },{ 20.0, 20.0 },{ 20.0, 15.0 } };
GLfloat small_wing[6][2] = { { 0.0, -18.0 },{ -11.0, -12.0 },{ -12.0, -7.0 },{ 0.0, -10.0 },{ 12.0, -7.0 },{ 11.0, -12.0 } };
GLfloat body[5][2] = { { 0.0, -25.0 },{ -6.0, 0.0 },{ -6.0, 22.0 },{ 6.0, 22.0 },{ 6.0, 0.0 } };
GLfloat back[5][2] = { { 0.0, 25.0 },{ -7.0, 24.0 },{ -7.0, 21.0 },{ 7.0, 21.0 },{ 7.0, 24.0 } };
GLfloat sidewinder1[5][2] = { { -20.0, 10.0 },{ -18.0, 3.0 },{ -16.0, 10.0 },{ -18.0, 20.0 },{ -20.0, 20.0 } };
GLfloat sidewinder2[5][2] = { { 20.0, 10.0 },{ 18.0, 3.0 },{ 16.0, 10.0 },{ 18.0, 20.0 },{ 20.0, 20.0 } };
GLfloat center[1][2] = { { 0.0, 0.0 } };

GLfloat airplane_color[7][3] = {
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // big_wing
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // small_wing
	{ 111 / 255.0f,  85 / 255.0f, 157 / 255.0f },  // body
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // back
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder1
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder2
	{ 255 / 255.0f,   0 / 255.0f,   0 / 255.0f }   // center
};

GLuint VBO_airplane, VAO_airplane;

int airplane_clock = 0;
float airplane_s_factor = 1.0f;

void prepare_airplane() {
	GLsizeiptr buffer_size = sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1) + sizeof(sidewinder2) + sizeof(center);
	glGenBuffers(1, &VBO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); 

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(big_wing), big_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing), sizeof(small_wing), small_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing), sizeof(body), body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body), sizeof(back), back);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back),
		sizeof(sidewinder1), sidewinder1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1), sizeof(sidewinder2), sidewinder2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1) + sizeof(sidewinder2), sizeof(center), center);

	glGenVertexArrays(1, &VAO_airplane);
	glBindVertexArray(VAO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_airplane() { 
	glBindVertexArray(VAO_airplane);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BIG_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SMALL_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BACK]);
	glDrawArrays(GL_TRIANGLE_FAN, 17, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER1]);
	glDrawArrays(GL_TRIANGLE_FAN, 22, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER2]);
	glDrawArrays(GL_TRIANGLE_FAN, 27, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_CENTER]);
	glPointSize(5.0);
	glDrawArrays(GL_POINTS, 32, 1);
	glPointSize(1.0);
	glBindVertexArray(0);
}


//집 -------------------------------------------------------------------------

#define HOUSE_ROOF 0
#define HOUSE_BODY 1
#define HOUSE_CHIMNEY 2
#define HOUSE_DOOR 3
#define HOUSE_WINDOW 4

float house_angle = 0.0f;	// 공전 각도 (time에 따라 증가)
float house_radius = 100.0f;	// 궤적 반지름

GLfloat roof[3][2] = { { -12.0, 0.0 },{ 0.0, 12.0 },{ 12.0, 0.0 } };
GLfloat house_body[4][2] = { { -12.0, -14.0 },{ -12.0, 0.0 },{ 12.0, 0.0 },{ 12.0, -14.0 } };
GLfloat chimney[4][2] = { { 6.0, 6.0 },{ 6.0, 14.0 },{ 10.0, 14.0 },{ 10.0, 2.0 } };
GLfloat door[4][2] = { { -8.0, -14.0 },{ -8.0, -8.0 },{ -4.0, -8.0 },{ -4.0, -14.0 } };
GLfloat window[4][2] = { { 4.0, -6.0 },{ 4.0, -2.0 },{ 8.0, -2.0 },{ 8.0, -6.0 } };

GLfloat house_color[5][3] = {
	{ 200 / 255.0f, 39 / 255.0f, 42 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 233 / 255.0f, 113 / 255.0f, 23 / 255.0f },
	{ 44 / 255.0f, 180 / 255.0f, 49 / 255.0f }
};



GLuint VBO_house, VAO_house;
void prepare_house() {
	GLsizeiptr buffer_size = sizeof(roof) + sizeof(house_body) + sizeof(chimney) + sizeof(door)
		+ sizeof(window);

	glGenBuffers(1, &VBO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); 

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(roof), roof);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof), sizeof(house_body), house_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body), sizeof(chimney), chimney);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body) + sizeof(chimney), sizeof(door), door);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body) + sizeof(chimney) + sizeof(door),
		sizeof(window), window);

	glGenVertexArrays(1, &VAO_house);
	glBindVertexArray(VAO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_house() {
	glBindVertexArray(VAO_house);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_ROOF]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 3, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_CHIMNEY]);
	glDrawArrays(GL_TRIANGLE_FAN, 7, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_DOOR]);
	glDrawArrays(GL_TRIANGLE_FAN, 11, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glBindVertexArray(0);
}


//자동차 -------------------------------------------------------------------------

#define CAR_BODY 0
#define CAR_FRAME 1
#define CAR_WINDOW 2
#define CAR_LEFT_LIGHT 3
#define CAR_RIGHT_LIGHT 4
#define CAR_LEFT_WHEEL 5
#define CAR_RIGHT_WHEEL 6

float car_scale = 1.0f;
float car_center_x = -300.0f, car_center_y = 300.0f;


GLfloat car_body[4][2] = { { -16.0, -8.0 },{ -16.0, 0.0 },{ 16.0, 0.0 },{ 16.0, -8.0 } };
GLfloat car_frame[4][2] = { { -10.0, 0.0 },{ -10.0, 10.0 },{ 10.0, 10.0 },{ 10.0, 0.0 } };
GLfloat car_window[4][2] = { { -8.0, 0.0 },{ -8.0, 8.0 },{ 8.0, 8.0 },{ 8.0, 0.0 } };
GLfloat car_left_light[4][2] = { { -9.0, -6.0 },{ -10.0, -5.0 },{ -9.0, -4.0 },{ -8.0, -5.0 } };
GLfloat car_right_light[4][2] = { { 9.0, -6.0 },{ 8.0, -5.0 },{ 9.0, -4.0 },{ 10.0, -5.0 } };
GLfloat car_left_wheel[4][2] = { { -10.0, -12.0 },{ -10.0, -8.0 },{ -6.0, -8.0 },{ -6.0, -12.0 } };
GLfloat car_right_wheel[4][2] = { { 6.0, -12.0 },{ 6.0, -8.0 },{ 10.0, -8.0 },{ 10.0, -12.0 } };

GLfloat car_color[7][3] = {
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 216 / 255.0f, 208 / 255.0f, 174 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f }
};

GLuint VBO_car, VAO_car;
void prepare_car() {
	GLsizeiptr buffer_size = sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light) + sizeof(car_left_wheel) + sizeof(car_right_wheel);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); 

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car_body), car_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body), sizeof(car_frame), car_frame);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame), sizeof(car_window), car_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window), sizeof(car_left_light), car_left_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light),
		sizeof(car_right_light), car_right_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light), sizeof(car_left_wheel), car_left_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light) + sizeof(car_left_wheel), sizeof(car_right_wheel), car_right_wheel);

	glGenVertexArrays(1, &VAO_car);
	glBindVertexArray(VAO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_car() {
	glBindVertexArray(VAO_car);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_FRAME]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_LEFT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_RIGHT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_LEFT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_RIGHT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 4);

	glBindVertexArray(0);
}


//칵테일 -------------------------------------------------------------------------

#define COCKTAIL_NECK 0
#define COCKTAIL_LIQUID 1
#define COCKTAIL_REMAIN 2
#define COCKTAIL_STRAW 3
#define COCKTAIL_DECO 4

GLfloat neck[6][2] = { { -6.0, -12.0 },{ -6.0, -11.0 },{ -1.0, 0.0 },{ 1.0, 0.0 },{ 6.0, -11.0 },{ 6.0, -12.0 } };
GLfloat liquid[6][2] = { { -1.0, 0.0 },{ -9.0, 4.0 },{ -12.0, 7.0 },{ 12.0, 7.0 },{ 9.0, 4.0 },{ 1.0, 0.0 } };
GLfloat remain[4][2] = { { -12.0, 7.0 },{ -12.0, 10.0 },{ 12.0, 10.0 },{ 12.0, 7.0 } };
GLfloat straw[4][2] = { { 7.0, 7.0 },{ 12.0, 12.0 },{ 14.0, 12.0 },{ 9.0, 7.0 } };
GLfloat deco[8][2] = { { 12.0, 12.0 },{ 10.0, 14.0 },{ 10.0, 16.0 },{ 12.0, 18.0 },{ 14.0, 18.0 },{ 16.0, 16.0 },{ 16.0, 14.0 },{ 14.0, 12.0 } };

#define COCKTAIL_NECK_START     0
#define COCKTAIL_NECK_COUNT     6
#define COCKTAIL_LIQUID_START   6
#define COCKTAIL_LIQUID_COUNT   6
#define COCKTAIL_REMAIN_START   12
#define COCKTAIL_REMAIN_COUNT   4
#define COCKTAIL_STRAW_START    16
#define COCKTAIL_STRAW_COUNT    4
#define COCKTAIL_DECO_START     20
#define COCKTAIL_DECO_COUNT     8

GLfloat cocktail_color[5][3] = {
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 0 / 255.0f, 63 / 255.0f, 122 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 191 / 255.0f, 255 / 255.0f, 0 / 255.0f },
	{ 218 / 255.0f, 165 / 255.0f, 32 / 255.0f }
};

GLuint VBO_cocktail, VAO_cocktail;
void prepare_cocktail() {
	GLsizeiptr buffer_size = sizeof(neck) + sizeof(liquid) + sizeof(remain) + sizeof(straw)
		+ sizeof(deco);

	glGenBuffers(1, &VBO_cocktail);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cocktail);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); 

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(neck), neck);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck), sizeof(liquid), liquid);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid), sizeof(remain), remain);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid) + sizeof(remain), sizeof(straw), straw);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid) + sizeof(remain) + sizeof(straw),
		sizeof(deco), deco);

	glGenVertexArrays(1, &VAO_cocktail);
	glBindVertexArray(VAO_cocktail);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cocktail);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_cocktail() {
	glm::mat4 ModelMatrix;

	// 전체 위치 및 스케일
	glm::vec3 base_pos = glm::vec3(300.0f, 300.0f, 0.0f);
	ModelMatrix = glm::translate(glm::mat4(1.0f), base_pos);
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.5f, 3.5f, 1.0f)); 

	glBindVertexArray(VAO_cocktail);
	float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

	// neck
	glm::mat4 part = ModelMatrix;
	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_NECK]);
	ModelViewProjectionMatrix = ViewProjectionMatrix * part;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glDrawArrays(GL_TRIANGLE_FAN, COCKTAIL_NECK_START, COCKTAIL_NECK_COUNT);

	// liquid
	float liquid_offset = sin(t * 1.5f) * 3.0f - 1.0f;  
	part = glm::translate(ModelMatrix, glm::vec3(0.0f, 5.0f + liquid_offset, 0.0f));
	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_LIQUID]);
	ModelViewProjectionMatrix = ViewProjectionMatrix * part;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glDrawArrays(GL_TRIANGLE_FAN, COCKTAIL_LIQUID_START, COCKTAIL_LIQUID_COUNT);

	// remain
	part = glm::translate(ModelMatrix, glm::vec3(0.0f, 7.0f, 0.0f));
	part = glm::rotate(part, sin(t * 1.5f) * 0.2f, glm::vec3(0.0f, 0.0f, 1.0f));
	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_REMAIN]);
	ModelViewProjectionMatrix = ViewProjectionMatrix * part;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glDrawArrays(GL_TRIANGLE_FAN, COCKTAIL_REMAIN_START, COCKTAIL_REMAIN_COUNT);

	// straw
	float straw_offset = -5.0f + sin(t * 2.0f) * 4.0f;
	part = glm::translate(ModelMatrix, glm::vec3(straw_offset, 2.0f, 0.0f));
	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_STRAW]);
	ModelViewProjectionMatrix = ViewProjectionMatrix * part;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glDrawArrays(GL_TRIANGLE_FAN, COCKTAIL_STRAW_START, COCKTAIL_STRAW_COUNT);

	// deco
	float deco_scale = 1.0f + 0.2f * sin(t * 4.0f);  
	part = glm::translate(ModelMatrix, glm::vec3(12.0f, 15.0f, 0.0f));
	part = glm::scale(part, glm::vec3(deco_scale, deco_scale, 1.0f));
	part = glm::translate(part, glm::vec3(-12.0f, -15.0f, 0.0f));
	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_DECO]);
	ModelViewProjectionMatrix = ViewProjectionMatrix * part;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glDrawArrays(GL_TRIANGLE_FAN, COCKTAIL_DECO_START, COCKTAIL_DECO_COUNT);

	glBindVertexArray(0);
}




//검 -------------------------------------------------------------------------

#define SWORD_BODY 0
#define SWORD_BODY2 1
#define SWORD_HEAD 2
#define SWORD_HEAD2 3
#define SWORD_IN 4
#define SWORD_DOWN 5
#define SWORD_BODY_IN 6

float sword_angle = 0.0f;
float sword_radius = 300.0f;


GLfloat sword_body[4][2] = { { -6.0, 0.0 },{ -6.0, -4.0 },{ 6.0, -4.0 },{ 6.0, 0.0 } };
GLfloat sword_body2[4][2] = { { -2.0, -4.0 },{ -2.0, -6.0 } ,{ 2.0, -6.0 },{ 2.0, -4.0 } };
GLfloat sword_head[4][2] = { { -2.0, 0.0 },{ -2.0, 16.0 } ,{ 2.0, 16.0 },{ 2.0, 0.0 } };
GLfloat sword_head2[3][2] = { { -2.0, 16.0 },{ 0.0, 19.46 } ,{ 2.0, 16.0 } };
GLfloat sword_in[4][2] = { { -0.3, 0.7 },{ -0.3, 15.3 } ,{ 0.3, 15.3 },{ 0.3, 0.7 } };
GLfloat sword_down[4][2] = { { -2.0, -6.0 } ,{ 2.0, -6.0 },{ 4.0, -8.0 },{ -4.0, -8.0 } };
GLfloat sword_body_in[4][2] = { { 0.0, -1.0 } ,{ 1.0, -2.732 },{ 0.0, -4.464 },{ -1.0, -2.732 } };

GLfloat sword_color[7][3] = {
	{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 155 / 255.0f, 155 / 255.0f, 155 / 255.0f },
{ 155 / 255.0f, 155 / 255.0f, 155 / 255.0f },
{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f }
};

GLuint VBO_sword, VAO_sword;

void prepare_sword() {
	GLsizeiptr buffer_size = sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in) + sizeof(sword_down) + sizeof(sword_body_in);

	glGenBuffers(1, &VBO_sword);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sword);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sword_body), sword_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body), sizeof(sword_body2), sword_body2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2), sizeof(sword_head), sword_head);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head), sizeof(sword_head2), sword_head2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2), sizeof(sword_in), sword_in);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in), sizeof(sword_down), sword_down);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in) + sizeof(sword_down), sizeof(sword_body_in), sword_body_in);

	glGenVertexArrays(1, &VAO_sword);
	glBindVertexArray(VAO_sword);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sword);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_sword() {
	glBindVertexArray(VAO_sword);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY2]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_HEAD]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_HEAD2]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 3);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_DOWN]);
	glDrawArrays(GL_TRIANGLE_FAN, 19, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 23, 4);

	glBindVertexArray(0);
}



//물고기(추가구현 1) -------------------------------------------------------------------------

#define FISH_BODY 0
#define FISH_TAIL 1
#define FISH_EYE_WHITE 2
#define FISH_EYE_BLACK 3
#define FISH_FIN_BOTTOM 4
#define FISH_FIN_TOP 5
#define FISH_BUBBLE1 6
#define FISH_BUBBLE2 7
#define FISH_BUBBLE3 8

float fish_x = -200.0f;
float fish_y = -200.0f;
float fish_scale = 2.5f;

float fish_dx = 2.0f;  // 이동 속도
float fish_direction = 1.0f;  // 오른쪽 이동 중이면 1, 왼쪽이면 -1
float fish_angle = 0.0f;  // 이동 곡선 경로
float fish_tail_angle = 0.0f;  // 꼬리 회전 각도
bool fish_facing_left = false;


GLfloat fish_tail[3][2] = { {0.0f, 30.0f}, {-30.0f, 50.0f}, {-30.0f, 10.0f} };
GLfloat eye_white[20][2];
GLfloat eye_black[20][2];
GLfloat fin_bottom[3][2] = { {40.0f, 10.0f}, {30.0f, -10.0f}, {50.0f, -10.0f} };
GLfloat fin_top[3][2] = { {50.0f, 70.0f}, {40.0f, 50.0f}, {60.0f, 50.0f} };
GLfloat fish_body[20][2]; 
GLfloat bubble1[20][2];
GLfloat bubble2[20][2];
GLfloat bubble3[20][2];

GLfloat fish_color[][3] = {
	{1.0f, 0.5f, 0.3f}, // body
	{1.0f, 0.4f, 0.2f}, // tail
	{1.0f, 1.0f, 1.0f}, // eye white
	{0.0f, 0.0f, 0.0f}, // eye black
	{1.0f, 0.6f, 0.2f}, // fin bottom
	{1.0f, 0.6f, 0.2f}, // fin top
	{0.7f, 0.9f, 1.0f}, // bubble1
	{0.7f, 0.9f, 1.0f}, // bubble2
	{0.7f, 0.9f, 1.0f}  // bubble3
};

GLuint VBO_fish, VAO_fish;


struct Bubble {
	float x, y;
	float lifetime;
	float radius;
	bool from_mermaid = false;

};

std::vector<Bubble> bubbles;
const float BUBBLE_SPEED = 30.0f;
const float BUBBLE_LIFETIME = 2.0f;
GLfloat white[3] = { 1.0f, 1.0f, 1.0f };
GLfloat pink[3] = { 1.0f, 0.7f, 0.8f };

void prepare_fish() {
	const int n_circle_pts = 20;
	float theta;

	// Body
	for (int i = 0; i < n_circle_pts; ++i) {
		theta = 2.0f * M_PI * i / n_circle_pts;
		fish_body[i][0] = 50.0f + 50.0f * cos(theta); 
		fish_body[i][1] = 30.0f + 20.0f * sin(theta);
	}

	// Eye white
	for (int i = 0; i < n_circle_pts; ++i) {
		theta = 2.0f * M_PI * i / n_circle_pts;
		eye_white[i][0] = 80.0f + 5.0f * cos(theta);
		eye_white[i][1] = 40.0f + 5.0f * sin(theta);
	}

	// Eye black
	for (int i = 0; i < n_circle_pts; ++i) {
		theta = 2.0f * M_PI * i / n_circle_pts;
		eye_black[i][0] = 80.0f + 2.0f * cos(theta);
		eye_black[i][1] = 40.0f + 2.0f * sin(theta);
	}

	// Bubbles 
	for (int i = 0; i < n_circle_pts; ++i) {
		theta = 2.0f * M_PI * i / n_circle_pts;
		float r = 10.0f;  

		bubble1[i][0] = 0.0f + r * cos(theta); 
		bubble1[i][1] = 0.0f + r * sin(theta);
	}

	glGenBuffers(1, &VBO_fish);
	glGenVertexArrays(1, &VAO_fish);
	glBindVertexArray(VAO_fish);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_fish);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fish_body) + sizeof(fish_tail) + sizeof(eye_white) +
		sizeof(eye_black) + sizeof(fin_bottom) + sizeof(fin_top) +
		sizeof(bubble1) + sizeof(bubble2) + sizeof(bubble3), NULL, GL_STATIC_DRAW);

	size_t offset = 0;
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(fish_body), fish_body); offset += sizeof(fish_body);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(fish_tail), fish_tail); offset += sizeof(fish_tail);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(eye_white), eye_white); offset += sizeof(eye_white);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(eye_black), eye_black); offset += sizeof(eye_black);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(fin_bottom), fin_bottom); offset += sizeof(fin_bottom);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(fin_top), fin_top); offset += sizeof(fin_top);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(bubble1), bubble1); offset += sizeof(bubble1);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(bubble2), bubble2); offset += sizeof(bubble2);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(bubble3), bubble3);

	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_fish() {
	float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

	// fish_direction에 따라 좌우반전된 스케일 적용
	glm::mat4 ModelMatrix = glm::mat4(1.0f);
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(fish_x, fish_y, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(fish_direction * fish_scale, fish_scale, 1.0f)); 
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_fish);

	// Body
	glUniform3fv(loc_primitive_color, 1, fish_color[FISH_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 20);

	// Tail
	glm::mat4 tailModel = ModelMatrix;
	tailModel = glm::translate(tailModel, glm::vec3(0.0f, 30.0f, 0.0f));
	tailModel = glm::rotate(tailModel, sin(t) * TO_RADIAN * 30.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	tailModel = glm::translate(tailModel, glm::vec3(0.0f, -30.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * tailModel;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3fv(loc_primitive_color, 1, fish_color[FISH_TAIL]);
	glDrawArrays(GL_TRIANGLES, 20, 3);

	// Eye white
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3fv(loc_primitive_color, 1, fish_color[FISH_EYE_WHITE]);
	glDrawArrays(GL_TRIANGLE_FAN, 23, 20);

	// Eye black
	glUniform3fv(loc_primitive_color, 1, fish_color[FISH_EYE_BLACK]);
	glDrawArrays(GL_TRIANGLE_FAN, 43, 20);

	// Fin bottom
	glUniform3fv(loc_primitive_color, 1, fish_color[FISH_FIN_BOTTOM]);
	glDrawArrays(GL_TRIANGLES, 63, 3);

	// Fin top
	glUniform3fv(loc_primitive_color, 1, fish_color[FISH_FIN_TOP]);
	glDrawArrays(GL_TRIANGLES, 66, 3);

	// 동적으로 생성된 버블들
	for (const auto& b : bubbles) {
		glm::mat4 bubbleModel = glm::translate(glm::mat4(1.0f), glm::vec3(b.x, b.y, 0.0f));
		bubbleModel = glm::scale(bubbleModel, glm::vec3(b.radius, b.radius, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * bubbleModel;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		if (b.from_mermaid)
			glUniform3fv(loc_primitive_color, 1, pink);
		else
			glUniform3fv(loc_primitive_color, 1, white);
		glDrawArrays(GL_TRIANGLE_FAN, 69, 20);
	}

	glBindVertexArray(0);
}




//인어공주(추가구현 2) -------------------------------------------------------------------------

GLuint VBO_mermaid, VAO_mermaid;
const float MERMAID_BUBBLE_MAX_RADIUS = 2.0f;
const float MERMAID_BUBBLE_MIN_RADIUS = 0.5f;


enum {
	MERMAID_HEAD, MERMAID_HAIR,
	MERMAID_BODY, 
	MERMAID_ARM_L, MERMAID_ARM_R,
	MERMAID_HAND_L, MERMAID_HAND_R,
	MERMAID_TAIL1, MERMAID_TAIL2,
	MERMAID_FIN_L, MERMAID_FIN_R
};


GLfloat mermaid_color[][3] = {
	{1.0f, 0.8f, 0.6f},   // MERMAID_HEAD      
	{0.9f, 0.2f, 0.2f},   // MERMAID_HAIR   
	{0.8f, 0.6f, 1.0f},   // MERMAID_BODY       
	{1.0f, 0.8f, 0.6f},   // MERMAID_ARM_L      
	{1.0f, 0.8f, 0.6f},   // MERMAID_ARM_R     
	{1.0f, 0.8f, 0.6f},   // MERMAID_HAND_L   
	{1.0f, 0.8f, 0.6f},   // MERMAID_HAND_R    
	{0.0f, 0.6f, 0.6f},   // MERMAID_TAIL1     
	{0.0f, 0.6f, 0.6f},   // MERMAID_TAIL2     
	{0.5f, 0.9f, 0.8f},   // MERMAID_FIN_L      
	{0.5f, 0.9f, 0.8f},   // MERMAID_FIN_R     
};


void prepare_mermaid() {
	GLfloat head[20][2], hand_l[20][2], hand_r[20][2];
	const int n = 20;
	for (int i = 0; i < n; ++i) {
		float theta = 2.0f * M_PI * i / n;
		head[i][0] = 0.0f + 20.0f * cos(theta);
		head[i][1] = 100.0f + 20.0f * sin(theta);

		hand_l[i][0] = -72.0f + 8.0f * cos(theta);
		hand_l[i][1] = 76.0f + 8.0f * sin(theta);
		hand_r[i][0] = 72.0f + 8.0f * cos(theta);
		hand_r[i][1] = 76.0f + 8.0f * sin(theta);
	}

	GLfloat vertices[] = {
		// 뒷머리
		-17.0f, 125.0f,
		 17.0f, 125.0f,
		 30.0f, 10.0f,
		-30.0f, 10.0f,

		// 앞머리
		0.0f, 120.0f,   -20.0f, 120.0f,   -35.0f, 80.0f,
		0.0f, 120.0f,    20.0f, 120.0f,    35.0f, 80.0f,

		// 팔
		-15.0f, 75.0f,  -5.0f, 75.0f,  -70.0f, 40.0f,  -80.0f, 40.0f,
		 15.0f, 75.0f,   5.0f, 75.0f,   70.0f, 40.0f,   80.0f, 40.0f,

		 // 몸통
		 -10.0f, 80.0f,  10.0f, 80.0f,  20.0f, 30.0f,  -20.0f, 30.0f,

		 // 꼬리, 지느러미
		 -20.0f, 30.0f,  20.0f, 30.0f,  30.0f, -30.0f, -30.0f, -30.0f,
		 -30.0f, -30.0f, 30.0f, -30.0f, 0.0f, -80.0f,
		 0.0f, -80.0f,  -20.0f, -100.0f, -10.0f, -120.0f,
		 0.0f, -80.0f,   20.0f, -100.0f,  10.0f, -120.0f
	};

	GLsizeiptr total_size = sizeof(vertices) + sizeof(head) + sizeof(hand_l) + sizeof(hand_r);
	glGenBuffers(1, &VBO_mermaid);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_mermaid);
	glBufferData(GL_ARRAY_BUFFER, total_size, NULL, GL_STATIC_DRAW);

	size_t offset = 0;
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices); offset += sizeof(vertices);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(head), head); offset += sizeof(head);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(hand_l), hand_l); offset += sizeof(hand_l);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(hand_r), hand_r);

	glGenVertexArrays(1, &VAO_mermaid);
	glBindVertexArray(VAO_mermaid);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_mermaid);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}




void draw_mermaid(glm::mat4 baseMatrix) {
	glBindVertexArray(VAO_mermaid);

	float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float hair_angle = sin(t * 2.0f) * 30.0f * TO_RADIAN;
	float tail_angle = sin(t * 2.5f) * 15.0f * TO_RADIAN;
	float arm_angle = sin(t * 3.0f) * 20.0f * TO_RADIAN;
	float fin_angle = sin(t * 3.5f) * 10.0f * TO_RADIAN;

	glm::mat4 model;
	int idx = 0;

	// 뒷머리
	model = glm::translate(baseMatrix, glm::vec3(0.0f, 125.0f, 0.0f));
	model = glm::rotate(model, hair_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(0.0f, -125.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * model;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3fv(loc_primitive_color, 1, mermaid_color[MERMAID_HAIR]);
	glDrawArrays(GL_TRIANGLE_FAN, idx, 4); idx += 4;

	int hair_start = idx;
	idx += 6;

	// 왼팔
	glm::mat4 armModelL = glm::translate(baseMatrix, glm::vec3(-15.0f, 75.0f, 0.0f));
	armModelL = glm::rotate(armModelL, arm_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	armModelL = glm::translate(armModelL, glm::vec3(15.0f, -75.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * armModelL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3fv(loc_primitive_color, 1, mermaid_color[MERMAID_ARM_L]);
	glDrawArrays(GL_TRIANGLE_FAN, idx, 4); idx += 4;

	// 오른팔
	glm::mat4 armModelR = glm::translate(baseMatrix, glm::vec3(15.0f, 75.0f, 0.0f));
	armModelR = glm::rotate(armModelR, -arm_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	armModelR = glm::translate(armModelR, glm::vec3(-15.0f, -75.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * armModelR;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3fv(loc_primitive_color, 1, mermaid_color[MERMAID_ARM_R]);
	glDrawArrays(GL_TRIANGLE_FAN, idx, 4); idx += 4;

	// 몸통
	ModelViewProjectionMatrix = ViewProjectionMatrix * baseMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3fv(loc_primitive_color, 1, mermaid_color[MERMAID_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, idx, 4); idx += 4;

	// 꼬리1
	model = glm::translate(baseMatrix, glm::vec3(0.0f, 30.0f, 0.0f));
	model = glm::rotate(model, tail_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(0.0f, -30.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * model;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3fv(loc_primitive_color, 1, mermaid_color[MERMAID_TAIL1]);
	glDrawArrays(GL_TRIANGLE_FAN, idx, 4); idx += 4;

	// 꼬리2
	model = glm::translate(model, glm::vec3(0.0f, -30.0f, 0.0f));
	model = glm::rotate(model, tail_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(0.0f, 30.0f, 0.0f));
	glm::mat4 tail2Matrix = model;
	ModelViewProjectionMatrix = ViewProjectionMatrix * tail2Matrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3fv(loc_primitive_color, 1, mermaid_color[MERMAID_TAIL2]);
	glDrawArrays(GL_TRIANGLES, idx, 3); idx += 3;

	// 지느러미 왼쪽
	model = glm::translate(tail2Matrix, glm::vec3(0.0f, -80.0f, 0.0f));
	model = glm::rotate(model, fin_angle, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(0.0f, 80.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * model;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3fv(loc_primitive_color, 1, mermaid_color[MERMAID_FIN_L]);
	glDrawArrays(GL_TRIANGLES, idx, 3); idx += 3;

	// 지느러미 오른쪽
	model = glm::translate(tail2Matrix, glm::vec3(0.0f, -80.0f, 0.0f));
	model = glm::rotate(model, -fin_angle, glm::vec3(0, 0, 1));
	model = glm::translate(model, glm::vec3(0.0f, 80.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * model;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3fv(loc_primitive_color, 1, mermaid_color[MERMAID_FIN_R]);
	glDrawArrays(GL_TRIANGLES, idx, 3); idx += 3;

	// 얼굴
	ModelViewProjectionMatrix = ViewProjectionMatrix * baseMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3fv(loc_primitive_color, 1, mermaid_color[MERMAID_HEAD]);
	glDrawArrays(GL_TRIANGLE_FAN, idx, 20); idx += 20;

	// 손
	glm::mat4 handModelL = glm::translate(armModelL, glm::vec3(-5.0f, -40.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * handModelL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3fv(loc_primitive_color, 1, mermaid_color[MERMAID_HAND_L]);
	glDrawArrays(GL_TRIANGLE_FAN, idx, 20); idx += 20;

	glm::mat4 handModelR = glm::translate(armModelR, glm::vec3(5.0f, -40.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * handModelR;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3fv(loc_primitive_color, 1, mermaid_color[MERMAID_HAND_R]);
	glDrawArrays(GL_TRIANGLE_FAN, idx, 20); idx += 20;

	// 앞머리
	model = baseMatrix;
	ModelViewProjectionMatrix = ViewProjectionMatrix * model;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3fv(loc_primitive_color, 1, mermaid_color[MERMAID_HAIR]);
	glDrawArrays(GL_TRIANGLES, hair_start, 3);
	glDrawArrays(GL_TRIANGLES, hair_start + 3, 3);

	glBindVertexArray(0);
}




enum DragTarget { NONE, AIRPLANE, CAR };
DragTarget current_drag_target = NONE;

float drag_offset_x = 0.0f, drag_offset_y = 0.0f;
bool is_dragging = false;







void timer(int value) {
	int time = glutGet(GLUT_ELAPSED_TIME);

	// 비행기
	airplane_scale = 2.0f + 0.9f * sin(2.0f * M_PI * time / 2000.0f);

	// 자동차
	car_scale = 1.5f + 0.6f * sin(2.0f * M_PI * time / 6000.0f);

	// 집
	house_angle += 0.02f;
	if (house_angle > 2.0f * M_PI)
		house_angle -= 2.0f * M_PI;

	// 칼
	sword_angle += 0.06f;
	if (sword_angle > 2.0f * M_PI)
		sword_angle -= 2.0f * M_PI;

	// 물고기 움직임
	fish_angle += 0.05f;
	if (fish_angle > 2.0f * M_PI)
		fish_angle -= 2.0f * M_PI;

	fish_x += fish_dx * fish_direction;
	fish_y = -200.0f + 40.0f * sin(fish_angle);  // 물결 경로

	// 화면 경계에서 반전
	if (win_width > 0) {
		if (fish_x > win_width / 2.0f - 100.0f) {
			fish_direction = -1.0f;
			fish_facing_left = true;
		}
		else if (fish_x < -win_width / 2.0f + 100.0f) {
			fish_direction = 1.0f;
			fish_facing_left = false;
		}
	}

	// 버블 이동
	for (auto& b : bubbles) {
		b.y += 1.0f;
	}

	// 버블 삭제 조건
	bubbles.erase(std::remove_if(bubbles.begin(), bubbles.end(),
		[](const Bubble& b) { return b.y > 600.0f; }),
		bubbles.end());

	glutPostRedisplay();
	glutTimerFunc(16, timer, 0);  
}





void display(void) {
	glm::mat4 ModelMatrix;

	glClear(GL_COLOR_BUFFER_BIT);

	// 좌표축
	ModelMatrix = glm::mat4(1.0f);
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_axes();

	// 비행기
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_center_x, airplane_center_y, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(airplane_scale, airplane_scale, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_airplane();

	// 집 
	float house_x = house_radius * cos(house_angle);
	float house_y = house_radius * sin(house_angle);
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(house_x, house_y, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, house_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_house();

	// 자동차
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(car_center_x, car_center_y, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(car_scale, car_scale, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_car();

	// 칵테일
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(300.0f, 0.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_cocktail();

	// 검
	glm::mat4 swordModel = glm::translate(glm::mat4(1.0f), glm::vec3(house_x, house_y, 0.0f));  // 집 위치 기준
	swordModel = glm::rotate(swordModel, sword_angle * 3.0f, glm::vec3(0.0f, 0.0f, 1.0f));      // 집 중심 회전
	swordModel = glm::translate(swordModel, glm::vec3(0.0f, 60.0f, 0.0f));                      // 거리 띄우기
	swordModel = glm::scale(swordModel, glm::vec3(3.5f, 3.5f, 1.0f));                           // 크기 조정

	ModelViewProjectionMatrix = ViewProjectionMatrix * swordModel;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_sword();

	// 물고기
	ModelMatrix = glm::mat4(1.0f);
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(fish_x, fish_y, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(fish_scale, fish_scale, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_fish();


	//인어
	float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float angle = t * 0.5f;  
	glm::mat4 base = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 0, 1));
	base = glm::translate(base, glm::vec3(250.0f, 0.0f, 0.0f)); 
	draw_mermaid(base);

	glFlush();
}


void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); 
		break;
	}
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			float mouse_x = (float)(x - win_width / 2);
			float mouse_y = (float)(win_height / 2 - y);

			// 비행기 클릭 감지
			float dx = mouse_x - airplane_center_x;
			float dy = mouse_y - airplane_center_y;
			if (sqrt((dx * dx + dy * dy)) < 50.0f * airplane_scale) {
				current_drag_target = AIRPLANE;
				drag_offset_x = dx;
				drag_offset_y = dy;
				is_dragging = true;
				return;
			}

			// 자동차 클릭 감지
			dx = mouse_x - car_center_x;
			dy = mouse_y - car_center_y;
			if (sqrt((dx * dx + dy * dy)) < 50.0f * car_scale) {
				current_drag_target = CAR;
				drag_offset_x = dx;
				drag_offset_y = dy;
				is_dragging = true;
				return;
			}

			// 물고기 클릭 → 버블 생성
			dx = mouse_x - fish_x;
			dy = mouse_y - fish_y;
			if (sqrt((dx * dx + dy * dy)) < 70.0f * fish_scale) {
				Bubble new_bubble;
				new_bubble.x = fish_x + fish_scale * 50.0f;
				new_bubble.y = fish_y + fish_scale * 70.0f;
				new_bubble.radius = 1.0f + static_cast<float>(rand() % 4);
				bubbles.push_back(new_bubble);
				return;
			}


			// 인어공주 클릭 → 연분홍 버블 생성
			glm::mat4 base = glm::rotate(glm::mat4(1.0f), glutGet(GLUT_ELAPSED_TIME) / 1000.0f * 0.5f, glm::vec3(0, 0, 1));
			base = glm::translate(base, glm::vec3(250.0f, 0.0f, 0.0f));
			glm::vec4 mermaid_world = base * glm::vec4(0.0f, 30.0f, 0.0f, 1.0f);
			dx = mouse_x - mermaid_world.x;
			dy = mouse_y - mermaid_world.y;
			if (sqrt(dx * dx + dy * dy) < 70.0f) {
				Bubble bubble;
				bubble.x = mermaid_world.x + 30.0f;
				bubble.y = mermaid_world.y + 70.0f;
				bubble.radius = MERMAID_BUBBLE_MIN_RADIUS + static_cast<float>(rand()) / RAND_MAX * (MERMAID_BUBBLE_MAX_RADIUS - MERMAID_BUBBLE_MIN_RADIUS);
				bubble.from_mermaid = true;
				bubbles.push_back(bubble);
				return;
			}
		}
		else if (state == GLUT_UP) {
			is_dragging = false;
			current_drag_target = NONE;
		}
	}
}




void motion(int x, int y) {
	if (!is_dragging) return;

	float mouse_x = (float)(x - win_width / 2);
	float mouse_y = (float)(win_height / 2 - y);

	if (current_drag_target == AIRPLANE) {
		airplane_center_x = mouse_x - drag_offset_x;
		airplane_center_y = mouse_y - drag_offset_y;
	}
	else if (current_drag_target == CAR) {
		car_center_x = mouse_x - drag_offset_x;
		car_center_y = mouse_y - drag_offset_y;
	}

	glutPostRedisplay();
}


void reshape(int width, int height) {
	win_width = width, win_height = height;

	glViewport(0, 0, win_width, win_height);
	ProjectionMatrix = glm::ortho(-win_width / 2.0, win_width / 2.0,
		-win_height / 2.0, win_height / 2.0, -1000.0, 1000.0);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	update_axes();
	update_line();

	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);

	glDeleteVertexArrays(1, &VAO_line);
	glDeleteBuffers(1, &VBO_line);

	glDeleteVertexArrays(1, &VAO_airplane);
	glDeleteBuffers(1, &VBO_airplane);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutCloseFunc(cleanup);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutTimerFunc(0, timer, 0);
}

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}

void initialize_OpenGL(void) {
	glEnable(GL_MULTISAMPLE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(135 / 255.0f, 206 / 255.0f, 235 / 255.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_line();
	prepare_airplane();
	prepare_house();
	prepare_car();
	prepare_cocktail();
	prepare_sword();
	prepare_fish();
	prepare_mermaid();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
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

#define N_MESSAGE_LINES 1
void main(int argc, char* argv[]) {
	char program_name[64] = "Sogang CSE4170 2DObjects_GLSL_3.0.1.3";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'ESC' "
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}


