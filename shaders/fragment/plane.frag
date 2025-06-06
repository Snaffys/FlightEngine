//#version 460 core
//in vec3 FragPos;
//in vec3 Normal;
//in vec2 UV;
//in vec3 Color;
//
//out vec4 FragColor;
//
//uniform sampler2D textureSampler;
//
//
//
//
//// Texture samplers
//uniform sampler2D baseColorTexture;
//uniform sampler2D metallicRoughnessTexture;
//uniform sampler2D emissiveTexture;
//uniform sampler2D aoTexture;
//
//// PBR material properties (in case textures are missing)
////uniform vec4 baseColorFactor;  // Base color multiplier (RGBA)
//uniform float metallicFactor;   // Metallic strength
//uniform float roughnessFactor;  // Roughness strength
//
//
////uniform vec3 lightPos;
////uniform vec3 lightColor;
//
//void main() {
//    //// Ambient lighting
//    //float ambientStrength = 0.1;
//    //vec3 ambient = ambientStrength * lightColor;
//    //
//    //// Diffuse lighting
//    //vec3 norm = normalize(Normal);
//    //vec3 lightDir = normalize(lightPos - FragPos);
//    //float diff = max(dot(norm, lightDir), 0.0);
//    //vec3 diffuse = diff * lightColor;
//    //
//    //// Combine lighting
//    //vec3 result = (ambient + diffuse) * texture(textureSampler, UV).rgb * Color;
//    //FragColor = vec4(result, 1.0);
//
//
//
//    //FragColor = vec4(texture(textureSampler, UV).rgb * Color, 1.0);
//
//
//
//    // Sample base color texture (with fallback to factor)
//    vec4 baseColor = texture(baseColorTexture, UV) * vec4(Color, 1.0);
//    
//    // Sample metallic-roughness texture
//    vec2 metalRough = texture(metallicRoughnessTexture, UV).bg;  // Metallic = blue, Roughness = green
//    float metallic = metalRough.r * metallicFactor;
//    float roughness = metalRough.g * roughnessFactor;
//
//    // Sample AO texture
//    float ao = texture(aoTexture, UV).r;
//
//    // Sample emissive texture
//    vec3 emissive = texture(emissiveTexture, UV).rgb;
//
//    // Final output color (for now, no lighting, just material response)
//    vec3 finalColor = baseColor.rgb * ao + emissive;
//    FragColor = vec4(finalColor, baseColor.a);
//}
//
//
////#version 460 core
////
////layout (location = 0) out vec4 fragColor;
////
////uniform sampler2D textureSampler;
////
////void main(){
////    fragColor = vec4(texture(textureSampler, vec2(1.0)).rgb, 1.0);
////}


#version 460 core

// Inputs from Vertex Shader
in vec3 FragPos;
in vec3 Normal;
in vec2 UV;
in vec3 Color;

// PBR Texture Samplers
uniform sampler2D baseColorTexture;
uniform sampler2D metallicTexture;
uniform sampler2D roughnessTexture;
uniform sampler2D metallicRoughnessTexture;
uniform sampler2D aoTexture;
uniform sampler2D emissiveTexture;

// Material Factors (for when textures are missing)
uniform vec4 baseColorFactor;
uniform float metallicFactor;
uniform float roughnessFactor;
uniform vec3 emissiveFactor;

// Flags for whether textures exist
uniform bool hasBaseColorTexture;
uniform bool hasMetallicTexture;
uniform bool hasRoughnessTexture;
uniform bool hasMetallicRoughnessTexture;
uniform bool hasAOTexture;
uniform bool hasEmissionTexture;

// Output Color
out vec4 FragColor;

void main() {
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(0.3, 0.3, 0.3);

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(1.0, 900.0, 1.0) - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(0.3, 0.3, 0.3);


    // Sample Base Color (Albedo)
    vec4 albedo = hasBaseColorTexture ? texture(baseColorTexture, UV) : baseColorFactor;
    
    // Sample Metallic & Roughness
    float metallic;
    float roughness;

    if (hasMetallicRoughnessTexture) {
        vec2 mr = texture(metallicRoughnessTexture, UV).bg; // Metallic (B), Roughness (G)
        metallic = mr.r * metallicFactor;
        roughness = mr.g * roughnessFactor;
    } else {
        metallic = hasMetallicTexture ? texture(metallicTexture, UV).r : metallicFactor;
        roughness = hasRoughnessTexture ? texture(roughnessTexture, UV).r : roughnessFactor;
    }
    
    // Sample Ambient Occlusion
    float ao = hasAOTexture ? texture(aoTexture, UV).r : 1.0;
    
    // Sample Emission
    vec3 emission = hasEmissionTexture ? texture(emissiveTexture, UV).rgb : emissiveFactor;
    
    // Apply Vertex Color (if present)
    vec3 finalColor = albedo.rgb * Color * ao + emission;
    
    finalColor = (ambient + diffuse) * finalColor;

    FragColor = vec4(finalColor, albedo.a);
}

