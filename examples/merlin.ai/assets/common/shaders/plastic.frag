//This file was automatically generated 
//DO NOT CHANGE !
#version 430 core

#include "../common/shaders/colors.comp"
#include "./common/uniforms.comp"
#include "./common/constants.comp"
#include "./common/buffers.comp"
#include "./common/nns.comp"
#include "./solver/sph.kernels.comp" 

in vec3 position;
in vec3 normal;
in vec3 color;

out vec4 FragColor;

vec2 mapHeat(in vec3 position){
	float temp = 0;
	float density = 0;
	float n = 0;
	OVERNNS
		float dist = distance(b_xj.xyz, position);
		if(dist < 1e-6 || dist > H) continue;
		float Wij = poly6(dist);

		if(phase(j) == SOLID || phase(j) == FLUID){
			temp += b_Tj;
			density += mi * Wij;
			n++;
		}
	OVERNNS_END
	if(n > 1) temp /= n;
	return vec2(temp, density);
}

void main() {
	
    vec2 mappedV = mapHeat(position);//length(_normal);
    float temp = mappedV.x;//length(_normal);
    float density = mappedV.y;//length(_normal);
	if(temp < 275.5 + 25.0) temp = 275.5 + 25.0;
	vec4 wcolor = colorMap(map(temp,275,275+300.0), inferno);

    FragColor.rgb = wcolor.rgb;


    //FragColor.rgb = N* 0.9 + FragColor.rgb * 0.1;
    //FragColor.rgb = normalize(vin.tangentBasis * (-lights[0].direction));
    //FragColor.rgb = N;
    FragColor.a = 1.0;

}