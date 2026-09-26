#include "OpenGL.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

std::random_device rd;
std::mt19937 dre{ rd() };
std::uniform_int_distribution col{ 0, 255 }, size{ 5, 15 };
short drawMode;

struct PointF {
	GLfloat x, y;
};

struct Attr {
	short bounceMove = 1; // 1, 2, 3, 4

	bool ZigzagUp = false;
	bool ZigzagRight = true;

	short SpikeZigZag = 1;

	GLfloat CircleX{}, CircleY{};
	GLfloat r{};
	GLfloat angle{};
	bool rotateRight;

	GLfloat triRotateAngle{};
};

struct Color { GLfloat r, g, b; };

enum class Move { None, Bounce, ZigZag, SpikeZigZag, CircleSpiral};
Move move;

class Triangle {
public:
	Triangle(GLfloat x, GLfloat y) : x{ x }, y{ y }, sz{ size(dre) / 100.f }, color{ { col(dre) / 255.f },{ col(dre) / 255.f },{ col(dre) / 255.f } }, isSizeUp{ true }, attr{}, mat{glm::identity<glm::mat4>()} {
		attr.CircleX = x;
		attr.CircleY = y;
	}
	void resize(GLfloat dsize)
	{
		if ((not isSizeUp and sz + dsize <= 0.05f) or (isSizeUp and sz + dsize >= 0.2f)) {
			isSizeUp ^= 1;
		}
		sz += (isSizeUp ? dsize : -dsize);
	}
	GLfloat getSize() const { return sz; }
	void draw(const Shader& shader) const
	{
		GLfloat vertices[] = {
			x - sz, y - sz, 0, color.r, color.g, color.b,
			x + sz, y - sz, 0, color.r, color.g, color.b,
			x, y + sz, 0, color.r, color.g, color.b,
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);


		
		auto rotate = glGetUniformLocation(shader.GetShaderID(), "rotate");
		glUniformMatrix4fv(rotate, 1, false, glm::value_ptr(mat));
		glDrawArrays(drawMode == 0 ? GL_TRIANGLES : GL_LINE_LOOP, 0, 3);
		glUniformMatrix4fv(rotate, 1, false, glm::value_ptr(glm::identity<glm::mat4>()));
	}
	void drawDegree() const {
		if (que.empty()) return;
		glLineWidth(3.0f);
		for (int i = 0; i < que.size() - 1; ++i) {
			GLfloat vertices[] = {
				que[i].x, que[i].y, 0.0f, color.r, color.g, color.b,
				que[i+1].x, que[i+1].y, 0.0f, color.r, color.g, color.b,
			};

			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glDrawArrays(GL_LINES, 0, 2);
		}
		glLineWidth(1.0f);
	}
	void Moving() {
		switch (move) {
		case Move::Bounce:
			BounceMove();
			break;
		case Move::ZigZag:
			ZigzagMove();
			break;
		case Move::SpikeZigZag:
			SpikeZigzagMove();
			break;
		case Move::CircleSpiral:
			CircleSpiralMove();
			break;
		}
		que.push_back({x, y});
		if (que.size() > 100)
			que.pop_front();
	}
	void clearQue() { que.clear(); }
	void setting() {
		attr.angle = attr.r = 0;
		attr.CircleX = x;
		attr.CircleY = y;
	}
private:
	GLfloat x, y;
	GLfloat sz;
	Color color;
	bool isSizeUp;
	Attr attr;
	glm::mat4 mat;
	std::deque<PointF> que;

	void BounceMove() {
		switch (attr.bounceMove) {
		case 1: // 북동
			x += 0.01f;
			y += 0.01f;
			if (x + sz >= 1.0f) {
				attr.bounceMove = 2;
				attr.triRotateAngle = 3.14f * 2 * (attr.bounceMove - 1) / 4;
			}
			else if(y + sz >= 1.0f) {
				attr.bounceMove = 4;
				attr.triRotateAngle = 3.14f * 2 * (attr.bounceMove - 1) / 4;
			}
			break;
		case 2: // 북서
			x -= 0.01f;
			y += 0.01f;
			if (x - sz <= -1.0f) {
				attr.bounceMove = 1;
				attr.triRotateAngle = 3.14f * 2 * (attr.bounceMove - 1) / 4;
			}
			else if (y + sz >= 1.0f) {
				attr.bounceMove = 3;
				attr.triRotateAngle = 3.14f * 2 * (attr.bounceMove - 1) / 4;
			}
			break;
		case 3: // 남서
			x -= 0.01f;
			y -= 0.01f;
			if (x - sz <= -1.0f) {
				attr.bounceMove = 4;
				attr.triRotateAngle = 3.14f * 2 * (attr.bounceMove - 1) / 4;
			}
			else if (y - sz <= -1.0f) {
				attr.bounceMove = 2;
				attr.triRotateAngle = 3.14f * 2 * (attr.bounceMove - 1) / 4;
			}
			break;
		case 4: // 남동
			x += 0.01f;
			y -= 0.01f;
			if (x + sz >= 1.0f) {
				attr.bounceMove = 3;
				attr.triRotateAngle = 3.14f * 2 * (attr.bounceMove - 1) / 4;
			}
			else if (y - sz <= -1.0f) {
				attr.bounceMove = 1;
				attr.triRotateAngle = 3.14f * 2 * (attr.bounceMove - 1) / 4;
			}
			break;
		}
		mat = glm::identity<glm::mat4>();
		mat = glm::translate(mat, glm::vec3(x, y, 0));
		mat = glm::rotate(mat, attr.triRotateAngle, glm::vec3(0, 0, 1));
		mat = glm::translate(mat, glm::vec3(-x, -y, 0));
	}
	void ZigzagMove() {
		switch (attr.ZigzagRight) {
		case true:
			x += 0.015625f;
			if (x + sz >= 1.0f) {
				attr.ZigzagRight ^= 1;
				attr.ZigzagUp ? y += 0.015625f : y -= 0.015625f;
				if (y + sz >= 1.0f) {
					attr.ZigzagUp = false;
					y = 1.0f - sz;
				} else if (y - sz <=  -1.0f) {
					attr.ZigzagUp = true;
					y = -1.0f + sz;
				}
				attr.triRotateAngle = 3.14f  / 2;
			}
			break;
		case false:
			x -= 0.015625f;
			if (x - sz <= -1.0f) {
				attr.ZigzagRight ^= 1;
				attr.ZigzagUp ? y += 0.015625f : y -= 0.015625f;
				if (y + sz >= 1.0f) {
					attr.ZigzagUp = false;
					y = 1.0f - sz;
				} else if (y - sz <= -1.0f) {
					attr.ZigzagUp = true;
					y =  -1.0f + sz;
				}
				attr.triRotateAngle = 3.14f * 3 / 2;
			}
			break;
		}
		mat = glm::identity<glm::mat4>();
		mat = glm::translate(mat, glm::vec3(x, y, 0));
		mat = glm::rotate(mat, attr.triRotateAngle, glm::vec3(0, 0, 1));
		mat = glm::translate(mat, glm::vec3(-x, -y, 0));
	}
	void SpikeZigzagMove() {
		switch (attr.SpikeZigZag) {
		case 1:
			x += 0.015625f;
			y += 0.015625f * 2;
			if (x + sz >= 1.0f) {
				attr.SpikeZigZag = 4;
				attr.triRotateAngle = 0;
			}
			else if (y + sz >= 1.0f) {
				attr.SpikeZigZag = 2;
				attr.triRotateAngle = 3.14f;
			}
			break;
		case 2:
			x += 0.015625f;
			y -= 0.015625f * 2;
			if (x + sz >= 1.0f) {
				attr.SpikeZigZag = 3;
				attr.triRotateAngle = 3.14f;
			}
			else if (y - sz <= -1.0f) {
				attr.SpikeZigZag = 1;
				attr.triRotateAngle = 0;
			}
			break;
		case 3:
			x -= 0.015625f;
			y -= 0.015625f * 2;
			if (x - sz <= -1.0f) {
				attr.SpikeZigZag = 2;
				attr.triRotateAngle = 3.14f;
			}
			else if (y - sz <= -1.0f) {
				attr.SpikeZigZag = 4;
				attr.triRotateAngle = 0;
			}
			break;
		case 4:
			x -= 0.015625f;
			y += 0.015625f * 2;
			if (x - sz <= -1.0f) 
			{
				attr.SpikeZigZag = 1;
				attr.triRotateAngle = 0;
			}
			else if (y + sz >= 1.0f)
			{
				attr.SpikeZigZag = 3;
				attr.triRotateAngle = 3.14f;
			}
			break;
		}
		mat = glm::identity<glm::mat4>();
		mat = glm::translate(mat, glm::vec3(x, y, 0));
		mat = glm::rotate(mat, attr.triRotateAngle, glm::vec3(0, 0, 1));
		mat = glm::translate(mat, glm::vec3(-x, -y, 0));
	}
	void CircleSpiralMove() {
		attr.r += 0.001f;
		if (attr.rotateRight) {
			attr.angle -= 0.1f;
			x = attr.CircleX + attr.r * cos(attr.angle);
			y = attr.CircleY + attr.r * sin(attr.angle);
		}
		else {
			attr.angle += 0.1f;
			x = attr.CircleX + attr.r * cos(attr.angle);
			y = attr.CircleY + attr.r * sin(attr.angle);
		}
		if (x + sz >= 1.0f or x - sz <= -1.0f or
			y + sz >= 1.0f or y - sz <= -1.0f) {
			attr.rotateRight ^= 1;
			x = attr.CircleX;
			y = attr.CircleY;
			attr.r = 0;
			attr.angle = attr.rotateRight ? 3.14f : 0.0f;
			que.clear();
		}

		mat = glm::identity<glm::mat4>();
		mat = glm::translate(mat, glm::vec3(x, y, 0));
		mat = glm::rotate(mat, attr.triRotateAngle + attr.angle, glm::vec3(0, 0, 1));
		mat = glm::translate(mat, glm::vec3(-x, -y, 0));
	}
};

std::vector<Triangle> triangles;

bool timer = false;
double dtime = 1;
double fps = 60;

void DrawAxis() {
	GLfloat line[] = {
		-1.0f, 0.0f, 0.0f, 0.0f,0.0f,0.0f,
		1.0f, 0.0f, 0.0f, 0.0f,0.0f,0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,0.0f,0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,0.0f,0.0f,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_LINES, 0, 4);
}
void OpenGL::SetBuffer()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

void OpenGL::startPractice()
{
	timer = true;
	dtime = 1 / fps;

	triangles.push_back(Triangle(0.4f, 0.4f));
	triangles.push_back(Triangle(-0.4f, 0.4f));
	triangles.push_back(Triangle(-0.4f, -0.4f));
	triangles.push_back(Triangle(0.4f, -0.4f));
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

	DrawAxis();
	// 그리기
	for (const auto& triangle : triangles) 
	{
		if (move != Move::None)triangle.drawDegree();
		triangle.draw(shader);
	}


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
			for (auto& tri : triangles) tri.Moving();
		}
	} else {
		first = glfwGetTime();
	}
}

// 폴링 함수
void OpenGL::keyboard() {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// 콜백 함수
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {}
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_RELEASE) return;
	double x, y;
	int width, height;
	glfwGetCursorPos(window, &x, &y);
	glfwGetWindowSize(window, &width, &height);
	float dx = (x - width / 2.f) / (width / 2.f);
	float dy = (height / 2.f - y) / (height / 2.f);

	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
	{
		Triangle tri{ dx, dy };
		if (dx > 0 and dy > 0) triangles[0] = tri;
		else if (dx < 0 and dy > 0) triangles[1] = tri;
		else if (dx < 0 and dy < 0) triangles[2] = tri;
		else if (dx > 0 and dy < 0) triangles[3] = tri;
	}
	break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		if (dx > 0 and dy > 0) triangles[0].resize(0.01f);
		else if (dx < 0 and dy > 0) triangles[1].resize(0.01f);
		else if (dx < 0 and dy < 0) triangles[2].resize(0.01f);
		else if (dx > 0 and dy < 0) triangles[3].resize(0.01f);
		break;
	}
}
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {}
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {}
void CharCallback(GLFWwindow* window, unsigned int codepoint)
{
	switch (codepoint) {
	case 'q':
		glfwSetWindowShouldClose(window, true);
		return;
	case 'a': case 'b':
		drawMode = codepoint - 'a';
		break;
	case 'c':
		triangles.clear();
		triangles.push_back(Triangle(0.4f, 0.4f));
		triangles.push_back(Triangle(-0.4f, 0.4f));
		triangles.push_back(Triangle(-0.4f, -0.4f));
		triangles.push_back(Triangle(0.4f, -0.4f));
		break;
	case '1': // 튕기기 이동
		move = Move::Bounce;
		for (auto& tri : triangles) tri.clearQue();
		break;
	case '2': // 좌우 지그재그 이동
		move = Move::ZigZag;
		for (auto& tri : triangles) tri.clearQue();
		break;
	case '3': // 상하로 뾰족 지그재그 이동
		move = Move::SpikeZigZag;
		for (auto& tri : triangles) tri.clearQue();
		break;
	case '4': // 원 스파이럴 이동 -> 반지름이 커지면서 돌아감
		if(move != Move::CircleSpiral) for (int i = 0; i < triangles.size(); ++i) triangles[i].setting();
		move = Move::CircleSpiral;
		for (auto& tri : triangles) tri.clearQue();
		break;
	}
}
void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {}
void windowSizeCallback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }
