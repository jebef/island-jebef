#version 330 core 
out vec4 FragColor;

in vec4 cPos;
in vec2 TexCoords;

uniform sampler2D reflection_texture;
uniform sampler2D refraction_texture;
uniform sampler2D dudv_map;
uniform float dudv_sampling_factor;

const float distortion_strength = 0.02;

void main() {
    // convert clip space position to normalized device coordinates
    vec2 nPos = (cPos.xy / cPos.w) / 2.0f + 0.5f;
    
    // calculate reflection/refraction texture coordinates 
    vec2 refl_tex_coords = vec2(nPos.x, -nPos.y);
    vec2 refr_tex_coords = vec2(nPos.x, nPos.y);

    // sample dudv map 
    vec2 distortion_1 = (texture(dudv_map, vec2(TexCoords.x + dudv_sampling_factor, TexCoords.y)).rg * 2.0f - 1.0f) * distortion_strength;
    vec2 distortion_2 = (texture(dudv_map, vec2 (-TexCoords.x + dudv_sampling_factor, TexCoords.y + dudv_sampling_factor)).rg * 2.0f - 1.0f) * distortion_strength;
    vec2 total_distortion = distortion_1 + distortion_2;

    // offset reflection/refraction texture coordinates with distortion
    refl_tex_coords += total_distortion;
    refl_tex_coords.x = clamp(refl_tex_coords.x, 0.001f, 0.999f);
    refl_tex_coords.y = clamp(refl_tex_coords.y, -0.999f, -0.001f);
    refr_tex_coords += total_distortion;
    refr_tex_coords = clamp(refr_tex_coords, 0.001f, 0.999f);


    FragColor = mix(texture(reflection_texture, refl_tex_coords), texture(refraction_texture, refr_tex_coords), 0.5f);
}
