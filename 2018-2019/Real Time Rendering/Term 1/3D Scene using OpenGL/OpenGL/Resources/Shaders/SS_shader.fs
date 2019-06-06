#version 330

uniform sampler2D texture0;
in vec2 texCoord;

uniform int resolutionWidth;
uniform int resolutionHeight;
uniform int sample;


layout (location = 0) out vec4 fragColour;

void main(void) 
{
    vec4 newSample = vec4(0,0,0,0);
    vec4 bestCurrentSample;

    float tx = 1.0 / (resolutionWidth * sample);
    float ty = 1.0 / (resolutionHeight * sample);

    int size = sample / 2;

    int sampleAgain = ((sample+1) * (sample+1));

    for(int y = -size; y <= size; y++)
    {
        for(int x = -size; x <= size; x++)
        {
            vec2 position = (texCoord + vec2(x, y)) + vec2(tx * x, ty * y);
            bestCurrentSample = (texture(texture0, position) / sampleAgain);

            newSample += bestCurrentSample;
        }
    }
    fragColour = newSample;
}