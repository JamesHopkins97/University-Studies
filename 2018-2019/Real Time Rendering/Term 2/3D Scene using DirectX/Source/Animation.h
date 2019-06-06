#pragma once
#include <d3d11_2.h>
#include <DirectXMath.h>


__declspec(align(16)) struct KeyFrame{
	DirectX::XMMATRIX mat;
	double rY;
	DirectX::XMVECTOR quart;
	double t;
};



class Animation
{


	//UINT type;// animation type
	double animStartT; // Game time when animation was started
	double animLengthT; //Total time for animation
	UINT startFrame;// first frame
	UINT endFrame;// last frame
	UINT currFrame; // current frame
	UINT nextFrame; // next frame

public:
	KeyFrame *	keyFrames;
	DirectX::XMMATRIX Animation::update(double seconds);
	Animation(int N, double time);
	~Animation();
};

