#version 420 core

//Textures
uniform sampler2D albedoTex;
uniform sampler2D normalTex;

//Lightings
uniform vec3 cameraPos;

const int MAX_POINT_LIGHTS = 100;
const int MAX_SPOT_LIGHTS = 2;
struct PointLight
{
	vec4 lightPosition;
	vec4 lightColor;
	vec4 lightAttenData;
};

struct DirectionalLight
{
	vec4 lightDirection;
	vec4 lightColor;
};

struct SpotLight
{
	vec4 lightPosition;
	vec4 lightDirection;
	vec4 lightColor;
	vec4 lightAttenData;
	vec4 lightCutoffData;
};

layout(std140, binding = 1) uniform u_PointLights
{
	int numPointLights;
	PointLight pointLights[MAX_POINT_LIGHTS];
};

layout(std140, binding = 2) uniform u_DirectionLight
{
	DirectionalLight directionalLight;
};

layout(std140, binding = 3) uniform u_SpotLights
{
	int numSpotLights;
	SpotLight spotLights[MAX_SPOT_LIGHTS];
};

in Vertex 
{
	vec2 texCoord;
	vec3 normal;
	vec3 fragWorldPos;
	vec3 tangent;
	vec3 bitangent;
	mat3 TBN;
} IN;

out vec4 fragColour;

void CalcDirectionalLight(inout vec3 result, in vec3 albedoColor, in vec3 normalColor)
{
	float ambientStrength = 0.3;
	vec3 ambient = ambientStrength * vec3(directionalLight.lightColor.xyz);

	vec3 norm = normalize(normalColor);
	vec3 lightDir = normalize(-directionalLight.lightDirection.xyz);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(directionalLight.lightColor.xyz);

	float specularStrength = 1.0;
	vec3 viewDir = normalize(cameraPos - IN.fragWorldPos);
	vec3 viewDirHalf = normalize(lightDir + viewDir);
	//vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(norm, viewDirHalf), 0.0), 128.0);
	vec3 specular = specularStrength * spec * vec3(directionalLight.lightColor.xyz);

	result = (ambient + diffuse + specular) * albedoColor;
}

void CalcPointsLights(inout vec3 result, in vec3 albedoColor, in vec3 normalColor)
{
	for(int i = 0; i < numPointLights; i++)
	{
		float ambientStrength = 0.3;
		vec3 ambient = ambientStrength * vec3(pointLights[i].lightColor.xyz);

		vec3 norm = normalize(normalColor);
		vec3 lightDir = normalize(pointLights[i].lightPosition.xyz - IN.fragWorldPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * vec3(pointLights[i].lightColor.xyz);

		float specularStrength = 1.0;
		vec3 viewDir = normalize(cameraPos - IN.fragWorldPos);
		vec3 viewDirHalf = normalize(lightDir + viewDir);
		//vec3 reflectDir = reflect(-lightDir, norm);

		float spec = pow(max(dot(norm, viewDirHalf), 0.0), 128.0);
		vec3 specular = specularStrength * spec * vec3(pointLights[i].lightColor.xyz);

		float distance = length(pointLights[i].lightPosition.xyz - IN.fragWorldPos);
		float attenuation = 1.0 / (pointLights[i].lightAttenData.x + pointLights[i].lightAttenData.y * distance + pointLights[i].lightAttenData.z * (distance * distance));
		//float attenuation = 1.0 / distance * distance;

		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;

		result += (ambient + diffuse + specular) * albedoColor;
	}
}

void CalcSpotLights(inout vec3 result, in vec3 albedoColor, in vec3 normalColor)
{
	for(int i = 0; i < numSpotLights; i++)
	{
		vec3 lightDir = normalize(spotLights[i].lightPosition.xyz - IN.fragWorldPos);
		float theta = dot(lightDir, normalize(-spotLights[i].lightDirection.xyz));
		float epsilon = spotLights[i].lightCutoffData.x - spotLights[i].lightCutoffData.y;
		float edgeFactor = clamp((theta - spotLights[i].lightCutoffData.y) / epsilon, 0.0, 1.0);

		float ambientStrength = 0.3;
		vec3 ambient = ambientStrength * vec3(spotLights[i].lightColor.xyz);
			
		vec3 norm = normalize(normalColor);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * vec3(spotLights[i].lightColor.xyz);

		float specularStrength = 1.0;
		vec3 viewDir = normalize(cameraPos - IN.fragWorldPos);
		vec3 viewDirHalf = normalize(lightDir + viewDir);
		//vec3 reflectDir = reflect(-lightDir, norm);

		float spec = pow(max(dot(norm, viewDirHalf), 0.0), 128.0);
		vec3 specular = specularStrength * spec * vec3(spotLights[i].lightColor.xyz);

		float distance = length(spotLights[i].lightPosition.xyz - IN.fragWorldPos);
		float attenuation = 1.0 / (spotLights[i].lightAttenData.x + spotLights[i].lightAttenData.y * distance + spotLights[i].lightAttenData.z * (distance * distance));
		//float attenuation = 1.0 / distance * distance;

		ambient *= attenuation * edgeFactor;
		diffuse *= attenuation * edgeFactor;
		specular *= attenuation * edgeFactor;

		result += (ambient + diffuse + specular) * albedoColor;
	}
}

void main(void) 
{
	vec3 albedoColor = texture(albedoTex, IN.texCoord).rgb;
	vec3 normalColor = texture(normalTex, IN.texCoord).rgb;
	normalColor = normalColor * 2.0 - 1.0;
	normalColor.xy *= 1.5;
	normalColor = normalize(IN.TBN * normalColor);

	vec3 result = vec3(0.0);
	CalcDirectionalLight(result, albedoColor, normalColor);
	
	if(numPointLights > 0)
		CalcPointsLights(result, albedoColor, normalColor);

	if(numSpotLights > 0)
		CalcSpotLights(result, albedoColor, normalColor);

	//Gamma
	//result = pow(result, vec3(1.0 / 2.2));
	fragColour = vec4(result, 1.0);	
}