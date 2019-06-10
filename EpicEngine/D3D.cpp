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
	//��������ȭ ���¸� �����մϴ�.
	m_vsync_enabled = vsync;

	//DirectX �׷��� �������̽� ���丮�� �����մϴ�.
	IDXGIFactory* factory = nullptr;
	if (FAILED(CreateDXGIFactory(_uuidof(IDXGIFactory), (void**)& factory)))
		return false;

	//���丮 ��ü�� ����� ù��° �׷��� ī�� �������̽� ����͸� �����մϴ�.
	IDXGIAdapter* adapter = nullptr;
	if (FAILED(factory->EnumAdapters(0, &adapter)))
		return false;

	//���(�����)�� ���� ù��° ����͸� �����մϴ�.
	IDXGIOutput* adapterOutput = nullptr;
	if (FAILED(adapter->EnumOutputs(0, &adapterOutput)))
		return false;

	//��� (�����)�� ���� DXGI_FORMAT_R8G8B8A8_UNORM ǥ�� ���Ŀ� �´� ��� ���� �����ɴϴ�.
	unsigned int numModes = 0;
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
		&numModes, NULL)))
		return false;

	// ������ ��� ����Ϳ� �׷���ī�� ������ ������ ����Ʈ�� �����մϴ�.
	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
		return false;

	//���� ���÷��� ��忡 ���� ����Ʈ�� ä��ϴ�.
	if(FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
		&numModes, displayModeList)))
		return false;



	//���� ��� ���÷��� ��忡 ���� ȭ�� �ʺ�/���� �´� ���÷��� ��带 ã���ϴ�.
	//������ ���� ã���� ������� ���ΰ�ħ ������ �и�� ���� ���� �����մϴ�.
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

	//����ī���� ����ü�� ����ϴ�.
	DXGI_ADAPTER_DESC adapterDesc;
	if (FAILED(adapter->GetDesc(&adapterDesc)))
		return false;	

	//����ī�� �޸� �뷮 ������ �ް�����Ʈ ������ �����մϴ�.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//����ī���� �̸��� �����մϴ�.
	size_t stringLength = 0;
	if (wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128) != 0)
		return false;

	//���÷��� ��� ����Ʈ�� �����մϴ�.
	delete[] displayModeList;
	displayModeList = 0;

	//��� ����͸� �����մϴ�.
	adapterOutput->Release();
	adapterOutput = 0;

	//����͸� �����մϴ�.
	adapter->Release();
	adapter = 0;

	//���丮 ��ü�� �����մϴ�.
	factory->Release();
	factory = 0;

	//����ü�� ����ü�� �ʱ�ȭ�մϴ�.
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//����۸� 1���� ����ϵ��� �����մϴ�.
	swapChainDesc.BufferCount = 1;

	//������� ���̿� ���̸� �����մϴ�.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	//32bit �����̽��� �����մϴ�.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//������� ���ΰ�ħ ������ �����մϴ�.
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	//������� ���뵵�� �����մϴ�.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//�������� ���� ������ �ڵ��� �����մϴ�.
	swapChainDesc.OutputWindow = hwnd;

	//��Ƽ���ø��� ���ϴ�.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	//â��� or Ǯ��ũ�� ��带 �����մϴ�.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	//��ĵ ���� ���� �� ũ�⸦ �������� �������� �����մϴ�.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//��µ� ���� ����۸� ��쵵�� �����մϴ�.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//�߰� �ɼ� �÷��׸� ������� �ʽ��ϴ�.
	swapChainDesc.Flags = 0;

	//��ó������ DirectX 11�� ����
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	//���� ü��, Direct3D ��ġ �� Direct3D ��ġ ���ؽ�Ʈ�� ����ϴ�.
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, 
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext)))
		return false;

	//D3D_DRIVER_TYPE_HARDWARE == D3DDEVTYPE_HAL

	//����ü���� ����	����ü���� ����۴� Direct3D�� ����Ÿ������ �����Ǿ����� �ʴ�.
	//�׷��� ����ü�ο��� ����۸� ���Ͽ� ����̽��� ����Ÿ������ �����ؾ� �Ѵ�.

	//GetBuffer�� ����� �����͸� ���� �� �ִ�.
	ID3D11Texture2D* backBufferPtr = nullptr; //2D �ؽ��ķν� ����۸� ������ �ִ�.
	if(FAILED(m_swapChain->GetBuffer(0, _uuidof(ID3D11Texture2D),(LPVOID*)&backBufferPtr)))
		return false;

	//�ؽ��Ĵ� �������������κ��� �並 ���Ͽ� �׼����ϸ� ����Ÿ�ٿ��� ����Ÿ�� �並 ����Ѵ�.
	//����� �����ͷ� ���� Ÿ�� �並 �����Ѵ�.
	if (FAILED(m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView)))
		return false;

	//����� �����͸� �����մϴ�.
	backBufferPtr->Release();
	backBufferPtr = nullptr;

	//���� ���� ����ü�� �ʱ�ȭ�մϴ�
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//���� ���� ����ü�� �ۼ��մϴ�.
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

	//������ ���̹��� ����ü�� ����ϸ� ���� ���� �ؽ��ĸ� �����մϴ�.
	if (FAILED(m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer)))
		return false;

	//���ٽ� ���� ����ü�� �ʱ�ȭ�մϴ�.
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	//���ٽ� ���� ����ü�� �ۼ��մϴ�.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//�ȼ� �޸��� ���ٽ� �����Դϴ�.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//���� ���Ľ� ���¸� �����մϴ�.
	if (FAILED(m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState)))
		return false;

	//���� ���ٽ� ���¸� �����մϴ�.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	//���� ���ٽ� ���� ����ü�� �ʱ�ȭ�մϴ�.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//���� ���ٽ� ����ü�� �����մϴ�.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//���� ���ٽ� �並 �����մϴ�.
	if(FAILED(m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc,&m_depthStencilView)))
		return false;

	//������ ��� ��� ���� ���ٽ� ���۸� ��� ���� ������ ���ο� ���ε��մϴ�.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	//�׷����� ������� ����� ������ ������ ����ü�� �����մϴ�.
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

	//��� �ۼ��� ����ü���� ������ ������ ���¸� ����ϴ�.
	if (FAILED(m_device->CreateRasterizerState(&rasterDesc, &m_rasterState)))
		return false;

	//���� ������ ������ ���¸� �����մϴ�.
	m_deviceContext->RSSetState(m_rasterState);

	//�������� ���� ����Ʈ�� �����մϴ�.
	D3D11_VIEWPORT viewport;
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//����Ʈ�� �����մϴ�.
	m_deviceContext->RSSetViewports(1, &viewport);

	//���� ����� �����մϴ�
	float fieldOfView = 3.141592654f / 4.0f;
	float screenAspect = (float)screenWidth / (float)screenHeight;

	//3D �������� ���� ���� ����� ����ϴ�.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

	//���� ����� �׵� ��ķ� �ʱ�ȭ�մϴ�.
	D3DXMatrixIdentity(&m_worldMatrix);

	//2D �������� ���� ���� ���� ����� ����ϴ�.
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

	//����۸� ����ϴ�
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	//���� ���۸� ����ϴ�
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3D::EndScene()
{
	//�������� �Ϸ�Ǿ����Ƿ� ȭ�鿡 ����۸� ǥ���մϴ�.
	if (m_vsync_enabled)
	{
		//ȭ��  ���ΰ�ħ ������ �����մϴ�.
		m_swapChain->Present(1, 0);
	}
	else
	{
		//������ ������ ����մϴ�.
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