#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCooords;

out VS_OUT{
	vec2 texCoords;
} vsOut;

void main()
{
	vsOut.texCoords = aTexCooords;
	gl_Position = vec4(aPos, 0.0, 1.0);
}