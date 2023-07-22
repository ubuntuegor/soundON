#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

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
        color += texture(texture0, uv * scale + center);
    }

    color /= float(nsamples);

    finalColor = color*colDiffuse;
}