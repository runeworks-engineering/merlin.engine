#version 430
#include "../common/shaders/colors.comp"
#include "toolpath.comp"

layout (location = 0) in vec3 _position;

out VS_out{
	vec4 position;
	vec4 screen_position;
	vec4 color;
	mat4 mv;
} vout;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main() {
	uint i = gl_InstanceID;

	vec3 offset = vec3(0);
	if(vout.position.x == 0) offset = ssbo_a[i].xyz;
	else offset = ssbo_a[i].xyz;

	vout.position = model * (vec4(_position + vec3(offset),1));
	vout.screen_position = projection * view * vout.position;
	vout.color = colorMap(length(ssbo_meta[i].x),parula);
	vout.mv = projection * view;
		
	gl_Position = vout.screen_position;
	//gl_PointSize = 50.0/(gl_Position.w);		
}
