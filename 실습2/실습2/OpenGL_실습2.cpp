#include<iostream>
#include<vector>
#include<random>
#include<gl/glew.h>
#include<gl/glfw3.h>
#include "OpenGL.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

struct Color {
	GLfloat r, g, b;
};

struct Rect {
	GLfloat x;
	GLfloat y;
	GLfloat size;
	Color color{};
};

std::vector<Color> bgc(4);
std::vector<std::pair<Rect, int>> select;
std::random_device rd;
std::mt19937 dre{ rd() };
std::uniform_int_distribution uid{ 0, 255 };
int selm = -1;

int cntRect(int where)
{
	int cnt = 0;
	for (const auto& sel : select) {
		if (sel.second == where) cnt++;
	}
	return cnt;
}
bool upCheck(const std::pair<Rect, int>& sel, double degree) {
	double toSize = sel.first.size + degree;
	switch (sel.second) {
	case 1:
		if (sel.first.x - toSize <= 0 or sel.first.x - toSize >= 1.0f or
			sel.first.y - toSize <= 0 or sel.first.y - toSize >= 1.0f or
			sel.first.x + toSize <= 0 or sel.first.x + toSize >= 1.0f or
			sel.first.y + toSize <= 0 or sel.first.y + toSize >= 1.0f
			) return false;
		break;
	case 2:
		if (sel.first.x - toSize >= 0 or sel.first.x - toSize <= -1.0f or
			sel.first.y - toSize <= 0 or sel.first.y - toSize >= 1.0f or
			sel.first.x + toSize >= 0 or sel.first.x + toSize <=  -1.0f or
			sel.first.y + toSize <= 0 or sel.first.y + toSize >= 1.0f
			) return false;
		break;
	case 3:
		if (sel.first.x - toSize >= 0 or sel.first.x - toSize <= -1.0f or
			sel.first.y - toSize >= 0 or sel.first.y - toSize <=  -1.0f or
			sel.first.x + toSize >= 0 or sel.first.x + toSize <=  -1.0f or
			sel.first.y + toSize >= 0 or sel.first.y + toSize <=  -1.0f
			) return false;
		break;
	case 4:
		if (sel.first.x - toSize <= 0 or sel.first.x - toSize >= 1.0f or
			sel.first.y - toSize >= 0 or sel.first.y - toSize <= -1.0f or
			sel.first.x + toSize <= 0 or sel.first.x + toSize >= 1.0f or
			sel.first.y + toSize >= 0 or sel.first.y + toSize <= -1.0f
			) return false;
		break;
	}
	return true;
}

bool timer = false;
double dtime = 1;
void OpenGL::startPractice()
{
	for (auto& col : bgc)
		col = { uid(dre) / 255.f, uid(dre) / 255.f, uid(dre) / 255.f };

}
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

// 렌더링 함수
void OpenGL::rendering() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// 배경 그리기
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 2; ++j) {
			glColor3f(bgc[2 * i + j].r, bgc[2 * i + j].g, bgc[2 * i + j].b);
			glRectf(i - 1, j - 1, i, j);
		}
	}

	// 생성된 사각형 그리기
	for (const auto& sel : select) {
		glColor3f(sel.first.color.r, sel.first.color.g, sel.first.color.b);
		glRectf(sel.first.x - sel.first.size, sel.first.y - sel.first.size,
			sel.first.x + sel.first.size, sel.first.y + sel.first.size);

	}
	// 선택된 사각형 그리기
	if (selm >= 0) {
		glColor3f(0.0f, 0.0f, 0.0f);
		glLineWidth(3.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glRectf(select[selm].first.x - select[selm].first.size, select[selm].first.y - select[selm].first.size,
			select[selm].first.x + select[selm].first.size, select[selm].first.y + select[selm].first.size);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_PRESS)
		{
			std::cout << "mouse left click\n";
			glfwGetCursorPos(window, &x, &y);
			glfwGetWindowSize(window, &width, &height);
			x = x - width / 2.;
			y = height / 2. - y;
			for (int i = select.size() - 1; i > -1; --i) {
				if ((width / 2.) * (select[i].first.x - select[i].first.size) <= x and x <= (width / 2.) * (select[i].first.x + select[i].first.size) and
					(height / 2.) * (select[i].first.y - select[i].first.size) <= y and y <= (height / 2.) * (select[i].first.y + select[i].first.size)) {
					selm = i;
					break;
				}
				else selm = -1;
			}
			std::cout << "selm == " << selm << '\n';
		}
		break;
	}
}
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{

}
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{

}
void CharCallback(GLFWwindow* window, unsigned int codepoint)
{
	std::cout << codepoint << " == " << (char)codepoint << '\n';

	std::uniform_int_distribution ran{ 10, 80 };
	double degree = pow(2, 5);
	switch (codepoint) {
	case 'q':
		glfwSetWindowShouldClose(window, true);
		break;
	case '1':
		if (cntRect(1) < 5) {
			Rect rect{ ran(dre)/100., ran(dre) / 100.,0.1f, {uid(dre) / 255.,uid(dre) / 255.,uid(dre) / 255.}};
			select.push_back({ rect , 1});
		}
		break;
	case '2':
		if (cntRect(2) < 5) {
			Rect rect{ -ran(dre) / 100., ran(dre) / 100., 0.1f, {uid(dre) / 255.,uid(dre) / 255.,uid(dre) / 255.} };
			select.push_back({ rect, 2 });
		}
		break;
	case '3':
		if (cntRect(3) < 5) {
			Rect rect{ -ran(dre) / 100., -ran(dre) / 100.,0.1f,  {uid(dre) / 255.,uid(dre) / 255.,uid(dre) / 255.} };
			select.push_back({ rect, 3 });
		}
		break;
	case '4':
		if (cntRect(4) < 5) {
			Rect rect{ ran(dre) / 100., -ran(dre) / 100.,0.1f, {uid(dre) / 255.f, uid(dre) / 255.f, uid(dre) / 255.f} };
			select.push_back({ rect, 4 });
		}
		break;
	case '+': // 크기 늘리기
		if (selm >= 0) {
			if (upCheck(select[selm], 1 / degree)) select[selm].first.size += 1 / degree;
		}
		break;
	case '-': // 크기 줄이기
		if (selm >= 0) {
			if (select[selm].first.size > 0.05f) select[selm].first.size -= 1 / degree;
		}
		break;
	case 'c': // 색 변경
		if (selm >= 0) {
			select[selm].first.color = { uid(dre) / 255.f, uid(dre) / 255.f,uid(dre) / 255.f };
		}
		break;
	case 'r':
		select.clear();
		selm = -1;
		for (auto& col : bgc)
			col = { uid(dre) / 255.f, uid(dre) / 255.f, uid(dre) / 255.f };
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
