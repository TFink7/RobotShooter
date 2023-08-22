varying vec3 texCoords;
varying vec3 N;
varying vec3 p;
varying vec2 vTexCoord;

uniform int isSkybox;

void main(void)
{
    // eye coords
    vec4 position = gl_ModelViewMatrix * gl_Vertex;

    if (isSkybox == 1) {
        texCoords = position.xyz;
    } else {
        N = normalize(gl_NormalMatrix * gl_Normal);
        p = vec3(position);
        vTexCoord = vec2(gl_MultiTexCoord0);
    }

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}