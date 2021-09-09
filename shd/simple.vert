#version 120
attribute vec3 Position;
attribute vec3 Color;

varying vec4 Frag_Color;

uniform mat4 ProjMtx;
uniform float Alpha;

void main()
{
    
    Frag_Color = vec4(Color, Alpha);
    gl_Position = ProjMtx * vec4(Position, 1);
};
