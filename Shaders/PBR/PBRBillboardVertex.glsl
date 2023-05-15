#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec3 u_WorldPos;
uniform vec3 u_CameraUpWorld;
uniform vec3 u_CameraRightWorld;

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoord;

out Vertex 
{
	vec2 texCoord;	
} OUT;

void main(void) 
{
	/*mat4 modelView = viewMatrix * modelMatrix;
	modelView[0][0] = 1;
	modelView[0][1] = 0;
	modelView[0][2] = 0;

	modelView[1][0] = 0;
	modelView[1][1] = 1;
	modelView[1][2] = 0;

	modelView[2][0] = 0;
	modelView[2][1] = 0;
	modelView[2][2] = 1;	 
	gl_Position = projMatrix * modelView * vec4(position, 1.0);*/

	vec2 size = vec2(0.5f, 0.5f);

	mat4 viewProj = projMatrix * viewMatrix;
	vec3 pos_worldSpace = u_WorldPos + u_CameraRightWorld * size.x + u_CameraUpWorld * size.y;
	gl_Position = viewProj * vec4(pos_worldSpace, 1.0);


	OUT.texCoord = texCoord;
}