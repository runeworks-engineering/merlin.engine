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

//uniform int layer = 0;
uniform float layer_height = 0.2;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform bool showG0 = false;
uniform int colorMode = 1;

void main() {
	uint i = gl_InstanceID;

	vec3 offset = vec3(0);

	bool show = true;
	if(!showG0 && ssbo_toolpath[i].meta_bis.y == 0) show = false;

	if( show){
		if(_position.x == 0) offset = ssbo_toolpath[i].start.xyz;
		else offset = ssbo_toolpath[i].end.xyz;
	}

	vout.position = model * (vec4(offset,1));
	vout.screen_position = projection * view * vout.position;
	

	if(colorMode == 0)		vout.color = vec4(randomColor(uint(ssbo_toolpath[i].meta.w) + 1),1);
	else if(colorMode == 1)	vout.color = colorMap(map(length(ssbo_toolpath[i].meta.x), 0, 1000),parula);
	else if(colorMode == 2)	vout.color = colorMap(map(length(ssbo_toolpath[i].meta.y), 0, 250),inferno);



	vout.mv = projection * view;
		
	gl_Position = vout.screen_position;	
}
