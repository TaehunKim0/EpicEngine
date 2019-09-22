#include "Font.h"



Font::Font()
{
}

Font::Font(const Font & other)
{
}


Font::~Font()
{
}

bool Font::Initialize(ID3D11Device * device, char * fontFileName, WCHAR * textureFileName)
{
	bool result;

	//글꼴 데이터가 있는 텍스트 파일 로드
	result = LoadFontData(fontFileName);
	if (!result)
		return false;

	//글꼴 문자가 있는 텍스쳐 파일 로드
	result = LoadTexture(device, textureFileName);
	if (!result)
		return false;

	return true;
}

void Font::ShutDown()
{
	ReleaseTexture();
	ReleaseFontData();
}

ID3D11ShaderResourceView * Font::GetTexture()
{
	return m_Texture->GetTexture();
}

/*
BuildVertexArray 함수는 Text에 의해 호출되어 인자로 받은 문장으로 정점 버퍼를 만들어냅니다.
이렇게 Text는 이렇게 만들어진 자신만의 정점 버퍼를 가지게 되기 때문에 손쉽게 글자를 그릴 수 있습니다.
첫번째 인자인 vertices는 정점 배열을 가리키는 포인터로서 Text에게 이 함수에서 내용을 채운 배열을 전달하는 데 사용됩니다.
그 다음 인자인 sentence는 정점 배열을 만드는 데 필요한 문장(문자얼)입니다. drawX와 drawY는 문장이 그려질 화면상의 좌표입니다.
*/
void Font::BuildVertexArray(void * vertices, char * sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, index, i, letter;

	//입력 정점을 VertexType 구조물에 동봉하십시오.
	vertexPtr = (VertexType*)vertices;

	//문장의 글자 수를 구하라
	numLetters = (int)strlen(sentence);

	index = 0;

	/*
	반복문을 돌면서 정점/인덱스 배열을 만듭니다. 
	우선 문장을 구성하는 문자들을 취하여 문자당 두 개의 삼각형을 만듭니다.
	그리고 나서 이 두 삼각형에 실제로 그려질 글자에 해당하는 텍스쳐를 m_Font 배열의 U좌표와 픽셀 너비를 사용하여 매핑시킵니다.
	일단 글자에 해당하는 도형이 생성되면 X좌표를 갱신하여 글자가 그려질 위치를 잡습니다.
	*/

	// 각 글자를 사분면에 그려라.
	for (i = 0; i < numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		// 글자가 공백이면 3픽셀 이상으로 이동하십시오.
		if (letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			// First triangle in quad.
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f); // Top left.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 0.0f); 
			index++;
			
			vertexPtr[index].position = D3DXVECTOR3((drawX + m_Font[letter].size), (drawY - 16), 0.0f); // Bottom right.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 1.0f); 
			index++; 
			
			vertexPtr[index].position = D3DXVECTOR3(drawX, (drawY - 16), 0.0f); // Bottom left.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 1.0f); 
			index++;
			
			// Second triangle in quad.
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f); // Top left. 
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 0.0f);
			index++;
			
			vertexPtr[index].position = D3DXVECTOR3(drawX + m_Font[letter].size, drawY, 0.0f); // Top right.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 0.0f); 
			index++;
			
			vertexPtr[index].position = D3DXVECTOR3((drawX + m_Font[letter].size), (drawY - 16), 0.0f); // Bottom right.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 1.0f);
			index++;
			
			// Update the x location for drawing by the size of the letter and one pixel.
			drawX = drawX + m_Font[letter].size + 1.0f;

		}

	}


	//여기부터 시작
}

bool Font::LoadFontData(char * fileName) //텍스쳐에서 글자 위치정보를 담고 있는 fontdata 파일을 불러옵니다.
{
	ifstream fin;
	int i;
	char temp;

	//	우선 FontType형의 배열을 만듭니다.배열의 크기는 텍스쳐에 있는 글자 수, 그리고 fontdata.txt 파일의 인덱스 개수인 95로 합니다.

	//글꼴 간격 버퍼를 만드세요
	m_Font = new FontType[95];
	if (!m_Font)
		return false;

	//파일을 열고 각 라인을 읽어 m_Font 배열에 저장합니다. 여기서는 단순히 텍스쳐의 U좌표인 left와 right, 그리고 문자의 픽셀 너비를 읽어들입니다.
	fin.open(fileName);
	if (fin.fail())
		return false;

	//텍스트에 사용되는 95개의 Asci 문자를 읽으십시오.
	for (i = 0; i < 95; i++)
	{
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	}

	fin.close();

	return true;
}

void Font::ReleaseFontData()
{
	if (m_Font)
	{
		delete [] m_Font;
		m_Font = nullptr;
	}
}

//LoadTexture 함수는 font.dds 파일을 읽어 텍스쳐 셰이더에 전달합니다.
//이 텍스쳐에서 글자를 뽑아내 각각의 사각형에 입혀 화면에 그리게 됩니다.
bool Font::LoadTexture(ID3D11Device * device, WCHAR * fileName)
{
	bool result;

	//텍스쳐 생성
	m_Texture = new Texture;
	if (!m_Texture)
		return false;

	result = m_Texture->Initialize(device, fileName);
	if (!result)
		return false;

	return true;
}

void Font::ReleaseTexture()
{
}
