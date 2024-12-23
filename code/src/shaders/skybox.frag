#version 330 core

out vec4 FragmentColor;

in vec3 exTexcoords;

uniform samplerCube skybox;

void main(void) {
    FragmentColor = vec4(1.0,1.0,1.0,1.0);//texture(skybox, exTexcoords);
}