//#version 460 core
//
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
//layout (location = 2) in vec2 texCoords;
//
//out vec3 colorOutput;
//
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
//
//void main(){
//    gl_Position = projection * view * model * vec4(aPos, 1.0);
//    colorOutput = aColor;
//}




#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 Height;
out vec3 Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Height = vec3(aPos.y / 256);
    Position = (view * model * vec4(aPos, 1.0)).xyz;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}