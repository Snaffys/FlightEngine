#version 460 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out; // Each triangle has 3 vertices, and we output 3 lines.

in vec3 Normal[]; // Interpolated normal from vertex shader
in vec3 FragPos[]; // Vertex position from vertex shader

uniform mat4 view;
uniform mat4 projection;
uniform float normalLength = 2.0; // Control the length of the normal lines

void main() {
    for (int i = 0; i < 3; i++) { // Loop over the three triangle vertices
        gl_Position = projection * view * vec4(FragPos[i], 1.0);
        EmitVertex(); // First point of the line

        vec3 normalEnd = FragPos[i] + Normal[i] * normalLength; // Move in world space
        gl_Position = projection * view * vec4(normalEnd, 1.0);
        EmitVertex(); // Second point of the line

        EndPrimitive(); // Finish the line segment
    }
}
