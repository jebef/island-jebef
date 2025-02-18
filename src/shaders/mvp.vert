#version 330 core 
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal;

uniform vec4 clip_plane;
uniform bool clip;

out vec3 wPos;
out vec3 wNorm;
out vec2 wTexCoords;

out float clip_distance;

void main() {
    wPos = vec3(model * vec4(aPos, 1.0f));
    wNorm = normalize(normal * aNorm);
    wTexCoords = aTexCoords;
    
    if (clip) {
        vec4 world_pos = model * vec4(aPos, 1.0f);
        clip_distance = dot(world_pos, clip_plane);
    } else {
        clip_distance = 1;
    }
    
    gl_Position = projection * view * vec4(wPos, 1.0f);
}