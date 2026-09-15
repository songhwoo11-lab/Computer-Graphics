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



struct Color3 {
	GLfloat r, g, b;
};
struct Point {
	double x, y;
 };
struct Rect {
	GLfloat x1, y1, x2, y2;
	Color3 color;
};
bool operator==(const Rect& lhs, const Rect& rhs) {
	return lhs.x1 == rhs.x1 and lhs.x2 == rhs.x2 and lhs.y1 == rhs.y1 and lhs.y2 == rhs.y2;
}

int PtInRect(const std::vector<Rect>& rs, double x, double y, int width, int height)
{
	double tx = (x - (width / 2)) / (width / 2), ty = ((height / 2) - y) / (height / 2);
	for (int i = rs.size() - 1; i > -1; --i) {
		if (rs[i].x1 <= tx and tx <= rs[i].x2 and rs[i].y1 <= ty and ty <= rs[i].y2) return i;
	}
	return -1;
}

std::random_device rd;
std::mt19937 dre{ rd() };
std::uniform_int_distribution col{ 0, 255 }, rp{ 10, 90 }, pm{ 0, 1 };
std::vector<Rect> rects;
bool isdrag;
int select =  -1;
Point pre;

// 사각형이동하기
// 키보드 a: 랜덤한 위치에 다른 색상의 사각형을 만든다. 최대 10개를 만든다.
	// (사각형들은서로겹쳐져있을수있는데, 항상나중에만든사각형이맨위로올라온다.)
// 왼쪽 마우스 버튼을 사각형 위에 클릭한 채 드래그: 사각형의위치가이동
// 마우스를 놓으면 더 이상 사각형이 이동하지 않음
// 왼쪽 마우스를 사각형 위에 놓고 선택, 드래그하여 두 사각형이 겹쳐지면,
		// 겹쳐진 사각형의 x축 최소, y축 최소값과 x축 최대, y축 최대값으로 큰 사각형으로 만들어진다. 
		//  색상은 랜덤하게바뀐다.
// 오른쪽 마우스를 사각형 위에서 클릭: 사각형은 2개 사각형으로 분리
		// 분리된사각형의크기와색상은랜덤하게설정한다.
		// 전체사각형의개수는최대20개로한다.

bool timer = false;
double dtime = 1;

float minMax(std::vector<Rect> rects, const char* flag) {
	float result;
	if (flag[0] == 'x') {
		if (flag[1] == '1') {
			result = rects[0].x1;
			for (int i = 1; i < rects.size(); ++i) {
				if (result > rects[i].x1) result = rects[i].x1;
			}
			return result;
		} else {
			result = rects[0].x2;
			for (int i = 1; i < rects.size(); ++i) {
				if (result < rects[i].x2) result = rects[i].x2;
			}
			return result;
		}
	} else {
		if (flag[1] == '1') {
			result = rects[0].y1;
			for (int i = 1; i < rects.size(); ++i) {
				if (result > rects[i].y1) result = rects[i].y1;
			}
			return result;
		} else {
			result = rects[0].y2;
			for (int i = 1; i < rects.size(); ++i) {
				if (result < rects[i].y2) result = rects[i].y2;
			}
			return result;
		}
	}
}

bool isOverlap(const Rect& rect1, const Rect& rect2) {
	rect2.x1 <= rect1.x1 and rect1.x1 <= rect2.x2;
	rect2.x1 <= rect1.x2 and rect1.x2 <= rect2.x2;
	rect2.y1 <= rect1.y1 and rect1.y1 <= rect2.y2;
	rect2.y1 <= rect1.y2 and rect1.y2 <= rect2.y2;
	if((rect2.x1 <= rect1.x1 and rect1.x1 <= rect2.x2 and rect2.y1 <= rect1.y1 and rect1.y1 <= rect2.y2) or
		(rect2.x1 <= rect1.x1 and rect1.x1 <= rect2.x2 and rect2.y1 <= rect1.y2 and rect1.y2 <= rect2.y2) or
		(rect2.x1 <= rect1.x2 and rect1.x2 <= rect2.x2 and rect2.y1 <= rect1.y1 and rect1.y1 <= rect2.y2) or
		(rect2.x1 <= rect1.x2 and rect1.x2 <= rect2.x2 and rect2.y1 <= rect1.y2 and rect1.y2 <= rect2.y2))
		return true;
	return false;
}
void CheckOverlap(std::vector<Rect>& rects, int idx) {
	std::vector<int> OvIdxs;
	for (int i = rects.size() - 1; i >= 0 ; --i) {
		if (i != idx and (isOverlap(rects[idx], rects[i]) or isOverlap(rects[i], rects[idx]))) {
			OvIdxs.push_back(i);
			std::cout << "오버랩!\n";
		}
	}
	if (OvIdxs.size() > 0) {
		std::vector<Rect> dels;
		for (int i = 0; i < OvIdxs.size(); ++i) {
			dels.push_back(rects[OvIdxs[i]]);
		}
		dels.push_back(rects[idx]);
		Rect overRect{ minMax(dels, "x1"),minMax(dels, "y1"),minMax(dels, "x2"),minMax(dels, "y2"),
			col(dre) / 255., col(dre) / 255., col(dre) / 255.,};

		rects.push_back(overRect);
		for (const auto& del : dels) {
			rects.erase(std::find(rects.begin(), rects.end(), del));
		}
	}
}

void OpenGL::startPractice()
{
	
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
		glRectf(rect.x1, rect.y1, rect.x2, rect.y2);
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
		if (action == GLFW_PRESS and not isdrag) {
			int idx = PtInRect(rects, x, y, width, height);
			if (idx != -1) {
				std::cout << "드래그 시작\n";
				isdrag = true;
				select = idx;
				pre = { (x - (width / 2)) / (width / 2),((height / 2) - y) / (height / 2) };
			}
		}
		else {
			if(isdrag) {
				CheckOverlap(rects, select);
				isdrag = false;
				select = -1;
			}
		}
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		if (action == GLFW_PRESS and rects.size() < 20) {
			int idx = PtInRect(rects, x, y, width, height);
			if (idx != -1) {
				std::cout << "분리 시작\n";
				// 분리한다.
				Rect tmp = rects[idx];
				rects.erase(rects.begin() + idx);
				GLfloat x = (tmp.x1 + tmp.x2) / 2.;
				GLfloat y = (tmp.y1 + tmp.y2) / 2.;
				Rect new1{};
				Rect new2{};
				float sz;
				if (tmp.x2 + (tmp.x2 - tmp.x1) / 2 >= 1.0f or
					tmp.x1 - (tmp.x2 - tmp.x1) / 2 <= -1.0f) {
					if (tmp.x2 - tmp.x1 < 0.1f) {
						sz = 0.05f;
					}
					else {
						std::uniform_int_distribution size{ 4, int((tmp.x2 - tmp.x1) / 2 * 100) };
						sz =size(dre) / 100.;
					}

					new1 = { x  - sz,
									y + (tmp.y2 - tmp.y1) / 2- sz,
									x + sz,
									y + (tmp.y2 - tmp.y1) / 2 + sz,
									col(dre) / 255.f,
									col(dre) / 255.f,
									col(dre) / 255.f, };
					new2 = { x  - sz,
									y - (tmp.y2 - tmp.y1) / 2 - sz,
									x + sz,
									y - (tmp.y2 - tmp.y1) / 2 + sz,
									col(dre) / 255.f,
									col(dre) / 255.f,
									col(dre) / 255.f, };
				}
				else {
					if ((tmp.y2 - tmp.y1) < 0.1f) {
						sz = 0.05f;
					} else {
						std::uniform_int_distribution size{ 5, int((tmp.y2 - tmp.y1) / 2 * 100) };
						sz = size(dre) / 100.;
					}
					new1 = { x + (tmp.x2 - tmp.x1) / 2 - sz,
									y - sz,
									x + (tmp.x2 - tmp.x1) / 2 + sz,
									y  + sz,
									col(dre) / 255.f,
									col(dre) / 255.f,
									col(dre) / 255.f, };
					new2 = { x - (tmp.x2 - tmp.x1) / 2 - sz,
									y  - sz,
									x - (tmp.x2 - tmp.x1) / 2 + sz,
									y  + sz,
									col(dre) / 255.f,
									col(dre) / 255.f,
									col(dre) / 255.f, };
				}
				if (new1.y2 >= 1.0f or new1.y1 <= -1.0f or
					new1.x2 >= 1.0f or new1.x1 <= -1.0f) {
					float x = (new1.x1 + new1.x2)/2, y = (new1.y1 + new1.y2) /2;
					new1.x1 -= x;
					new1.x2 -= x;
					new1.y1 -= y;
					new1.y2 -= y;
				}
				if (new2.y2 >= 1.0f or new2.y1 <= -1.0f or
					new2.x2 >= 1.0f or new2.x1 <= -1.0f) {
					float x = (new2.x1 + new2.x2) / 2, y = (new2.y1 + new2.y2) / 2;
					new2.x1 -= x;
					new2.x2 -= x;
					new2.y1 -= y;
					new2.y2 -= y;
				}
				rects.push_back(new1);
				rects.push_back(new2);
				
				
			}
		}

		break;
	}
}
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{

	if (isdrag) {
		std::cout << "드래그 중\n";
		int width, height;
		glfwGetWindowSize(window, &width, &height);

		double tx = (xpos - (width/2.)) / (width / 2.), ty = ((height/2. - ypos) / (height/2.));
		double dx = tx - pre.x, dy = ty - pre.y;
		rects[select].x1 += dx;
		rects[select].x2 += dx;
		rects[select].y1 += dy;
		rects[select].y2 += dy;
		pre = { tx, ty };
	}

}
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{

}
void CharCallback(GLFWwindow* window, unsigned int codepoint)
{
	switch (codepoint) {
	case 'a':
		if (rects.size() < 10) {
			double sz = 0.1f, centerx = (pm(dre) ? rp(dre) : -rp(dre)) / 100., centery = (pm(dre) ? rp(dre) : -rp(dre)) / 100.;
			Rect rect{centerx - sz, centery -sz, centerx + sz, centery + sz, col(dre)/255. , col(dre) / 255. , col(dre) / 255. };
			rects.push_back(rect);
		}
		std::cout << "총 사각형 수: " << rects.size() << '\n';
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