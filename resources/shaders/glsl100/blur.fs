#version 100

precision mediump float;

// Input vertex attributes (from vertex shader)
varying vec2 fragTexCoord;
varying vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// NOTE: Add here your custom variables
const int nsamples = 20;

uniform float blurWidth;

void main()
{
    vec2 center = vec2(0.5);

	vec2 uv = fragTexCoord - center;
    float precompute = blurWidth / float(nsamples - 1);

    vec4 color = vec4(0.0);
    for(int i = 0; i < nsamples; i++)
    {
        float scale = 1.0 + (float(i)* precompute);
        color += texture2D(texture0, uv * scale + center);
    }

    color /= float(nsamples);

    gl_FragColor = color*colDiffuse;
}