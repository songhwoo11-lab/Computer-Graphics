#include<iostream>
#include<vector>
#include<random>
#include<gl/glew.h>
#include<gl/glfw3.h>
#include "OpenGL.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

struct PntF {
	GLfloat x, y;
};
struct ColorF {
	GLfloat r, g, b;
};
enum class Particle { wasd = 1, qe, one, wasdqe };

struct RectS {
	PntF center;
	GLfloat size;
	ColorF color;
	Particle particle;
	PntF degree;
	int direct;
};

std::vector<RectS> rects; // 처음 생성되는 사각형
std::vector<RectS> clickeds; // 눌린 사각형 따로 처리
std::random_device rd;
std::mt19937 dre{ rd() };

int PtInRect(const std::vector<RectS>& rs, double x, double y)
{
	if (rs.empty()) return -1;
	for (int i = rs.size()  -1; i > - 1; --i) {
		if (rs[i].center.x - rs[i].size <= x and x <= rs[i].center.x + rs[i].size and
			rs[i].center.y - rs[i].size <= y and y <= rs[i].center.y + rs[i].size)
			return i;
	}
	return -1;
}

void drawWASD(const RectS& clicked) {
	glRectf(clicked.center.x - clicked.degree.x - clicked.size,
		clicked.center.y - clicked.size,
		clicked.center.x - clicked.degree.x + clicked.size,
		clicked.center.y + clicked.size);
	glRectf(clicked.center.x + clicked.degree.x - clicked.size,
		clicked.center.y - clicked.size,
		clicked.center.x + clicked.degree.x + clicked.size,
		clicked.center.y + clicked.size);
	glRectf(clicked.center.x - clicked.size,
		clicked.center.y - clicked.degree.y - clicked.size,
		clicked.center.x + clicked.size,
		clicked.center.y - clicked.degree.y + clicked.size);
	glRectf(clicked.center.x - clicked.size,
		clicked.center.y + clicked.degree.y - clicked.size,
		clicked.center.x + clicked.size,
		clicked.center.y + clicked.degree.y + clicked.size);
}
void drawQE(const RectS& clicked)
{
	glRectf(clicked.center.x - clicked.degree.x - clicked.size,
		clicked.center.y - clicked.degree.y - clicked.size,
		clicked.center.x - clicked.degree.x + clicked.size,
		clicked.center.y - clicked.degree.y + clicked.size);
	glRectf(clicked.center.x - clicked.degree.x - clicked.size,
		clicked.center.y + clicked.degree.y - clicked.size,
		clicked.center.x - clicked.degree.x + clicked.size,
		clicked.center.y + clicked.degree.y + clicked.size);
	glRectf(clicked.center.x + clicked.degree.x - clicked.size,
		clicked.center.y - clicked.degree.y - clicked.size,
		clicked.center.x + clicked.degree.x + clicked.size,
		clicked.center.y - clicked.degree.y + clicked.size);
	glRectf(clicked.center.x + clicked.degree.x - clicked.size,
		clicked.center.y + clicked.degree.y - clicked.size,
		clicked.center.x + clicked.degree.x + clicked.size,
		clicked.center.y + clicked.degree.y + clicked.size);
}
void drawOne(const RectS& clicked) {
	switch (clicked.direct) {
		case 1:
		glRectf(clicked.center.x - clicked.size/2.f * 3 - clicked.size,
			clicked.center.y - clicked.size / 2.f * 3 - clicked.degree.y - clicked.size,
			clicked.center.x - clicked.size / 2.f * 3 + clicked.size,
			clicked.center.y - clicked.size / 2.f * 3 - clicked.degree.y + clicked.size);
		glRectf(clicked.center.x + clicked.size / 2.f * 3 - clicked.size,
			clicked.center.y - clicked.size / 2.f * 3 - clicked.degree.y - clicked.size,
			clicked.center.x + clicked.size / 2.f * 3 + clicked.size,
			clicked.center.y - clicked.size / 2.f * 3 - clicked.degree.y + clicked.size);
		glRectf(clicked.center.x - clicked.size / 2.f * 3 - clicked.size,
			clicked.center.y + clicked.size / 2.f * 3 - clicked.degree.y - clicked.size,
			clicked.center.x - clicked.size / 2.f * 3 + clicked.size,
			clicked.center.y + clicked.size / 2.f * 3 - clicked.degree.y + clicked.size);
		glRectf(clicked.center.x + clicked.size / 2.f * 3 - clicked.size,
			clicked.center.y + clicked.size / 2.f * 3 - clicked.degree.y - clicked.size,
			clicked.center.x + clicked.size / 2.f * 3 + clicked.size,
			clicked.center.y + clicked.size / 2.f * 3 - clicked.degree.y + clicked.size);
		break;
	case 2:
		glRectf(clicked.center.x - clicked.size / 2.f * 3 - clicked.size,
			clicked.center.y - clicked.size / 2.f * 3 + clicked.degree.y - clicked.size,
			clicked.center.x - clicked.size / 2.f *3 + clicked.size,
			clicked.center.y - clicked.size / 2.f * 3 + clicked.degree.y + clicked.size);
		glRectf(clicked.center.x + clicked.size / 2.f * 3 - clicked.size,
			clicked.center.y - clicked.size / 2.f * 3 + clicked.degree.y - clicked.size,
			clicked.center.x + clicked.size / 2.f * 3 + clicked.size,
			clicked.center.y - clicked.size / 2.f * 3 + clicked.degree.y + clicked.size);
		glRectf(clicked.center.x - clicked.size / 2.f * 3 - clicked.size,
			clicked.center.y + clicked.size / 2.f * 3 + clicked.degree.y - clicked.size,
			clicked.center.x - clicked.size / 2.f * 3 + clicked.size,
			clicked.center.y + clicked.size / 2.f * 3 + clicked.degree.y + clicked.size);
		glRectf(clicked.center.x + clicked.size / 2.f * 3 - clicked.size,
			clicked.center.y + clicked.size / 2.f * 3 + clicked.degree.y - clicked.size,
			clicked.center.x + clicked.size / 2.f * 3 + clicked.size,
			clicked.center.y + clicked.size / 2.f * 3 + clicked.degree.y + clicked.size);
		break;
	case 3:
		glRectf(clicked.center.x - clicked.size / 2.f * 3 - clicked.degree.x - clicked.size,
			clicked.center.y - clicked.size / 2.f * 3 - clicked.size,
			clicked.center.x - clicked.size / 2.f * 3 - clicked.degree.x + clicked.size,
			clicked.center.y - clicked.size / 2.f * 3 + clicked.size);
		glRectf(clicked.center.x + clicked.size / 2.f * 3 - clicked.degree.x - clicked.size,
			clicked.center.y - clicked.size / 2.f * 3 - clicked.size,
			clicked.center.x + clicked.size / 2.f * 3 - clicked.degree.x + clicked.size,
			clicked.center.y - clicked.size / 2.f * 3 + clicked.size);
		glRectf(clicked.center.x - clicked.size / 2.f * 3 - clicked.degree.x - clicked.size,
			clicked.center.y + clicked.size / 2.f * 3 - clicked.size,
			clicked.center.x - clicked.size / 2.f * 3 - clicked.degree.x + clicked.size,
			clicked.center.y + clicked.size / 2.f * 3 + clicked.size);
		glRectf(clicked.center.x + clicked.size / 2.f * 3 - clicked.degree.x - clicked.size,
			clicked.center.y + clicked.size / 2.f * 3 - clicked.size,
			clicked.center.x + clicked.size / 2.f * 3 - clicked.degree.x + clicked.size,
			clicked.center.y + clicked.size / 2.f * 3 + clicked.size);
		break;
	case 4:
		glRectf(clicked.center.x - clicked.size / 2.f * 3 + clicked.degree.x - clicked.size,
			clicked.center.y - clicked.size / 2.f * 3 - clicked.size,
			clicked.center.x - clicked.size / 2.f * 3 + clicked.degree.x + clicked.size,
			clicked.center.y - clicked.size / 2.f * 3 + clicked.size);
		glRectf(clicked.center.x + clicked.size / 2.f * 3 + clicked.degree.x - clicked.size,
			clicked.center.y - clicked.size / 2.f * 3 - clicked.size,
			clicked.center.x + clicked.size / 2.f * 3 + clicked.degree.x + clicked.size,
			clicked.center.y - clicked.size / 2.f * 3 + clicked.size);
		glRectf(clicked.center.x - clicked.size / 2.f * 3 + clicked.degree.x - clicked.size,
			clicked.center.y + clicked.size / 2.f * 3 - clicked.size,
			clicked.center.x - clicked.size / 2.f * 3 + clicked.degree.x + clicked.size,
			clicked.center.y + clicked.size / 2.f * 3 + clicked.size);
		glRectf(clicked.center.x + clicked.size / 2.f * 3 + clicked.degree.x - clicked.size,
			clicked.center.y + clicked.size / 2.f * 3 - clicked.size,
			clicked.center.x + clicked.size / 2.f * 3 + clicked.degree.x + clicked.size,
			clicked.center.y + clicked.size / 2.f * 3 + clicked.size);
		break;
	}
}
void drawParticle(const RectS& clicked) {
	glColor3f(clicked.color.r, clicked.color.g, clicked.color.b);
	switch (clicked.particle) {
	case Particle::wasd:
		drawWASD(clicked);
		break;
	case Particle::qe:
		drawQE(clicked);
		break;
	case Particle::one:
		drawOne(clicked);
		break;
	case Particle::wasdqe:
		drawWASD(clicked);
		drawQE(clicked);
		break;
	}
}

bool timer = false;
double dtime = 1;
void OpenGL::startPractice()
{
	std::uniform_int_distribution size{ 5, 20 }, color{ 0, 255 }, Pnt{ 0, 60 }, pm{ 0, 1 }, ptc{ 1, 4 }, dir{ 1, 4 };
	for (int i = 0; i < 10; ++i) {
		RectS rect{ (pm(dre) ? 1 : -1) * (Pnt(dre) / 100.), (pm(dre) ? 1 : -1) * (Pnt(dre) / 100.),
							size(dre) / 100., 
							color(dre) / 255. , color(dre) / 255. , color(dre) / 255. ,
							Particle(ptc(dre)),
							0.0f, 0.0f,
							dir(dre)};
		rects.push_back(rect);
	}
	dtime = 1/60.f;
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
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < rects.size(); ++i) {
		glColor3f(rects[i].color.r, rects[i].color.g, rects[i].color.b);
		glRectf(rects[i].center.x - rects[i].size, rects[i].center.y - rects[i].size,
			rects[i].center.x + rects[i].size, rects[i].center.y + rects[i].size);
	}
	for (int i = 0; i < clickeds.size(); ++i) {
		drawParticle(clickeds[i]);
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
			int remove = -1;
			for (int i = 0; i < clickeds.size(); ++i) {
				switch (clickeds[i].particle) {
				case Particle::wasd:
				case Particle::qe:
				case Particle::wasdqe:
					clickeds[i].degree.x += 0.05f;
					clickeds[i].degree.y += 0.05f;
					clickeds[i].size -= 0.0025f;
					if (clickeds[i].size <= 0.f) remove = i;
					break;
				case Particle::one:
					clickeds[i].degree.x += 0.05f;
					clickeds[i].degree.y += 0.05f;
					clickeds[i].size -= 0.0025f;
					if (clickeds[i].size <= 0.f) remove = i;
					break;
				}
				if (clickeds[i].color.r  < 1.0f) clickeds[i].color.r += 0.05f;
				if (clickeds[i].color.g  < 1.0f) clickeds[i].color.g += 0.05f;
				if (clickeds[i].color.b  < 1.0f) clickeds[i].color.b += 0.05f;
			}
			if (remove >= 0) clickeds.erase(clickeds.begin() + remove);
			if (clickeds.empty()) time = false;
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
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_PRESS) {
			double xpos, ypos;
			int width, height;
			glfwGetCursorPos(window, &xpos, &ypos);
			glfwGetWindowSize(window, &width, &height);
			xpos = (xpos - width / 2.) / (width / 2.);
			ypos = (height / 2. - ypos) / (height / 2.);
			int idx = PtInRect(rects, xpos, ypos);
			if (idx != -1) {
				rects[idx].size /= 2.f;
				rects[idx].degree = { rects[idx].size / 2 + 0.01f , rects[idx].size / 2 + 0.01f };
				clickeds.push_back(rects[idx]);
				rects.erase(rects.begin() + idx);
				timer = true;
			}
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
	
}
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{

}
void windowSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}