#version 330


// input packet
in packet {

	vec4 colour;
	vec2 textureCoord;

} inputFragment;


// output packet
layout (location = 0) out vec4 fragmentColour;


void main(void) {

	fragmentColour = inputFragment.colour;
}