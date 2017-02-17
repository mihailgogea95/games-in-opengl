#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

//unde sa produs unda
uniform float corX;
uniform float corZ;

//pentru formarea undei
uniform float A;
uniform float Timp;
uniform float W;
uniform float Phi;
uniform float animatie;
uniform float lung;

// Output values to fragment shader
out vec3 world_position;
out vec3 world_normal;

float waveH (float x, float z) {
	
	float H ;
	H = 0.0f;
	float putere1;
	float putere2;
	float distanta;
	putere1 = pow( corX - abs(x) , 2);
	putere2 = pow( corZ - abs(z) , 2 );
	distanta = (-1) * sqrt(putere1 + putere2);
	float inf;
	float ext;
	ext = lung;
	inf = lung - 3 * 12.23f;

	if( -distanta < ext && -distanta > inf) {
		H = 2 * A *(sin(distanta * W + Timp * Phi) + 1)/2;
	}

	return H;
}

vec3 calculVarf(float a, float b ){

	float valoareY = waveH(a, b);

	return vec3(a , valoareY, b );

}

void main()
{
	// TODO: send world position and world normal to Fragment Shader
	
	vec3 varf1;
	vec3 varf2;
	vec3 varf3;
	vec3 varf4;
	vec3 varf5;
	vec3 normala1;
	vec3 normala2;
	vec3 normala3;
	vec3 normala4;
	vec3 normalaMedie;

	if(animatie == 1){
		
		varf1 = calculVarf(v_position.x, v_position.z);

		varf2 = calculVarf (v_position.x - 1, v_position.z);
		vec3 dif21 = (varf2 - varf1);

		varf3 = calculVarf (v_position.x + 1, v_position.z);
		vec3 dif31 = (varf3 - varf1);

		varf4 = calculVarf (v_position.x, v_position.z - 1);
		vec3 dif41 = (varf4 - varf1);

		varf5 = calculVarf (v_position.x,  v_position.z + 1);
		vec3 dif51 = (varf5 - varf1);


		normala1 = cross( dif41, dif21 );
		normala2 = cross( dif21, dif51 );
		normala3 = cross( dif51, dif31 );
		normala4 = cross( dif31, dif41 );

		normalaMedie = normalize ((normala1 + normala2 + normala3 + normala4)/4);

		world_position = vec3(( Model * vec4(varf1,1)));
		world_normal = vec3(normalize( mat3( Model) * normalaMedie ));
	
	
		gl_Position = Projection * View * Model * vec4(varf1, 1.0);

	}else {
		
		world_position = vec3(( Model * vec4(v_position,1)));
		world_normal = vec3(normalize( mat3( Model) * v_normal ));

		gl_Position = Projection * View * Model * vec4(v_position, 1.0);
		
	}
	
	
}
