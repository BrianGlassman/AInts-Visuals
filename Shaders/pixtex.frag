//  Per Pixel Lighting shader with texture
#version 120

varying vec3 View;
varying vec3 Light;
varying vec3 Normal;

uniform float p; // Wave period
uniform float amp; // Wave amplitude

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;
uniform sampler2D tex5;
uniform sampler2D tex6;
uniform sampler2D tex7;
uniform sampler2D tex8;
uniform sampler2D tex9;
uniform sampler2D tex10;
uniform sampler2D tex11;
uniform sampler2D tex12;
uniform sampler2D tex13;
uniform sampler2D tex14;
uniform sampler2D tex15;
uniform sampler2D tex16;

float fade(float offset)
{
   // Fade using an offset triangle wave, clamped to [0, 1]
   float a = 2*amp * abs(gl_TexCoord[0].z / p - floor(gl_TexCoord[0].z / p + 0.5)) - offset;
   return min(1, max(0, a));
}

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

   vec4 t0 = texture2D(tex0, gl_TexCoord[0].xy);
   vec4 t1 = texture2D(tex1, gl_TexCoord[0].xy);
   vec4 t2 = texture2D(tex2, gl_TexCoord[0].xy);
   vec4 t3 = texture2D(tex3, gl_TexCoord[0].xy);
   vec4 t4 = texture2D(tex4, gl_TexCoord[0].xy);
   vec4 t5 = texture2D(tex5, gl_TexCoord[0].xy);
   vec4 t6 = texture2D(tex6, gl_TexCoord[0].xy);
   vec4 t7 = texture2D(tex7, gl_TexCoord[0].xy);
   vec4 t8 = texture2D(tex8, gl_TexCoord[0].xy);
   vec4 t9 = texture2D(tex9, gl_TexCoord[0].xy);
   vec4 t10 = texture2D(tex10, gl_TexCoord[0].xy);
   vec4 t11 = texture2D(tex11, gl_TexCoord[0].xy);
   vec4 t12 = texture2D(tex12, gl_TexCoord[0].xy);
   vec4 t13 = texture2D(tex13, gl_TexCoord[0].xy);
   vec4 t14 = texture2D(tex14, gl_TexCoord[0].xy);
   vec4 t15 = texture2D(tex15, gl_TexCoord[0].xy);
   vec4 t16 = texture2D(tex16, gl_TexCoord[0].xy);

   // Start at the deepest and work out
   vec4 temp = t16;
   temp = mix(t15, temp, fade(15));
   temp = mix(t14, temp, fade(14));
   temp = mix(t13, temp, fade(13));
   temp = mix(t12, temp, fade(12));
   temp = mix(t11, temp, fade(11));
   temp = mix(t10, temp, fade(10));
   temp = mix(t9, temp, fade(9));
   temp = mix(t8, temp, fade(8));
   temp = mix(t7, temp, fade(7));
   temp = mix(t6, temp, fade(6));
   temp = mix(t5, temp, fade(5));
   temp = mix(t4, temp, fade(4));
   temp = mix(t3, temp, fade(3));
   temp = mix(t2, temp, fade(2));
   temp = mix(t1, temp, fade(1));
   temp = mix(t0, temp, fade(0));

   // Blend the textures and apply (ref https://distrustsimplicity.net/articles/texture-blending-in-glsl/)
   gl_FragColor = color * temp;
}
