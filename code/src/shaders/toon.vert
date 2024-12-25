#version 330 core

layout (location = 1) in vec3 inPosition;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inTexcoord;

out vec3 exPosition;
out vec2 exTexcoord;
out vec3 exNormal;
out vec3 viewPos;

uniform mat4 ModelMatrix;

uniform Camera {
   mat4 ViewMatrix;
   mat4 ProjectionMatrix;
};

void main(void)
{
	exPosition = vec3(ModelMatrix * vec4(inPosition, 1.0f));
	exTexcoord = inTexcoord;
	exNormal = mat3(transpose(inverse(ModelMatrix))) * inNormal;
	mat4 invView = inverse(ViewMatrix);
	viewPos = vec3(invView[3][0], invView[3][1], invView[3][2])/invView[3][3];
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(inPosition, 1.0);
}
