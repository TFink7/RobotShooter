varying vec3 N;
varying vec3 p;
uniform sampler2D myTexture;
varying vec2 vTexCoord;
varying vec3 texCoords;

uniform int isSkybox;
uniform samplerCube skybox;
uniform int applyTexture;

void main (void)
{
    if (isSkybox == 1) {
        vec4 skyColor = textureCube(skybox, texCoords);
        gl_FragColor = skyColor;
    } else {
        vec3 L = normalize(gl_LightSource[0].position.xyz - p);
        vec3 V = normalize(-p);
        vec3 R = normalize(-reflect(L, N));

        vec4 Iamb = gl_FrontLightProduct[0].ambient;
        vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(N, L), 0.0);
        vec4 Ispec = gl_FrontLightProduct[0].specular * pow(max(dot(R, V), 0.0), 0.3 * gl_FrontMaterial.shininess);

        float scaleFactor = 4.0;
		
        if (applyTexture == 1) {
			gl_FragColor = (Iamb + Idiff + Ispec) * scaleFactor * texture2D(myTexture, vTexCoord);
		} else {
			gl_FragColor = (Iamb + Idiff + Ispec) * scaleFactor;
		}
    }
}