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

/*
void line(vec3 a, vec3 b) {
    // Calculate the center and radius of the sphere
    vec2 center = (gl_PointCoord - vec2(0.5)) * 2.0;
    float radius = length(center);
    // Discard fragments outside of the sphere's radius
    if (radius > 1.0) discard;

    // Calculate the normal vector for the sphere surface at this fragment
    vec3 n = vec3(center.x, center.y, sqrt(1.0 - radius * radius));
    n.xy = n.yx;
    // Transform the normal with the model-view matrix
    vec4 transformedNormal = vin.mv * vec4(n, 0.0);
    n = normalize(transformedNormal.xyz);

    
    //_normal.y = normal.z; 

    // Light calculations
    //vec3 lightD = normalize(lightPos - (mv * gl_FragCoord).xyz);
    vec3 lightD = normalize(lightDir);
    float diff = max(dot(lightD, n), 0.0);

    vec3 viewDir = normalize(vec3(position) - viewPos);
	vec3 reflectDir = reflect(lightD, n);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 0.5);

    // Ambient, Diffuse, and Specular Components
    vec3 ambient = 0.6 * vec3(lightColor);
    vec3 diffuse = 0.8 * diff * vec3(lightColor);
    vec3 specular = 0.3 * spec * vec3(lightColor) ;

    // Calculate final color
    vec4 finalColor = vec4(ambient + diffuse + specular, 1.0) * vin.color;

    // Apply smoothstep for alpha blending at the edges
	//finalColor.w = smoothstep(1, 0,length(center)*length(center));
	
    if(position.z < 0.08) finalColor = vec4(ambient + diffuse + specular, 1.0) * vec4(1,0,0,1);
    // Output the final color
    FragColor = finalColor;
}*/

void main() {
	FragColor = vin.color;
    //if(gl_PointCoord != vec2(0)) line(gl_PointCoord);
}



  
