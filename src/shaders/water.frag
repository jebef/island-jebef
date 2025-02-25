#version 330 core 
out vec4 FragColor;

in vec2 wTexCoords;

uniform sampler2D reflection_texture;
uniform sampler2D refraction_texture;

void main() {
    FragColor = mix(texture(reflection_texture, wTexCoords), texture(refraction_texture, wTexCoords), 0.5f);
}
