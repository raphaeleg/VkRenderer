#version 450

// Vertex Attributes
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform GlobalUbo {
	mat4 projectionViewMatrix;
	vec4 ambientLightColor;
	vec4 lightPosition;
	vec4 lightColor;
} ubo;

// only one push constant block per shader entry point
layout (push_constant) uniform Push {
	mat4 modelMatrix;
} push;	

void main() {
   vec4 positionWorld = push.modelMatrix * vec4(position, 1.0f);
   gl_Position = ubo.projectionViewMatrix * push.modelMatrix * vec4(position, 1.0f);

   vec3 normalWorldSpace = normalize((push.modelMatrix * vec4(normal, 0.0)).xyz);

   vec3 directionToLight = ubo.lightPosition.xyz - positionWorld.xyz;
   float attenuation = 1.0 / dot(directionToLight, directionToLight);

   vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attenuation;
   vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
   vec3 diffuseLight = lightColor * max(dot(normalWorldSpace, normalize(directionToLight)), 0);

   fragColor = (diffuseLight + ambientLight) * color;
}