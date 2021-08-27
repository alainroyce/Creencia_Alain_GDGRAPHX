#version 330 core
uniform vec3 u_color;
out vec4 FragColor;
in vec3 FragPos;
in vec2 UV;
in vec3 Normal;
in vec2 TexCoords;
uniform sampler2D texture_diffuse;

uniform vec3 u_light_pos;
uniform vec3 u_camera_pos;
uniform vec3 u_ambient_color;

float attenuate(float value, float maximum)
{
	float clampedValue = min(value, maximum);
	return 1.0 / (pow(5 * clampedValue / maximum, 2) + 1);
}


void main()
{
	vec3 lightVector = normalize(u_light_pos - FragPos);

	float distance = length(u_light_pos - FragPos);
	float diff = max(dot(Normal, lightVector), 0.0);

	//phong shading
	vec3 reflectDir = reflect(-lightVector, Normal);

	vec3 viewDir = normalize(u_camera_pos - FragPos);
	float spec = pow(max(dot(reflectDir, viewDir), 0.1), 4);
	//blinn phong
	//spec = pow(max(dot(halfwayDir, normal), 0), 64);

	//color value of the specular highlight color
	vec3 lightColor = vec3(1.0, 1.0, 1.0); //color red

	vec3 ambient = u_ambient_color * lightColor * vec3(texture(texture_diffuse, TexCoords));
	//vec3 ambient = u_ambient_color * lightColor;

	vec3 diffuse = diff * vec3(max(dot(Normal, lightVector), 0.0)) * lightColor * vec3(texture(texture_diffuse, TexCoords));

	//shininess
	float specularStrength = 0.3;
	vec3 specular = specularStrength * spec * lightColor * vec3(texture(texture_diffuse, TexCoords));;

	float gradient = attenuate(distance, 30.0);

	//FragColor = vec4(ambient + (diffuse + specular) * gradient, 1.0);
	FragColor = vec4(u_color / 10 + (ambient + (diffuse * 30.0 + specular * 30.0) * gradient), 1.0) * texture(texture_diffuse, UV);
	//FragColor = vec4(u_color / 10 + (ambient + (diffuse * 3.0 + specular * 3.0) * gradient), 1.0) * texture(texture_diffuse, UV);
}

/*
void main()
{
	vec3 lightVector = normalize(u_light_pos - FragPos);

	float distance = length(u_light_pos - FragPos);
	float gradient = attenuate(distance, 30.0);

	//color value of the specular highlight color
	vec3 lightColor = vec3(0.5, 0.5, 0.5); //color red

	//shininess
	float specularStrength = 0.3;
	vec3 viewDir = normalize(u_camera_pos - FragPos);
	//phong shading
	vec3 reflectDir = reflect(-lightVector, Normal);

	float spec = pow(max(dot(reflectDir, viewDir), 0.1), 4);

	//blinn phong
	//spec = pow(max(dot(halfwayDir, normal), 0), 64);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 diffuse = vec3(max(dot(Normal, lightVector), 0.0)) * lightColor;
	vec3 ambient = u_ambient_color * lightColor;

	//FragColor = vec4(ambient + (diffuse + specular) * gradient, 1.0);

	FragColor = vec4(u_color / 10 + (ambient + (diffuse * 3.0 + specular * 3.0) * gradient), 1.0) * texture(texture_diffuse, UV);
}
*/