#version 330 core
out vec4 fragColor;  
in vec3 colorOutput;
  
void main()
{
    fragColor = vec4(colorOutput, 1.0);
}