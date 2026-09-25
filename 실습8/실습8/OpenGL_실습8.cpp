#include "OpenGL.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

std::random_device rd;
std::mt19937 dre{ rd() }; 
short drawMode;

class Triangle {
public:
	Triangle(GLfloat x, GLfloat y) : vertexs{ std::vector<GLfloat>(18) }, isSizeUp{true}
	{
		std::uniform_int_distribution col{ 0, 255 }, size{ 5, 15 };
		sz = { size(dre) / 100.f };
		GLfloat r{ col(dre) / 255.f }, g{ col(dre) / 255.f }, b{ col(dre) / 255.f };
		for (int i = 3; i < vertexs.size(); i += 6) vertexs[i] = r;
		for (int i = 4; i < vertexs.size(); i += 6) vertexs[i] = g;
		for (int i = 5; i < vertexs.size(); i += 6) vertexs[i] = b;

		for (int i = 2; i < vertexs.size(); i += 6) vertexs[i] = 0.0f;
		vertexs[0] = x - sz ;
		vertexs[1] = y - sz;

		vertexs[6] = x + sz;
		vertexs[7] = y - sz;

		vertexs[12] = x;
		vertexs[13] = y + sz;
	}
	void resize(GLfloat dsize) 
	{
		if ((not isSizeUp and sz + dsize <= 0.05f) or (isSizeUp and sz + dsize >= 0.2f)) {
			isSizeUp ^= 1;
		}
		switch (isSizeUp) {
		case true:
			vertexs[0] -= dsize;
			vertexs[1] -= dsize;

			vertexs[6] += dsize;
			vertexs[7] -= dsize;

			vertexs[13] += dsize;
			sz += dsize;
			break;
		case false:
			vertexs[0] -=  -dsize;
			vertexs[1] -=  -dsize;

			vertexs[6] +=  -dsize;
			vertexs[7] -=  -dsize;

			vertexs[13] += -dsize;
			sz -= dsize;
		}

	}
	GLfloat getSize() const { return sz; }
	void draw() const
	{
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexs.size(), vertexs.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glDrawArrays(drawMode == 0 ? GL_TRIANGLES : GL_LINE_LOOP, 0, 3);
	}
private:
	std::vector<GLfloat> vertexs;
	GLfloat sz;
	bool isSizeUp;
};

std::vector<Triangle> triangles;

bool timer = false;
double dtime = 1;

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
	for (const auto& triangle : triangles) triangle.draw();

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
	case 'a': case 'b':
		drawMode = codepoint - 'a';
		break;
	case 'c':
		triangles.clear();	
		triangles.push_back(Triangle(0.4f, 0.4f));
		triangles.push_back(Triangle(-0.4f, 0.4f));
		triangles.push_back(Triangle(-0.4f, -0.4f));
		triangles.push_back(Triangle(0.4f, -0.4f));
	}
}
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{

}
void windowSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}