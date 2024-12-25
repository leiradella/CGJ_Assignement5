#version 330 core

layout (location = 1) in vec3 inPosition;
layout (location = 2) in vec3 inNormal;

out vec3 exPosition;
out vec3 exNormal;

uniform mat4 ModelMatrix;

uniform Camera {
   mat4 ViewMatrix;
   mat4 ProjectionMatrix;
};

void main(void)
{
	exPosition = vec3(ModelMatrix * vec4(inPosition, 1.0f));
	exNormal = mat3(transpose(inverse(ModelMatrix))) * inNormal;
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(inPosition, 1.0);
}
