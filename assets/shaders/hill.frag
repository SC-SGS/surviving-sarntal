#version 330

in vec3 fragNormal;
in vec2 fragTexCoord;

uniform sampler2D hilltex;

void main()
{
	vec3 l = normalize(vec3(0.0, 1.0, 2.0));
	float diffuse = max(dot(fragNormal, l), 0.0);
	float ambient = 0.3;
	
	float factor = ambient + diffuse;

	//vec4 color = texture(hilltex, fragTexCoord);
	//vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
	//color *= ambient + diffuse;
	//color.w = 1.0;
	//gl_FragColor = color;
	
	//gl_FragColor = vec4(0.0, (ambient + diffuse) * 1.0, 0.0, 1.0);
	//gl_FragColor = vec4(fragNormal.x, fragNormal.y, fragNormal.z, 1.0);

	gl_FragColor = texture(hilltex, fragTexCoord);// * (factor, factor, factor, 1.0);
};