#version 420 core

uniform sampler2D srcTexture;
uniform float brightnessThreshold;

in Vertex
{
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void)
{
	vec3 result = texture(srcTexture, IN.texCoord).rgb;
	float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
	if(brightness >= brightnessThreshold)
		fragColour = vec4(result, 1.0);
	else
		fragColour = vec4(0.0, 0.0, 0.0, 0.0);
}