varying vec3 normal;
varying vec3 vertex;
varying vec2 texCoord;

void main()
{
    // texCoord = gl_MultiTexCoord0.xy;
    vertex = gl_Vertex.xyz;
    normal = gl_Normal.xyz;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
