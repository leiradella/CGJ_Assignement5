#version 330 core

layout (location = 1) in vec3 inPosition;
layout (location = 2) in vec3 inNormal;

out vec3 exNormal;
out vec3 ViewDir;

uniform mat4 ModelMatrix;

uniform Camera {
   mat4 ViewMatrix;
   mat4 ProjectionMatrix;
};

void main(void)
{
	exNormal = mat3(transpose(inverse(ModelMatrix))) * inNormal;
	ViewDir = vec3(inverse(ViewMatrix) * vec4(0.0, 0.0, 0.0, 1.0)) - vec3(ModelMatrix * vec4(inPosition, 1.0f));
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(inPosition, 1.0);
}
