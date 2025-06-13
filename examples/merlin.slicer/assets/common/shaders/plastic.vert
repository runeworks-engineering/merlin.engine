//This file was automatically generated 
//DO NOT CHANGE !
#version 440 core

layout (location = 0) in vec4 _position;
layout (location = 1) in vec3 _normal;
layout (location = 2) in vec3 _color;
layout (location = 3) in vec2 _texcoord;
layout (location = 4) in vec3 _tangent;
layout (location = 5) in vec3 _bitangent;

#include "../common/shaders/colors.comp"
#include "./common/uniforms.comp"
#include "./common/constants.comp"
#include "./common/buffers.comp"
#include "./common/nns.comp"
#include "./solver/sph.kernels.comp" 

#define MAX_LIGHTS 10


out vec3 position;
out vec3 normal;
out vec3 color;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform vec3 viewPos;

void main() {
	position = vec3(model * vec4(vec3(_position), 1.0f));
	color = _color;
	normal = normalize(_normal);

	gl_Position = projection * view * vec4(position, 1.0f);
}
