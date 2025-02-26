#version 330 core
layout (location = 0) in vec3 aPos;

out vec4 cPos;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view; 
uniform mat4 projection;

const float tiling_factor = 6.0f;

void main () {
    TexCoords = vec2(aPos.x / 2.0f + 0.5f, aPos.y / 2.0f + 0.5f) * tiling_factor;
    cPos = projection * view * model * vec4(aPos, 1.0f);
    gl_Position = cPos;
}