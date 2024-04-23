#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;
in vec3 vertexNormal;

uniform mat4 mvp;

out vec3 fragNormal;
out vec2 fragTexCoord;

void main()
{ 
	fragNormal  = normalize(vertexNormal);
	gl_Position = mvp*vec4(vertexPosition, 1.0);
};