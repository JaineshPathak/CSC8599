#version 420 core

//Textures
uniform sampler2D albedoTex;

//Lightings
uniform vec3 cameraPos;

const int MAX_POINT_LIGHTS = 100;
struct PointLight
{
	vec3 lightPosition;
	vec4 lightColor;
};

layout(std140, binding = 1) uniform PointLights
{
	int numPointLights;
	PointLight pointLights[MAX_POINT_LIGHTS];
};

in Vertex 
{
	vec2 texCoord;
	vec3 normal;
	vec3 fragWorldPos;
} IN;

out vec4 fragColour;

void main(void) 
{
	vec3 albedoColor = texture(albedoTex, IN.texCoord).rgb;

	/*float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * vec3(lightColor.xyz);

	vec3 norm = normalize(IN.normal);
	vec3 lightDir = normalize(lightPos - IN.fragWorldPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(lightColor.xyz);

	float specularStrength = 1.0;
	vec3 viewDir = normalize(cameraPos - IN.fragWorldPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * vec3(lightColor.xyz);

    vec3 result = (ambient + diffuse + specular) * albedoColor;*/

	vec3 result = vec3(0.0);
	for(int i = 0; i < numPointLights; i++)
	{
		float ambientStrength = 0.5;
		vec3 ambient = ambientStrength * vec3(pointLights[i].lightColor.xyz);

		vec3 norm = normalize(IN.normal);
		vec3 lightDir = normalize(pointLights[i].lightPosition - IN.fragWorldPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * vec3(pointLights[i].lightColor.xyz);

		float specularStrength = 1.0;
		vec3 viewDir = normalize(cameraPos - IN.fragWorldPos);
		vec3 reflectDir = reflect(-lightDir, norm);

		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		vec3 specular = specularStrength * spec * vec3(pointLights[i].lightColor.xyz);

		result += (ambient + diffuse + specular) * albedoColor;
	}

	fragColour = vec4(result, 1.0);
}