#version 330 core
layout (location = 0) in vec3 aPos;

out vec4 cPos;

uniform mat4 model;
uniform mat4 view; 
uniform mat4 projection;

void main () {
    cPos = projection * view * model * vec4(aPos, 1.0f);
    gl_Position = cPos;
}