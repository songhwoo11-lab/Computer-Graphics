#pragma once

class OpenGL {
public:
	OpenGL() : window{ nullptr }, width{800}, height{800} {}
	~OpenGL();
	int init_GLFW();
	int init_CreateWindow(std::string_view title);
	int init_GLEW();
	void SetViewport(GLint x, GLint y, GLsizei width, GLsizei height) { glViewport(x, y, width, height); }
	void mainloop();
	void SetCallBack();
	void startPractice();
private:
	GLFWwindow* window;
	int width, height;
	void keyboard();
	void rendering();
	void Timer(double);
};

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void CharCallback(GLFWwindow* window, unsigned int codepoint);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void windowSizeCallback(GLFWwindow* window, int width, int height);