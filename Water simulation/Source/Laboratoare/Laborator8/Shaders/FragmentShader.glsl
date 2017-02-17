#version 330

// TODO: get color value from vertex shader
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;
uniform vec3 object_color;
uniform int so;


layout(location = 0) out vec4 out_color;

void main()
{
	vec3 L = normalize( light_position - world_position );
	vec3 V = normalize( eye_position - world_position );
	vec3 H = normalize( L + V );

	// TODO: define ambient light component
	
	
	float ambient_light = 0.35;

	if(so == 1)
		ambient_light = 0.95;
	

	// TODO: compute diffuse light component
	float diffuse_light = 0;
	if(dot(L, world_normal) > 0)
		diffuse_light = material_kd * max( dot(L, world_normal), 0 );

	// TODO: compute specular light component
	float specular_light = 0;
	if (dot(L, world_normal) > 0) {
		specular_light = material_ks * pow (max (dot(world_normal, H), 0), material_shininess);
	}
	

	// TODO: compute light
	float atenuare = 1 / pow(distance(world_position, light_position),2);
	float light = ambient_light + atenuare * (diffuse_light + specular_light);
	
	out_color = vec4(light * object_color , 1.0f);
}