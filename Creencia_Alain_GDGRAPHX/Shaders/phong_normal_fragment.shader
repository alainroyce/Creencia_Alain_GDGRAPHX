#version 330 core
uniform vec3 u_color;
out vec4 FragColor;
in vec3 FragPos;
in vec2 UV;
in vec3 Normal;
in vec2 TexCoords;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;

uniform vec3 u_light_pos;
uniform vec3 u_camera_pos;
uniform vec3 u_ambient_color;



void main()
{
	vec3 lightVector = normalize(u_light_pos - FragPos);

	float distance = length(u_light_pos - FragPos);


	vec3 reflectDir = reflect(-lightVector, Normal);

	vec3 viewDir = normalize(u_camera_pos - FragPos);
	float spec = pow(max(dot(reflectDir, viewDir), 0.1), 4);

	vec3 lightColor = vec3(1.0, 1.0, 1.0); //color red

	float specularStrength = 0.3;
	vec3 specular = specularStrength * spec * lightColor;

	vec3 diffuse = vec3(max(dot(Normal, lightVector), 0.0)) * lightColor;
	vec3 ambient = u_ambient_color * lightColor;

	FragColor = texture(texture_normal, UV);
}
