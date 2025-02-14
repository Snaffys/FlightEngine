//#version 330 core
//out vec4 fragColor;  
//in vec3 colorOutput;
//  
//void main()
//{
//    fragColor = vec4(colorOutput, 1.0);
//}



#version 460 core

out vec4 FragColor;

in vec3 Height;
in vec3 Normal;  
in vec3 FragPos;  

uniform float posX;
uniform float posY;

void main()
{
    //float h = (Height + 16)/32.0f;	// shift and scale the height into a grayscale value
    //vec3 res = Height;
    //res = normalize(res);
    //res.y = Height.y * -1;

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(0.3,0.3,0.3);

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(1.0, 500.0, 1.0) - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(0.3,0.3,0.3);

    vec3 resultColor;
    if(Height.y < 0.3){
        resultColor = vec3(0.0, 0.2, 0.8);
        diffuse = vec3(0);    
    }
    else if(Height.y < 0.5)
        resultColor = vec3(0.1, 0.8, 0.05);
    else if(Height.y < 0.7)
        resultColor = vec3(0.6f);
    else
        resultColor = vec3(1.0f);

    resultColor = (ambient + diffuse) * resultColor;
    FragColor = vec4(resultColor, 1.0);
}