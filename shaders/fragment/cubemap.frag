#version 460 core

layout (location = 0) out vec4 fragColor;

in VS_OUT {
    vec3 texCoords;
} fsIn;

uniform samplerCube cubemap;

void main(){
    fragColor = texture(cubemap, fsIn.texCoords);
}