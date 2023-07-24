#version 100

precision mediump float;

// Input vertex attributes (from vertex shader)
varying vec2 fragTexCoord;
varying vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// NOTE: Add here your custom variables

const float threshold = 0.1;
const float unit = 5.0;
const int radius = 4;
const float dim = 28.0;

uniform float strength;

void main()
{
    vec4 sum = vec4(0);

    // Texel color fetching from texture sampler
    vec4 source = texture2D(texture0, fragTexCoord);

    const float sampleRange = unit / 800.0;

    for (int x = -radius; x <= radius; x++)
    {
        for (int y = -radius; y <= radius; y++)
        {
            vec4 clr = texture2D(texture0, fragTexCoord + vec2(x, y)*sampleRange);
            float gray = dot(clr.rgb, vec3(0.299, 0.587, 0.114));
            float factor = 1.0 / (1.0 + abs(float(x)) + abs(float(y)));
            if (gray > threshold) {
                sum += clr * factor;
            }
        }
    }

    // Calculate final fragment color
    gl_FragColor = ((sum/dim*strength) + source)*colDiffuse;
}
