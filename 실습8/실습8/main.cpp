#include<iostream>
#include<gl/glew.h>
#include<gl/glfw3.h>
#include"OpenGL.h"

int main() {
	OpenGL opengl;
	opengl.init_GLFW();
	opengl.init_CreateWindow("practice 8");
	opengl.init_GLEW();
	if (opengl.AttachShader("vertex.glsl", "fragment.glsl") == -1) {
		exit(1);
	}
	opengl.SetBuffer();
	opengl.startPractice();
	opengl.mainloop();
}