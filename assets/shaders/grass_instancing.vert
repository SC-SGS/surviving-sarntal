#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;

in mat4 instanceTransform;

// Input uniform values
uniform mat4 mvp;
uniform mat4 matNormal;
uniform float time;

// Output vertex attributes (to fragment shader)
out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;

void main()
{
    // Compute MVP for current instance
    mat4 mvpi = mvp*instanceTransform;

    // Send vertex attributes to fragment shader
    fragTexCoord = vertexTexCoord;
    float wind = 1.0 - fragTexCoord.y;
    fragNormal = normalize(vec3(matNormal*vec4(vertexNormal, 1.0)));

    vec3 pos = vertexPosition;
    pos.x = pos.x + sin(time) * wind * 0.5f;

    fragPosition = vec3(mvpi*vec4(pos, 1.0));

    // Calculate final vertex position
    gl_Position = mvpi*vec4(pos, 1.0);
}