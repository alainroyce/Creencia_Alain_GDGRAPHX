#version 330 core
uniform vec3 u_color;
out vec4 FragColor;
in vec3 FragPos;
in vec2 UV;
in vec3 Normal;
uniform sampler2D texture_diffuse;
uniform sampler2D night_diffuse;

uniform vec3 u_light_pos;
uniform vec3 u_camera_pos;
uniform vec3 u_ambient_color;

void main()
{
	vec3 lightVector = normalize(u_light_pos - FragPos);

	float distance = length(u_light_pos - FragPos);
	//disable for now
	//float gradient = attenuate(distance, 30.0);

	vec3 lightColor = vec3(1.0, 1.0, 1.0); //color white

	//shininess
	float specularStrength = 0.3;
	vec3 viewDir = normalize(u_camera_pos - FragPos);
	vec3 reflectDir = reflect(-lightVector, Normal);

	float spec = pow(max(dot(reflectDir, viewDir), 0.0), 4);

	vec3 specular = specularStrength * spec * lightColor;

	float NdotL = max(dot(Normal, lightVector), 0.0);

	vec3 diffuse = vec3(NdotL) * lightColor;
	vec3 ambient = u_ambient_color * lightColor;

	//* texture(texture_diffuse, UV) //use for displaying the textures
	//FragColor = vec4(ambient + (diffuse + specular) * gradient, 1.0) * texture(texture_diffuse, UV); //w/ gradient
	//FragColor = vec4(ambient + (diffuse + specular), 1.0) * texture(texture_diffuse, UV); //for day time
	//FragColor = vec4(ambient + (diffuse + specular), 1.0) * texture(night_diffuse, UV); //for night time
	FragColor = vec4(ambient + (diffuse + specular), 1.0) * texture(texture_diffuse, UV)
		+ texture(night_diffuse, UV) * (1.0 - NdotL); //for day and night combination; secondary map or multitexturing
	//FragColor = texture(night_diffuse, UV) * (1.0 - NdotL); //use for debugging, checking the secondary map (night)
	//FragColor = texture(texture_diffuse, UV) * (1.0 - NdotL); //use for debugging, checking the secondary map (day)
}