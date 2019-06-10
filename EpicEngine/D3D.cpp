#include"stdafx.h"
#include "D3D.h"

D3D::D3D()
{
	m_swapChain = nullptr;
	m_depthStencilBuffer = nullptr;
	m_depthStencilState = nullptr;
	m_depthStencilView = nullptr;
	m_device = nullptr;
	m_deviceContext = nullptr;
	m_rasterState = nullptr;
	m_renderTargetView = nullptr;
}

D3D::D3D(const D3D& other) : D3D()
{
}

D3D::~D3D()
{
}

bool D3D::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	//수직동기화 상태를 저장합니다.
	m_vsync_enabled = vsync;

	//DirectX 그래픽 인터페이스 팩토리를 생성합니다.
	IDXGIFactory* factory = nullptr;
	if (FAILED(CreateDXGIFactory(_uuidof(IDXGIFactory), (void**)& factory)))
		return false;

	//팩토리 객체를 사용해 첫번째 그래픽 카드 인터페이스 어댑터를 생성합니다.
	IDXGIAdapter* adapter = nullptr;
	if (FAILED(factory->EnumAdapters(0, &adapter)))
		return false;

	//출력(모니터)에 대한 첫번째 어댑터를 지정합니다.
	IDXGIOutput* adapterOutput = nullptr;
	if (FAILED(adapter->EnumOutputs(0, &adapterOutput)))
		return false;

	//출력 (모니터)에 대한 DXGI_FORMAT_R8G8B8A8_UNORM 표시 형식에 맞는 모드 수를 가져옵니다.
	unsigned int numModes = 0;
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
		&numModes, NULL)))
		return false;

	// 가능한 모든 모니터와 그래픽카드 조합을 저장할 리스트를 생성합니다.
	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
		return false;

	//이제 디스플레이 모드에 대한 리스트를 채웁니다.
	if(FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
		&numModes, displayModeList)))
		return false;



	//이제 모든 디스플레이 모드에 대해 화면 너비/높이 맞는 디스플레이 모드를 찾습니다.
	//적합한 것을 찾으면 모니터의 새로고침 비율의 분모와 분자 값을 저장합니다.
	unsigned int numerator = 0;
	unsigned int denominator = 0;
	for (unsigned int i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//비디오카드의 구조체를 얻습니다.
	DXGI_ADAPTER_DESC adapterDesc;
	if (FAILED(adapter->GetDesc(&adapterDesc)))
		return false;	

	//비디오카드 메모리 용량 단위를 메가바이트 단위로 저장합니다.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//비디오카드의 이름을 저장합니다.
	size_t stringLength = 0;
	if (wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128) != 0)
		return false;

	//디스플레이 모드 리스트를 해제합니다.
	delete[] displayModeList;
	displayModeList = 0;

	//출력 어댑터를 해제합니다.
	adapterOutput->Release();
	adapterOutput = 0;

	//어댑터를 해제합니다.
	adapter->Release();
	adapter = 0;

	//팩토리 객체를 해제합니다.
	factory->Release();
	factory = 0;

	//스왑체인 구조체를 초기화합니다.
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//백버퍼를 1개만 사용하도록 지정합니다.
	swapChainDesc.BufferCount = 1;

	//백버퍼의 넓이와 높이를 지정합니다.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	//32bit 서페이스를 설정합니다.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//백버퍼의 새로고침 비율을 설정합니다.
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	//백버퍼의 사용용도를 지정합니다.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//렌더링에 사용될 윈도우 핸들을 지정합니다.
	swapChainDesc.OutputWindow = hwnd;

	//멀티샘플링을 끕니다.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	//창모드 or 풀스크린 모드를 지정합니다.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	//스캔 라인 순서 및 크기를 지정하지 않음으로 설정합니다.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//출력된 다음 백버퍼를 비우도록 지정합니다.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//추가 옵션 플래그를 사용하지 않습니다.
	swapChainDesc.Flags = 0;

	//피처러벨을 DirectX 11로 설정
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	//스왑 체인, Direct3D 장치 및 Direct3D 장치 컨텍스트를 만듭니다.
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, 
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext)))
		return false;

	//D3D_DRIVER_TYPE_HARDWARE == D3DDEVTYPE_HAL

	//스왑체인을 만들어도	스왑체인의 백버퍼는 Direct3D의 렌더타겟으로 설정되어있지 않다.
	//그래서 스왑체인에서 백버퍼를 취하여 디바이스의 렌더타겟으로 설정해야 한다.

	//GetBuffer로 백버퍼 포인터를 얻어올 수 있다.
	ID3D11Texture2D* backBufferPtr = nullptr; //2D 텍스쳐로써 백버퍼를 얻어오고 있다.
	if(FAILED(m_swapChain->GetBuffer(0, _uuidof(ID3D11Texture2D),(LPVOID*)&backBufferPtr)))
		return false;

	//텍스쳐는 파이프라인으로부터 뷰를 통하여 액세스하며 렌더타겟에는 렌더타겟 뷰를 사용한다.
	//백버퍼 포인터로 렌더 타겟 뷰를 생성한다.
	if (FAILED(m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView)))
		return false;

	//백버퍼 포인터를 해제합니다.
	backBufferPtr->Release();
	backBufferPtr = nullptr;

	//깊이 버퍼 구조체를 초기화합니다
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//깊이 버퍼 구조체를 작성합니다.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	//설정된 깊이버퍼 구조체를 사용하며 깊이 버퍼 텍스쳐를 생성합니다.
	if (FAILED(m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer)))
		return false;

	//스텐실 상태 구조체를 초기화합니다.
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	//스텐실 상태 구조체를 작성합니다.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//픽셀 뒷면의 스텐실 설정입니다.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//깊이 스탠실 상태를 생성합니다.
	if (FAILED(m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState)))
		return false;

	//깊이 스텐실 상태를 설정합니다.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	//깊이 스텐실 뷰의 구조체를 초기화합니다.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//깊이 스텐실 구조체를 설정합니다.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//깊이 스텐실 뷰를 생성합니다.
	if(FAILED(m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc,&m_depthStencilView)))
		return false;

	//렌더링 대상 뷰와 깊이 스텐실 버퍼를 출력 렌더 파이프 라인에 바인딩합니다.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	//그려지는 폴리곤과 방법을 결정할 래스터 구조체를 설정합니다.
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	//방금 작성한 구조체에서 래스터 라이저 상태를 만듭니다.
	if (FAILED(m_device->CreateRasterizerState(&rasterDesc, &m_rasterState)))
		return false;

	//이제 래스터 라이저 상태를 설정합니다.
	m_deviceContext->RSSetState(m_rasterState);

	//렌더링을 위해 뷰포트를 설정합니다.
	D3D11_VIEWPORT viewport;
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//뷰포트를 생성합니다.
	m_deviceContext->RSSetViewports(1, &viewport);

	//투영 행렬을 설정합니다
	float fieldOfView = 3.141592654f / 4.0f;
	float screenAspect = (float)screenWidth / (float)screenHeight;

	//3D 렌더링을 위한 투영 행렬을 만듭니다.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

	//세계 행렬을 항등 행렬로 초기화합니다.
	D3DXMatrixIdentity(&m_worldMatrix);

	//2D 렌더링을 위한 직교 투영 행렬을 만듭니다.
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}

void D3D::Shutdown()
{
	if (m_swapChain)
		m_swapChain->SetFullscreenState(false, NULL);

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = nullptr;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = nullptr;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = nullptr;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = nullptr;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = nullptr;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = nullptr;
	}

}

void D3D::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4] = { red, green, blue, alpha };

	//백버퍼를 지웁니다
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	//깊이 버퍼를 지웁니다
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3D::EndScene()
{
	//렌더링이 완료되었으므로 화면에 백버퍼를 표시합니다.
	if (m_vsync_enabled)
	{
		//화면  새로고침 비율을 고정합니다.
		m_swapChain->Present(1, 0);
	}
	else
	{
		//가능한 빠르게 출력합니다.
		m_swapChain->Present(0, 0);
	}
}

ID3D11Device* D3D::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext* D3D::GetDeviceContext()
{
	return m_deviceContext;
}

void D3D::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	 projectionMatrix = m_projectionMatrix;
}

void D3D::GetWorldMatrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
}

void D3D::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}

void D3D::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
}