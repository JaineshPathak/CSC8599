#version 420 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;

layout(std140, binding = 0) uniform Matrices
{
	mat4 projMatrix;
	mat4 viewMatrix;
};

uniform mat4 modelMatrix;

out Vertex 
{
	vec2 texCoord;
	vec3 normal;
	vec3 fragWorldPos;
} OUT;

void main(void) 
{
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(position, 1.0);

	//OUT.texCoord = (textureMatrix * vec4(texCoord , 0.0, 1.0)).xy;
	OUT.texCoord = texCoord;
	OUT.normal = mat3(transpose(inverse(modelMatrix))) * normal;
	OUT.fragWorldPos = vec3(modelMatrix * vec4(position, 1.0));
}