#version 420 core

uniform sampler2D srcTexture;
uniform sampler2D postProcessTexture;

in Vertex
{
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void)
{
	vec3 srcColor = texture(srcTexture, IN.texCoord).rgb;
	vec3 postColor = texture(postProcessTexture, IN.texCoord).rgb;

	vec3 result = srcColor + postColor;

	fragColour = vec4(result, 1.0);
}