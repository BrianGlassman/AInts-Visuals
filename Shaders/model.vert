// Vertex shader for brick shader
// Derived from Orange Book Chapter 6
#version 120

// Light indices
uniform int ambientLight;
uniform int directedLight;

//  Light intensity and model position required by fragment shader
varying float LightIntensity;
varying vec3  ModelPos;

//  Phong lighting intensity only
float phong()
{
   //  P is the vertex coordinate on body
   vec3 P = vec3(gl_ModelViewMatrix * gl_Vertex);
   //  N is the object normal at P
   vec3 N = normalize(gl_NormalMatrix * gl_Normal);
   //  Light Position for light 0
   vec3 LightPos = vec3(gl_LightSource[directedLight].position);
   //  L is the light vector
   vec3 L = normalize(LightPos - P);
   //  R is the reflected light vector R = 2(L.N)N - L
   vec3 R = reflect(-L, N);
   //  V is the view vector (eye at the origin)
   vec3 V = normalize(-P);

   //  Diffuse light intensity is cosine of light and normal vectors
   float Id = max(dot(L,N) , 0.0);
   //  Shininess intensity is cosine of light and reflection vectors to a power
   float Is = (Id>0.0) ? pow(max(dot(R,V) , 0.0) , gl_FrontMaterial.shininess) : 0.0;

   //  Vertex color
   vec3 color = gl_FrontLightProduct[ambientLight].ambient.rgb
           +    gl_FrontLightProduct[directedLight].ambient.rgb
           + Id*gl_FrontLightProduct[directedLight].diffuse.rgb
           + Is*gl_FrontLightProduct[directedLight].specular.rgb;

   //  Vertex intensity
   return length(color);
}

void main()
{
   //  Scalar light intensity (for fragment shader)
   LightIntensity = phong();

   //  Save model coordinates (for fragment shader)
   ModelPos = gl_Vertex.xyz;

   //  Return fixed transform coordinates for this vertex
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
