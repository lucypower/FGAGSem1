#pragma once
#ifndef _CAMERA_H
#define _CAMERA_H
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
using namespace DirectX;

class Camera
{
private:

	XMFLOAT3 _eye;
	XMFLOAT3 _at;
	XMFLOAT3 _up;

	FLOAT _windowWidth;
	FLOAT _windowHeight;
	FLOAT _nearDepth;
	FLOAT _farDepth;

	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;

	UINT _WindowHeight;
	UINT _WindowWidth;

public:

	Camera(XMFLOAT3 _eye, XMFLOAT3 _at, XMFLOAT3 _up, FLOAT _windowWidth, FLOAT _windowHeight, FLOAT _nearDepth, FLOAT _farDepth);
	~Camera();

	void Update();
	void Reshape(FLOAT _windowWidth, FLOAT _windowHeight, FLOAT _nearDepth, FLOAT _farDepth);

};

#endif