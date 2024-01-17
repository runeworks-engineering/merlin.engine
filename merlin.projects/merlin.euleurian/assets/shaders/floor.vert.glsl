#version 330 core

layout (location = 0) in vec3 _position;
layout (location = 1) in vec3 _normal;
layout (location = 2) in vec3 _color;

out vec3 position;
out vec3 normal;
out vec3 color;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main() {
	position = vec3(model * vec4(_position, 1.0f));
	color = _color;
	normal = _normal;

	gl_Position = projection * view * vec4(position, 1.0f);
}