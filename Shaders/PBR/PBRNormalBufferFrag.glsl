#version 420 core

uniform sampler2D normalTex;

in Vertex 
{
	vec3 position;
	vec2 texCoord;
	vec3 normal;
	vec3 fragWorldPos;
	vec3 tangent;
	vec3 bitangent;	
} IN;

out vec4 fragColour;

void main(void)
{
	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.bitangent), normalize(IN.normal));
	vec3 normal = texture(normalTex, IN.texCoord).rgb * 2.0 - 1.0;
	normal = normalize(TBN * normalize(normal));

	fragColour = vec4(normal.xyz * 0.5 + 0.5, 1.0);
}