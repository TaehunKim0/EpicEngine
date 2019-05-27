cbuffer Matrixbuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

/*
���� ���̻� ������ ������ ������� �ʰ� �ؽ��� ��ǥ�� ����ϰ� �� ���Դϴ�. 
�ؽ��Ŀ����� U�� V��ǥ���� ����ϱ� ������ �̸� ǥ���ϱ� ���� float2 �ڷ����� �̿��մϴ�.
���� ���̴��� �ȼ� ���̴����� �ؽ��� ��ǥ�� ��Ÿ���� ���� TEXCOORD0�̶�� ���� ����մϴ�.
���� ���� �ؽ��� ��ǥ�� �����ϴٸ� �� ���� 0���� �ƹ� ���ڷγ� ������ ���� �ֽ��ϴ�.
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

	//������ ��� ����� ���ؼ� 4���� ����(4����)���� ��ġ���͸� �����մϴ�.
	input.position.w = 1.0f;

	//������ ���� , �� , ���� ��ȯ�մϴ�.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//�ռ� Ʃ�丮���� �÷� ���� ���̴��� �ؽ��� ���� ���̴��� ������ ��������
	//������ �״�� �����ϴ� ���� �ƴ϶� �ؽ����� ��ǥ���� �����ؼ� �ȼ� ���̴��� �����Ѵٴ� ���Դϴ�.
	output.tex = input.tex;

	return output;
}
