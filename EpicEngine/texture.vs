cbuffer Matrixbuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

/*
이제 더이상 정점에 색상을 사용하지 않고 텍스쳐 좌표를 사용하게 될 것입니다. 
텍스쳐에서는 U와 V좌표만을 사용하기 때문에 이를 표현하기 위해 float2 자료형을 이용합니다.
정점 셰이더와 픽셀 셰이더에서 텍스쳐 좌표를 나타내기 위해 TEXCOORD0이라는 것을 사용합니다.
여러 개의 텍스쳐 좌표가 가능하다면 그 값을 0부터 아무 숫자로나 지정할 수도 있습니다.
*/

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

//Vertex Shader
PixelInputType TextureVertexShader(VertexInputType input)
{
	PixelInputType output;

	//적절한 행렬 계산을 위해서 4개의 유닛(4단위)으로 위치벡터를 변경합니다.
	input.position.w = 1.0f;

	//정점을 월드 , 뷰 , 투시 변환합니다.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//앞선 튜토리얼의 컬러 정점 셰이더와 텍스쳐 정점 셰이더의 유일한 차이점은
	//색상을 그대로 전달하는 것이 아니라 텍스쳐의 좌표들을 복사해서 픽셀 셰이더로 전달한다는 것입니다.
	output.tex = input.tex;

	return output;
}
