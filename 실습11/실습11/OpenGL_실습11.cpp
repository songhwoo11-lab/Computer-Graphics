#include "OpenGL.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

std::random_device rd;
std::mt19937 dre{rd()};
std::uniform_int_distribution col{ 0, 255 }, pntuid{ 1, 20 }, modeluid{ 0, 2 };
bool timer = true;
double fps = 60;
double dtime = 1 / fps;
int count = 0;

struct Point {
	int x, y;
};
bool operator==(const Point & lhs, const Point & rhs) {
	return lhs.x == rhs.x and lhs.y == rhs.y;
}

struct PointF {
	GLfloat x, y;
};
struct ColorF {
	GLfloat r, g, b;
};

class Polygon {
public:
	enum class Model { triangle, inverted_triangle, rectangle };
	Polygon(Polygon::Model model, int x, int y) : pnt{x, y}, size { 0.05f }, color{ col(dre) / 255.f ,col(dre) / 255.f ,col(dre) / 255.f }, model{ model } {}
	void draw(GLuint DrawMode) const {
		PointF center = { -1.05f + 0.1f * pnt.x, 1.05f - 0.1f * pnt.y };
		switch (model) {
		case Model::triangle:
			DrawTriangle(center, DrawMode);
			break;
		case Model::inverted_triangle:
			DrawInvertedTriangle(center, DrawMode);
			break;
		case Model::rectangle:
			DrawRectangle(center, DrawMode);
			break;
		}
	}
	Point getPnt() const { return pnt; }
	GLfloat getSize() const { return size; }
	bool MoveX(int dx) {
		int positionX = pnt.x + dx;
		if (positionX >= 1 and positionX <= 20) {
			pnt.x = positionX;
			return true;
		}
		else return false;
	}
	bool MoveY(int dy) {
		int positionY = pnt.y + dy;
		if (positionY >= 1 and positionY <= 20) {
			pnt.y = positionY;
			return true;
		}
		else return false;
	}
	void SetColor(ColorF clr) { color = clr; }
	void SetSize(GLfloat newSize) { size = newSize; }
private:
	void DrawTriangle(PointF center, GLuint drawMode) const {
		GLfloat vertices[] = {
			center.x - size, center.y - size, 0, color.r, color.g, color.b,
			center.x + size, center.y - size, 0, color.r, color.g, color.b,
			center.x, center.y + size, 0, color.r, color.g, color.b,
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glDrawArrays(drawMode, 0, 3);
	}
	void DrawInvertedTriangle(PointF center, GLuint drawMode) const {
		GLfloat vertices[] = {
			center.x, center.y - size, 0, color.r, color.g, color.b,
			center.x + size, center.y + size, 0, color.r, color.g, color.b,
			center.x - size, center.y + size, 0, color.r, color.g, color.b,
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glDrawArrays(drawMode, 0, 3);
	}
	void DrawRectangle(PointF center, GLuint drawMode) const
	{
		GLfloat vertices[] = {
			center.x - size, center.y - size, 0, color.r, color.g, color.b,
			center.x + size, center.y - size, 0, color.r, color.g, color.b,
			center.x + size, center.y + size, 0, color.r, color.g, color.b,
			center.x - size, center.y + size, 0, color.r, color.g, color.b,
		};
		GLint indices[] = {
			0, 1, 2,
			0, 2, 3,
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		if (drawMode == GL_LINE_LOOP) glDrawArrays(drawMode, 0, 4);
		else glDrawElements(drawMode, 6, GL_UNSIGNED_INT, 0);
	}
	Point pnt;
	GLfloat size;
	ColorF color;
	Model model;
};

class Particle {
public:
	Particle(const Polygon& poly) : polygon{poly} {}
	bool running() {
		GLfloat newSize = polygon.getSize() + 0.005f;
		polygon.SetSize(newSize);
		if(newSize < 0.3f) return true;
		return false;
	}
	void draw() const {
		glLineWidth(3.0f);
		polygon.draw(GL_LINE_LOOP);
		glLineWidth(1.0f);
	}
private:
	Polygon polygon;
};

class Player {
public:
	Player(Polygon poly) : poly{poly}, ismove{false}, moveLeft{false}, moveUp{false} {}
	void draw() const {
		Polygon pol = poly;
		pol.SetColor(ColorF{ 0, 0, 0 });
		pol.SetSize(0.055f);
		pol.draw(GL_TRIANGLES);
		poly.draw(GL_TRIANGLES);
	}
	bool move() {
		if (not ismove) return false;
			if (moveLeft) {
				if (not poly.MoveX(-1)) {
					moveLeft = false;
					if (moveUp) {
						if (not poly.MoveY(-1)) {
							moveUp = false;
							poly.MoveY(1);
						}
					}
					else {
						if (not poly.MoveY(1)) {
							moveUp = true;
							poly.MoveY(-1);
						}
					}
				}
			}
			else {
				if (not poly.MoveX(1)) {
					moveLeft = true;
					if (moveUp) {
						if (not poly.MoveY(-1)) {
							moveUp = false;
							poly.MoveY(1);
						}
					}
					else {
						if (not poly.MoveY(1)) {
							moveUp = true;
							poly.MoveY(-1);
						}
					}
				}
			}
		return true;
	}
	void collisionCheck(std::vector<Polygon>& obstacles, std::vector<Particle>& particles) {
		for (int i = 0; i < obstacles.size(); ++i) {
			if(poly.getPnt() == obstacles[i].getPnt()) {
				particles.push_back(poly);
				std::swap(poly, obstacles[i]);
				break;
			}
		}
	}
	void SwitchMove() {
		ismove ^= 1;
	}
private:
	Polygon poly = { Polygon::Model::triangle, 1, 1 };
	bool ismove = false;
	bool moveLeft = false;
	bool moveUp = false;
};

std::vector<Polygon> obstacles;
std::vector<Particle> particles;
Player player = { Polygon{Polygon::Model::triangle, 1, 1} };

bool isExistPosition(const std::vector<Polygon>& obstacles, Point& p) {
	for (const auto& obstacle : obstacles) {
		if (p == obstacle.getPnt() or p == Point{1, 1}) return true;
	}
	return false;
}

void drawAxis(GLFWwindow* window) {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	for (int i = 0; i < 20; ++i) {
		GLfloat vertices[] = {
			(i - 10) * (width / 10.f) / (GLfloat)width, -1.0f, 0,	0,0,0,
			(i - 10) * (width / 10.f) / (GLfloat)width, 1.0f, 0,	0,0,0,
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glDrawArrays(GL_LINES, 0, 2);
	}
	for (int i = 0; i < 20; ++i) {
		GLfloat vertices[] = {
			-1.0f, (i - 10)* (height / 10.f) / (GLfloat)height, 0,	0,0,0,
			1.0f, (i - 10)* (height / 10.f) / (GLfloat)height, 0,	0,0,0,
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glDrawArrays(GL_LINES, 0, 2);
	}
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
	player = Polygon{ Polygon::Model::triangle, 1, 1 };
	for (int i = 0; i < 100; ++i) {
		while (true) {
			Point p = { pntuid(dre), pntuid(dre) };
			if (not isExistPosition(obstacles, p)) {
				obstacles.push_back({ Polygon::Model(modeluid(dre)), p.x, p.y});
				break;
			}
		}
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

	glUseProgram(shader.GetShaderID());

	// 그리기
	drawAxis(window);
	for (const auto& particle : particles) particle.draw();
	for (const auto& obstacle : obstacles) obstacle.draw(GL_TRIANGLES);
	player.draw();

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
			for (int i = 0; i < particles.size(); ++i) 
				if(not particles[i].running()) particles.erase(particles.begin() + i--);
			if (count < fps / 6) count++;
			else {
				count = 0;
				// 플레이어 움직이기
				if (player.move()) player.collisionCheck(obstacles, particles);
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
	case 'r':
	{
		// reset
		particles.clear();
		obstacles.clear();
		player = Polygon{ Polygon::Model::triangle, 1, 1 };
		for (int i = 0; i < 100; ++i) {
			while (true) {
				Point p = { pntuid(dre), pntuid(dre) };
				if (not isExistPosition(obstacles, p)) {
					obstacles.push_back({ Polygon::Model(modeluid(dre)), p.x, p.y });
					break;
				}
			}
		}
	}
		break;
	case 's':
		player.SwitchMove();
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