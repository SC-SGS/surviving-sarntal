#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
//in vec4 fragColor;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D grasstex;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;


struct MaterialProperty {
    vec3 color;
    int useSampler;
    sampler2D sampler;
};


// Input lighting values
uniform vec4 ambient;
uniform vec3 viewPos;

void main()
{
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(grasstex, fragTexCoord);
    if(texelColor.a < 0.1)
        discard;
    vec3 lightDot = vec3(0.0);
    vec3 normal = normalize(fragNormal);
    vec3 viewD = normalize(viewPos - fragPosition);
    vec3 specular = vec3(0.0);
    float strenght = 1-fragTexCoord.y;
    texelColor.rgb = texelColor.rgb * strenght;
    finalColor = texelColor;
    //finalColor = vec4(0,1-fragTexCoord.y,0,1);

    // NOTE: Implement here your fragment shader code

//    for (int i = 0; i < MAX_LIGHTS; i++)
//    {
//        if (lights[i].enabled == 1)
//        {
//            vec3 light = vec3(0.0);
//
//            if (lights[i].type == LIGHT_DIRECTIONAL)
//            {
//                light = -normalize(lights[i].target - lights[i].position);
//            }
//
//            if (lights[i].type == LIGHT_POINT)
//            {
//                light = normalize(lights[i].position - fragPosition);
//            }
//
//            float NdotL = max(dot(normal, light), 0.0);
//            lightDot += lights[i].color.rgb*NdotL;
//
//            float specCo = 0.0;
//            if (NdotL > 0.0) specCo = pow(max(0.0, dot(viewD, reflect(-(light), normal))), 16.0); // 16 refers to shine
//            specular += specCo;
//        }
//    }

//    finalColor = (texelColor*((colDiffuse + vec4(specular, 1.0))*vec4(lightDot, 1.0)));
//    finalColor += texelColor*(ambient/10.0)*colDiffuse;
//
//    // Gamma correction
//    finalColor = pow(finalColor, vec4(1.0/2.2));
}