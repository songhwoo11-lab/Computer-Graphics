#include<iostream>
#include<gl/glew.h>
#include<gl/glfw3.h>
#include"OpenGL.h"




int main() {
	OpenGL opengl;
	opengl.init_GLFW();
	opengl.init_CreateWindow();
	opengl.init_GLEW();
	
	opengl.mainloop();
}