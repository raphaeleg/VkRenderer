#version 450

layout (location = 0) in vec2 fragOffset;
layout (location = 0) out vec4 outColor;

struct PointLight {
	vec4 position;
	vec4 color;
};
layout(set = 0, binding = 0) uniform GlobalUbo {
	mat4 projectionMatrix;
	mat4 viewMatrix;
	mat4 inverseViewMatrix;
	vec4 ambientLightColor;
	PointLight pointLights[10];
	int numLights;
} ubo;
layout(push_constant) uniform Push {
	vec4 position;
	vec4 color;
	float radius;
} push;

const float PI = 3.1415926538;

void main() {
	float d = sqrt(dot(fragOffset, fragOffset));
	if (d >= 1.0) {
		discard;
	}
	float cosDis = 0.5 * (cos(d*PI)+1.0);
	// outColor = vec4(push.color.xyz, cosDis);
	outColor = vec4(push.color.xyz + cosDis, cosDis);
}