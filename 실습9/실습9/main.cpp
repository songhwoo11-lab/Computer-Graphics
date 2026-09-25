#include"OpenGL.h"

int main() {
	OpenGL opengl;
	opengl.init_GLFW();
	opengl.init_CreateWindow("practice 9");
	opengl.init_GLEW();
	if (opengl.AttachShader("vertex.glsl", "fragment.glsl") == -1) {
		exit(1);
	}
	opengl.SetBuffer();
	opengl.startPractice();
	opengl.mainloop();
}