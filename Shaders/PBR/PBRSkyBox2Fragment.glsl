#version 420 core

uniform samplerCube cubeTex;

in Vertex
{
	vec3 viewDir;
} IN;

out vec4 fragColour;

void main(void)
{
	vec3 cubeColor = texture(cubeTex, IN.viewDir).rgb;
	cubeColor = cubeColor / (cubeColor + vec3(1.0));
	cubeColor = pow(cubeColor, vec3(1.0 / 2.2));

	fragColour = vec4(cubeColor, 1.0);
}