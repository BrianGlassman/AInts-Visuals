//  Per Pixel Lighting shader with texture
#version 120

varying vec3 View;
varying vec3 Light;
varying vec3 Normal;
uniform sampler2D tex0;
uniform sampler2D tex1;

void main()
{
   //  N is the object normal
   vec3 N = normalize(Normal);
   //  L is the light vector
   vec3 L = normalize(Light);
   //  R is the reflected light vector R = 2(L.N)N - L
   vec3 R = reflect(-L,N);
   //  V is the view vector (eye vector)
   vec3 V = normalize(View);

   //  Diffuse light is cosine of light and normal vectors
   float Id = max(dot(L,N) , 0.0);
   //  Specular is cosine of reflected and view vectors
   float Is = (Id>0.0) ? pow(max(dot(R,V),0.0) , gl_FrontMaterial.shininess) : 0.0;

   //  Sum color types
   vec4 color = gl_FrontMaterial.emission
              + gl_FrontLightProduct[0].ambient
              + Id*gl_FrontLightProduct[1].diffuse
              + Is*gl_FrontLightProduct[1].specular;

   // Apply the vertex coloring, if any (ref https://stackoverflow.com/a/2554526)
   color *= gl_Color;

   // Blend two textures (ref https://distrustsimplicity.net/articles/texture-blending-in-glsl/)
   vec4 t0 = texture2D(tex0, gl_TexCoord[0].xy);
   vec4 t1 = texture2D(tex1, gl_TexCoord[0].xy);

   // Use a triangle wave
   float p = 0.1;
   float a = 2*abs(gl_TexCoord[0].z / p - floor(gl_TexCoord[0].z / p + 0.5));

   //  Apply texture
   gl_FragColor = color * mix(t0, t1, a);
}
