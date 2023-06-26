#version 420 core

out vec4 fragColour;

in Vertex
{
	vec3 localPos;
} IN;

uniform sampler2D equiRectTex;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

void main(void)
{
	vec2 uv = SampleSphericalMap(normalize(IN.localPos));
	fragColour = vec4(texture(equiRectTex, uv).rgb, 1.0);
}