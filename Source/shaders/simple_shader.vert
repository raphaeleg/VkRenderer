#version 450

// Vertex Attributes
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

layout(location = 0) out vec3 fragColor;

// only one push constant block per shader entry point
layout (push_constant) uniform Push {
	mat4 transform;
	vec3 color;
} push;	

void main() {
   gl_Position = push.transform * vec4(position, 1.0f);
   fragColor = color;
}