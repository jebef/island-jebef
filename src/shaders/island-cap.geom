#version 330 core 
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 world_pos;
    vec3 normal;
    vec2 texture_coords;
} gs_in[];

uniform mat4 projection;
uniform mat4 view;

out vec3 WorldPos;
out vec3 Normal;
out vec2 TexCoords;

void main() {

    for (int i = 0; i < 3; i++) {
        // vertex is getting clipped 
        if (gl_in[i].gl_ClipDistance[0] < 0.0f) {
            gl_Position = projection * view * vec4(gs_in[i].world_pos.x, 0.0f, gs_in[i].world_pos.z, 1.0f);
        // vertex is safe 
        } else {
            gl_Position = gl_in[i].gl_Position;
        }
        WorldPos = gs_in[i].world_pos;
        Normal = gs_in[i].normal;
        TexCoords = gs_in[i].texture_coords;
        EmitVertex();
    }
    EndPrimitive();
}