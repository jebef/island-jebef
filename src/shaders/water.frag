#version 330 core 
out vec4 FragColor;

in vec4 cPos;

uniform sampler2D reflection_texture;
uniform sampler2D refraction_texture;

void main() {
    // convert clip space position to normalized device coordinates
    vec2 nPos = (cPos.xy / cPos.w) / 2.0f + 0.5f;
    
    // calculate reflection/refraction texture coordinates 
    vec2 refl_tex_coords = vec2(nPos.x, -nPos.y);
    vec2 refr_tex_coords = vec2(nPos.x, nPos.y);

    FragColor = mix(texture(reflection_texture, refl_tex_coords), texture(refraction_texture, refr_tex_coords), 0.5f);
}
