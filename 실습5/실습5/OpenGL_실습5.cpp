#include<iostream>
#include<vector>
#include<random>
#include<algorithm>
#include<gl/glew.h>
#include<gl/glfw3.h>
#include "OpenGL.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

struct Color {
	GLfloat r, g, b;
};

struct RectS {
	GLfloat CentX,CentY;
	GLfloat size;
	Color color;
};

struct Eraser {
	GLfloat x, y;
	GLfloat size;
	Color color;
};

std::vector<RectS> rects;
std::vector<RectS> erases;
Eraser mouse;
std::random_device rd;
std::mt19937 dre{ rd() };
std::uniform_int_distribution pos{ 1, 90 }, col{ 0,255 }, PorM{ 0,1 };
bool isdrag;
int newRect;


// 화면지우기
	// 윈도우를띄우고화면에같은크기의작은사각형을다양한색으로임의의위치에20~40개그린다.
	// 왼쪽마우스버튼을누르면, 화면의사각형의2배의크기의검정색사각형(지우개사각형)이그려진다.
	// 마우스를누른채로이동시키면지우개사각형이위치를이동한다.
// 지우개
	// 사각형과부딪친사각형은사라진다.사각형이사라지면지우개사각형의크기가커지고, 지우개색상은
	// 부딪친사각형색상으로색상을변경한다.
// 왼쪽마우스버튼을떼면, 지우개사각형은사라지고, 사라졌던사각형들은다시원래의위치에나타난다.
// 다시마우스를누르면검정색의
// 기존의지우개사각형크기로지우개사각형이생긴다.
// 오른쪽마우스버튼을누르면, 그자리에사각형이새롭게생긴다.
// 새로운사각형이생기면지우개사각형의크기가줄어든다.최대10개만큼새롭게만들수있다.
// 키보드명령어r : 기존사각형들삭제되고리셋하여새고시작하기


bool timer = false;
double dtime = 1;

bool isOverlap(const RectS& rect1, const RectS& rect2) {
	if ((rect2.CentX - rect2.size <= rect1.CentX - rect1.size and rect1.CentX - rect1.size <= rect2.CentX +rect2.size and rect2.CentY - rect2.size <= rect1.CentY - rect1.size and rect1.CentY - rect1.size <= rect2.CentY + rect2.size) or
		(rect2.CentX - rect2.size <= rect1.CentX - rect1.size and rect1.CentX - rect1.size <= rect2.CentX +rect2.size and rect2.CentY - rect2.size <= rect1.CentY + rect1.size and rect1.CentY + rect1.size <= rect2.CentY + rect2.size) or
		(rect2.CentX - rect2.size <= rect1.CentX + rect1.size and rect1.CentX + rect1.size <= rect2.CentX +rect2.size and rect2.CentY - rect2.size <= rect1.CentY - rect1.size and rect1.CentY - rect1.size <= rect2.CentY + rect2.size) or
		(rect2.CentX - rect2.size <= rect1.CentX + rect1.size and rect1.CentX + rect1.size <= rect2.CentX +rect2.size and rect2.CentY - rect2.size <= rect1.CentY + rect1.size and rect1.CentY + rect1.size <= rect2.CentY + rect2.size))
		return true;
	return false;
}
void CheckErase(std::vector<RectS>& rects) {
	int OvIdx = -1;
	RectS m = {mouse.x, mouse.y, mouse.size};
	for (int i = rects.size() - 1; i >= 0; --i) {
		if (isOverlap(m, rects[i]) or isOverlap(rects[i], m)) {
			std::cout << "지우기!" << "\n";
			OvIdx = i;
			break;
		}
	}
	if (OvIdx >= 0) {
		mouse.color = { col(dre) / 255.f, col(dre) / 255.f, col(dre) / 255.f, };
		if(mouse.size < 0.48f) mouse.size += 1 / pow(2,7);
		erases.push_back(rects[OvIdx]);
		rects.erase(rects.begin() + OvIdx);
	}
}

void OpenGL::startPractice()
{
	mouse.size = 0.1f;
	mouse.color = {0,0,0};
	std::uniform_int_distribution cnt{ 20, 40 };
	size_t cnts = cnt(dre);
	for (int i = 0; i < cnts; ++i) {
		rects.push_back(RectS{ (PorM(dre) ? 1: -1) *pos(dre) / 100.f , (PorM(dre) ? 1 : -1) * pos(dre) / 100.f , 0.05f, col(dre) / 255.f , col(dre) / 255.f , col(dre) / 255.f });
	}
}
void OpenGL::mainloop() {
	while (!glfwWindowShouldClose(window)) {

		keyboard();
		if (timer) Timer(dtime);


		rendering();


		glfwPollEvents();
	}
}

// 렌더링 함수
void OpenGL::rendering() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);


	for (const auto& rect : rects) {
		glColor3f(rect.color.r, rect.color.g, rect.color.b);
		glRectf(rect.CentX - rect.size, rect.CentY - rect.size, rect.CentX + rect.size, rect.CentY + rect.size);
	}
	if (isdrag) {
		glColor3f(mouse.color.r, mouse.color.g, mouse.color.b);
		glRectf(mouse.x - mouse.size, mouse.y - mouse.size, mouse.x + mouse.size, mouse.y + mouse.size);
	}

	glfwSwapBuffers(window);
}

// 타이머
void OpenGL::Timer(double time)
{
	static GLdouble first, second;
	if (first != 0.0) {
		second = glfwGetTime();
		if (second - first >= time) {
			first = second;
			// 작업 처리
		}
	}
	else {
		first = glfwGetTime();
	}
}

// 폴링 함수
void OpenGL::keyboard() {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// 콜백 함수
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
}
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	double x, y;
	int width, height;
	glfwGetCursorPos(window, &x, &y);
	glfwGetWindowSize(window, &width, &height);
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_PRESS) {
			isdrag = true;
			mouse.x = (x - width / 2.) / (width / 2.);
			mouse.y = (height / 2. - y) / (height / 2.);
		}
		else {
			mouse.color = { 0,0,0 };
			isdrag = false;
			for (int i = erases.size() - 1; i >= 0; --i) {
				rects.push_back(erases[i]);
				erases.pop_back();
			}
		}
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		if (action == GLFW_PRESS and newRect < 10) {
			if(mouse.size > 0.05f)  mouse.size -= 1 / pow(2, 5);
			x = (x - width / 2.) / (width / 2.);
			y = (height / 2. - y) / (height / 2.);
			rects.push_back(RectS{ float(x), float(y), 0.05f, col(dre) / 255.f , col(dre) / 255.f , col(dre) / 255.f });
			newRect++;
		}
		break;
	}
}
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if (isdrag) {
		mouse.x = (xpos - width / 2.) / (width / 2.);
		mouse.y = (height / 2. - ypos) / (height / 2.);
		CheckErase(rects);
	}
}
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{

}
void CharCallback(GLFWwindow* window, unsigned int codepoint)
{
	switch (codepoint) {
	case 'q':
		glfwSetWindowShouldClose(window, true);
		break;
	case 'a':
		break;
	case 'r':
	{
		newRect = 0;
		mouse.size = 0.1f;
		mouse.color = { 0,0,0 };
		std::uniform_int_distribution cnt{ 20, 40 };
		size_t cnts = cnt(dre);
		rects.clear();
		for (int i = 0; i < cnts; ++i) {
			rects.push_back(RectS{ (PorM(dre) ? 1 : -1) * pos(dre) / 100.f , (PorM(dre) ? 1 : -1) * pos(dre) / 100.f , 0.05f, col(dre) / 255.f , col(dre) / 255.f , col(dre) / 255.f });
		}
	}
		break;
	}
}
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{

}
void windowSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}