
#include "OpenGL.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

OpenGL::~OpenGL() {
	//--- 종료 처리
	if (VAO) glDeleteVertexArrays(1, &VAO);
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
int OpenGL::init_CreateWindow(std::string_view title)
{
	//--- 윈도우생성
	window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
	if (not window) {
		std::cerr << "failed creating window" << std::endl;
		glfwTerminate();
		return -1;
	}
	std::cout << "completed creating window" << std::endl;
	//--- 컨텍스트설정
	glfwMakeContextCurrent(window);

	SetCallBack();
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


int OpenGL::AttachShader(std::string_view vertexfile, std::string_view fragfile) {
	return shader.LoadShader(vertexfile, fragfile);
}

void OpenGL::SetCallBack()
{
	glfwSetKeyCallback(window, keyboardCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetCharCallback(window, CharCallback);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	glfwSetWindowSizeCallback(window, windowSizeCallback);
}

GLuint Shader::CreateVertexShader(std::string_view filename)
{
	std::string vertexSource = filetobuf(filename);
	const GLchar* vs = vertexSource.c_str();
	GLuint shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shader, 1, &vs, NULL);
	glCompileShader(shader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if (!result) {
		glGetShaderInfoLog(shader, 512, NULL, errorLog);
		std::cerr << "vertex\n";
		std::cerr << "ERROR: 컴파일 실패\n" << errorLog << std::endl;
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}
GLuint Shader::CreateFragmentShader(std::string_view filename)
{
	std::string vertexSource = filetobuf(filename);
	const GLchar* vs = vertexSource.c_str();
	GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shader, 1, &vs, NULL);
	glCompileShader(shader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if (!result) {
		glGetShaderInfoLog(shader, 512, NULL, errorLog);
		std::cerr << "fragment\n";
		std::cerr << "ERROR: 컴파일 실패\n" << errorLog << std::endl;
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

	int Shader::LoadShader(std::string_view vertexfile, std::string_view fragfile) {

		GLuint vertexShader = CreateVertexShader(vertexfile);
		if (not vertexShader) return -1;
		GLuint fragmentShader = CreateFragmentShader(fragfile);
		if (not fragmentShader) return -1;

		shaderID = glCreateProgram();
		glAttachShader(shaderID, vertexShader);
		glAttachShader(shaderID, fragmentShader);

		glLinkProgram(shaderID);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		GLint result;
		GLchar errorLog[512];

		glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
		if (!result) {
			glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
			std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
			return -1;
		}

		glUseProgram(shaderID);
		return 0;
	}

std::string Shader::filetobuf(std::string_view filename) {
	std::ifstream in{ filename.data() };
	if (not in) {
		return "";
	}

	std::string source((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

	return source;
}
