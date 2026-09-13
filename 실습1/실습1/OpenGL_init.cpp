#include<iostream>
#include<gl/glew.h>
#include<gl/glfw3.h>
#include "OpenGL.h"


#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

OpenGL::~OpenGL() {
	//--- 종료 처리
	glfwDestroyWindow(window);
	glfwTerminate();
	std::cout << "glfw end" << std::endl;
}
int OpenGL::init_GLFW() {
	//--- GLFW 초기화
	if (!glfwInit()) {
		std::cerr << "failed initializing GLFW!" << std::endl;
		return -1;
	}
	std::cerr << "completed initializing GLFW" << std::endl;
	//--- OpenGL 버전 설정(예: 3.3 Core Profile)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	return 0;
}
int OpenGL::init_CreateWindow()
{
	//--- 윈도우생성
	window = glfwCreateWindow(800, 600, "OpenGL Window", nullptr, nullptr);
	if (not window) {
		std::cerr << "failed creating window" << std::endl;
		glfwTerminate();
		return -1;
	}
	std::cout << "completed creating window" << std::endl;
	//--- 컨텍스트설정
	glfwMakeContextCurrent(window);
	return 0;
}
int OpenGL::init_GLEW() {
	//--- GLEW 초기화
	glewExperimental = GL_TRUE; // 최신 기능 사용
	if (glewInit() != GLEW_OK) {
		std::cerr << "failed initializing GLEW!" << std::endl;
		return -1;
	}
	std::cout << "completed initializing GLEW" << std::endl;
	return 0;
}
