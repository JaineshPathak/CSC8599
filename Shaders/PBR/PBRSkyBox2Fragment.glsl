#version 420 core

layout(std140, binding = 4) uniform u_SkyboxData
{
	vec4 skyboxData;
};

uniform samplerCube cubeTex;

in Vertex
{
	vec3 viewDir;
} IN;

out vec4 fragColour;

void main(void)
{
	vec3 cubeColor = texture(cubeTex, IN.viewDir).rgb;
	//cubeColor = pow(cubeColor, vec3(skyboxData.y));
	//vec3 cubeColor = textureLod(cubeTex, IN.viewDir, 1.0).rgb;
	
	//cubeColor = cubeColor / (cubeColor + vec3(1.0));
	//cubeColor = vec3(1.0) - exp(-cubeColor * skyboxData.x);
	//cubeColor = pow(cubeColor, vec3(1.0 / skyboxData.y));

	fragColour = vec4(cubeColor, 1.0);
}