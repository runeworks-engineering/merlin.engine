#version 430
in vec4 position;
in vec4 screen_position;

in GS_out{
	vec4 position;
	vec4 screen_position;
	vec4 color;
	mat4 mv;
} vin;

out vec4 FragColor;

uniform vec3 viewPos;
uniform vec4 lightColor = vec4(1);

uniform float pointRadius;
uniform vec3 lightDir = vec3(0.577, 0.577, -0.8);


void main() {
	FragColor = vin.color;
}



  
