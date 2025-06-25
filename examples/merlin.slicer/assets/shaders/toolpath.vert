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

uniform int layer = 0;
uniform float layer_height = 0.2;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform bool showG0 = false;

void main() {
	uint i = gl_InstanceID;

	vec3 offset = vec3(0);

	bool show = true;
	if(!showG0 && ssbo_toolpath[i].meta_bis.y == 0) show = false;

	if(layer > ssbo_toolpath[i].meta_bis.x && show){
		if(_position.x == 0) offset = ssbo_toolpath[i].start.xyz;
		else offset = ssbo_toolpath[i].end.xyz;
	}

	vout.position = model * (vec4(_position + vec3(offset),1));
	vout.screen_position = projection * view * vout.position;
	vout.color = colorMap(map(length(ssbo_toolpath[i].meta.x), 0, 1000),parula);
	vout.mv = projection * view;
		
	gl_Position = vout.screen_position;	
}
