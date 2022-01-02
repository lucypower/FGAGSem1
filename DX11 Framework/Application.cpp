#include "Application.h"
#include "DDSTextureLoader.h"
#include "Camera.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) // recieving windows messages
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

Application::Application() // setup (create application object)
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pVertexBuffer = nullptr;
	_pIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
}

Application::~Application() // deconstructor
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow) // initializing various things
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

    objMeshData = OBJLoader::Load("donut.obj", _pd3dDevice, false); // blender model // Load(".obj", _pd3dDevice) for 3dsmax model
    objMeshDataPlane = OBJLoader::Load("Hercules.obj", _pd3dDevice);
    objGroundPlane = OBJLoader::Load("cube.obj", _pd3dDevice);

	// Initialize the world matrix
	XMStoreFloat4x4(&_sun, XMMatrixIdentity());


    // Initialize the view matrix (camera)
    XMVECTOR Eye = XMVectorSet(2.0f, 3.0f, 0.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));

    // Initialize the projection matrix
    XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _WindowWidth / (FLOAT)_WindowHeight, 0.01f, 100.0f));


    // create the sample state
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD - D3D11_FLOAT32_MAX;

    _pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);


	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob); // compiling file into shader so dx11 can use it

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}

HRESULT Application::InitVertexBuffer() // 
{
	HRESULT hr;

    // NEED TO DO TEXCOORDS
     
    // Create vertex buffer // create the square
    SimpleVertex vertices[] = // doesn't matter what order these are, vertex x, starts at 0
    {
        { XMFLOAT3( -1.0f, 1.0f, 0.0f ), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)}, // vertex 0
        { XMFLOAT3( 1.0f, 1.0f, 0.0f ), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) }, // vertex 1
        { XMFLOAT3( 1.0f, -1.0f, 0.0f ), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) }, // etc
        { XMFLOAT3( -1.0f, -1.0f, 0.0f ), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
    };

    //how much memory is needed to store those vertices (bytewidth)
    D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 8;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;

    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBuffer);

    // ground plane vertex buffer

    //SimpleVertex groundPlaneVertices[] =
    //{
    //    { XMFLOAT3(0.0f, 1.0f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f ) }, // vertex 0
    //    { XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f ) },
    //    { XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 0.0f ) },
    //    { XMFLOAT3(-1.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f )},
    //    { XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f )},
    //};

    //how much memory is needed to store those vertices (bytewidth)
    D3D11_BUFFER_DESC pd;
    ZeroMemory(&pd, sizeof(pd));
    pd.Usage = D3D11_USAGE_DEFAULT;
    pd.ByteWidth = sizeof(SimpleVertex) * 5;
    pd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    pd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA PyramidInitData;
    ZeroMemory(&PyramidInitData, sizeof(PyramidInitData));
    //PyramidInitData.pSysMem = pyramidVertices;

    //hr = _pd3dDevice->CreateBuffer(&pd, &PyramidInitData, &_pPyramidVertexBuffer);

    // ground plane
    const int m = 20, n = 20, w = 500, d = 500; // m = rows, n = columns, w = width, d = depth
    float dx = w / (n - 1); // cell spacing along x axis
    float dz = d / (m - 1); // cell spacing lalong z axis

    SimpleVertex groundPlaneVertices [(m*n)] = {};

        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < n; j++)
            {
                groundPlaneVertices[(i * m) + j] = { XMFLOAT3(((-0.5 * (float)w) + ((float)j * dx)),
                                                         0.0f,
                                                        ((0.5 * (float)d) - ((float)i * dz))),
                                               XMFLOAT3(1.0f, 1.0f, 1.0f),
                                               XMFLOAT2(1.0f, .0f) };

            }
        }
    

    //how much memory is needed to store those vertices (bytewidth)
    D3D11_BUFFER_DESC gd;
    ZeroMemory(&gd, sizeof(gd));
    gd.Usage = D3D11_USAGE_DEFAULT;
    gd.ByteWidth = sizeof(SimpleVertex) * m * n ;
    gd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    gd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA GroundPlaneInitData;
    ZeroMemory(&GroundPlaneInitData, sizeof(GroundPlaneInitData));
    GroundPlaneInitData.pSysMem = vertices;

    hr = _pd3dDevice->CreateBuffer(&bd, &GroundPlaneInitData, &_pGroundPlaneVertexBuffer);



    if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

    // Create index buffer
    WORD indices[] =
    {
        // front face
        0, 1, 2,
        0, 2, 3,
        // side
        1, 5, 6,
        1, 6, 2,
        // back
        5, 4, 7,
        5, 7, 6,
        // side
        4, 0, 3,
        4, 3, 7,
        // top
        4, 5, 1,
        4, 1, 0,
        // bottom
        3, 2, 6,
        3, 6, 7,
    };

    //how much memory is needed to store those indices (bytewidth)
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;     
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indices;
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBuffer);

    // pyramid index buffer

    WORD pyramidIndices[] =
    {
        0, 2, 1,
        0, 4, 2,
        0, 3, 4,
        0, 1, 3,
        3, 4, 1,
        2, 1, 4,
    };

    //how much memory is needed to store those indices (bytewidth)
    D3D11_BUFFER_DESC pd;
    ZeroMemory(&pd, sizeof(pd));

    pd.Usage = D3D11_USAGE_DEFAULT;
    pd.ByteWidth = sizeof(WORD) * 18;
    pd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    pd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA PyramidInitData;
    ZeroMemory(&PyramidInitData, sizeof(PyramidInitData));
    PyramidInitData.pSysMem = pyramidIndices;
    hr = _pd3dDevice->CreateBuffer(&pd, &PyramidInitData, &_pPyramidIndexBuffer);

    // ground plane

    // Create index buffer
    const int m = 20, n = 20;

    WORD groundPlaneIndices[(((m - 1) * (n - 1)) * 6)] = {}; // x 2 for triangles, x 3 for each point on triangle

    for (int i = 0; i < m-1; i++)
    {
        for (int j = 0; j < n-1; j++)
        {
            groundPlaneIndices[((i * (m - 1) + j) * 6) + 0] = (i + (j * m));
            groundPlaneIndices[((i * (m - 1) + j) * 6) + 1] = ((i + 1) + (j * m));
            groundPlaneIndices[((i * (m - 1) + j) * 6) + 2] = (i + ((j + 1) * m));
            groundPlaneIndices[((i * (m - 1) + j) * 6) + 3] = (i + ((j + 1) * m));
            groundPlaneIndices[((i * (m - 1) + j) * 6) + 4] = ((i + 1) + (j * m));
            groundPlaneIndices[((i * (m - 1) + j) * 6) + 5] = ((i + 1) + ((j + 1) * m));
                
                
                /*XMFLOAT3(((-0.5 * (float)w) + ((float)j * dx)),
                                                 0.0f,
                                                ((0.5 * (float)d) + ((float)i * dz))),
                                       XMFLOAT3(0.0f, 0.0f, 0.0f),
                                       XMFLOAT2(0.0f, 0.0f)*/
        }
    }

    //how much memory is needed to store those indices (bytewidth)
    D3D11_BUFFER_DESC gd;
    ZeroMemory(&gd, sizeof(gd));

    gd.Usage = D3D11_USAGE_DEFAULT;
    gd.ByteWidth = sizeof(WORD) * ((m - 1) * (n - 1)) * 6;
    gd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    gd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA groundPlaneInitData;
    ZeroMemory(&groundPlaneInitData, sizeof(groundPlaneInitData));
    groundPlaneInitData.pSysMem = indices;
    hr = _pd3dDevice->CreateBuffer(&gd, &groundPlaneInitData, &_pGroundPlaneIndexBuffer);



    if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow) // initialising of actual window
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 640, 480};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut) 
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice() // initialising dx11 device
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    D3D11_TEXTURE2D_DESC depthStencilDesc;
    depthStencilDesc.Width = _WindowWidth;
    depthStencilDesc.Height = _WindowHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    _pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
    _pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

    _pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

	InitVertexBuffer();

    // Set vertex buffer
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    _pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBuffer, &stride, &offset);

	InitIndexBuffer();

    // Set index buffer
    _pImmediateContext->IASetIndexBuffer(_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set all primitives to triangles

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

    D3D11_RASTERIZER_DESC wfdesc;
    ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
    wfdesc.FillMode = D3D11_FILL_WIREFRAME;
    wfdesc.CullMode = D3D11_CULL_NONE;
    hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);

    if (FAILED(hr))
        return hr;

    hr = CreateDDSTextureFromFile(_pd3dDevice, L"Hercules_COLOR.dds", nullptr, &_pTextureRV);
    hr = CreateDDSTextureFromFile(_pd3dDevice, L"crateTexture.dds", nullptr, &_pCubeTexture);

    // blending

    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));

    D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc;
    ZeroMemory(&renderTargetBlendDesc, sizeof(renderTargetBlendDesc));

    renderTargetBlendDesc.BlendEnable = true;
    renderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_COLOR;
    renderTargetBlendDesc.DestBlend = D3D11_BLEND_BLEND_FACTOR;
    renderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
    renderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
    renderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
    renderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    renderTargetBlendDesc.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.RenderTarget[0] = renderTargetBlendDesc;

    _pd3dDevice->CreateBlendState(&blendDesc, &Transparency);


    if (FAILED(hr))
        return hr;

    return S_OK;
}

/*void Application::CreateRasterizerState(ID3D11Device renderState, ID3D11RasterizerState& _wireFrame)
{

}*/

void Application::Cleanup() // called when application is destroyed // check to release every pointer to avoid memory leaks
{
    if (_pImmediateContext) _pImmediateContext->ClearState();

    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pVertexBuffer) _pVertexBuffer->Release();
    if (_pIndexBuffer) _pIndexBuffer->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
    if (_depthStencilView) _depthStencilView->Release();
    if (_depthStencilBuffer) _depthStencilBuffer->Release();
    if (_wireFrame) _wireFrame->Release();
    if (Transparency) Transparency->Release();
}

void Application::Update() // currently just rotating the square (8th oct) // concearned with moving the object
{
    // Update our time
    static float t = 0.0f;

    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float) XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();

        if (dwTimeStart == 0)
            dwTimeStart = dwTimeCur;

        t = (dwTimeCur - dwTimeStart) / 1000.0f;
    }

    //
    // Animate the cube
    //
    XMStoreFloat4x4(&_sun, XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(t) * XMMatrixTranslation(0.0f, 0.0f, 0.0f));
    XMStoreFloat4x4(&_planet1, XMMatrixRotationZ(t) * XMMatrixTranslation(5.0f, 0.0f, -5.0f) /** XMMatrixScaling(0.5f, 0.5f, 0.5f)*/ * XMMatrixRotationX(t));
    XMStoreFloat4x4(&_planet2,  XMMatrixRotationX(t) * XMMatrixTranslation(-10.0f, 2.0f, 0.0f) * XMMatrixScaling(0.45f, 0.45f, 0.45f) * XMMatrixRotationY(-t));
    XMStoreFloat4x4(&_moon1, XMMatrixTranslation(0.0f, -20.0f, 0.0f) * XMMatrixScaling(0.0f, 0.1f, 0.0f));
    XMStoreFloat4x4(&_moon2, XMMatrixTranslation(-7.0f, 0.0f, 0.0f) * XMMatrixScaling(0.75f, 0.75f, 0.75) * XMMatrixRotationY(t) * XMLoadFloat4x4(&_moon2));
    XMStoreFloat4x4(&_pyramid1, XMMatrixRotationZ(t) * XMMatrixRotationX(t) * XMMatrixTranslation(3.0f, 1.0f, 0.0f));
    XMStoreFloat4x4(&_ground, XMMatrixRotationX(-90) * XMMatrixTranslation(0.0f, 0.0f, 0.0f));
}

void Application::Draw() // concearned with drawing the object
{
    //
    // Clear the back buffer
    //
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
    _pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	XMMATRIX sun = XMLoadFloat4x4(&_sun);
	XMMATRIX view = XMLoadFloat4x4(&_view);
	XMMATRIX projection = XMLoadFloat4x4(&_projection);
    
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    //
    // Update variables
    //
    ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(sun);
    
	cb.mView = XMMatrixTranspose(view);
	cb.mProjection = XMMatrixTranspose(projection);

    cb.LightVecW = XMFLOAT3(0.25f, 0.5f, -1.0f);
    
    cb.diffuseMaterial = XMFLOAT4(0.4f, 0.7f, 0.5f, 1.0f);
    cb.diffuseLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    
    cb.ambientMaterial = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
    cb.ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);

    cb.specularMaterial = XMFLOAT4(0.8f, 0.8f, 0.8f, 0.8f);
    cb.specularLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
    cb.specularPower = float(0.5f);

    cb.EyePosW = XMFLOAT3(0.0f, 0.0f, -3.0f);


    _pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    _pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

     /*blending state
     "fine tune" the blending equation*/
    //float blendFactor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
     //set the default blend state (no blending) for opaque objects)
    //_pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);
    //render opaque objects (think need code here)
     //set the blend state for transparent objects
    //_pImmediateContext->OMSetBlendState(Transparency, blendFactor, 0xffffffff);

    //
    // Renders a triangle
    //
	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0); // donut
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
    _pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);

    // plan e
    /*_pImmediateContext->IASetVertexBuffers(0, 1, &objMeshDataPlane.VertexBuffer, &stride, &offset);
    _pImmediateContext->IASetIndexBuffer(objMeshDataPlane.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	_pImmediateContext->DrawIndexed(objMeshDataPlane.IndexCount, 0, 0); 
    _pImmediateContext->PSSetShaderResources(0, 1, &_pCubeTexture);*/

    // draws second cube 
    //XMMATRIX planet1 = XMLoadFloat4x4(&_planet1); // cube
    //cb.mWorld = XMMatrixTranspose(planet1);
    //_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    //_pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBuffer, &stride, &offset);
    //_pImmediateContext->IASetIndexBuffer(_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    //_pImmediateContext->DrawIndexed(36, 0, 0);

    //XMMATRIX planet2 = XMLoadFloat4x4(&_planet2); // donut
    //cb.mWorld = XMMatrixTranspose(planet2);
    //_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    //_pImmediateContext->IASetVertexBuffers(0, 1, &objMeshData.VertexBuffer, &stride, &offset);
    //_pImmediateContext->IASetIndexBuffer(objMeshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    //_pImmediateContext->DrawIndexed(objMeshData.IndexCount, 0, 0);

    //XMMATRIX moon1 = XMLoadFloat4x4(&_moon1); // ground plane
    //cb.mWorld = XMMatrixTranspose(moon1);
    //_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    //_pImmediateContext->IASetVertexBuffers(0, 1, &objGroundPlane.VertexBuffer, &stride, &offset);
    //_pImmediateContext->IASetIndexBuffer(objGroundPlane.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    //_pImmediateContext->DrawIndexed(objGroundPlane.IndexCount, 0, 0);

    //XMMATRIX moon2 = XMLoadFloat4x4(&_moon2);
    //cb.mWorld = XMMatrixTranspose(moon2);
    //_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    //_pImmediateContext->DrawIndexed(36, 0, 0);

    // pyramid
    //_pImmediateContext->IASetVertexBuffers(0, 1, &_pPyramidVertexBuffer, &stride, &offset);

    /*XMMATRIX pyramid1 = XMLoadFloat4x4(&_pyramid1);
    cb.mWorld = XMMatrixTranspose(pyramid1);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    _pImmediateContext->DrawIndexed(18, 0, 0);*/
    

    XMMATRIX ground = XMLoadFloat4x4(&_ground);
    cb.mWorld = XMMatrixTranspose(ground);
    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
    _pImmediateContext->IASetVertexBuffers(0, 1, &_pGroundPlaneVertexBuffer, &stride, &offset);
    _pImmediateContext->IASetIndexBuffer(_pGroundPlaneIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    _pImmediateContext->DrawIndexed((((m - 1) * (n - 1)) * 6), 0, 0);
    
    //
    // Present our back buffer to our front buffer
    //
    _pSwapChain->Present(0, 0);
}