#version 330 core

in vec3 exPosition;
in vec2 exTexcoord;
in vec3 exNormal;

out vec4 FragmentColor;

uniform sampler2D tex0;
uniform vec3 viewPos;

void main(void)
{
    //light attributes
    vec3 lightPos = vec3(0.0, 2.0, 0.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0); //white

    //variables to calculate each component
    vec3 normal = normalize(exNormal);
    vec3 lightDir = normalize(lightPos - exPosition);
    vec3 viewDir = normalize(viewPos - exPosition);
    vec3 reflectDir = reflect(-lightDir, normal);

    //ambient component
    vec3 ambient = 0.25 * lightColor;
    //diffuse component
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    //specular component
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * lightColor;  

    //texture color
    vec4 texColor = texture(tex0, exTexcoord);

    FragmentColor = vec4(ambient + diffuse + specular, 1.0) * texColor;
}
