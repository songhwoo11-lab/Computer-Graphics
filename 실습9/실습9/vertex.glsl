#version 330 core

uniform mat4 rotate;
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vColor;

out vec3 outColor;

void main()
{
	gl_Position = rotate * vec4(vPos, 1.0);
	outColor = vColor;
}
