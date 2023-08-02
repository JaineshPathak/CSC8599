#version 420 core

//Textures
uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D metallicTex;
uniform sampler2D roughnessTex;
uniform sampler2D emissiveTex;
uniform sampler2D occlusionTex;

uniform samplerCube irradianceTex;
uniform samplerCube prefilterTex;
uniform sampler2D brdfLUTTex;
uniform sampler2D ssaoTex;

//Flags
uniform bool ssaoEnabled;
uniform bool hasAlbedoTex = false;
uniform bool hasNormalTex = false;
uniform bool hasMetallicTex = false;
uniform bool hasRoughnessTex = false;
uniform bool hasEmissiveTex = false;
uniform bool hasOcclusionTex = false;

//Disney Vars
uniform vec3 baseColor = vec3(1.0);
uniform float metallic = 0.01;
uniform float subsurface = 0.0;
uniform float specular = 0.0;
uniform float roughness = 0.5;
uniform float specularTint = 0.0;
uniform float anisotropic = 0.0;
uniform float sheen = 1.0;
uniform float sheenTint = 0.5;
uniform float clearCoat = 0.0;
uniform float clearCoatRoughness = 1.0;
uniform float emission = 1.5;

//Lightings
uniform vec3 cameraPos;

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

layout(std140, binding = 4) uniform u_SkyboxData
{
	vec4 skyboxData;
};

in Vertex 
{
	vec3 position;
	vec2 texCoord;
	vec3 normal;
	vec3 fragWorldPos;
	vec4 fragClipSpacePos;
	vec3 tangent;
	vec3 bitangent;
	mat3 TBN;
} IN;

out vec4 fragColour;

float sqr(float x) 
{ 
	return x * x;
}

float FresnelSchlick(float cos)
{
	float m = clamp(1.0 - cos, 0.0, 1.0);
	return (m * m) * (m * m) * m;
}

vec3 FresnelSchlickRoughness(float cos, vec3 F0, float roughnessStrength)
{
	return F0 + (max(vec3(1.0 - roughnessStrength), F0) - F0) * pow(clamp(1.0 - cos, 0.0, 1.0), 5.0);
}

//Distribution Term D for Clear Coat using Trowbridge-Reitz
float GTR1(float NdotH, float a)
{
	if(a >= 1.0) return 1.0 / PI;

	float a2 = a * a;
	float t = 1.0 + (a2 - 1.0) * NdotH * NdotH;

	return (a2 - 1.0) / (PI * log(a2) * t);
}

float GTR2(float NdotH, float a)
{
	float a2 = a * a;
	float t = 1.0 + (a2 - 1.0) * NdotH * NdotH;

	return a2 / (PI * t * t);
}

float GTR2_Aniso(float NdotH, float HdotX, float HdotY, float ax, float ay)
{
	return 1.0 / (PI * ax * ay * sqr( sqr(HdotX / ax) + sqr(HdotY / ay) + NdotH * NdotH ));
}

float SmithG_GGX(float NdotV, float alphaG)
{
	float a = alphaG * alphaG;
	float b = NdotV * NdotV;
	return 1.0 / (NdotV + sqrt(a + b - a * b));
}

float SmithG_GGX_Aniso(float NdotV, float VdotX, float VdotY, float ax, float ay)
{
	return 1.0 / (NdotV + sqrt( sqr(VdotX * ax) + sqr(VdotY * ay) + sqr(NdotV) ));
}

vec3 Mon2Linear(vec3 x)
{
    return vec3(pow(x[0], 2.2), pow(x[1], 2.2), pow(x[2], 2.2));
}

//X - Tangent, Y - BiTangent or BiNormal
//Diffuse
vec3 DisneyDiffuse(const in vec3 albedoColor, const in float roughnessStrength, const in float NdotL, const in float NdotV, const in float LdotH)
{
	float FL = FresnelSchlick(NdotL);
	float FV = FresnelSchlick(NdotV);

	float Fd90 = 0.5 + 2.0 * LdotH * LdotH * roughnessStrength;
	float Fd = mix(1.0, Fd90, FL) * mix(1.0, Fd90, FV);

	return (1.0 / PI) * Fd * albedoColor;
}

//Diffuse Flatten
vec3 DisneySubsurface(const in vec3 albedoColor, const in float roughnessStrength, const in float NdotL, const in float NdotV, const in float LdotH)
{
	float FL = FresnelSchlick(NdotL);
	float FV = FresnelSchlick(NdotV);

	float Fss90 = LdotH * LdotH * roughnessStrength;
	float Fss = mix(1.0, Fss90, FL) * mix(1.0, Fss90, FV);
	float ss = 1.25 * (Fss * (1.0 / (NdotL + NdotV) - 0.5) + 0.5);

	return (1.0 / PI) * ss * albedoColor;
}

vec3 DisneyMicrofacetIsotropic(const in vec3 albedoColor, const in float metallicStrength, const in float roughnessStrength, float NdotL, float NdotV, float NdotH, float LdotH)
{
	float cdLum = 0.3 * albedoColor.r + 0.6 * albedoColor.g + 0.1 * albedoColor.b;

	vec3 cTint = (cdLum > 0) ? albedoColor / cdLum : vec3(1.0);
	vec3 cSpec0 = mix(specular * 0.08 * mix(vec3(1.0), cTint, specularTint), albedoColor, metallicStrength);

	float a = max(0.001, sqr(roughnessStrength));
	float Ds = GTR2(NdotH, a);
	float FH = FresnelSchlick(LdotH);

	vec3 Fs = mix(cSpec0, vec3(1.0), FH);
	float Gs;
	Gs  = SmithG_GGX(NdotL, a);
	Gs *= SmithG_GGX(NdotV, a);

	return Gs * Fs * Ds;
}

vec3 DisneyMicrofacetAnisotropic(const in vec3 albedoColor, const in float metallicStrength, const in float roughnessStrength, 
								 float NdotL, float NdotV, float NdotH, float LdotH,
								 const in vec3 L, const in vec3 V, const in vec3 H,
								 const in vec3 X, const in vec3 Y)
{
	float cdLum = 0.3 * albedoColor.r + 0.6 * albedoColor.g + 0.1 * albedoColor.b;

	vec3 cTint = (cdLum > 0) ? albedoColor / cdLum : vec3(1.0);
	vec3 cSpec0 = mix(specular * 0.08 * mix(vec3(1.0), cTint, specularTint), albedoColor, metallicStrength);

	float aspect = sqrt(1.0 - anisotropic * 0.9);
	float ax = max(0.001, sqr(roughnessStrength) / aspect);
	float ay = max(0.001, sqr(roughnessStrength) * aspect);

	float Ds = GTR2_Aniso(NdotH, dot(H, X), dot(H, Y), ax, ay);
	float FH = FresnelSchlick(LdotH);
	vec3 Fs = mix(cSpec0, vec3(1.0), FH);

	float Gs;
	Gs =  SmithG_GGX_Aniso(NdotL, dot(L, X), dot(L, Y), ax, ay);
	Gs *= SmithG_GGX_Aniso(NdotV, dot(V, X), dot(V, Y), ax, ay);

	return Gs * Fs * Ds;
}

float DisneyClearCoat(float NdotL, float NdotV, float NdotH, float LdotH)
{
	float gloss = mix(0.1, 0.001, clearCoatRoughness);
	float Dr = GTR1(abs(NdotH), gloss);
	float FH = FresnelSchlick(LdotH);
	float Fr = mix(0.04, 1.0, FH);
	float Gr = SmithG_GGX(NdotL, 0.25) * SmithG_GGX(NdotV, 0.25);

	return clearCoat * Fr * Gr * Dr;
}

vec3 DisneySheen(const in vec3 albedoColor, float LdotH)
{
	float FH = FresnelSchlick(LdotH);
	float cdLum = 0.3 * albedoColor.r + 0.6 * albedoColor.g + 0.1 * albedoColor.b;
	
	vec3 cTint = (cdLum > 0) ? albedoColor / cdLum : vec3(1.0);
	vec3 cSheen = mix(vec3(1.0), cTint, sheenTint);
	vec3 fSheen = FH * sheen * cSheen;

	return FH * sheen * fSheen;
}

//w0 = L
//wi = V
void CalcDirectionalLight(inout vec3 result, vec3 albedoColor, float metallicStrength, float roughnessStrength, vec3 V, vec3 N, vec3 F0)
{
	vec3 X = IN.tangent;
	vec3 Y = IN.bitangent;

	vec3 L = normalize(directionalLight.lightDirection.xyz);
	vec3 H = normalize(V + L);
	float NdotH = dot(N, H);
	float LdotH = dot(L, H);

	float NdotL = max(dot(N, L), 0.0001);
	float NdotV = max(dot(N, V), 0.0001);
	//if(NdotL < 0.0 || NdotV < 0.0)
		//result = vec3(0.0);	

	vec3 radiance = directionalLight.lightColor.xyz;

	vec3 diffuse = DisneyDiffuse(albedoColor, roughnessStrength, NdotL, NdotV, LdotH);
	vec3 subSurface = DisneySubsurface(albedoColor, roughnessStrength, NdotL, NdotV, LdotH);
	
	vec3 glossy = DisneyMicrofacetAnisotropic(albedoColor, metallicStrength, roughnessStrength, NdotL, NdotV, NdotH, LdotH, L, V, H, X, Y);
	//vec3 glossy = DisneyMicrofacetIsotropic(albedoColor, metallicStrength, roughnessStrength, NdotL, NdotV, NdotH, LdotH);
	float clearCoat = DisneyClearCoat(NdotL, NdotV, NdotH, LdotH);
	
	vec3 sheen = DisneySheen(albedoColor, LdotH);

	vec3 kD = mix(diffuse, subSurface, subsurface) + sheen;
	kD *= 1.0 - metallicStrength;
	vec3 kS = glossy + clearCoat;

	result = (kD + kS) * radiance;
}

void CalcPointLights(inout vec3 result, vec3 albedoColor, float metallicStrength, float roughnessStrength, vec3 N, vec3 V, vec3 F0)
{
	vec3 X = IN.tangent;
	vec3 Y = IN.bitangent;	

	for(int i = 0; i < numPointLights; i++)
	{
		vec3 L = normalize(pointLights[i].lightPosition.xyz - IN.fragWorldPos);
		vec3 H = normalize(V + L);
		float NdotH = dot(N, H);
		float LdotH = dot(L, H);

		float NdotL = max(dot(N, L), 0.0001);
		float NdotV = max(dot(N, V), 0.0001);
		if(NdotL < 0.0 || NdotV < 0.0)
			result = vec3(0.0);

		float distance = length(pointLights[i].lightPosition.xyz - IN.fragWorldPos);
		float attenuation = 1.0 / (distance * distance);
		//float attenuation = 1.0 / (pointLights[i].lightAttenData.x + pointLights[i].lightAttenData.y * distance + pointLights[i].lightAttenData.z * (distance * distance));
		vec3 radiance = pointLights[i].lightColor.xyz * attenuation;

		vec3 diffuse = DisneyDiffuse(albedoColor, roughnessStrength, NdotL, NdotV, LdotH);
		vec3 subSurface = DisneySubsurface(albedoColor, roughnessStrength, NdotL, NdotV, LdotH);
	
		vec3 glossy = DisneyMicrofacetAnisotropic(albedoColor, metallicStrength, roughnessStrength, NdotL, NdotV, NdotH, LdotH, L, V, H, X, Y);
		float clearCoat = DisneyClearCoat(NdotL, NdotV, NdotH, LdotH);
	
		vec3 sheen = DisneySheen(albedoColor, LdotH);

		vec3 kD = mix(diffuse, subSurface, subsurface) + sheen;
		kD *= 1.0 - metallicStrength;
		vec3 kS = glossy + clearCoat;

		result = (kD + kS) * radiance;
	}
}

void CalcAmbientLight(inout vec3 result, vec3 albedoColor, float metallicStrength, float roughnessStrength, vec3 N, vec3 V, vec3 R, vec3 F0)
{
	float NdotV = max(dot(N, V), 0.0);	
	vec3 F = FresnelSchlickRoughness(NdotV, F0, roughnessStrength);
	
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallicStrength;

	vec3 irradiance = texture(irradianceTex, N).rgb;
	vec3 diffuse = irradiance * albedoColor;	

	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefilteredColor = textureLod(prefilterTex, R, roughnessStrength * MAX_REFLECTION_LOD).rgb;
		
	vec2 envBRDF = texture(brdfLUTTex, vec2(NdotV, roughnessStrength)).rg;
	vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

	float aoTexStrength = hasOcclusionTex ? texture(occlusionTex, IN.texCoord).r : 1.0;

	//SSAO
	float aoStrength = 1.0;
	if(ssaoEnabled)
	{
		vec2 NDCSpaceFragPos = IN.fragClipSpacePos.xy / IN.fragClipSpacePos.w;
		vec2 texLoopUps = NDCSpaceFragPos * 0.5 + 0.5;
		aoStrength = texture(ssaoTex, texLoopUps).r;
	}

	//vec3 ambient = vec3(0.03) * albedoColor;
	vec3 ambient = (kD * diffuse + specular);
	ambient *= aoTexStrength;
	ambient *= aoStrength;

	result += ambient;
}

void main(void) 
{
	float m_GAMMA = skyboxData.y;
	float m_Exposure = skyboxData.x;

	vec3 albedoColor = hasAlbedoTex ? texture(albedoTex, IN.texCoord).rgb * baseColor : baseColor;
	albedoColor = pow(albedoColor, vec3(m_GAMMA));

	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.bitangent), normalize(IN.normal));
	vec3 normalColor = IN.normal;
	if(hasNormalTex)
	{
		normalColor = texture(normalTex, IN.texCoord).rgb;
		normalColor = normalColor * 2.0 - 1.0;
		normalColor.xy *= 1.0;
	}	
	normalColor = normalize(TBN * normalize(normalColor));

	float metallicStrength = hasMetallicTex ? texture(metallicTex, IN.texCoord).r : metallic;
	float roughnessStrength = hasRoughnessTex ? texture(roughnessTex, IN.texCoord).r : roughness;

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedoColor, metallicStrength);

	vec3 N = normalize(normalColor);
	vec3 V = normalize(cameraPos - IN.fragWorldPos);
	vec3 R = reflect(-V, N);

	vec3 result = vec3(0.0);
	CalcDirectionalLight(result, albedoColor, metallicStrength, roughnessStrength, N, V, F0);
	//CalcPointLights(result, albedoColor, metallicStrength, roughnessStrength, N, V, F0);
	//CalcAmbientLight(result, albedoColor, metallicStrength, roughnessStrength, N, V, R, F0);

	result = vec3(1.0) - exp(-result * m_Exposure);
	result = pow(result, vec3(1.0 / m_GAMMA));

	if(hasEmissiveTex)
	{
		vec3 emissiveColor = texture(emissiveTex, IN.texCoord).rgb;
		result += emissiveColor * emission;
	}

	fragColour = vec4(result, 1.0);
}