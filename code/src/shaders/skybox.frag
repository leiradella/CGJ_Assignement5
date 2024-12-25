#version 330 core

out vec4 FragmentColor;

in vec3 exTexcoords;

uniform samplerCube skybox;

void main(void) {
    FragmentColor = texture(skybox, normalize(exTexcoords));
}