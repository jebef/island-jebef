#version 330 core 

out vec4 FragColor;

in vec2 wTexCoords;

uniform sampler2D reflection_texture;
uniform sampler2D refraction_texture;

void main() {
    vec4 refl_color = vec4(texture(reflection_texture, wTexCoords).rgb, 0.5);
    vec4 refr_color = vec4(texture(refraction_texture, wTexCoords).rgb, 0.5);
    FragColor = refl_color;

}