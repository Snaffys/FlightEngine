#version 460 core

in vec4 vColor;
in vec2 vUV;
in float vLife;

out vec4 FragColor;

void main() {
    // Option 1: Point sprite rendering
    float alpha = smoothstep(0.5, 0.4, length(gl_PointCoord - 0.5));
    FragColor = vec4(vColor.rgb, vColor.a * alpha);

    // Option 2: Quad-based rendering (if using textured quads)
    // FragColor = vColor * texture(uTexture, vUV);
    
    // Fade out at end of life
    FragColor.a *= smoothstep(0.0, 0.1, vLife);
}