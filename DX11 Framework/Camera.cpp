#include "Camera.h"
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>


Camera::Camera(XMFLOAT3 _eye, XMFLOAT3 _at, XMFLOAT3 _up, FLOAT _windowWidth, FLOAT _windowHeight, FLOAT _nearDepth, FLOAT _farDepth)
{
	Update();
}

Camera::~Camera()
{

}

void Camera::Update()
{
	// Initialize the view matrix (camera)
	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -8.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));

	// Initialize the projection matrix
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _WindowWidth / (FLOAT)_WindowHeight, 0.01f, 100.0f));

}

void Camera::Reshape(FLOAT _windowWidth, FLOAT _windowHeight, FLOAT _nearDepth, FLOAT _farDepth)
{
	/*XMMATRIX XMMatrixLookAtLH(
		[in] XMVECTOR EyePosition,
		[in] XMVECTOR FocusPosition,
		[in] XMVECTOR UpDirection
	);

	XMMATRIX XMMatrixLookToLH(
		[in] XMVECTOR EyePosition,
		[in] XMVECTOR FocusPosition,
		[in] XMVECTOR UpDirection
	);*/
}