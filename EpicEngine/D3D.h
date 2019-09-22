#pragma once

/*Direct 3D 11 �� �ʱ�ȭ �ϴ� Ŭ���� , �������� ����ϴ� Graphic Ŭ���� �ȿ��� Directx �κ��� ����*/
/* TEXT
����(Blendig)�� ���ڰ� 3D ��ü���� ������� ��ȭ�Ӱ� �׷������� �մϴ�.
���� ������ ���� �ʴ´ٸ� �ؽ�Ʈ �ڿ� ���� �ﰢ���� �ִ� ���� ���� �� ���Դϴ�.
�ݴ�� ������ ���� �ִٸ� ������ ���ڿ� ���õ� �ȼ��鸸 ȭ�鿡 ��Ÿ���� ������ �κ��� �ﰢ���� ���� ó���˴ϴ�.
���⼭�� �׿� ���� �� �ڼ��� �ٷ��� �ʰ� Ʃ�丮���� ������ ���ư��� ������ ������ ����� ���Դϴ�.
*/
class D3D
{
public:
	D3D();
	D3D(const D3D& other);
	~D3D();
	
	bool Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear);
	void Shutdown();

	void BeginScene(float red , float green, float blue, float alpha);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(D3DXMATRIX& projectionMatrix);
	void GetWorldMatrix(D3DXMATRIX& worldMatrix);
	void GetOrthoMatrix(D3DXMATRIX& orthoMatrix);

	void GetVideoCardInfo(char* cardName, int& memory);

	void TurnZBufferOn();
	void TurnZBufferOff();

	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();


private:
	bool m_vsync_enabled = false;
	int m_videoCardMemory = 0;
	char m_videoCardDescription[128] = { 0, };

private:
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11Texture2D* m_depthStencilBuffer = nullptr;
private:
	ID3D11DepthStencilState* m_depthStencilState = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	ID3D11RasterizerState* m_rasterState = nullptr;

private:
	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_worldMatrix;
	D3DXMATRIX m_orthoMatrix;

	ID3D11DepthStencilState* m_depthDisabledStencilState; //���� ���ٽ� ���� ����
	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaDisableBlendingState;

};