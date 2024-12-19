#version 330 core

in vec3 exPosition;
in vec2 exTexcoord;
in vec3 exNormal;

out vec4 FragmentColor;

uniform sampler2D tex0;
uniform vec3 viewPos;

void main(void)
{
    vec3 lightPos = vec3(10.0, 10.0, 0.0);
    vec3 lightColor = vec3(100.0, 100.0, 100.0);

    
    vec4 texColor = texture(tex0, exTexcoord);
    FragmentColor = texture(tex0, exTexcoord);
}
