//  Per Pixel Lighting shader with texture
#version 120

// Light indices
uniform int ambientLight;
uniform int directedLight;

varying vec3 View;
varying vec3 Light;
varying vec3 Normal;

void main()
{
   //  Vertex location in modelview coordinates
   vec4 P = gl_ModelViewMatrix * gl_Vertex;
   //  Light position
   Light  = gl_LightSource[directedLight].position.xyz - P.xyz;
   //  Normal
   Normal = gl_NormalMatrix * gl_Normal;
   //  Eye position
   View  = -P.xyz;
   //  Vertex color (ref https://stackoverflow.com/a/2554526)
   gl_FrontColor = gl_Color;
   //  Texture based on position
   gl_TexCoord[0] = gl_Vertex;
   //  Set vertex position
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
