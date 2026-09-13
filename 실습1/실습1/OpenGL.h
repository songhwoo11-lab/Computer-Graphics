#pragma once
class OpenGL {
public:
	OpenGL() : window{ nullptr } {}
	~OpenGL();
	int init_GLFW();
	int init_CreateWindow();
	int init_GLEW();
	void SetViewport(GLint x, GLint y, GLsizei width, GLsizei height) { glViewport(x, y, width, height); }
	void mainloop();

private:
	GLFWwindow* window;
	void keyboard();
	void rendering();
	void Timer(double);
};

