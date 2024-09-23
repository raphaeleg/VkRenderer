#version 450

// Vertex Attributes
layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;

// only one push constant block per shader entry point
layout (push_constant) uniform Push {
	vec2 offset;
	vec3 color;
} push;	

void main() {
  gl_Position = vec4(position + push.offset, 0.0, 1.0);
  fragColor = color;
}