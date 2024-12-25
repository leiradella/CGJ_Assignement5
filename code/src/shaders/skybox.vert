#version 330 core

layout (location = 1) in vec3 inPosition;

out vec3 exTexcoords;

uniform Camera {
   mat4 ViewMatrix;
   mat4 ProjectionMatrix;
};

void main(void) {
    exTexcoords = inPosition;
    mat4 viewNoTranslation = mat4(mat3(ViewMatrix));
    vec4 ClipCoordVertex = ProjectionMatrix * viewNoTranslation * vec4(inPosition, 1.0);
    //gl_Position = ProjectionMatrix * ViewMatrix * vec4(inPosition, 1.0);
    gl_Position = ClipCoordVertex.xyww;
}