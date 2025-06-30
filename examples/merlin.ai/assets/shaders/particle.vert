#version 430
#include "common/uniforms.comp"
#include "common/constants.comp"
#include "common/buffers.comp"
#include "common/nns.comp"
#include "common/colors.comp"
#include "solver/sph.kernels.comp"

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

uniform int colorCycle = 3;
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

	bool test = phase(gl_InstanceID) == UNUSED || (phase(gl_InstanceID) >= BOUNDARY && showBoundary == 0);
	color = vec4(1);
	if(colorCycle == 0){ 
		color = vec4(vec3(0.8), 1.0);
	}else if(colorCycle == 1){ 
		color = vec4(randomColor(binindex), 1);
	}else if(colorCycle == 2){ 
		color = colorMap(map(b_rho_i*1000, 1000 * u_rho0, 1050 * u_rho0), warmcool);
	}else if(colorCycle == 3){ 
		color = colorMap(map(b_Ti,275,275+300.0), inferno);
	}else if(colorCycle == 4){ 
		color = colorMap(map(length(b_vi),0,1000.0), parula);

		/*
		vec3 sumSDFGrad = vec3(0.0);
		float sumSDFWeight = 0.0;
		uint numNeihbors = 0;
		float H = cst_smoothingRadius;
		vec3 position = b_xi.xyz;
		OVERNNS
			//if(j == i) continue;
			float dist = c_Dij;
			if(dist > H || dist < EPSILON) continue;

			if( phase(j) >= BOUNDARY){
				//if(c_Dij < 0.1*H) {
					numNeihbors ++;
					sumSDFGrad += b_dpj.xyz*poly6(c_Dij); // push out
					sumSDFWeight += b_dpj.w;
				//}
			}

		OVERNNS_END
		//SDF Correction
		float stiffness = 0.1; // 0 = no correction, 1 = full projection
		sumSDFWeight /= float(numNeihbors);
		
		color = vec4((normalize(sumSDFGrad)), 1);
		color = colorMap(map(sumSDFWeight, 0, 1.0), jet);
		*/

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

	//if(phase(gl_InstanceID) >= BOUNDARY) color =  vec4((normalize(b_dpi.xyz)), 1);
	//if(phase(gl_InstanceID) >= BOUNDARY) color =  colorMap(map(b_dpi.w, 0, 3.0), jet);

	if( test || !binTest){
		screen_position = projection * view * vec4(0,0,0,1);
		gl_Position = screen_position;
		gl_PointSize = 0;
	}else{
		screen_position = projection * view * position;
		mv = projection * view;
		
		gl_Position = screen_position;
		//gl_PointSize = 0.3*cst_particleRadius*600.0/(gl_Position.w); //real scale
		gl_PointSize = 3.0*cst_particleRadius*600.0/(gl_Position.w);
		if(colorCycle == 5 && !hTest && !(gl_InstanceID == particleTest)) gl_PointSize = 600.0/(gl_Position.w);
		
	}
}