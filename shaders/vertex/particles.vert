#version 460 core
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec2 aTexCoords;
//
//out vec2 texCoords;
//out vec4 particleColor;
//
//
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
//uniform vec3 offset;
//uniform vec4 color;
////uniform float scale;
//
//void main()
//{
//    float scale = 10.0f;
//    texCoords = aTexCoords;
//    particleColor = color;
//    gl_Position = projection * view * model * vec4(aPos, 1.0);
//        //gl_Position = projection * view * vec4((aPos * scale) + offset, 1.0);
//}


// Vertex shader for proper screen-aligned particles
layout(location = 0) in vec3 aQuadPos;   // [-0.5, 0.5] quad
layout(location = 1) in vec2 aQuadUV;
layout(location = 2) in vec3 aPosition;  // World position
layout(location = 3) in vec3 aVelocity;
layout(location = 4) in vec4 aColorStart;
layout(location = 5) in vec4 aColorEnd;
layout(location = 6) in float aSizeStart;
layout(location = 7) in float aSizeEnd;
layout(location = 8) in float aLife;
layout(location = 9) in float aRotation;

uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uCameraRight;    // Camera's right vector in world space
uniform vec3 uCameraUp;       // Camera's up vector in world space
uniform float uTime;
uniform float uWindStrength;

out vec4 vColor;
out vec2 vUV;
out float vLife;

void main() {
    // Interpolate properties
    vColor = mix(aColorStart, aColorEnd, aLife);
    float size = mix(aSizeStart, aSizeEnd, aLife);
    vLife = aLife;
    vUV = aQuadUV;

    // Calculate world position with billboarding
    vec3 worldPos = aPosition;
    
    // Wind effect (optional)
    worldPos.x += sin(uTime + aPosition.z * 0.1) * uWindStrength * (1.0 - aLife);
    
    // Calculate billboarded vertex position in world space
    vec3 vertexWorldPos = worldPos 
        + uCameraRight * aQuadPos.x * size
        + uCameraUp * aQuadPos.y * size;
    
    // Apply rotation if needed
    if (aRotation != 0.0) {
        vec3 offset = vertexWorldPos - worldPos;
        float c = cos(aRotation);
        float s = sin(aRotation);
        offset.xz = vec2(
            offset.x * c - offset.z * s,
            offset.x * s + offset.z * c
        );
        vertexWorldPos = worldPos + offset;
    }
    
    // Transform to clip space
    gl_Position = uProjection * uView * vec4(vertexWorldPos, 1.0);
}