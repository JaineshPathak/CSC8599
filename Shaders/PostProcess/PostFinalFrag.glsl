#version 420 core

uniform sampler2D srcTexture;
uniform sampler2D postProcessTexture;

layout(std140, binding = 4) uniform u_SkyboxData
{
	vec4 skyboxData;
};

in Vertex
{
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void)
{
	float m_GAMMA = skyboxData.y;
	float m_Exposure = skyboxData.x;

	vec3 result = texture(srcTexture, IN.texCoord).rgb;
	/*result = pow(result, vec3(2.2));

	result = vec3(1.0) - exp(-result * 5.0);
	result = pow(result, vec3(1.0 / 2.2));*/

    fragColour = vec4(result, 1.0);
}