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

	//�۲� �����Ͱ� �ִ� �ؽ�Ʈ ���� �ε�
	result = LoadFontData(fontFileName);
	if (!result)
		return false;

	//�۲� ���ڰ� �ִ� �ؽ��� ���� �ε�
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
BuildVertexArray �Լ��� Text�� ���� ȣ��Ǿ� ���ڷ� ���� �������� ���� ���۸� �������ϴ�.
�̷��� Text�� �̷��� ������� �ڽŸ��� ���� ���۸� ������ �Ǳ� ������ �ս��� ���ڸ� �׸� �� �ֽ��ϴ�.
ù��° ������ vertices�� ���� �迭�� ����Ű�� �����ͷμ� Text���� �� �Լ����� ������ ä�� �迭�� �����ϴ� �� ���˴ϴ�.
�� ���� ������ sentence�� ���� �迭�� ����� �� �ʿ��� ����(���ھ�)�Դϴ�. drawX�� drawY�� ������ �׷��� ȭ����� ��ǥ�Դϴ�.
*/
void Font::BuildVertexArray(void * vertices, char * sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, index, i, letter;

	//�Է� ������ VertexType �������� �����Ͻʽÿ�.
	vertexPtr = (VertexType*)vertices;

	//������ ���� ���� ���϶�
	numLetters = (int)strlen(sentence);

	index = 0;

	/*
	�ݺ����� ���鼭 ����/�ε��� �迭�� ����ϴ�. 
	�켱 ������ �����ϴ� ���ڵ��� ���Ͽ� ���ڴ� �� ���� �ﰢ���� ����ϴ�.
	�׸��� ���� �� �� �ﰢ���� ������ �׷��� ���ڿ� �ش��ϴ� �ؽ��ĸ� m_Font �迭�� U��ǥ�� �ȼ� �ʺ� ����Ͽ� ���ν�ŵ�ϴ�.
	�ϴ� ���ڿ� �ش��ϴ� ������ �����Ǹ� X��ǥ�� �����Ͽ� ���ڰ� �׷��� ��ġ�� ����ϴ�.
	*/

	// �� ���ڸ� ��и鿡 �׷���.
	for (i = 0; i < numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		// ���ڰ� �����̸� 3�ȼ� �̻����� �̵��Ͻʽÿ�.
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


	//������� ����
}

bool Font::LoadFontData(char * fileName) //�ؽ��Ŀ��� ���� ��ġ������ ��� �ִ� fontdata ������ �ҷ��ɴϴ�.
{
	ifstream fin;
	int i;
	char temp;

	//	�켱 FontType���� �迭�� ����ϴ�.�迭�� ũ��� �ؽ��Ŀ� �ִ� ���� ��, �׸��� fontdata.txt ������ �ε��� ������ 95�� �մϴ�.

	//�۲� ���� ���۸� ���弼��
	m_Font = new FontType[95];
	if (!m_Font)
		return false;

	//������ ���� �� ������ �о� m_Font �迭�� �����մϴ�. ���⼭�� �ܼ��� �ؽ����� U��ǥ�� left�� right, �׸��� ������ �ȼ� �ʺ� �о���Դϴ�.
	fin.open(fileName);
	if (fin.fail())
		return false;

	//�ؽ�Ʈ�� ���Ǵ� 95���� Asci ���ڸ� �����ʽÿ�.
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

//LoadTexture �Լ��� font.dds ������ �о� �ؽ��� ���̴��� �����մϴ�.
//�� �ؽ��Ŀ��� ���ڸ� �̾Ƴ� ������ �簢���� ���� ȭ�鿡 �׸��� �˴ϴ�.
bool Font::LoadTexture(ID3D11Device * device, WCHAR * fileName)
{
	bool result;

	//�ؽ��� ����
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
