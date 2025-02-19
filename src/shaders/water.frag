#version 330 core 
out vec4 FragColor;

in vec2 wTexCoords;

uniform sampler2D reflection_texture;
uniform sampler2D refraction_texture;

void main() {
    vec4 refl_color = texture(reflection_texture, wTexCoords);
    vec4 refr_color = texture(refraction_texture, wTexCoords);
    //FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    FragColor = mix(refl_color, refr_color, 0.5);
    //FragColor = refl_color;
}
