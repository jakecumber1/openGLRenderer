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
struct Light {
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
//definition for directional light (like the sun)
struct DirectionalLight {
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


uniform Light light;
uniform DirectionalLight sun;
uniform SpotLight flashlight;
uniform bool useSun;
uniform bool usePoint;
uniform bool useSpot;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;


void main()
{
	if (usePoint) {
	//Define ambient lighting
	//ambient lighting is just a constant light applied to all objects in a scene
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
	//diffuse impact on light is the dot product between norm and light direction vectors
	//if our angle is greater than 90 we would end up with a negative component (which would result in negative colors), so we clamp the value to 0.0
	vec3 norm = normalize(Normal);
	vec3 lightDir;
	lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	//get the diffuse component of the light by multiplying the diffuse strength with the light color
	vec3 diffuse =  light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

	//Now specular lighting component
	vec3 viewDir = normalize(viewPos - FragPos);
	//negate the lightDir vector, since the reflect function expects the dir vector to point FROM the light source to the fragment, but lightDir is currently pointing from frag toward the light
	//Norm can be passed in as is
	vec3 reflectDir = reflect(-lightDir, norm);
	//finally calculate the component
	//32, the power we selected, is acting as our "shininess" value for the object
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	float distance = length(light.position - FragPos);
	//calculate attenuation value
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	//apply attenuation across all components
	ambient *= attenuation;
	diffuse *= attenuation;
	spec *= attenuation;
	vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

	//Emission Component
	vec3 emission = texture(material.emission, TexCoords).rgb;

	//result is just the ambient + diffuse light multiplied by the color of the object (so that the light can affect the color of the object)
	vec3 result = (ambient + diffuse + specular + emission);
	FragColor = vec4(result, 1.0);
	}
	if (useSun) {
		//Define ambient lighting
		//ambient lighting is just a constant light applied to all objects in a scene
		vec3 ambient = sun.ambient * texture(material.diffuse, TexCoords).rgb;
		//diffuse impact on light is the dot product between norm and light direction vectors
		//if our angle is greater than 90 we would end up with a negative component (which would result in negative colors), so we clamp the value to 0.0
		vec3 norm = normalize(Normal);
		vec3 lightDir;
		//negated gives us the direction pointing away from the light source, not a direction pointing TO the light source
		lightDir = normalize(-sun.direction);
		float diff = max(dot(norm, lightDir), 0.0);
		//get the diffuse component of the light by multiplying the diffuse strength with the light color
		vec3 diffuse =  sun.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

		//Now specular lighting component
		vec3 viewDir = normalize(viewPos - FragPos);
		//negate the lightDir vector, since the reflect function expects the dir vector to point FROM the light source to the fragment, but lightDir is currently pointing from frag toward the light
		//Norm can be passed in as is
		vec3 reflectDir = reflect(-lightDir, norm);
		//finally calculate the component
		//32, the power we selected, is acting as our "shininess" value for the object
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 specular = sun.specular * spec * texture(material.specular, TexCoords).rgb;

		//Emission Component
		vec3 emission = texture(material.emission, TexCoords).rgb;

		//result is just the ambient + diffuse light multiplied by the color of the object (so that the light can affect the color of the object)
		vec3 result = (ambient + diffuse + specular + emission);
		FragColor = vec4(result, 1.0);
	}
	if (useSpot) {
		vec3 lightDir;
		lightDir = normalize(flashlight.position - FragPos);
		vec3 norm = normalize(Normal);
		float theta = dot(lightDir, normalize(-flashlight.direction));
		float epsilon = flashlight.cutOff - flashlight.outerCutOff;
		float intensity = clamp((theta - flashlight.outerCutOff) / epsilon, 0.0, 1.0);
		vec3 ambient = flashlight.ambient * texture(material.diffuse, TexCoords).rgb;
		//diffuse impact on light is the dot product between norm and light direction vectors
		//if our angle is greater than 90 we would end up with a negative component (which would result in negative colors), so we clamp the value to 0.0
		float diff = max(dot(norm, lightDir), 0.0);
		//get the diffuse component of the light by multiplying the diffuse strength with the light color
		vec3 diffuse =  flashlight.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

		//Now specular lighting component
		vec3 viewDir = normalize(viewPos - FragPos);
		//negate the lightDir vector, since the reflect function expects the dir vector to point FROM the light source to the fragment, but lightDir is currently pointing from frag toward the light
		//Norm can be passed in as is
		vec3 reflectDir = reflect(-lightDir, norm);
		//finally calculate the component
		//32, the power we selected, is acting as our "shininess" value for the object
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		float distance = length(flashlight.position - FragPos);
		//apply intensity, leave ambient alone so that light remains the same
		diffuse *= intensity;
		spec *= intensity;
		//calculate attenuation value
		float attenuation = 1.0 / (flashlight.constant + flashlight.linear * distance + flashlight.quadratic * (distance * distance));
		//apply attenuation across all components
		ambient *= attenuation;
		diffuse *= attenuation;
		spec *= attenuation;
		vec3 specular = flashlight.specular * spec * texture(material.specular, TexCoords).rgb;

		//Emission Component
		vec3 emission = texture(material.emission, TexCoords).rgb;

		//result is just the ambient + diffuse light multiplied by the color of the object (so that the light can affect the color of the object)
		vec3 result = (ambient + diffuse + specular + emission);
		FragColor = vec4(result, 1.0);
	}
}