#version 330 core
struct Material {
	//ambient unnecessary since it's now equal to diffuse now that ambient is controlled with the light directly
	//the diffuse map of the material
	sampler2D diffuse;
	//Specular map of the material surface
	sampler2D specular;
	//Emission map which dictates when objects glow
	sampler2D emission;
	//impacts scattering/radius of the specular highlight
	float shininess;
};

uniform Material material;
//definition for point light
struct PointLight {
	//position of the light
	vec3 position;
	//color and strength of ambient light
	vec3 ambient;
	//color and strength of diffuse light
	vec3 diffuse;
	//color and strength of specular light
	vec3 specular;

	//Use attenuation to simulate light getting weaker with distance
	/* 
	formula:
	k_att = 1.0 / (k_c + k_l * d + k_q * d^2)
	k_c = k constant
	k_l = k linear
	k_q = k quadratic
	d = distance
	*/
	float constant;
	float linear;
	float quadratic;
};
#define NR_POINT_LIGHTS 4


uniform PointLight pointLights[NR_POINT_LIGHTS];
//definition for directional light (like the sun)
struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	/*Used in calculating the edges of the SpotLight
	we want an intensity value for our light which ranges 1.0 to 0.0
	where 1.0 is the inner cone of our spotlight and 0.0 is for fragments outside our outer cone

	here's the formula
	intensity = (theta - gamma) / epsilon
	epsilon being the difference between the inner (theta) cone and the outer (gamma) cone
	*/
	float cutOff;
	float outerCutOff;



	//color and strength of ambient light
	vec3 ambient;
	//color and strength of diffuse light
	vec3 diffuse;
	//color and strength of specular light
	vec3 specular;


	//Use attenuation to simulate light getting weaker with distance
	/* 
	formula:
	k_att = 1.0 / (k_c + k_l * d + k_q * d^2)
	k_c = k constant
	k_l = k linear
	k_q = k quadratic
	d = distance
	*/
	float constant;
	float linear;
	float quadratic;
};

//Function definitions
//calculate direct Light
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

uniform DirLight dirLight;
uniform SpotLight spotLight;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;


void main()
{
	//we want a final frag shader which basically does this
	/*
	
	output += directionalLightCalc()
	//account for every point light
	for (int i = 0; i < nr_of_point_lights; i++) {
		output += pointLightCalc(pointLights[i])
	}
	output += spotLightCalc()
	FragColor = vec4(output, 1.0)
	*/
	//this will clean up our shader file, and allow us to place multiple lights
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 output = CalcDirLight(dirLight, norm, viewDir);
	//point lights
	for (int i = 0; i < NR_POINT_LIGHTS; i++) {
		output += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	}

	//finally SpotLight
	output += CalcSpotLight(spotLight, norm, FragPos, viewDir);
	FragColor = vec4(output, 1.0);


}
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
	//negative to get the direction pointing from the light source TO the object
	vec3 lightDir = normalize(-light.direction);
	//diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	//combine results
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
	vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
	return (ambient + diffuse + specular);

}
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	
	//attenuation
	float distance = length(light.position - fragPos);
	//atten = 1 / (const + linear * distance + quad * distance^2)
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse,TexCoords).rgb;
	vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
	
	//apply attenuation for reducing strength across distance
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	
	//attenuation
	float distance = length(light.position - fragPos);
	//atten = 1 / (const + linear * distance + quad * distance^2)
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	//intensity
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	//I = (theta - gamma) / epsilon [0.0, 1.0]
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);


	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse,TexCoords).rgb;
	vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
	
	//apply attenuation & intensity for reducing strength across distance and towards the edges of the cone
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	return (ambient + diffuse + specular);
}