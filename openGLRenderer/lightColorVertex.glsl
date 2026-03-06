#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	FragPos = vec3(model * vec4(aPos, 1.0));
	//This creates a 3x3 normal matrix, which will allow us to do non uniform scaling while keeping the normal vector perp to the surface
	//inverse operations are costly, so try to limit their use
	Normal = mat3(transpose(inverse(model))) * aNormal;
}