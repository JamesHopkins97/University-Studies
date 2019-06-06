
// Basic colour pixel shader


// input fragment - this is the per-fragment packet interpolated by the rasteriser stage
struct fragmentInputPacket {

	float4				colour		: COLOR;
	float4				posH			: SV_POSITION;
};


struct fragmentOutputPacket {

	float4				fragmentColour : SV_TARGET;
};



fragmentOutputPacket main(fragmentInputPacket inputFragment) {

	fragmentOutputPacket outputFragment;

	outputFragment.fragmentColour = inputFragment.colour;

	return outputFragment;
}
