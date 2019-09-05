cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

/*
ī�޶��� ������ ������ ���ο� ��� ���۸� �߰��մϴ�.
�� ���̴������� ���ݻ��� ����� ���� �� ������ ��� ������������ ������ �ʿ��ϹǷ� ī�޶��� ��ġ�� �˾ƾ� �մϴ�.
*/
cbuffer CameraBuffer
{
	float3 cameraPosition;
	float padding;
};
//������� ��

//Typedef
struct VertexInputType
{
	float4 position : POSITION;
	float4 tex : TEXCOORD0; //diffuse�ε� �������� ���Ͱ� �ƴ� ���� �ϳ��ε� �뵵�� �´� �ø�ƽ�� ������ ���� TEXCOORD�� ����.	
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

	// ������ ��� ����� ���� ��ġ ���͸� 4 ������ �����Ͻʽÿ�.
	input.position.w = 1.0f;

	//����, �� �� ���� ��Ŀ� ���� ������ ��ġ�� ����Ѵ�.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//�ȼ� ���̴��� �ؽ�ó ��ǥ�� �����Ͻʽÿ�.
	output.tex = input.tex;

	//���� ��Ʈ������ ���ؼ��� ���� ���͸� ����Ͻʽÿ�
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal); //�������� ���

	return output;
}

/*
������ -> �������κ��� ���� �޴� ��
������ -> �ٸ� ��ü�� �ݻ�Ǵ� ��(���� Ʈ���̽�)

�� - ���ݻ籤(diffuse) , ���ݻ籤(specular)�� ũ�� �ΰ��� ����

��ü�� ǥ�鿡�� �ݻ�ɶ� ���� �������� ���� �ݻ�Ǵ� ��
���ݻ籤 ���ϴ� ��� - ������Ʈ ��

ǥ������� �Ի籤�� �̷�� ���� �ڻ��� �� : ���ݻ籤�� ��
-> �ڻ��� ������ ����� �ʹ� Ŀ�� ���̴����� �Ź� ȣ���ϱ� �δ㽺����
������ �ڻ��� ������ ����� �� ����

A*B = cos(ceta)|a||b|
-> �Ի籤 ���Ϳ� ���� ������ ���̴� 1�̾ ������� ������
-> 1�� �ٲ㼭 ���� �ϸ� 
-> �� ���Ͱ� �̷�� ���� �ڻ��� ���� �� ������ ������ ����

�Ի籤 ���͸� ������� ?
-> ���� ��ġ���� ���� ��ġ���� ���� ������ �� -> ������ ����

���� �������Ϳ� �Ի籤������ ������ �ϸ� diffuse ���� ����

�ȼ����̴��� ȭ��ӿ� �ִ� �ȼ����� ����˴ϴ�.
���� ȭ�� ��(�����¿�� ��... ����Ʈ ���̰ڱ���)�� �ִ� �ȼ��� ���ؼ��� �ȼ����̴��� ������� �ʽ��ϴ�.
���̹��۶�� ���� �ڿ��ִ� �ȼ��� �׳� �������ݴϴ�

���簣�� �̷� ���� ����� ���� ���ı׸��� ���� ���� ����°� GPU ����ȭ�� �⺻�Դϴ�.
*/