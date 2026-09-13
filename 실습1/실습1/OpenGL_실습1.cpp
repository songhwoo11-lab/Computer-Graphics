#include<iostream>
#include<vector>
#include<random>
#include<gl/glew.h>
#include<gl/glfw3.h>
#include "OpenGL.h"


#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

std::vector<GLfloat> color = {1.0f, 1.0f , 1.0f};
bool timer = false;
double dtime = 1.;
GLdouble first, second;
std::random_device rd;
std::mt19937 dre{ rd() };
std::uniform_int_distribution uid{ 0, 255 };

void OpenGL::mainloop() {
	//--- 메인 루프
	while (!glfwWindowShouldClose(window)) {
		// 입력처리
		keyboard();
		if (timer) Timer(dtime);
		
		// 렌더링
		rendering();

		// 메시지 받기
		glfwPollEvents();
	}

}

void OpenGL::keyboard() {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		// 청록색
		color = { 0.0f, 1.0f, 1.0f };
	}
	else if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		// 자홍색
		color = { 1.0f, 0.0f, 1.0f };
	}
	else if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
		// 노란색
		color = { 1.0f, 1.0f, 0.0f };
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		// 랜덤
		for (auto& c : color) c = uid(dre) / 255.f;
	}
	else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		// 회색
		color = { 0.5f, 0.5f, 0.5f };
	}
	else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		// 검정색
		color = { 0.0f, 0.0f, 0.0f };
	}
	else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		// 타이머를 설정하여 특정 시간마다 랜덤색으로 계속 바뀌게 한다.
		timer = true;
		glfwSetTime(0.0f);
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		// 타이머 종료
		timer = false;
		first = 0.0f;
	}

}

void OpenGL::rendering() {
	// 화면지우기(파란색)
	glClearColor(color[0], color[1], color[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// 버퍼교체
	glfwSwapBuffers(window);
}

void OpenGL::Timer(double time) 
{	
	if (first != 0.0) {
		second = glfwGetTime();
		if (second - first >= time) {
			first = second;
			// 작업 처리
			for (auto& c : color) c = uid(dre) / 255.f;
		}
	}
	else {
		first = glfwGetTime();
	}
}