#version 330 core

//Textures
uniform sampler2D diffuseTex;

//Lightings
uniform vec3 lightPos;
uniform vec4 lightColor;

in Vertex 
{
	vec2 texCoord;
	vec3 normal;
	vec3 fragWorldPos;
} IN;

out vec4 fragColour;

void main(void) 
{
	vec3 diffuseColor = texture(diffuseTex, IN.texCoord).rgb;

	float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * vec3(lightColor.xyz);
    vec3 result = ambient * diffuseColor;

	fragColour = vec4(result, 1.0);
}