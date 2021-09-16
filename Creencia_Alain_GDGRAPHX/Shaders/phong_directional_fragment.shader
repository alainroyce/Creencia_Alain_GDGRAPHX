#version 330 core
uniform vec3 u_color;
out vec4 FragColor;
in vec3 FragPos;
in vec2 UV;
in vec3 Normal;
in vec2 TexCoords;
uniform sampler2D texture_diffuse;
uniform int model_id;
in mat3 TBN;

uniform vec3 u_light_dir;
uniform vec3 u_;
uniform vec3 u_camera_pos;
uniform vec3 u_ambient_color;

//this is for multitexturing
uniform sampler2D secondary_diffuse;
uniform vec3 u_light_pos;

//for texture bumb / normal
uniform sampler2D texture_normal;

float attenuate(float value, float maximum)
{
	float clampedValue = min(value, maximum);
	return 1.0 / (pow(5 * clampedValue / maximum, 2) + 1);
}


float attenuateSpot (float value, float minimum, float maximum)
{
	return 1.0f - (clamp(value, minimum, maximum) - minimum) / (maximum - minimum);
}

float simple_attenuate(float value, float maximum)
{
	if (value > maximum)
	{
		value = 0.0;
	}
	else
	{
		value = 1.0;
	}
	return value;
}

void pointLight()
{
	vec3 lightVector = normalize(u_light_pos - FragPos);

	float distance = length(u_light_pos - FragPos);
	vec3 lightColor = vec3(1.0, 1.0, 1.0); //color red

	//shininess
	float specularStrength = 0.3;
	vec3 viewDir = normalize(u_camera_pos - FragPos);
	//phong shading
	vec3 reflectDir = reflect(-lightVector, Normal);

	float spec = pow(max(dot(reflectDir, viewDir), 0.0), 4);

	vec3 specular = specularStrength * spec * lightColor;

	vec3 diffuse = vec3(max(dot(Normal, lightVector), 0.0)) * lightColor;
	vec3 ambient = u_ambient_color * lightColor;

	float gradient = attenuate(distance, 25.0);

	FragColor = vec4(ambient + (diffuse + specular) * gradient, 1.0) * texture(texture_diffuse, UV);
}

void multiTexturing()
{
	vec3 lightVector = normalize(u_light_pos - FragPos);

	float distance = length(u_light_pos - FragPos);

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
	//FragColor = vec4(ambient + (diffuse + specular), 1.0) * texture(secondary_diffuse, UV); //for night time
	FragColor = vec4(ambient + (diffuse + specular), 1.0) * texture(texture_diffuse, UV)
		+ texture(secondary_diffuse, UV) * (1.0 - NdotL); //for day and night combination; secondary map or multitexturing
	//FragColor = texture(secondary_diffuse, UV) * (1.0 - NdotL); //use for debugging, checking the secondary map (night)
	//FragColor = texture(texture_diffuse, UV) * (1.0 - NdotL); //use for debugging, checking the secondary map (day)
}

void directionalLight()
{
	vec3 lightVector = normalize(u_light_dir);

	//color value of the specular highlight color
	vec3 lightColor = vec3(1.0, 1.0, 1.0); //color red

	float specularStrength = 0.3;
	vec3 viewDir = normalize(u_camera_pos - FragPos);
	//phong shading
	vec3 reflectDir = reflect(-lightVector, Normal);

	float spec = pow(max(dot(reflectDir, viewDir), 0.0), 4);

	//blinn phong
	//spec = pow(max(dot(halfwayDir, normal), 0), 64);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 diffuse = vec3(max(dot(Normal, lightVector), 0.0)) * lightColor;
	vec3 ambient = u_ambient_color * lightColor;

	FragColor = vec4(ambient + diffuse + specular, 1.0) * texture(texture_diffuse, UV);
}

void normalBump()
{
	vec3 lightVector = normalize(u_light_pos - FragPos);
	vec3 texNormal = texture(texture_normal, UV).rgb;

	texNormal = TBN * texNormal;

	float NdotL = dot(texNormal, u_light_dir);
	vec3 normalMap = vec3(clamp(NdotL, 0, 1));

	FragColor = vec4(normalMap, 1.0) * texture(texture_diffuse, UV);
}

void spotLight()
{
	vec3 lightToSurface = normalize(u_light_pos - FragPos);
	vec3 lightDir = normalize(-u_light_dir);

	float distance = length(u_light_pos - FragPos);

	//color value of the specular highlight color
	vec3 lightColor = vec3(1.0, 1.0, 1.0); //color red

	float specularStrength = 0.3;
	vec3 viewDir = normalize(u_camera_pos - FragPos);
	//phong shading
	vec3 reflectDir = reflect(-lightDir, Normal); //ndotl

	float spec = pow(max(dot(reflectDir, viewDir), 0.0), 4);

	//blinn phong
	//spec = pow(max(dot(halfwayDir, normal), 0), 64);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 diffuse = vec3(max(dot(Normal, lightDir), 0.0)) * lightColor;
	vec3 ambient = u_ambient_color * lightColor;

	float angle = abs(acos(dot(lightDir, lightToSurface)));
	//radius length
	//float gradient = simple_attenuate(angle, 0.1);
	float gradient = attenuateSpot(angle, 0.0, 1.0);

	FragColor = vec4(ambient + (diffuse + specular) * gradient, 1.0) * texture(texture_diffuse, UV);
	//FragColor = vec4(u_color / 10 + (ambient + (diffuse + specular) * gradient), 1.0) * texture(texture_diffuse, UV);
}

void main()
{
	if (model_id == 1)
	{
		directionalLight();
	}
	if (model_id == 2)
	{
		multiTexturing();
	}
	if (model_id == 3)
	{
		pointLight();
	}
	if (model_id == 4)
	{
		normalBump();
	}
	if (model_id == 5)
	{
		spotLight();
	}
}

/*#version 330 core
uniform vec3 u_color;
out vec4 FragColor;
in vec3 FragPos;
in vec2 UV;
in vec3 Normal;
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

	

	vec3 lightColor = vec3(1.0, 1.0, 1.0);

	float specularStrength = 1.0;
	vec3 viewDir = normalize(u_camera_pos - FragPos);
	vec3 reflectDir = reflect(-lightVector, Normal);

	float spec = pow(max(dot(reflectDir, viewDir), 0.0), 4);

	vec3 specular = specularStrength * spec * lightColor;

	vec3 diffuse = vec3(max(dot(Normal, lightVector), 0.0)) * lightColor;
	vec3 ambient = u_ambient_color * lightColor;


	FragColor = vec4(ambient + (diffuse + specular) * gradient, 1.0) * texture(texture_diffuse, UV);
}*/
