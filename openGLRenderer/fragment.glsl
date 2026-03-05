#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform float mix_percentage = 0.2f;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    //Under the hood, mix is just an interpolation function.
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mix_percentage);
}