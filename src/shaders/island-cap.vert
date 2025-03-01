#version 330 core 
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal;

uniform vec4 clip_plane;

out VS_OUT {
    vec3 world_pos;
    vec3 normal;
    vec2 texture_coords;
} vs_out;

void main() {
    vec4 world_pos = model * vec4(aPos, 1.0f);

    vs_out.world_pos = vec3(world_pos);
    vs_out.normal = normalize(normal * aNorm);
    vs_out.texture_coords = aTexCoords;

    gl_ClipDistance[0] = dot(world_pos, clip_plane);
    
    gl_Position = projection * view * world_pos;
}