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

uniform vec3 viewPos;
uniform int colorMapSelection = 0;
uniform int colorMode = 3;

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
	if(position.z > 20) temp = 275.98 + 220;

	return vec2(temp, density);
}

void main() {
	
    vec2 mappedV = mapHeat(position);//length(_normal);
    float temp = mappedV.x;//length(_normal);
    float density = mappedV.y;//length(_normal);
	if(temp < 275.5 + 25.0) temp = 275.5 + 25.0;



	Palette palette = inferno;


	float minValue = 0;
	float maxValue = 0;
	float value = 0;

	if(colorMapSelection == 0)
		palette = inferno;
	else if(colorMapSelection == 1)
		palette = blackbody;
	else if(colorMapSelection == 2)
		palette = plasma;
	else if(colorMapSelection == 3)
		palette = viridis;
	else if(colorMapSelection == 4)
		palette = warmcool;
	else if(colorMapSelection == 5)
		palette = parula;
	else if(colorMapSelection == 6)
		palette = jet;
		

	if(colorMode == 0){ // solid color
		FragColor = vec4(1,0.2,0.2,1.0);
		return;
	}else if(colorMode == 1){ // bin index
		uint binI = getBinIndex(position);
		FragColor = vec4(randomColor(binI).rgb,1.0);
		return;
	}else if(colorMode == 2){ // density
		value = density;
		minValue = 0.95 * u_rho0;
		maxValue =  1.05 * u_rho0;
	}else if(colorMode == 3){ // Temperature
		value = temp;
		minValue = 275;
		maxValue =  275+200.0;
	}else if(colorMode == 4){ // solid color
		value = temp;
		minValue = 275;
		maxValue =  275+300.0;
		
	}else if(colorMode == 5){ // solid color
		value = temp;
		minValue = 275;
		maxValue =  275+300.0;
	}else if(colorMode == 6){ // solid color
		value = temp;
		minValue = 275;
		maxValue =  275+300.0;
	}

	vec4 wcolor = colorMap(map(value,minValue,maxValue), palette);
    
	vec3 viewDir = normalize(viewPos - position);
	vec3 lightDir = normalize(vec3(1,1,5));
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = 0;
    if(diff != 0){
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), 0.1); //blinn
    }

	vec3 ambient = 0.6 * wcolor.rgb;
    vec3 diffuse = 0.4 * diff * wcolor.rgb;
    vec3 specular = 0.3 * spec * wcolor.rgb;
    //FragColor.rgb = N* 0.9 + FragColor.rgb * 0.1;
    //FragColor.rgb = normalize(vin.tangentBasis * (-lights[0].direction));
    //FragColor.rgb = N;

	FragColor.rgb = ambient + diffuse + specular;//wcolor.rgb;
    FragColor.a = 1.0;

}