#version 330 core

in vec3 exPosition;
in vec2 exTexcoord;
in vec3 exNormal;

out vec4 FragmentColor;

uniform sampler2D tex0;
uniform int colorSteps;
uniform vec3 lightPos;
uniform vec3 lightColor;

void main(void)
{
    vec4 texColor = texture(tex0, exTexcoord);

    //variables to calculate each component
    vec3 normal = normalize(exNormal);
    vec3 lightDir = normalize(lightPos - exPosition);

    //ambient component
    vec3 ambient = 0.2 * lightColor * vec3(texColor);
    //diffuse component
    float diff = dot(normal, lightDir);
    float diffuseToon = max(ceil(diff * float(colorSteps)) / float(colorSteps), 0.0);
    //texture color

    vec4 toonColor = diffuseToon * vec4(lightColor, 1.0) * texColor + vec4(ambient, 1.0);


    FragmentColor = toonColor;
}
