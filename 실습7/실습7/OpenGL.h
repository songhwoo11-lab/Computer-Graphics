#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<random>
#include<gl/glew.h>
#include<gl/glfw3.h>
#pragma once

class Shader {
public:
	int LoadShader(std::string_view vertexfile, std::string_view fragfile);
	GLuint GetShaderID() const { return shaderID; }
private:
	GLuint shaderID = 0;

	GLuint CreateVertexShader(std::string_view file);
	GLuint CreateFragmentShader(std::string_view file);
	std::string filetobuf(std::string_view filename);
};

class OpenGL {
public:
	OpenGL() : window{ nullptr }, width{800}, height{600} {}
	~OpenGL();
	int init_GLFW();
	int init_CreateWindow(std::string_view title);
	int init_GLEW();
	int AttachShader(std::string_view vertexfile, std::string_view fragfile);
	void SetViewport(GLint x, GLint y, GLsizei width, GLsizei height) { glViewport(x, y, width, height); }
	void SetBuffer();
	void mainloop();
	void SetCallBack();
	void startPractice();
private:
	GLFWwindow* window;
	Shader shader{};
	GLuint VAO = 0;
	GLuint VBO = 0;
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