#version 420 core

uniform sampler2D srcTexture;
uniform sampler2D postProcessTexture;
uniform float bloomStrength;

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

	vec3 srcTex = texture(srcTexture, IN.texCoord).rgb;
	vec3 postTex =  texture(postProcessTexture, IN.texCoord).rgb;

	vec3 result = mix(srcTex, postTex, bloomStrength);
	result = pow(result, vec3(m_GAMMA));

	result = vec3(1.0) - exp(-result * m_Exposure);
	result = pow(result, vec3(1.0 / m_GAMMA));

    fragColour = vec4(result, 1.0);
}