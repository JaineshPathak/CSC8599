#version 420 core

const int MAX_KERNEL_SIZE = 64;
const float INV_MAX_KERNEL_SIZE_F = 1.0 / float(MAX_KERNEL_SIZE);
const vec2 HALF_2 = vec2(0.5);

layout(std140, binding = 0) uniform Matrices
{
	mat4 projMatrix;
	mat4 viewMatrix;
};

//uniform sampler2D depthTex;
uniform sampler2D positionTex;
uniform sampler2D normalTex;
uniform sampler2D noiseTex;

uniform float sampleRadius;
uniform float intensity;
uniform vec2 noiseScale;
uniform vec3 kernel[MAX_KERNEL_SIZE];

in Vertex
{
	vec2 texCoord;
} IN;

out vec4 fragColour;

/*vec3 CalcViewPosition(vec2 coords)
{
	float fragmentDepth = texture(depthTex, coords).r;

	//Convert Coords and Frag Depth to NDC -1 to 1 (Clip Space)
	vec4 ndc = vec4(coords.x * 2.0 - 1.0, coords.y * 2.0 - 1.0, fragmentDepth * 2.0 - 1.0, 1.0);

	//Convert to View Space
	vec4 vs_pos = inverse(viewMatrix) * ndc;
	vs_pos.xyz = vs_pos.xyz / vs_pos.w;

	return vs_pos.xyz;
}*/

void main(void)
{
	vec3 viewPos = texture(positionTex, IN.texCoord).rgb;
	vec3 normal = normalize(texture(normalTex, IN.texCoord).rgb);

	//vec3 viewNormal = cross(dFdy(viewPos.xyz), dFdx(viewPos.xyz));
	//viewNormal = normalize(viewNormal * -1.0);

	vec3 randomVec = texture(noiseTex, IN.texCoord * noiseScale).xyz;
	randomVec = normalize(randomVec);
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);

	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusionFactor = 0.0;
	for(int i = 0; i < MAX_KERNEL_SIZE; i++)
	{
		vec3 samplePos = TBN * kernel[i];
		samplePos = viewPos + samplePos * sampleRadius;

		vec4 offset = vec4(samplePos, 1.0);
		offset = projMatrix * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5;

		float geometryDepth = texture(positionTex, offset.xy).z;
		float rangeCheck = smoothstep(0.0, 1.0, sampleRadius / abs(viewPos.z - geometryDepth));

		occlusionFactor += (geometryDepth >= samplePos.z + 0.025 ? 1.0 : 0.0) * rangeCheck;
	}

	occlusionFactor = 1.0 - (occlusionFactor / MAX_KERNEL_SIZE);
	occlusionFactor = pow(occlusionFactor, intensity);

	fragColour = vec4(occlusionFactor, occlusionFactor, occlusionFactor, 1.0);
}