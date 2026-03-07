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

struct Light {
	//position of the light
	vec3 position;
	//color and strength of ambient light
	vec3 ambient;
	//color and strength of diffuse light
	vec3 diffuse;
	//color and strength of specular light
	vec3 specular;

};
uniform Light light;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;


void main()
{
	//Define ambient lighting
	//ambient lighting is just a constant light applied to all objects in a scene
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
	//diffuse impact on light is the dot product between norm and light direction vectors
	//if our angle is greater than 90 we would end up with a negative component (which would result in negative colors), so we clamp the value to 0.0
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
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
	vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

	//Emission Component
	vec3 emission = texture(material.emission, TexCoords).rgb;

	//result is just the ambient + diffuse light multiplied by the color of the object (so that the light can affect the color of the object)
	vec3 result = (ambient + diffuse + specular + emission);
	FragColor = vec4(result, 1.0);
}