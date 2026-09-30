#include "OpenGL.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

std::random_device rd;
std::mt19937 dre{rd()};
std::uniform_int_distribution col{ 0, 255 }, shuf{ 0, 4 }, dx{ 5, 90 }, dy{ 5, 90 }, pm{0, 1};
bool timer = false;
double dtime = 1;
GLfloat pi = 3.1415926535979f;

struct PointF { GLfloat x, y; };
struct Color { GLfloat r, g, b; };


class Polygon {
public:
	enum class Mode { Rectangle, Triangle, RGTriangle };
	Polygon(GLfloat x, GLfloat y, GLfloat sx, GLfloat sy, GLfloat radian, Polygon::Mode mode) : center{ x,y }, sizeX{ sx }, sizeY{ sy }, radian{ radian }, color{ col(dre) / 255.f, col(dre) / 255.f, col(dre) / 255.f }, mode{ mode }, mat{ glm::identity<glm::mat4>() } { makeMat(); }
	Polygon(GLfloat x, GLfloat y, GLfloat sx, GLfloat sy, GLfloat radian, Color color, Polygon::Mode mode) : center{ x,y }, sizeX{ sx }, sizeY{ sy }, radian{ radian }, color{ color }, mode{ mode }, mat{ glm::identity<glm::mat4>() } { makeMat(); }
	void draw(const Shader& shader, GLuint drawMode) const {
		switch (mode) {
		case Mode::Rectangle:
			drawRectangle(shader, drawMode);
			break;
		case Mode::Triangle:
			drawTriangle(shader, drawMode);
			break;
		case Mode::RGTriangle:
			drawRGTriangle(shader, drawMode);
			break;
		}
	}
	void setColor(Color col) { color = col; }
	void plusPos(PointF p) {
		center.x += p.x;
		center.y += p.y;
		makeMat();
	}
	void setPos(PointF p) {
		center.x = p.x;
		center.y = p.y;
		makeMat();
	}
	void setAlright(bool isal) { isAlright = isal; }
	bool PtInPoly(PointF p) {
		return center.x - sizeX <= p.x and p.x <= center.x + sizeX and
			center.y - sizeY <= p.y and p.y <= center.y + sizeY;
	}
	bool getAlright() const { return isAlright; }
	Mode getMode() const { return mode; }
	GLfloat getSizeX() const { return sizeX; }
	GLfloat getSizeY() const { return sizeY; }
	GLfloat getRadian() const { return radian; }
	PointF getPos() const { return center; }
private:
	glm::mat4 makeMat() {
		mat = glm::translate(glm::identity<glm::mat4>(), glm::vec3(center.x, center.y, 0));
		mat = glm::rotate(mat, radian, glm::vec3(0, 0, 1));
		mat = glm::translate(mat, glm::vec3(-center.x, -center.y, 0));
		return mat;
	}
	void drawTriangle(const Shader& shader, GLuint drawMode) const {
		GLfloat vertices[] = {
			center.x - sizeX, center.y - sizeY, 0, color.r, color.g, color.b,
			center.x + sizeX, center.y - sizeY, 0, color.r, color.g, color.b,
			center.x, center.y + sizeY, 0, color.r, color.g, color.b,
		};

		auto rotate = glGetUniformLocation(shader.GetShaderID(), "rotate");
		glUniformMatrix4fv(rotate, 1, false, glm::value_ptr(mat));
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glDrawArrays(drawMode, 0, 3);

		glUniformMatrix4fv(rotate, 1, false, glm::value_ptr(glm::identity<glm::mat4>()));

	}
	void drawRectangle(const Shader& shader, GLuint drawMode) const {
		GLfloat vertices[] = {
			center.x - sizeX, center.y - sizeY, 0, color.r, color.g, color.b,
			center.x + sizeX, center.y - sizeY, 0, color.r, color.g, color.b,
			center.x + sizeX, center.y + sizeY, 0, color.r, color.g, color.b,
			center.x - sizeX, center.y + sizeY, 0, color.r, color.g, color.b,
		};
		GLuint indices[] = {
			0, 1, 2,
			0,2, 3,
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		auto rotate = glGetUniformLocation(shader.GetShaderID(), "rotate");
		glUniformMatrix4fv(rotate, 1, false, glm::value_ptr(mat));

		if (drawMode == GL_LINE_LOOP) glDrawArrays(drawMode, 0, 4);
		else glDrawElements(drawMode, 6, GL_UNSIGNED_INT, 0);
		glUniformMatrix4fv(rotate, 1, false, glm::value_ptr(glm::identity<glm::mat4>()));
	}
	void drawRGTriangle(const Shader& shader, GLuint drawMode) const {
		GLfloat vertices[] = {
			center.x - sizeX, center.y - sizeY, 0, color.r, color.g, color.b,
			center.x + sizeX, center.y - sizeY, 0, color.r, color.g, color.b,
			center.x + sizeX, center.y + sizeY, 0, color.r, color.g, color.b,
		};

		auto rotate = glGetUniformLocation(shader.GetShaderID(), "rotate");
		glUniformMatrix4fv(rotate, 1, false, glm::value_ptr(mat));

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glDrawArrays(drawMode, 0, 3);
		
		glUniformMatrix4fv(rotate, 1, false, glm::value_ptr(glm::identity<glm::mat4>()));
	}
	PointF center;
	GLfloat sizeX;
	GLfloat sizeY;
	GLfloat radian;
	Color color;
	Mode mode;
	glm::mat4 mat;
	bool isAlright = false;
};
bool operator==(const Polygon& lhs, const Polygon& rhs)
{
	return lhs.getMode() == rhs.getMode() and 
		lhs.getSizeX() == rhs.getSizeX() and
		lhs.getSizeY() == rhs.getSizeY() and
		lhs.getRadian() == rhs.getRadian();
}

bool isOverlap(const Polygon& polygon1, const Polygon& polygon2) {
	PointF p1 = polygon1.getPos(), p2 = polygon2.getPos();
		if ((p2.x - polygon2.getSizeX() <= p1.x - polygon1.getSizeX() and p1.x - polygon1.getSizeX() <= p2.x + polygon2.getSizeX() and p2.y - polygon2.getSizeY() <= p1.y - polygon1.getSizeY() and p1.y - polygon1.getSizeY() <= p2.y + polygon2.getSizeY()) or
			(p2.x - polygon2.getSizeX() <= p1.x - polygon1.getSizeX() and p1.x - polygon1.getSizeX() <= p2.x + polygon2.getSizeX() and p2.y - polygon2.getSizeY() <= p1.y + polygon1.getSizeY() and p1.y + polygon1.getSizeY() <= p2.y + polygon2.getSizeY()) or
			(p2.x - polygon2.getSizeX() <= p1.x + polygon1.getSizeX() and p1.x + polygon1.getSizeX() <= p2.x + polygon2.getSizeX() and p2.y - polygon2.getSizeY() <= p1.y - polygon1.getSizeY() and p1.y - polygon1.getSizeY() <= p2.y + polygon2.getSizeY()) or
			(p2.x - polygon2.getSizeX() <= p1.x + polygon1.getSizeX() and p1.x + polygon1.getSizeX() <= p2.x + polygon2.getSizeX() and p2.y - polygon2.getSizeY() <= p1.y + polygon1.getSizeY() and p1.y + polygon1.getSizeY() <= p2.y + polygon2.getSizeY()))
			return true;
	return false;
}

Color tleColor = Color{ 0.8f,0.8f,0.8f };
std::vector<Polygon> pantles[5];
std::vector<Polygon> polygons;
bool isDrag = false;
int sel = -1;
PointF mouse;
PointF initPos[5] = { {0.3, 0.7}, {0.7, 0.4}, {0.4, 0.0}, {0.7, -0.3}, {0.3, -0.7} };

void shuffle() {
	for (int i = 0; i < 100; ++i) {
		std::swap(initPos[shuf(dre)], initPos[shuf(dre)]);
	}
}
void init() {
	// 크기
	// 0: 0.22f 가로 세로
	// 1: 0.3f 가로 세로
	// 2: 0.1f * 0.3f
	// 3: 0.28f * 0.12f
	// 4: 0.21f * 0.21f
	shuffle();
	polygons.clear();
	for (int i = 0; i < 5; ++i) pantles[i].clear();
	pantles[0].push_back({ -0.06f, -0.06f, 0.05f, 0.05f,0.0f, tleColor, Polygon::Mode::Rectangle });
	pantles[0].push_back({ 0.06f, -0.06f, 0.05f, 0.05f, 0.0f, tleColor,Polygon::Mode::Rectangle });
	pantles[0].push_back({ 0.06f, 0.06f, 0.05f, 0.05f, 0.0f, tleColor,Polygon::Mode::Rectangle });
	pantles[0].push_back({ -0.06f, 0.06f, 0.05f, 0.05f,0.0f, tleColor, Polygon::Mode::Rectangle });

	pantles[1].push_back({ 0, -.1f, 0.05f, 0.05f, 0.0f, tleColor, Polygon::Mode::Triangle });
	pantles[1].push_back({ .1f, 0, 0.05f, 0.05f, pi / 2, tleColor,Polygon::Mode::Triangle });
	pantles[1].push_back({ 0, .1f, 0.05f, 0.05f, pi, tleColor, Polygon::Mode::Triangle });
	pantles[1].push_back({ -.1f, 0, 0.05f, 0.05f, pi * 3 / 2, tleColor,Polygon::Mode::Triangle });

	pantles[2].push_back({ 0, 0, 0.05f, 0.15f, 0.0f, tleColor, Polygon::Mode::RGTriangle });
	pantles[2].push_back({ 0, 0, 0.05f, 0.15f, pi, tleColor, Polygon::Mode::RGTriangle });

	pantles[3].push_back({ 0, 0, 0.06f, 0.06f,0.0f, tleColor, Polygon::Mode::Rectangle });
	pantles[3].push_back({ -0.1f, 0.0f, 0.04f, 0.04f, pi * 3 / 2, tleColor,Polygon::Mode::Triangle });
	pantles[3].push_back({ 0.1f, 0.0f, 0.04f, 0.04f, pi / 2, tleColor,Polygon::Mode::Triangle });

	pantles[4].push_back({ 0, 0, 0.07f, 0.07f,0.0f, tleColor, Polygon::Mode::Rectangle });
	pantles[4].push_back({ 0, 0.14f, 0.07f, 0.07f, 0.0f, tleColor, Polygon::Mode::RGTriangle });
	pantles[4].push_back({ -0.14f, 0, 0.07f, 0.07f, pi / 2, tleColor, Polygon::Mode::RGTriangle });
	pantles[4].push_back({ 0, -0.14f, 0.07f, 0.07f, pi, tleColor, Polygon::Mode::RGTriangle });
	pantles[4].push_back({ 0.14f, 0, 0.07f, 0.07f, pi * 3 / 2, tleColor, Polygon::Mode::RGTriangle });
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < pantles[i].size(); ++j) {
			polygons.push_back(pantles[i][j]);
			polygons.back().setColor({ col(dre) / 255.f, col(dre) / 255.f, col(dre) / 255.f });
			pantles[i][j].plusPos(initPos[i]);
		}
	}
	for (int i = 0; i < polygons.size(); ++i) {
		bool isflag = true;
		while(isflag) {
			isflag = false;
			PointF p = { -dx(dre) / 100.f, dy(dre) / 100.f * (pm(dre) ? 1 : -1) };
			polygons[i].setPos(p);
			for (int j = 0; j < i; ++j) {
				if (isOverlap(polygons[i], polygons[j]) or isOverlap(polygons[j], polygons[i])) {
					isflag = true;
					break;
				}
			}
		}
	}
}
GLfloat distance(const PointF& lhs, const PointF& rhs)
{
	return sqrt(pow(lhs.x - rhs.x, 2) + pow(lhs.y - rhs.y, 2));
}

void OpenGL::SetBuffer()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}


void OpenGL::startPractice()
{
	init();
	
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

	// 그리기
	for (int i = 0; i < 5; ++i) 
		for (const auto& poly : pantles[i]) poly.draw(shader, GL_LINE_LOOP);
	for (const auto& poly : polygons) poly.draw(shader, GL_TRIANGLES);

	glfwSwapBuffers(window);
}

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
	switch(button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_PRESS) {
			double x, y;
			int width, height;
			glfwGetCursorPos(window, &x, &y);
			glfwGetWindowSize(window, &width, &height);
			float dx = (x - width / 2.) / (width / 2.);
			float dy = (height / 2. - y) / (height / 2.);
			for (int i = polygons.size() - 1; i >= 0; --i) {
				if (polygons[i].PtInPoly({ dx, dy }) and not polygons[i].getAlright()) {
					polygons[i].setPos({dx, dy});
					sel = i;
					isDrag = true;
					break;
				}
			}
		}
		else {
			if(isDrag) {
				// 처리하기
				bool isbreak = false;
				for (int i = 0; i < 5; ++i) {
					for (int j = 0; j < pantles[i].size(); ++j) {
						if (pantles[i][j] == polygons[sel]) {
							if (not pantles[i][j].getAlright() and distance(pantles[i][j].getPos(), polygons[sel].getPos()) < 0.01f) {
								std::cout << "접합 확인 - " << sel << '\n';
								pantles[i][j].setAlright(true);
								polygons[sel].setAlright(true);
								polygons[sel].setPos(pantles[i][j].getPos());
								isbreak = true;
								break;
							}
						}
					}
					if (isbreak) break;
				}
				isDrag = false;
				sel = -1;
			}
		}
		break;
	}
}
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (isDrag) {
		double x, y;
		int width, height;
		glfwGetCursorPos(window, &x, &y);
		glfwGetWindowSize(window, &width, &height);
		float dx = (x - width / 2.) / (width / 2.);
		float dy = (height / 2. - y) / (height / 2.);

		polygons[sel].setPos({ dx, dy });

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
	case 'r':
		init();
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