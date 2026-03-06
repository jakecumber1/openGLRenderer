#version 330 core
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
	//Define ambient lighting
	//ambient lighting is just a constant light applied to all objects in a scene
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	//diffuse impact on light is the dot product between norm and light direction vectors
	//if our angle is greater than 90 we would end up with a negative component (which would result in negative colors), so we clamp the value to 0.0
	float diff = max(dot(norm, lightDir), 0.0);
	//get the diffuse component of the light by multiplying the diffuse strength with the light color
	vec3 diffuse = diff * lightColor;

	//result is just the ambient + diffuse light multiplied by the color of the object (so that the light can affect the color of the object)
	vec3 result = (ambient + diffuse) * objectColor;
	FragColor = vec4(result, 1.0);
}