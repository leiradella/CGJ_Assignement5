#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 normalOut;
layout (location = 2) out vec4 depthOut;

in vec3 exPosition;
in vec2 exTexcoord;
in vec3 exNormal;

uniform vec3 lightPos = vec3(0, 2, 0);
uniform vec3 viewPos;
uniform vec3 coolColor = vec3(1, 0.5, 0);
uniform vec3 warmColor = vec3(0.5, 0, 1);
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform float specularStrength = 0.5;
uniform float alpha = 0.0;
uniform float beta = 0.0;
uniform sampler2D tex0;

void main()
{
	vec3 objectColor = vec3(texture(tex0, exTexcoord));
	vec3 norm = normalize(exNormal);
	vec3 lightDir = normalize(lightPos - exPosition);
	// if angle between norm and lightDir > 90 degrees, dot product is neg
	float diff = max(dot(norm, lightDir), 0.0);

	vec3 finalCool = coolColor + alpha * objectColor;
	vec3 finalWarm = warmColor + beta * objectColor;

	float lerp = (1.0 + diff) / 2.0;

	finalCool = (1 - lerp) * finalCool;
	finalWarm = lerp * finalWarm;

	// specular
	vec3 viewDir = normalize(viewPos - exPosition);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	FragColor = vec4(finalCool + finalWarm + specular, 1.0);
	normalOut = vec4(exNormal, 1.0);

	depthOut = vec4(exPosition.z, exPosition.z, exPosition.z, 1.0);
}