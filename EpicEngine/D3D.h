#pragma once

/*Direct 3D 11 을 초기화 하는 클래스 , 렌더링을 담당하는 Graphic 클래스 안에서 Directx 부분을 맡음*/
/* TEXT
블렌딩(Blendig)은 글자가 3D 물체들을 배경으로 조화롭게 그려지도록 합니다.
만약 블렌딩을 켜지 않는다면 텍스트 뒤에 검은 삼각형이 있는 것을 보게 될 것입니다.
반대로 블렌딩이 켜져 있다면 오로지 글자와 관련된 픽셀들만 화면에 나타나고 나머지 부분의 삼각형은 투명 처리됩니다.
여기서는 그에 관해 더 자세히 다루지 않고 튜토리얼이 적당히 돌아가는 정도의 블렌딩을 사용할 것입니다.
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

	ID3D11DepthStencilState* m_depthDisabledStencilState; //깊이 스텐실 상태 변수
	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaDisableBlendingState;

};