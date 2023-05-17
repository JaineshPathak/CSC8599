#version 420 core

//Textures
uniform sampler2D albedoTex;

//Lightings
uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec4 lightColor;

const int MAX_POINT_LIGHTS = 100;
struct PointLights
{
	Vector3 lightPosition;
	Vector4 lightColor;
};

layout(std140, binding = 1) uniform PointLight
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

	float ambientStrength = 0.5;
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

    vec3 result = (ambient + diffuse + specular) * albedoColor;

	fragColour = vec4(result, 1.0);
}