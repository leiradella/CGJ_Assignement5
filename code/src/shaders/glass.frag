#version 330 core

in vec3 exNormal;
in vec3 ViewDir;

out vec4 FragmentColor;

uniform samplerCube skybox;
uniform float refractiveIndex;
uniform float airIndex;

void main(void) {
    vec3 V = normalize(ViewDir);
    vec3 N = normalize(exNormal);

    //reflection and refraction vectors
    vec3 reflected = reflect(V, N);
    vec3 refracted = refract(V, N, airIndex/refractiveIndex);

    //get the skybox colors based on the vectors
    vec4 reflectedColor =  texture(skybox, reflected);
    vec4 refractedColor =  texture(skybox, refracted);

    //schlick approximation
    float f = pow((airIndex - refractiveIndex)/(airIndex + refractiveIndex),2.0);
    float fresnel = f + (1-f)*pow(1 - dot(V, N), 5.0);
    
    FragmentColor = mix(refractedColor, reflectedColor, fresnel);
    FragmentColor.a = 0.3;
}
