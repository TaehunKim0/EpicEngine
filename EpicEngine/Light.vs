cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

/*
카메라의 정보를 저장할 새로운 상수 버퍼를 추가합니다.
이 셰이더에서는 정반사의 계산을 위해 이 정점이 어디서 보여지는지의 정보가 필요하므로 카메라의 위치를 알아야 합니다.
*/
cbuffer CameraBuffer
{
	float3 cameraPosition;
	float padding;
};
//여기까지 함

//Typedef
struct VertexInputType
{
	float4 position : POSITION;
	float4 tex : TEXCOORD0; //diffuse인듯 내적값은 벡터가 아닌 숫자 하나인데 용도에 맞는 시맨틱이 없으면 보통 TEXCOORD를 쓴다.	
	float4 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};

//VERTEX SHADER
PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output;

	// 적절한 행렬 계산을 위해 위치 벡터를 4 단위로 변경하십시오.
	input.position.w = 1.0f;

	//월드, 뷰 및 투영 행렬에 대한 정점의 위치를 계산한다.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//픽셀 셰이더의 텍스처 좌표를 저장하십시오.
	output.tex = input.tex;

	//월드 매트릭스에 대해서만 정규 벡터를 계산하십시오
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal); //법선벡터 계산

	return output;
}

/*
직접광 -> 광원으로부터 직접 받는 빛
간접광 -> 다른 물체에 반사되는 빛(레이 트레이싱)

빛 - 난반사광(diffuse) , 정반사광(specular)로 크게 두개로 구성

물체의 표면에서 반사될때 여러 방향으로 고르게 반사되는 빛
난반사광 구하는 방법 - 람베르트 모델

표면법선과 입사광이 이루는 각의 코사인 값 : 난반사광의 양
-> 코사인 연산은 비용이 너무 커서 쉐이더에서 매번 호출하기 부담스러움
내적이 코사인 연산을 대신할 수 있음

A*B = cos(ceta)|a||b|
-> 입사광 벡터와 법선 벡터의 길이는 1이어도 상관없기 때문에
-> 1로 바꿔서 쉽게 하면 
-> 두 벡터가 이루는 각의 코사인 값은 두 벡터의 내적과 같다

입사광 벡터를 만들려면 ?
-> 광원 위치에서 현재 위치까지 선을 그으면 됨 -> 벡터의 뺄셈

이제 법선벡터와 입사광벡터의 내적을 하면 diffuse 값이 나옴

픽셀쉐이더는 화면속에 있는 픽셀에만 실행됩니다.
따라서 화면 밖(상하좌우로 밖... 뷰포트 밖이겠군요)에 있는 픽셀에 대해서는 픽셀쉐이더가 실행되지 않습니다.
깊이버퍼라는 놈이 뒤에있는 픽셀을 그냥 무시해줍니다

뭐든간에 이런 저런 기법을 통해 덥쳐그리는 일이 적게 만드는게 GPU 최적화의 기본입니다.
*/