#include "stdafx.h"
#include "Animation.h"
#include <iostream>

using namespace DirectX;
using namespace std;

Animation::Animation(int N, double time)
{
	keyFrames = (KeyFrame*)_aligned_malloc(sizeof(KeyFrame)*N, 16);
	
	keyFrames[0].t = 0.0;
	keyFrames[0].rY = 0;
	keyFrames[0].quart = XMQuaternionRotationRollPitchYaw(0, XMConvertToRadians(0), 0);

	keyFrames[1].t = 1.0;
	keyFrames[1].rY = 90;
	keyFrames[1].quart = XMQuaternionRotationRollPitchYaw(0, XMConvertToRadians(90), 0);

	keyFrames[2].t = 2.0;
	keyFrames[2].rY = 180;
	keyFrames[2].quart = XMQuaternionRotationRollPitchYaw(0, XMConvertToRadians(180), 0);

	keyFrames[3].t = 3.0;
	keyFrames[3].rY = 270;
	keyFrames[3].quart = XMQuaternionRotationRollPitchYaw(0, XMConvertToRadians(270), 0);

	keyFrames[4].t = 4.0;
	keyFrames[4].rY = 360;
	keyFrames[4].quart = XMQuaternionRotationRollPitchYaw(0, XMConvertToRadians(360), 0);


	startFrame = 0;// first frame
	cout << N - 1 << endl;
	endFrame = 4;// N - 1;// last frame
	currFrame = 0;//startFrame; // current frame
	nextFrame = 1;//currFrame++;// startFrame++; // next frame
	animStartT = 0.0; // Game time when animation was started
	animLengthT = keyFrames[endFrame].t - keyFrames[startFrame].t; //Total time for animation
}


DirectX::XMMATRIX Animation::update(double time )
{
	animLengthT = keyFrames[endFrame].t - keyFrames[startFrame].t; //Total time for animation

	double animTotalRunT = time-animStartT; // Time since animation started
	double animCurrentTime = fmod(animTotalRunT, animLengthT); //Time from the start of the current animation loop
	if (animCurrentTime < keyFrames[currFrame].t)// Animation has reset
	{
		currFrame = 0;
		nextFrame = 1;
	}
	while (animCurrentTime>keyFrames[nextFrame].t) // loop untill our next frame is greater than current loop time
	{
		currFrame = nextFrame;
		nextFrame++;
	}

	double frameCurrentT = animCurrentTime - keyFrames[currFrame].t; // Time since the start of the current frame
	double frameLengthT = keyFrames[nextFrame].t - keyFrames[currFrame].t; // Length of time between current frame and the next frame
	double w = frameCurrentT / frameLengthT; //Calculate the weighting(w)

	//Interpolate animation using the weighting w
	//double r = keyFrames[currFrame].rY*(1-w0) +  keyFrames[nextFrame].rY*(w); // Euller
	cout << animCurrentTime << "  " << currFrame << "  " << nextFrame << "  " << w << endl;
	XMVECTOR quart = XMQuaternionSlerp(keyFrames[currFrame].quart, keyFrames[nextFrame].quart, w); //Quarternion
	// return appropriate transformation
	//return XMMatrixRotationY(XMConvertToRadians(r)); // Euller
	return XMMatrixRotationQuaternion(quart); //Quarternion
}






Animation::~Animation()
{
	free(keyFrames);
}
