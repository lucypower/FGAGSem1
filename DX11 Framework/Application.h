#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include "Structures.h"
#include "OBJLoader.h"
#include "Camera.h"

using namespace DirectX;



struct ConstantBuffer // shader constant buffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;

	XMFLOAT4 diffuseMaterial;
	XMFLOAT4 diffuseLight;

	XMFLOAT4 ambientMaterial;
	XMFLOAT4 ambientLight;

	XMFLOAT4 specularMaterial;
	XMFLOAT4 specularLight;

	XMFLOAT3 EyePosW;	
	float specularPower;
	
	XMFLOAT3 LightVecW;
	float pad1;

};


class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;
	ID3D11Buffer*           _pVertexBuffer;
	ID3D11Buffer*			_pPyramidVertexBuffer;
	ID3D11Buffer*			_pGroundPlaneVertexBuffer;
	ID3D11Buffer*           _pIndexBuffer;
	ID3D11Buffer*			_pPyramidIndexBuffer;
	ID3D11Buffer*			_pGroundPlaneIndexBuffer;
	ID3D11Buffer*           _pConstantBuffer;
	XMFLOAT4X4              _sun, _planet1, _planet2, _moon1, _moon2, _pyramid1, _ground; // hold the transformations for the objects
	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;
	ID3D11BlendState*		Transparency;

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();
	ID3D11Device CreateRasterizerState(ID3D11Device renderState, ID3D11RasterizerState &_wireFrame);

	UINT _WindowHeight;
	UINT _WindowWidth;

	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D* _depthStencilBuffer;

	ID3D11RasterizerState* _wireFrame;
	ID3D11ShaderResourceView* _pTextureRV = nullptr;
	ID3D11ShaderResourceView* _pCubeTexture = nullptr;
	ID3D11SamplerState* _pSamplerLinear = nullptr;

	MeshData objMeshData;
	MeshData objMeshDataPlane;
	MeshData objGroundPlane;

	const int m = 20, n = 20;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();

};

