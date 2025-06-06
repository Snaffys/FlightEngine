#version 460 core
layout (location = 0) in vec3 aPos;

out VS_OUT{
    vec3 texCoords;
} vsOut;

uniform mat4 projection;
uniform mat4 view;

void main(){
    vsOut.texCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}