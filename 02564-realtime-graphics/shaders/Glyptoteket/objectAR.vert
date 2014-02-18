varying vec4 color; // A varying variable used to transfer the color from
					// vertex to fragment program.

void main()
{

	// Simple ambient light. We do not rely on the material property
	// but provide the same ambient reflection for all materials.
	color = gl_LightSource[0].ambient*vec4(0.2,0.2,0.2,0);
	
	// Compute eyespace normal and position
	vec3 norm = normalize(gl_NormalMatrix*gl_Normal);
	vec4 pos = gl_ModelViewMatrix*gl_Vertex;
		
	// Compute dot product of normal and light source direction
	float d = dot(norm,normalize(gl_LightSource[0].position.xyz));

	// Note that if d<=0, no light from the light source can reach
	// the surface, so we do not add neither diffuse nor specular
	// contributions in that case.
	if(d>0.0)
	{
		color += d*gl_LightSource[0].diffuse*gl_FrontMaterial.diffuse;
		vec3 h = normalize(gl_LightSource[0].halfVector.xyz);
		float s = dot(norm, h);
		if(s>0.0)
			color += pow(s, gl_FrontMaterial.shininess)*
				gl_LightSource[0].specular*gl_FrontMaterial.specular;
	}
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	// ftransform is equivalent to gl_ModelViewProjectionMatrix * gl_Vertex
	gl_Position = ftransform();
}
