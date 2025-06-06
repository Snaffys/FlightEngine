#version 460 core

out vec4 FragColor;

in vec3 Height;
in vec3 Normal;  
in vec3 FragPos;  

uniform float posX;
uniform float posY;

vec3 lerp(vec3 a, vec3 b, float t) {
    return a + t * (b - a);
}

void main()
{
    //float h = (Height + 16)/32.0f;	// shift and scale the height into a grayscale value
    //vec3 res = Height;
    //res = normalize(res);
    //res.y = Height.y * -1;

//    float ambientStrength = 0.1;
//    vec3 ambient = ambientStrength * vec3(0.3,0.3,0.3);
//
//    vec3 norm = normalize(Normal);
//    vec3 lightDir = normalize(vec3(1.0, 900.0, 1.0) - FragPos);
//    float diff = max(dot(norm, lightDir), 0.0);
//    vec3 diffuse = diff * vec3(0.3,0.3,0.3);
//
//    vec3 resultColor;
//    if(Height.y < 0.3){
//        resultColor = vec3(0.0, 0.2, 0.8);
//        diffuse = vec3(0);    
//    }
//    else if(Height.y < 0.5)
//        resultColor = vec3(0.01, 0.7, 0.05);
//    else if(Height.y < 0.7)
//        resultColor = vec3(0.6f);
//    else
//        resultColor = vec3(1.0f);
//
//    resultColor = (ambient + diffuse) * resultColor;
//    FragColor = vec4(resultColor, 1.0);



    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(0.3, 0.3, 0.3);

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(1.0, 900.0, 1.0) - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(0.3, 0.3, 0.3);

    vec3 colorDeepWater = vec3(0.0, 0.2, 0.8);
    vec3 colorShallowWater = vec3(0.0, 0.5, 0.9);
    vec3 colorSand = vec3(0.9, 0.8, 0.6);
    vec3 colorGrass = vec3(0.01, 0.7, 0.05);
    vec3 colorRock = vec3(0.6);
    vec3 colorSnow = vec3(1.0);

    vec3 resultColor;

    if (Height.y < 0.3) {
        resultColor = lerp(colorDeepWater, colorShallowWater, smoothstep(0.1, 0.3, Height.y));
        diffuse = vec3(0);
    } else if (Height.y < 0.5) {
        resultColor = lerp(colorShallowWater, colorSand, smoothstep(0.3, 0.4, Height.y));
        resultColor = lerp(resultColor, colorGrass, smoothstep(0.4, 0.5, Height.y));
    } else if (Height.y < 0.7) {
        resultColor = lerp(colorGrass, colorRock, smoothstep(0.5, 0.7, Height.y));
    } else {
        resultColor = lerp(colorRock, colorSnow, smoothstep(0.7, 0.9, Height.y));
    }

    resultColor = (ambient + diffuse) * resultColor;
    FragColor = vec4(resultColor, 1.0);
}



//float rand(vec2 co) {
//    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
//}
//
//void main()
//{
//    float ambientStrength = 0.1;
//    vec3 ambient = ambientStrength * vec3(0.3, 0.3, 0.3);
//
//    vec3 norm = normalize(Normal);
//    vec3 lightDir = normalize(vec3(1.0, 900.0, 1.0) - FragPos);
//    float diff = max(dot(norm, lightDir), 0.0);
//    vec3 diffuse = diff * vec3(0.3, 0.3, 0.3);
//
//    vec3 colorDeepWater = vec3(0.0, 0.2, 0.8);
//    vec3 colorShallowWater = vec3(0.0, 0.5, 0.9);
//    vec3 colorSand = vec3(0.9, 0.8, 0.6);
//    vec3 colorGrass = vec3(0.01, 0.7, 0.05);
//    vec3 colorRock = vec3(0.6);
//    vec3 colorSnow = vec3(1.0);
//
//    float noise = rand(FragPos.xz) * 0.05;
//
//    vec3 resultColor;
//
//    if (Height.y + noise < 0.3) {
//        resultColor = lerp(colorDeepWater, colorShallowWater, smoothstep(0.1, 0.3, Height.y + noise));
//        diffuse = vec3(0);
//    } else if (Height.y + noise < 0.5) {
//        resultColor = lerp(colorShallowWater, colorSand, smoothstep(0.3, 0.4, Height.y + noise));
//        resultColor = lerp(resultColor, colorGrass, smoothstep(0.4, 0.5, Height.y + noise));
//    } else if (Height.y + noise < 0.7) {
//        resultColor = lerp(colorGrass, colorRock, smoothstep(0.5, 0.7, Height.y + noise));
//    } else {
//        resultColor = lerp(colorRock, colorSnow, smoothstep(0.7, 0.9, Height.y + noise));
//    }
//
//    resultColor = (ambient + diffuse) * resultColor;
//    FragColor = vec4(resultColor, 1.0);
//}