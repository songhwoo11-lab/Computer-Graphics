#include "OpenGL.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

std::random_device rd;
std::mt19937 dre;
std::uniform_int_distribution pnt{ 1,80 }, pm{ 0,1 };
const GLfloat DotSize = 5.0f, size = 0.05f;
GLfloat DotSizeToScreenX, DotSizeToScreenY;
int select =  -1;

enum class Polygon {Vertex, Line, Triangle, Rectangle};
class PolyManager {
public:
	PolyManager(Polygon p, GLfloat x, GLfloat y) : poly{p}
	{
		switch (p) {
		case Polygon::Vertex:
			Vertex(x, y);
			break;
		case Polygon::Line:
			Line(x, y);
			break;
		case Polygon::Triangle:
			Triangle(x, y);
			break;
		case Polygon::Rectangle:
			Rectangle(x, y);
			break;
		}
	}
	void draw() const {
		switch (poly) {
		case Polygon::Vertex:
			drawVertex();
			break;
		case Polygon::Line:
			drawLine();
			break;
		case Polygon::Triangle:
			drawTriangle();
			break;
		case Polygon::Rectangle:
			drawRectangle();
			break;
		}
	}
	bool PtInPolygon(double x, double y, int width, int height) const
	{

		GLfloat dot_sz = DotSize / (width < height ? width : height) * 2;
		switch (poly) {
		case Polygon::Vertex:
			if (vertexs[0] - dot_sz <= x and x <= vertexs[0] + dot_sz and
				vertexs[1] - dot_sz <= y and y <= vertexs[1] + dot_sz)
				return true;
			break;
		case Polygon::Line:
			if (vertexs[0]  <= x and x <= vertexs[6] and
				vertexs[1] <= y and y <= vertexs[7])
				return true;
			break;
		case Polygon::Triangle:
		case Polygon::Rectangle:
		{
			GLfloat tx = vertexs[0] + size, ty = vertexs[1]  + size;
			if (tx - size <= x and x <= tx + size and
				ty - size <= y and y <= ty + size)
				return true;
		}
			break;
		}
		return false;
	}
	void drawLayout() 
	{
		if (poly == Polygon::Vertex) {
			vertexs[5] = 0.0f;

			glBufferData(GL_ARRAY_BUFFER, vertexs.size() * sizeof(GLfloat), vertexs.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glPointSize(DotSize);
			glDrawArrays(GL_POINTS, 0, 1);
			glPointSize(1.0f);
			vertexs[5] = 1.0f;
		}
		else if (poly == Polygon::Line) {
			for (int i = 3; i < 12; i += 6) vertexs[i] = 1.0f;
			for (int i = 5; i < 12; i += 6) vertexs[i] = 1.0f;

			glBufferData(GL_ARRAY_BUFFER, vertexs.size() * sizeof(GLfloat), vertexs.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glDrawArrays(GL_LINES, 0, 2);
			for (int i = 3; i < 12; i += 6) vertexs[i] = 0.0f;
			for (int i = 5; i < 12; i += 6) vertexs[i] = 0.0f;
		}
		if(poly == Polygon::Triangle) {
			for (int i = 4; i < 18; i += 6) vertexs[i] = 0.0f;

			glBufferData(GL_ARRAY_BUFFER, vertexs.size() * sizeof(GLfloat), vertexs.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);


			glDrawArrays(GL_LINE_LOOP, 0, 3);
			for (int i = 4; i < 18; i += 6) vertexs[i] = 1.0f;
		}
		else if (poly == Polygon::Rectangle) {
			for (int i = 3; i < 36; i += 6) vertexs[i] = 0.0f;

			glBufferData(GL_ARRAY_BUFFER, vertexs.size() * sizeof(GLfloat), vertexs.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);


			glDrawArrays(GL_LINE_LOOP, 0, 3);
			glDrawArrays(GL_LINE_LOOP, 3, 3);
			for (int i = 3; i < 36; i += 6) vertexs[i] = 1.0f;
		}
	}
	void move(GLfloat dx, GLfloat dy) {
		if (check('x', dx) and check('y', dy)) {
			if (dx != 0) for (int i = 0; i < vertexs.size(); i += 6) vertexs[i] += dx;
			if (dy != 0) for (int i = 1; i < vertexs.size(); i += 6) vertexs[i] += dy;
		}
	}
private:
	std::vector<GLfloat> vertexs;
	Polygon poly;

	void Vertex(GLfloat x, GLfloat y) {
		vertexs.resize(6);
		for (int i = 5; i < 6; i += 6) vertexs[i] = 1.0f;
		vertexs[0] = x;
		vertexs[1] = y;
	}
	void Line(GLfloat x, GLfloat y) {
		vertexs.resize(12);
		for (int i = 3; i < 12; i += 6) vertexs[i] = vertexs[i+1] = vertexs[i+2] = 0.0f;
		for (int i = 2; i < 12; i += 6) vertexs[i] = 0.0f;
		vertexs[0] = x - size;
		vertexs[1] = y - size;

		vertexs[6] = x + size;
		vertexs[7] = y + size;
	}
	void Triangle(GLfloat x, GLfloat y) {
		vertexs.resize(18);
		for (int i = 4; i < 18; i += 6) vertexs[i] = 1.0f;
		for (int i = 2; i < 18; i += 6) vertexs[i] = 0.0f;
		vertexs[0] = x - size;
		vertexs[1] = y - size;

		vertexs[6] = x + size;
		vertexs[7] = y - size;

		vertexs[12] = x;
		vertexs[13] = y + size;
	}
	void Rectangle(GLfloat x, GLfloat y) {
		vertexs.resize(36);
		for (int i = 3; i < 36; i += 6) vertexs[i] = 1.0f;
		for (int i = 2; i < 36; i += 6) vertexs[i] = 0.0f;
		vertexs[0] = x - size;
		vertexs[1] = y - size;

		vertexs[6] = vertexs[24] = x + size;
		vertexs[7] = vertexs[25] = y - size;

		vertexs[12] = vertexs[18] = x - size;
		vertexs[13] = vertexs[19] = y + size;

		vertexs[30] = x + size;
		vertexs[31] = y + size;
	}

	
	void drawVertex() const {
		glBufferData(GL_ARRAY_BUFFER, vertexs.size() * sizeof(GLfloat), vertexs.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glPointSize(DotSize);
		glDrawArrays(GL_POINTS, 0, 1);
		glPointSize(1.0f);
	}
	void drawLine() const {
		glBufferData(GL_ARRAY_BUFFER, vertexs.size() * sizeof(GLfloat), vertexs.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glDrawArrays(GL_LINES, 0, 2);
	}
	void drawTriangle() const 
	{
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexs.size(), vertexs.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glDrawArrays(GL_TRIANGLES, 0, 3);

	}
	void drawRectangle() const {
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexs.size(), vertexs.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	
	bool check(char what, GLfloat dw) {
		if (dw == 0) return true;
		if (poly == Polygon::Vertex) {
			return dotCheck(what, dw);
		}
		switch (what) {
		case 'x':
			for (int i = 0; i < vertexs.size(); i += 6) {
				if (dw > 0 and vertexs[i] + dw >= 1.0f) return false;
				else if (dw < 0 and vertexs[i] + dw <= -1.0f) return false;
			}
			break;
		case 'y':
			for (int i = 1; i < vertexs.size(); i += 6) {
				if (dw > 0 and vertexs[i] + dw >= 1.0f) return false;
				else if (dw < 0 and vertexs[i] + dw <= -1.0f) return false;
			}
		}
		return true;
	}
	bool dotCheck(char what,GLfloat dw) {
		switch (what) {
		case 'x':
			if (dw > 0 and vertexs[0] + dw >= 1.0f - DotSizeToScreenX) return false;
			else if (dw < 0 and vertexs[0] + dw <= -1.0f + DotSizeToScreenX) return false;
			break;
		case 'y':
			if (dw > 0 and vertexs[1] + dw >= 1.0f - DotSizeToScreenY) return false;
			else if (dw < 0 and vertexs[1] + dw <=  -1.0f + DotSizeToScreenY) return false;

		}
		return true;
	}
};


std::vector<PolyManager> manager;
bool timer = false;
double dtime = 1;

void OpenGL::SetBuffer()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

void OpenGL::startPractice()
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	DotSizeToScreenX = DotSize / width;
	DotSizeToScreenY = DotSize / height;
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

	glUseProgram(shader.GetShaderID());
	glBindVertexArray(VAO);

	// 그리기
	 for(const auto& polygon : manager)
		polygon.draw();
	if (select != -1) manager[select].drawLayout();

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
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_PRESS) {
			double x, y;
			int width, height;
			glfwGetCursorPos(window, &x, &y);
			glfwGetWindowSize(window, &width, &height);
			for (int i = manager.size() -1; i > -1; --i) {
				if (manager[i].PtInPolygon((x - width / 2.) / (width / 2.), (height / 2. - y) / (height / 2.), width, height)) {
					select = i;
					break;
				}
				else select =  - 1;
			}
			std::cout << select << '\n';
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
	switch (codepoint) {
	case 'q':
		glfwSetWindowShouldClose(window, true);
		return;
	case 'p':
		if(manager.size() < 50){
			PolyManager m{ Polygon::Vertex , (pm(dre) ? 1 : -1) * (pnt(dre) / 100.f) , (pm(dre) ? 1 : -1) * (pnt(dre) / 100.f) };
			manager.push_back(m);
		}
		break;
	case 'e':
		if (manager.size() < 50) {
			PolyManager m{ Polygon::Line , (pm(dre) ? 1 : -1) * (pnt(dre) / 100.f) , (pm(dre) ? 1 : -1) * (pnt(dre) / 100.f) };
			manager.push_back(m);
		}
		break;
	case 't':
		if (manager.size() < 50) {
			PolyManager m{ Polygon::Triangle , (pm(dre) ? 1 : -1) * (pnt(dre) / 100.f) , (pm(dre) ? 1 : -1) * (pnt(dre) / 100.f) };
			manager.push_back(m);
		}
		break;
	case 'r':
		if (manager.size() < 50) {
			PolyManager m{ Polygon::Rectangle , (pm(dre) ? 1 : -1) * (pnt(dre) / 100.f) , (pm(dre) ? 1 : -1) * (pnt(dre) / 100.f) };
			manager.push_back(m);
		}
		break;
	case 'w': // 선택된 도형을 위로 이동
		if(select != -1) manager[select].move(0, 0.01f);
		break;
	case 'a': // 선택된 도형을  좌측으로 이동
		if (select != -1) manager[select].move(-0.01f, 0);
		break;
	case 's': // 선택된 도형을 아래로 이동
		if (select != -1) manager[select].move(0, -0.01f);
		break;
	case 'd': // 선택된 도형을 우측으로 이동
		if (select != -1) manager[select].move(0.01f, 0);
		break;
	case 'i': // 선택된 도형을 대각선 (좌상)으로 이동
		if (select != -1) manager[select].move(-0.01f, 0.01f);
		break;
	case 'j': // 선택된 도형을 대각선 (우상)으로 이동
		if (select != -1) manager[select].move(0.01f, 0.01f);
		break;
	case 'k': // 선택된 도형을 대각선 (좌하)으로 이동
		if (select != -1) manager[select].move(-0.01f, -0.01f);
		break;
	case 'l': // 선택된 도형을 대각선 (우하)으로 이동
		if (select != -1) manager[select].move(0.01f, -0.01f);
		break;
	case '1': // 모든 도형들이 선택되어 이동 (좌)
		for(auto& poly : manager) poly.move(-0.01f, 0);
		break;
	case '2': // 모든 도형들이 선택되어 이동 (우)
		for (auto& poly : manager) poly.move(0.01f, 0);
		break;
	case '3': // 모든 도형들이 선택되어 이동 (상)
		for (auto& poly : manager) poly.move(0, 0.01f);
		break;
	case '4': // 모든 도형들이 선택되어 이동 (하)
		for (auto& poly : manager) poly.move(0, -0.01f);
		break;
	case 'c':
		manager.clear();
		select = -1;
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