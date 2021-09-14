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


	vec3 lightColor = vec3(1.0, 1.0, 1.0); //color red
	vec3 tbnNormal = texture(texture_normal, UV).rgb;

	///convert pixel to vector
	tbnNormal = tbnNormal * 2.0 - 1.0;
	tbnNormal =  normalize(TBN * tbnNormal);


	vec3 reflectDir = reflect(-lightVector, tbnNormal);

	vec3 viewDir = normalize(u_camera_pos - FragPos);
	float spec = pow(max(dot(reflectDir, viewDir), 0.1), 4);

	float specularStrength = 2.0;
	vec3 specular = specularStrength * spec * lightColor;

	vec3 diffuse = vec3(max(dot(tbnNormal, lightVector), 0.0)) * lightColor;
	vec3 ambient = u_ambient_color * lightColor;

	//FragColor = vec4(tbnNormal);// for debug purposes turns the object red and blue
	//texture(texture_normal, UV);
	FragColor = vec4(ambient + (diffuse +  specular), 1.0 * texture(texture_diffuse, UV);// applies the normal mapping

}
