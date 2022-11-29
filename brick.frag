//  Brick fragment shader
//  Derived from Orange Book Chapter 6 by Charles Gruenwald
#version 120

//  Hardwire brick dimensions and color
const vec3 BrickColor  = vec3(1.0,0.3,0.2);
const vec3 MortarColor = vec3(0.85,0.86,0.84);
const vec3 BrickSize   = vec3(0.30,0.15,0.15);
const vec3 BrickPct    = vec3(0.90,0.85,0.85);

//  Model coordinates and light from vertex shader
varying float LightIntensity;
varying vec3  ModelPos;
uniform float time;

void main()
{
   //  Divide by brick size for unit coordinates
   vec3 position = ModelPos / BrickSize;
   //  Stagger rows
   if (fract(0.5*position.y)>0.5) position.x += 0.5;
   //  Scroll bricks at 2.5 bricks per second
   position.x += 2.5*time;
   //  Don't care about the integer part
   position = fract(position);
   //  Snap to 0 or 1
   vec3 useBrick = step(position,BrickPct);
   //  Interpolate color (0 or 1 gives sharp transition)
   vec3 color  = mix(MortarColor, BrickColor, useBrick.x*useBrick.y*useBrick.z);
   //  Adjust color intensity for lighting (interpolated from vertex shader values)
   color *= LightIntensity;
   //  Pad color with alpha
   gl_FragColor = vec4(color,1.0);
}
