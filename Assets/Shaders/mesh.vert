#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vUV0;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec4 vTanget;

layout (location = 0) out vec3 fColor;

layout(push_constant) uniform constants
{
	mat4 N;
	mat4 MVP;
} uCam;

void main()
{
	gl_Position = uCam.MVP * vec4(vPosition, 1.0);
	fColor = vNormal;
}