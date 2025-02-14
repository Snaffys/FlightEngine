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
layout (location = 1) in vec3 aNormal;

out vec3 Height;
out vec3 Position;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Height = vec3(aPos.y / 256);
    Position = (view * model * vec4(aPos, 1.0)).xyz;
    FragPos = vec3(model * vec4(aPos, 1.0f));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}