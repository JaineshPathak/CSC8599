#version 330 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoord;

uniform mat4 billboardMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec3 u_WorldPos;
uniform vec3 u_CameraUpWorld;
uniform vec3 u_CameraRightWorld;

out Vertex 
{
	vec2 texCoord;
} OUT;

void main(void) 
{
	mat4 mvp = projMatrix * viewMatrix * billboardMatrix;
	gl_Position = mvp * vec4(position, 1.0);

	OUT.texCoord = texCoord;
}