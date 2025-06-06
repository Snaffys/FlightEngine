#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec3 aColor;

out vec3 FragPos;
out vec3 Normal;
out vec2 UV;
out vec3 Color;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 transform;

void main() {
    //gl_Position = transform * vec4(aPos, 1.0);
    gl_Position = projection * view * transform * vec4(aPos, 1.0);
    FragPos = vec3(transform * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(transform))) * aNormal; // Transform normals
    Color = aColor;
    //Color = vec3(projection * view * transform * vec4(aPos, 1.0));
    UV = aUV;
}