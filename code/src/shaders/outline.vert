#version 330 core

layout (location = 1) in vec3 inPosition;
layout (location = 2) in vec3 inNormal;

uniform float OutlineThickness;

uniform mat4 ModelMatrix;

uniform Camera {
   mat4 ViewMatrix;
   mat4 ProjectionMatrix;
};

void main(void)
{
	vec3 offsetPosition = inPosition + inNormal * OutlineThickness;
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(offsetPosition, 1.0);
}
