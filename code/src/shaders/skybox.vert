#version 330 core

layout (location = 1) in vec3 inPosition;
layout (location = 3) in vec2 inTexcoords;

out vec2 TexCoords;

uniform Camera {
   mat4 ViewMatrix;
   mat4 ProjectionMatrix;
};

void main(void) {
    TexCoords = inTexcoords;
    mat4 viewNoTranslation = mat4(mat3(ViewMatrix));
    vec4 ClipCoordVertex = ProjectionMatrix * viewNoTranslation * vec4(inPosition, 1.0);
    gl_Position = ClipCoordVertex.xyzz;
}