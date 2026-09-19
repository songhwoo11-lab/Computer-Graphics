#include<iostream>
#include<iomanip>
#include<vector>
#include<random>
#include<algorithm>
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
struct Attr {
	bool isYSize; // 크기 

	short dirX; // 대각선 방향

	bool isUp; // 지그재그 위 아래 방향
	bool isRight; // 지그재그 좌우 방향

	short Line; // 1: 위, 2: 우측, 3: 아래, 4: 좌측
	bool isFirst; // 처음부분
};

struct RectS {
	PntF center;
	GLfloat sizeX, sizeY;
	ColorF color;
	Attr attr;
};

std::vector<RectS> rects;
std::vector<PntF> backs;
std::random_device rd;
std::default_random_engine dre{ rd() };
std::uniform_int_distribution col{ 0, 255 };
int Move;
bool Size, Color;
int check, Tm;

bool timer = false;
double dtime = 1;

void MoveDiagonal(RectS& rect) 
{
	switch (rect.attr.dirX) {
	case 1: // 우측 위
		if(rect.center.x+rect.sizeX < 1.0f and rect.center.y + rect.sizeY < 1.0f) {
			rect.center.x += 0.015625f;
			rect.center.y += 0.015625f;
		} else if (rect.center.x + rect.sizeX >= 1.0f) { // 좌측 위로 변경
			rect.attr.dirX = 4;
		} else if (rect.center.y + rect.sizeY >= 1.0f) { // 우측 아래로 변경
		rect.attr.dirX = 2;
		}
		break;
	case 2: // 우측 아래
		if (rect.center.x + rect.sizeX < 1.0f and rect.center.y - rect.sizeY >  -1.0f) {
			rect.center.x += 0.015625f;
			rect.center.y -= 0.015625f;
		} else if (rect.center.x + rect.sizeX >= 1.0f) { // 좌측 아래로 변경
			rect.attr.dirX = 3;
		} else if (rect.center.y - rect.sizeY <=  -1.0f) { // 우측 위로 변경
			rect.attr.dirX = 1;
		}
		break;
	case 3: // 좌측 아래
		if (rect.center.x - rect.sizeX >  -1.0f and rect.center.y - rect.sizeY >  -1.0f) {
			rect.center.x -= 0.015625f;
			rect.center.y -= 0.015625f;
		}
		else if (rect.center.x - rect.sizeX <= -1.0f) { // 우측 아래로 변경
			rect.attr.dirX = 2;
		}
		else if (rect.center.y - rect.sizeY <= -1.0f) { // 좌측 위로 변경
			rect.attr.dirX = 4;
		}
		break;
	case 4: // 좌측 위
		if (rect.center.x - rect.sizeX >  -1.0f and rect.center.y + rect.sizeY < 1.0f) {
			rect.center.x -= 0.015625f;
			rect.center.y += 0.015625f;
		}
		else if (rect.center.x - rect.sizeX <=  -1.0f) { // 우측 위로 변경
			rect.attr.dirX = 1;
		}
		else if (rect.center.y + rect.sizeY >= 1.0f) { // 좌측 아래로 변경
			rect.attr.dirX = 3;
		}
		break;
	}
}
void MoveZigZag(RectS& rect) 
{
	switch (rect.attr.isRight) {
	case true:
		rect.center.x += 0.015625f;
		if (rect.center.x + rect.sizeX >= 1.0f) {
			if (rect.center.y + rect.sizeY >= 1.0f) rect.attr.isUp = false;
			else if (rect.center.y - rect.sizeY <= -1.0f)rect.attr.isUp = true;
			rect.attr.isUp ? rect.center.y += 0.03125f : rect.center.y -= 0.03125f;
			rect.attr.isRight ^= 1;
		}
		break;
	case false:
		rect.center.x -= 0.015625f;	
		if (rect.center.x - rect.sizeX <= -1.0f) {
			if (rect.center.y + rect.sizeY >= 1.0f) rect.attr.isUp = false;
			else if (rect.center.y - rect.sizeY <= -1.0f)rect.attr.isUp = true;
			rect.attr.isUp ? rect.center.y += 0.03125f : rect.center.y -= 0.03125f;
			rect.attr.isRight ^= 1;
		}
		break;

	}

}
void SetEdge(RectS& rect) {
	double degree[4]{};
	degree[3] = 1.0f - (rect.center.y + rect.sizeY);
	degree[0] = 1.0f - (rect.center.x + rect.sizeX);
	degree[1] =  abs( - 1.0f - (rect.center.y - rect.sizeY));
	degree[2] =  abs( - 1.0f - (rect.center.x - rect.sizeX));
	int min = 0;
	for (int i = 1; i < 4; ++i) {
		if (degree[min] > degree[i]) min = i;
	}
	rect.attr.Line = min;
}
void MoveEdge(RectS& rect) 
{
	switch (rect.attr.Line) {
	case 0: // 위측 가장자리(좌->우 이동)
		rect.center.x += 0.015625f;
		if (rect.center.x + rect.sizeX>= 1.0f) {
			rect.attr.Line = 1;
			if (rect.attr.isFirst) rect.attr.isFirst = false;
		}
		if (Size and not rect.attr.isFirst) {
			if (rect.attr.isYSize) rect.center.y -= 0.015625f;
			else rect.center.y += 0.015625f;
		}
		break;
	case 1: // 우측 가장자리 (위 -> 아래 이동)
		rect.center.y -= 0.015625f;
		if (rect.center.y - rect.sizeY <= -1.0f) {
			rect.attr.Line = 2;
			if (rect.attr.isFirst) rect.attr.isFirst = false;
		}
		if (Size and not rect.attr.isFirst) {
			if (not rect.attr.isYSize) rect.center.x -= 0.015625f;
			else rect.center.x += 0.015625f;
		}
		break;
	case 2: // 아래측 가장자리(우->좌로 이동)
		rect.center.x -= 0.015625f;
		if (rect.center.x - rect.sizeX <=  -1.0f) {
			rect.attr.Line = 3;
			if (rect.attr.isFirst) rect.attr.isFirst = false;
		}
		if (Size and not rect.attr.isFirst) {
			if (rect.attr.isYSize) rect.center.y += 0.015625f;
			else rect.center.y -= 0.015625f;
		}
		break;
	case 3: // 좌측 가장자리(아래 -> 위 이동)
		rect.center.y += 0.015625f;
		if (rect.center.y + rect.sizeY >= 1.0f) {
			rect.attr.Line = 0;
			if (rect.attr.isFirst) rect.attr.isFirst = false;
		}
		if (Size and not rect.attr.isFirst) {
			if (not rect.attr.isYSize) rect.center.x += 0.015625f;
			else rect.center.x -= 0.015625f;
		}
		break;
	}

}
void SizeRect(RectS& rect) {
	if (not rect.attr.isYSize) {
		rect.sizeX += 0.015625f;
		rect.sizeY -= 0.015625f;
		if (rect.sizeY <= 0.05f)rect.attr.isYSize = true;
	} else {
		rect.sizeX -= 0.015625f;
		rect.sizeY += 0.015625f;
		if (rect.sizeX <= 0.05f) rect.attr.isYSize = false;
	}
}

void OpenGL::startPractice()
{
	Move = 0;
	Size = false;
	Color = false;
	check = 0;
	Tm = 30;
	dtime = 1 / double(Tm * 2);
	timer = true;
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
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < rects.size(); ++i) {
		glColor3f(rects[i].color.r, rects[i].color.g, rects[i].color.b);
		glRectf(rects[i].center.x - rects[i].sizeX, rects[i].center.y - rects[i].sizeY,
			rects[i].center.x + rects[i].sizeX, rects[i].center.y + rects[i].sizeY);
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
			switch (Move) {
			case 1: // 대각선 이동
				for (int i = 0; i < rects.size(); ++i)
					MoveDiagonal(rects[i]);
				break;
			case 2: // 지그재그 이동
				for (int i = 0; i < rects.size(); ++i)
					MoveZigZag(rects[i]);
				break;
			case 3: // 윈도우 가장자리로 이동
				for (int i = 0; i < rects.size(); ++i)
					MoveEdge(rects[i]);
				break;
			}
			if(Size) {
				for (int i = 0; i < rects.size(); ++i) {
					SizeRect(rects[i]);
				}
			}
			if(Color) {
				if (check < Tm) check++;
				else {
					for (auto& rect : rects) 
						rect.color = { col(dre) / 255.f , col(dre) / 255.f , col(dre) / 255.f };
					check = 0;
				}
			}
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
		if (action == GLFW_PRESS and rects.size() < 5) {
			RectS rect{ {(x - width / 2.) / (width / 2.),(height / 2. - y) / (height / 2.) },
								0.1f, 0.1f,
								{col(dre) / 255.f, col(dre) / 255.f, col(dre) / 255.f, },
								{false, 1, false, false, 0, true}
			};

			// 범위 밖 체크
			if (rect.center.x + rect.sizeX >= 1.0f) rect.center.x = 1.0f - rect.sizeX;
			if (rect.center.x - rect.sizeX <= -1.0f) rect.center.x = rect.sizeX - 1.0f;
			if (rect.center.y + rect.sizeY >= 1.0f) rect.center.y = 1.0f - rect.sizeY;
			if (rect.center.y - rect.sizeY <= -1.0f) rect.center.y = rect.sizeY - 1.0f;

			// 3번 위치 확인
			SetEdge(rect);

			backs.push_back(rect.center);
			rects.push_back(rect);
		}

		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		break;
	}
	std::cout << rects.size() << '\n';
}
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
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
	case '+':
		if(Tm < 72) {
			Tm += 1;
			dtime = 1 / double(Tm * 2);
		}
		std::cout << "타이머: " << std::setprecision(3) << dtime << '\n';
		break;
	case '-':
		if (Tm > 15) {
			Tm -= 1;
			dtime = 1 / double(Tm * 2);
		}
		std::cout << "타이머: " << std::setprecision(3) << dtime << '\n';
		break;
	case 's': // stop
		Color = false;
		Size = false;
		Move = 0;
		check = 0;
		break;
	case 'm': // 원래 위치로 이동
		for (int i = 0; i < rects.size(); ++i) {
			rects[i].center = backs[i];
		}
		break;
	case 'r': // reset
		rects.clear();
		backs.clear();
		Color = false;
		Size = false;
		Move = 0;
		check = 0;
		break;
	case '1': // 대각선 이동
		Move = (Move != 1 ? 1 : 0);
		break;
	case '2': // 지그재그 이동
		Move = (Move != 2 ? 2 : 0);
		break;
	case '3': // 가장자리에 따라 이동
		Move = (Move != 3 ? 3 : 0);
		if (Move != 0) {
			for (int i = 0; i < rects.size(); ++i) 
				SetEdge(rects[i]);
		}
		else {
			for (int i = 0; i < rects.size(); ++i) 
				rects[i].attr.isFirst = true;			
		}
		break;
	case '4': // 크기 변화 
		Size ^= 1;
		break; 
	case '5': // 색상 변화
		Color ^= 1;
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