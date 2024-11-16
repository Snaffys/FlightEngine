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

uniform float posX;
uniform float posY;

void main()
{
    //float h = (Height + 16)/32.0f;	// shift and scale the height into a grayscale value
    //vec3 res = Height;
    //res = normalize(res);
    //res.y = Height.y * -1;
    vec3 resultColor;
    if(Height.y < 0.3)
        resultColor = vec3(0.0, 0.2, 0.8);
    else if(Height.y < 0.5)
        resultColor = vec3(0.1, 0.8, 0.05);
    else if(Height.y < 0.7)
        resultColor = vec3(0.6f);
    else
        resultColor = vec3(1.0f);
    FragColor = vec4(resultColor, 1.0);
}