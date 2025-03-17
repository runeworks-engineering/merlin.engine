#version 430
#include "../uniforms.comp"
#include "../constants.comp"
#include "../buffers.comp"
#include "../nns.comp"
#include "../../graphics/colors.comp"

layout(location = 0) in vec3 _position;
layout(location = 1) in vec3 _normal;

out vec4 position;
out vec4 screen_position;
out vec4 normal;
out vec4 color;
out vec2 texCoord;
out mat4 mv;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform int colorCycle = 0;
uniform uint particleTest = 50;
uniform int showBoundary = 0;
uniform vec2 WindowSize;
uniform float zoomLevel = 20;

void main() {
	uint i = gl_InstanceID;
	vec3 offset = b_xi.xyz;
	position = model * (vec4(_position + vec3(offset),1));

	uint testsortedID = sorted_id(particleTest);
	
	bool binTest = true;
	bool nnTest = false;
	bool hTest = false;
	uint binindex = bin_index(gl_InstanceID);//getBinIndex(particles[sortedID].position);

	bool test = phase(gl_InstanceID) == UNUSED || (phase(gl_InstanceID) == BOUNDARY && showBoundary == 0);
	color = vec4(1);
	if(colorCycle == 0){ 
		color = vec4(vec3(0.8), 1.0);
	}else if(colorCycle == 1){ 
		color = vec4(randomColor(binindex), 1);
	}else if(colorCycle == 2){ 
		color = colorMap(map(b_rho_i*1000, 1000 * u_rho0, 1050 * u_rho0), warmcool);
	}else if(colorCycle == 3){ 
		color = colorMap(map(b_Ti,200,275+750.0), inferno);
	}else if(colorCycle == 4){ 
		color = colorMap(map(length(b_vi),0,1000.0), parula);
	}else if(colorCycle == 5){ 
		color = vec4(randomColor(binindex), 1);
	}else{ //NNS Test
		if(gl_InstanceID == testsortedID){
			color = vec4(1,0,0, 1);
			binTest = true;
		}else{

			binTest = false;
			uvec3 binIndexVec3 = getBinCoord(ssbo_predicted_position[testsortedID].xyz);
			for (int z = int(binIndexVec3.z) - 1; z <= int(binIndexVec3.z) + 1; z++) {
				for (int y = int(binIndexVec3.y) - 1; y <= int(binIndexVec3.y) + 1; y++) {
					for (int x = int(binIndexVec3.x) - 1; x <= int(binIndexVec3.x) + 1; x++) {
						if (x < 0 || y < 0) continue;
						if (x >= cst_binMax.x || y >= cst_binMax.y) continue; 
						if (getBinIndexFromCoord(uvec3(x,y,z)) == binindex) binTest = true;
					}
				}
			}

			vec3 position = ssbo_predicted_position[testsortedID].xyz;
			OVERNNS
				if(gl_InstanceID == j){
					nnTest = true;
					if(length(ssbo_predicted_position[testsortedID].xyz - b_pj.xyz) <= cst_smoothingRadius) hTest = true;
				}
			OVERNNS_END

			color = vec4(0,0,0, 1);
			if(nnTest) color = vec4(0,0,1, 1);
			if(hTest) color = vec4(0,1,0, 1);
		}
		
	}

	if( test || !binTest){
		screen_position = projection * view * vec4(0,0,0,1);
		gl_Position = screen_position;
		gl_PointSize = 0;
	}else{
		screen_position = projection * view * position;
		mv = projection * view;
		
		gl_Position = screen_position;
		if(phase(i) == GRANULAR) gl_PointSize = 5.0*cst_particleRadius*400.0/(gl_Position.w);
		else gl_PointSize = 5.0*cst_particleRadius*400.0/(gl_Position.w);
		if(colorCycle == 5 && !hTest && !(gl_InstanceID == particleTest)) gl_PointSize = 400.0/(gl_Position.w);
		
	}
}