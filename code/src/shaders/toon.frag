#version 330 core

in vec3 exPosition;
in vec2 exTexcoord;
in vec3 exNormal;

out vec4 FragmentColor;

uniform sampler2D tex0;
uniform vec3 viewPos;
uniform int colorSteps = 3;

void main(void)
{
    //light attributes
    vec3 lightPos = vec3(0.0, 2.0, 0.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0); //white
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
