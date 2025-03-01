#version 330 core
layout (location = 0) in vec3 aPos;

out vec4 cPos;
out vec2 TexCoords;
out vec3 dFragToCamera;

uniform mat4 model;
uniform mat4 view; 
uniform mat4 projection;
uniform vec3 camera_pos;

const float tiling_factor = 5.0f;

void main () {
    TexCoords = vec2(aPos.x / 2.0f + 0.5f, aPos.y / 2.0f + 0.5f) * tiling_factor;
    vec4 wPos = model * vec4(aPos, 1.0f);
    dFragToCamera = normalize(camera_pos - wPos.xyz);
    cPos = projection * view * wPos;
    gl_Position = cPos;
}