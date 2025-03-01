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
    int triangle_safe = 0;

    for (int i = 0; i < 3; i++) {
        // check to see if the entire triangle is safe
        if (gl_in[i].gl_ClipDistance[0] > 0.0f) {
            triangle_safe += 1;
        } 
    }
    // if safe, clamp y values to just above 0 to hide palm tree base 
    if (triangle_safe == 3) {
        for (int j = 0; j < 3; j++) {
            gl_Position = projection * view * vec4(gs_in[j].world_pos.x, 0.3f, gs_in[j].world_pos.z, 1.0f);
            WorldPos = gs_in[j].world_pos;
            Normal = gs_in[j].normal;
            TexCoords = gs_in[j].texture_coords;
            EmitVertex();
        }
        EndPrimitive();
    }
}