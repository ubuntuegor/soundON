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

const float threshold = 0.1;
const float unit = 3.0;
const int radius = 4;
const float dim = 15.0;

uniform float strength;

void main()
{
    vec4 sum = vec4(0);

    // Texel color fetching from texture sampler
    vec4 source = texture(texture0, fragTexCoord);

    const float sampleRange = unit / 800.0;

    for (int x = -radius; x <= radius; x++)
    {
        for (int y = -radius; y <= radius; y++)
        {
            vec4 clr = texture(texture0, fragTexCoord + vec2(x, y)*sampleRange);
            float gray = dot(clr.rgb, vec3(0.299, 0.587, 0.114));
            float factor = 1.0 / (1 + abs(x) + abs(y));
            if (gray > threshold) {
                sum += clr * factor;
            }
        }
    }

    // Calculate final fragment color
    finalColor = ((sum/dim*strength) + source)*colDiffuse;
}
