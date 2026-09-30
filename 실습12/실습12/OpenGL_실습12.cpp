#include "OpenGL.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

std::random_device rd;
std::mt19937 dre{rd()};
std::uniform_int_distribution col{ 0, 255 }, spd{ 30, 70 };
bool timer = true;
double fps = 60;
double dtime = 1 / fps;

struct Color {
	GLfloat r, g, b;
};

class Rectangle {
public:
	Rectangle(GLfloat x, GLfloat y, bool up) : x{ x }, y{ y }, sizeX{ 0.05f }, sizeY{ 0.05f }, speed{ spd(dre) / 1000.f }, isUp{ up },
		color{ { col(dre) / 255.f },{ col(dre) / 255.f },{ col(dre) / 255.f } } {}
	Rectangle(GLfloat x, GLfloat y, GLfloat szX, GLfloat szY, Color color) : x{ x }, y{ y }, sizeX{ szX }, sizeY{ szY }, 
		speed{ 0.0f }, isUp{ false },
		color{ color } {}
	void draw(GLuint drawMode) const
	{
		GLfloat vertices[] = {
			x - sizeX, y - sizeY, 0, color.r, color.g, color.b,
			x + sizeX, y - sizeY, 0, color.r, color.g, color.b,
			x + sizeX, y + sizeY, 0, color.r, color.g, color.b,
			x - sizeX, y + sizeY, 0, color.r, color.g, color.b,
		};
		GLuint indices[] = {
			0,1,2,
			0,2,3,
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		if (drawMode == GL_LINE_LOOP) glDrawArrays(drawMode, 0, 4);
		else glDrawElements(drawMode, 6,  GL_UNSIGNED_INT, 0);
	}
	void move() {
		if (isUp) {
			y += speed;
			if (y >= 0.9f) {
				y = 0.9f;
				isUp = false;
			}
		} else {
			y -= speed;
			if (y <= -0.9f) {
				y = -0.9f;
				isUp = true;
			}
		}
	}
	GLfloat GetPointX() const { return x; }
	GLfloat GetPointY() const { return y; }
	void SetPoint(GLfloat newX, GLfloat newY) { x = newX; y = newY; }
	GLfloat GetSizeX() const { return sizeX; }
	GLfloat GetSizeY() const { return sizeY; }
	void SetSize(GLfloat size) { sizeX = sizeY = size; }
	void DownSizeY(GLfloat dsizey) {
		GLfloat newSizey = sizeY - dsizey;
		if (newSizey > 0.1f) { sizeY = newSizey; }
	}
	bool InRect(const Rectangle& rect) const {
		return x - sizeX <= rect.x - rect.sizeX and rect.x - rect.sizeX <= x + sizeX and
		x - sizeX <= rect.x + rect.sizeX and rect.x + rect.sizeX <= x + sizeX and
		y - sizeY <= rect.y - rect.sizeY and rect.y - rect.sizeY <= y + sizeY and
		y - sizeY <= rect.y + rect.sizeY and rect.y + rect.sizeY <= y + sizeY;
	}
private:
	GLfloat x, y;
	GLfloat sizeX, sizeY;
	GLfloat speed;
	bool isUp;
	Color color;
};

class Particle {
public:
	Particle(const Rectangle& poly) : rect{ poly } {}
	bool running() {
		rect.SetSize(rect.GetSizeY() + 0.01f);
		if (rect.GetSizeY() < 0.3f) return true;
		return false;
	}
	void draw() const {
		glLineWidth(3.0f);
		rect.draw(GL_LINE_LOOP);
		glLineWidth(1.0f);
	}
private:
	Rectangle rect;
};

class Animation {
public:
	Animation(const Rectangle& poly, GLfloat goalX, GLfloat goalY) : rect{ poly }, x{poly.GetPointX()}, y{poly.GetPointY()}, goalX{goalX}, goalY{goalY} {}
	bool running() {
		distance += 0.01f;
		if (distance >= 1.0f) {
			rect.SetPoint(goalX, goalY);
			return false;
		}
		GLfloat newx = (1.0f - distance) * x + goalX * distance;
		GLfloat newy = (1.0f - distance) * y + goalY * distance;
		rect.SetPoint(newx, newy);
		return true;
	}
	void draw() const {
		glLineWidth(3.0f);
		rect.draw(GL_TRIANGLES);
		glLineWidth(1.0f);
	}
	Rectangle GetRect() const { return rect; }
private:
	Rectangle rect;
	GLfloat x, y;
	GLfloat goalX, goalY;
	GLfloat distance;
};

GLint count = 0;
GLfloat firstX = -0.6f, secondX = 0.0f, initY = 0.8f;
std::unique_ptr<Rectangle> firstRect{}, secondRect{}, goalRect{};
std::vector<Rectangle> backgrounds;
std::vector<Rectangle> bricks;
std::vector<Animation> animations;
std::vector<Particle> particles;

void moveToAnimations(std::unique_ptr<Rectangle>& rect, std::vector<Animation>& animations) {
	animations.push_back(Animation{ *(rect.get()), 0.95f - (count / 20) * 0.1f, -0.95f + count % 20 * 0.1f });
	count++;
	rect.release();
}

bool check(std::unique_ptr<Rectangle>& rectA, std::unique_ptr<Rectangle>& rectB, std::unique_ptr<Rectangle>& rectG)
{
	return rectG->InRect(*(rectA.get())) and rectG->InRect(*(rectB.get()));
}

void init() {
	count = 0;
	firstRect = std::make_unique<Rectangle>(firstX, initY, false);
	secondRect = std::make_unique<Rectangle>(secondX, -initY, true);
	goalRect = std::make_unique<Rectangle>((firstX + secondX) / 2, 0.0f, 0.6f, 0.5f, Color{ 0.8f,0.8f,0.8f });
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
	backgrounds.push_back(Rectangle(firstX, 0.0f, 0.1f, 0.98f, Color{ 0.8f,0.8f,1.0f }));
	backgrounds.push_back(Rectangle(secondX, 0.0f, 0.1f, 0.98f, Color{ 1.0f,0.8f, 1.0f }));
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
	for (const auto& bg : backgrounds) bg.draw(GL_LINE_LOOP);
	goalRect->draw(GL_LINE_LOOP);
	firstRect->draw(GL_TRIANGLES);
	secondRect->draw(GL_TRIANGLES);
	for (const auto& brick : bricks) brick.draw(GL_TRIANGLES);
	for (const auto& particle : particles) particle.draw();
	for (const auto& animation : animations) animation.draw();


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
			firstRect->move();
			secondRect->move();
			for (int i = 0; i < particles.size(); ++i) {
				if (not particles[i].running()) particles.erase(particles.begin() + i--);
			}
			for (int i = 0; i < animations.size(); ++i) {
				if (not animations[i].running()) {
					bricks.push_back(animations[i].GetRect());
					animations.erase(animations.begin() + i--);
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
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		// 여기서 도형 처리
		if(check(firstRect, secondRect, goalRect)){
			if (bricks.size() >= 100) bricks.clear();
			particles.push_back(*(firstRect.get()));
			particles.push_back(*(secondRect.get()));
			moveToAnimations(firstRect, animations);
			moveToAnimations(secondRect, animations);
			firstRect = std::make_unique<Rectangle>(firstX, initY, false);
			secondRect = std::make_unique<Rectangle>(secondX, -initY, true);
			goalRect->DownSizeY(0.01f);
		}
	}
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
		// reset
		firstRect.release();
		secondRect.release();
		particles.clear();
		bricks.clear();
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