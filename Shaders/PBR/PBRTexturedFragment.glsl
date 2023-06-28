#version 420 core

//Textures
uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D metallicTex;
uniform sampler2D roughnessTex;
uniform sampler2D emissiveTex;
uniform samplerCube irradianceTex;

//Flags

//Lightings
uniform vec3 cameraPos;

const float GAMMA = 2.2;
const int MAX_POINT_LIGHTS = 100;
const int MAX_SPOT_LIGHTS = 2;
const float PI = 3.14159265359;

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
	vec3 position;
	vec2 texCoord;
	vec3 normal;
	vec3 fragWorldPos;
	vec3 tangent;
	vec3 bitangent;
	mat3 TBN;
} IN;

out vec4 fragColour;

float DistributionGGX(vec3 N, vec3 H, float roughnessStrength)
{
	float numerator = roughnessStrength * roughnessStrength;
	
	float NdotH = max(dot(N, H), 0.0);
	float NdotHSq = NdotH * NdotH;

	float denominator = (NdotHSq * (numerator - 1.0) + 1.0);
	denominator = PI * denominator * denominator;
	
	return numerator / denominator;
}

float GeometrySchlickGGX(float dot, float roughnessStrength)
{
	float numerator = dot;
	float k = roughnessStrength * roughnessStrength;

	float denominator = dot / (1.0 - k) + k;

	return numerator / denominator;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughnessStrength)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);

	//float NdotV = dot(N, V);
	//float NdotL = dot(N, L);

	float GSubOne = GeometrySchlickGGX(NdotV, roughnessStrength);
	float GSubTwo = GeometrySchlickGGX(NdotL, roughnessStrength);

	return GSubOne * GSubTwo;
}

vec3 FresnelSchlick(float cos, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cos, 5.0);
}

vec3 FresnelSchlickRoughness(float cos, vec3 F0, float roughnessStrength)
{
	return F0 + (max(vec3(1.0 - roughnessStrength), F0) - F0) * pow(clamp(1.0 - cos, 0.0, 1.0), 5.0);
}

void CalcDirectionalLight(inout vec3 result, vec3 albedoColor, float metallicStrength, float roughnessStrength, vec3 N, vec3 V, vec3 F0)
{
	vec3 L = normalize(-directionalLight.lightDirection.xyz);
	vec3 H = normalize(V + L);

	vec3 radiance = directionalLight.lightColor.xyz;

	float NDF = DistributionGGX(N, H, roughnessStrength);
	float G = GeometrySmith(N, V, L, roughnessStrength);
	vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallicStrength;

	//Cook-Torrance BRDF
	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
	vec3 specular = numerator / denominator;

	float NdotL = max(dot(N, L), 0.0);
	result = (kD * albedoColor / PI + specular) * radiance * NdotL;
}

void CalcPointLights(inout vec3 result, vec3 albedoColor, float metallicStrength, float roughnessStrength, vec3 N, vec3 V, vec3 F0)
{
	for(int i = 0; i < numPointLights; i++)
	{
		vec3 L = normalize(pointLights[i].lightPosition.xyz - IN.fragWorldPos);
		vec3 H = normalize(V + L);

		float distance = length(pointLights[i].lightPosition.xyz - IN.fragWorldPos);
		float attenuation = 1.0 / (distance * distance);
		//float attenuation = 1.0 / (pointLights[i].lightAttenData.x + pointLights[i].lightAttenData.y * distance + pointLights[i].lightAttenData.z * (distance * distance));
		vec3 radiance = pointLights[i].lightColor.xyz * attenuation;

		float NDF = DistributionGGX(N, H, roughnessStrength);
		float G = GeometrySmith(N, V, L, roughnessStrength);
		vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallicStrength;

		//Cook-Torrance BRDF
		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
		vec3 specular = numerator / denominator;

		float NdotL = max(dot(N, L), 0.0);
		result += (kD * albedoColor / PI + specular) * radiance * NdotL;
	}
}

void CalcAmbientLight(inout vec3 result, vec3 albedoColor, float metallicStrength, vec3 N, vec3 V, vec3 F0)
{
	vec3 kS = FresnelSchlick(max(dot(N, V), 0.0), F0);
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallicStrength;

	vec3 irradiance = texture(irradianceTex, N).rgb;
	vec3 diffuse = irradiance * albedoColor;
	vec3 ambient = (kD * diffuse);

	//vec3 ambient = vec3(0.03) * albedoColor;

	result += ambient;
}

void main(void) 
{
	vec3 albedoColor = texture(albedoTex, IN.texCoord).rgb;
	albedoColor = pow(albedoColor, vec3(GAMMA));

	vec3 normalColor = texture(normalTex, IN.texCoord).rgb;
	normalColor = normalColor * 2.0 - 1.0;
	normalColor.xy *= 1.0;
	normalColor = normalize(IN.TBN * normalColor);

	float metallicStrength = texture(metallicTex, IN.texCoord).r;
	float roughnessStrength = texture(roughnessTex, IN.texCoord).r;

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedoColor, metallicStrength);

	vec3 N = normalize(normalColor);
	vec3 V = normalize(cameraPos - IN.fragWorldPos);

	//float metallicStrength = 1.0;
	//float roughnessStrength = 0.1;

	vec3 result = vec3(0.0);
	CalcDirectionalLight(result, albedoColor, metallicStrength, roughnessStrength, N, V, F0);
	CalcPointLights(result, albedoColor, metallicStrength, roughnessStrength, N, V, F0);
	CalcAmbientLight(result, albedoColor, metallicStrength, N, V, F0);

	vec3 emissiveColor = texture(emissiveTex, IN.texCoord).rgb;
	result += emissiveColor;

	result = vec3(1.0) - exp(-result * 10.0);
	result = pow(result, vec3(1.0 / GAMMA));

	vec3 metallicColor = texture(metallicTex, IN.texCoord).rgb;
	vec3 roughnessColor = texture(roughnessTex, IN.texCoord).rgb;

	fragColour = vec4(result, 1.0);
}