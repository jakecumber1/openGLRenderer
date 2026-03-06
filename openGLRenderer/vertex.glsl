#version 330 core
layout (location = 0) in vec3 aPos;

uniform bool flip = false;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    if (flip) {
        gl_Position = projection * view * model * vec4(aPos.x * -1, aPos.y * -1, aPos.z, 1.0);
    } else {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
    //todo: comeback if flipping doesn't work
}