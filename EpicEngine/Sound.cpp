#include "Sound.h"



Sound::Sound()
{
	m_DirectSound = nullptr;
	m_primaryBuffer = nullptr;
	m_secondaryBuffer1 = nullptr;
}

Sound::Sound(const Sound & other)
{
}


Sound::~Sound()
{
}

bool Sound::Initialize(HWND hwnd)
{
	bool result;

	/*
	�켱 DirectSound API�� 1�� ���۸� �ʱ�ȭ�մϴ�.
	�� �������� LoadWaveFile �Լ��� ȣ���Ͽ� .wav ������ �ε��ϰ� �װ����� 2�� ���۸� �ʱ�ȭ�մϴ�.
	�ε尡 �Ϸ�Ǹ� PlayWaveFile�� ȣ���Ͽ� �ٷ� .wav ������ ����մϴ�.
	*/

	//���� ����� �⺻ ���� ���� �ʱ�ȭ
	result = InitializeDirectSound(hwnd);
	if (!result)
		return false;

	//�ĵ� ����� ������ ���� ���ۿ� �ε��Ͻʽÿ�
	result = LoadWaveFile("../EpicEngine/data/sound01.wav", &m_secondaryBuffer1);
	if (!result)
		return false;

	//���� �ε�� ���̺� ������ ����մϴ�.
	result = PlayWaveFile();
	if (!result)
		return false;

	return true;
}

/*
Shutdown �Լ��� ShutdownWaveFile �Լ��� ���Ͽ� .
wav ����� �����͸� ������ �ִ� 2�� ���۸� �����մϴ�. 
�װ��� �Ϸ�� �Ŀ��� ShutdownDirectSound �Լ��� 1�� ���ۿ� DirectSound �������̽��� �����մϴ�.
*/

void Sound::Shutdown()
{
	ShutdownWaveFile(&m_secondaryBuffer1);
	ShutdownDirectSound();
}
/*
InitializeDirectSound �Լ��� DirectSound�� 1�� ������ �������̽��� ���� �����͸� ���ɴϴ�.
���⼭ ��� ���� ��ġ�� ��ȸ�Ͽ� �� �� Ư�� ��ġ�� 1�� ���۷� ���� �� �����͸� ���� �� ������,
�� Ʃ�丮�󿡼��� �ܼ����� �����ϱ� ���Ͽ� �⺻ ���� ��ġ�� ���� �����͸� �������� �Ͽ����ϴ�.
*/
bool Sound::InitializeDirectSound(HWND hwnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;

	//���̷�Ʈ ���� �������̽� �����͸� �⺻ ���� ��ġ�� �����մϴ�.
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if (FAILED(result))
		return false;

	// �⺻ ���� ������ ������ ������ �� �ֵ��� ���� ������ �켱������ �����Ͻʽÿ�.
	result = m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(result))
		return false;

	/*
	1�� ���ۿ� �����ϱ� ���� ����� ���ϱ� ���� description�� �ۼ��ؾ� �մϴ�.
	�� ����ü������ dwFlags�� ���� �߿��մϴ�.
	�� Ʃ�丮�󿡼��� 1�� ������ ������ ������ �� �ֵ��� �ۼ��մϴ�.
	�ٸ� ��ɵ鵵 �ǵ�� �� �� ������ �ܼ����� �����ϵ��� �ϰڽ��ϴ�.
	*/

	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	//�⺻ ���� ��ġ���� �⺻ ���� ���۸� �����Ͻʽÿ�.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if (FAILED(result))
		return false;

	/*
	�⺻ ���� ��ġ�� 1�� ���۷� �Ͽ� ������� ����ٸ� �� ��ġ�� ������ �츮�� ����� ���� �������� �ٲ�� �մϴ�.
	��ǰ���� ���� ǰ���� ���ϱ� ������ ����� CD ����� ǰ���� �����մϴ�.
	*/

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	//�⺻ ���۸� ������ ���� �������� �����Ͻʽÿ�
	result = m_primaryBuffer->SetFormat(&waveFormat);
	if (FAILED(result))
		return false;

	return true;
}

void Sound::ShutdownDirectSound()
{
	if (m_primaryBuffer)
	{
		m_primaryBuffer->Release();
		m_primaryBuffer = nullptr;
	}
	if (m_secondaryBuffer1)
	{
		m_secondaryBuffer1->Release();
		m_secondaryBuffer1 = nullptr;
	}
}
//LoadWaveFile �Լ��� .wav ����� ������ �ε��Ͽ� �� �����͸� 2�� ���ۿ� �����մϴ�.
//�ٸ� �������� �ϱ� ���Ѵٸ� �� �Լ��� �ٲٰų� �׷� ����� ���ο� �Լ��� ����� ����� �� �ֽ��ϴ�.
bool Sound::LoadWaveFile(char * filename, IDirectSoundBuffer8 ** secondaryBuffer)
{
	int error;
	FILE* filePtr;
	unsigned int count;
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;

	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned char *bufferPtr;
	unsigned long bufferSize;

	HRESULT h;
	//���� ������ �������� ��.
	h = fopen_s(&filePtr, filename, "rb");
	if (h != 0)
	{
		return false;
	}

	//���̺� ������ ����� ����
	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// ûũ ID�� RIFF �������� Ȯ���Ͻʽÿ�.
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	// ���� ������ ���̺� ���� Ȯ����
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	// ���� ûũ ID�� fmt �������� Ȯ���Ͻʽÿ�.
	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}

	//����� ������ WAVE_FORMAT_PCM���� Ȯ���Ͻʽÿ�
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}


	//������ ���׷��� �������� ��ϵǾ����� Ȯ���Ѵ�.
	if (waveFileHeader.numChannels != 2)
	{
		return false;
	}

	// ���� ������ 44.1KHz�� ���ø� �ӵ��� ��ϵǾ����� Ȯ���Ͻʽÿ�.
	if (waveFileHeader.sampleRate != 44100)
	{
		return false;
	}

	if (waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}

	// ������ ûũ ����� Ȯ���Ͻʽÿ�.
	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		return false;
	}


	/*
	������� �Դٸ� ���̺� ������ ����� ����� ������ �����̹Ƿ� �����͸� ������ 2�� ���۸� ������ �� �ֽ��ϴ�. 
	1�� ���ۿ� ����ϰ� �켱 ���̺� ������ �����ϰ� ������ description�� �ۼ��մϴ�.
	1�����۰� �ƴϱ� ������ dwFlag�� dwBufferBytes�� �� ���� ���� �ٸ��ϴ�.
	*/
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// ���� ������ �ε��� ���� ���� ������ ���� ������ �����Ͻʽÿ�.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	/*
	2�� ���۸� �����ϴ� ����� ����� �̻��մϴ�.
	�켱 ������ description�� ������ �ӽ������� IDirectSoundBuffer ��ü�� ����ϴ�.
	�̰��� �����ϸ� QueryInterface �Լ��� IID_IDirectSoundBuffer8���ڿ� �Բ� ȣ���Ͽ�
	�� �ӽ� ���۷� IDirectSoundBuffer8�� 2�� ���۸� ����� �˴ϴ�.
	�̰ͱ��� �����ϸ� �������� �ӽ� ���۸� �����ϰ� 2�� ���۸� ����ϰ� �˴ϴ�.
	*/

	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// ���� ���� 8 �������̽����� ���� ������ �׽�Ʈ�ϰ� ���� ���۸� �����Ͻʽÿ�.
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if (FAILED(result))
	{
		return false;
	}

	tempBuffer->Release();
	tempBuffer = nullptr;

	/*
	2�� ���۰� �غ�Ǿ��ٸ� ����� ������ ���̺� �����͸� �����ɴϴ�.
	�켱 �޸� ���ۿ� �ҷ��鿩�� �ʿ��ϴٸ� ������ ������ ���մϴ�.
	�� �ڿ� 2�� ���۸� ��� ���� ������ ����� ��, memcpy�� ����Ͽ� �����͸� �����ϰ� ����� �����մϴ�.
	���� 2�� ���۴� ����� �غ� �Ǿ����ϴ�. ����� �� ���� 2�� ���۸� ��� ��
	�� ���� �����͸� �޾� ���ٴ� ���Դϴ�.

	���۰� �������� �����Ǿ� �ֱ� ������ �߰����� ���۸� ���� �Ǵ� ��� 
	�� ���������� ���� ����� �ƴ� ���� �ʿ��ϱ� �����Դϴ�.
	
	�׷��� ������ ������ �Ѿ�� �ʰ� ���� �۾��� �� �� �ֽ��ϴ�.
	�̷� ������ ������� ����ϴ� �Ͱ� ���� �Ͽ� �����մϴ�. 
	�� Ʃ�丮�󿡼��� �۾��� �����ϰ� �ϱ� ���� ����� ���ϰ� ���� ũ���� ���۸� �����
	������ ���ۺ��� ������ ����ϰ� �մϴ�.
	*/

	// ������ ûũ ����� ������ �����ϴ� ���̺� �������� �������� �̵��Ͻʽÿ�.
	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	//���� ���� �����͸� ������ �ӽ� ���۸� �����Ͻʽÿ�.
	waveData = new unsigned char[waveFileHeader.dataSize];
	if (!waveData)
	{
		return false;
	}

	//���� ������ ���۷� ���� ���� �����͸� �о�ʽÿ�.
	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if (count != waveFileHeader.dataSize)
	{
		return false;
	}

	// �бⰡ �Ϸ�Ǹ� ������ �����ʽÿ�.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	//�ĵ� �����͸� ������ ���� ���۸� ��ٴ�.
	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result))
	{
		return false;
	}

	//���ۿ� �ĵ� ������ ����
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	//�ι�° ���ۿ� �����͸� �� �� ���
	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if (FAILED(result))
		return false;
	
	delete[] waveData;
	waveData = nullptr;

	return true;
}

void Sound::ShutdownWaveFile(IDirectSoundBuffer8 ** secondaryBuffer)
{
	if (*secondaryBuffer)
	{
		(*secondaryBuffer)->Release();
		*secondaryBuffer = nullptr;
	}
}

/*
PlayWaveFile �Լ��� 2�� ���ۿ� ����Ǿ� �ִ� ����� ������ ����մϴ�. 
Play �Լ����� 1�� ������ ��������� �ͽ��� �ڵ����� ������ �� ����մϴ�.
���⼭�� 2�� ������ ���� �κ����� ���� ��ġ�� �����Ͽ����ϴ�.
�׷��� ������ ���� �ֱٿ� ����Ǿ��� �κк��� �����ϰ� �˴ϴ�.
�׸��� �Ҹ��� ������ �� �ְ� �����Ͽ����Ƿ� ������ �ִ�� ���� ���ϴ�.
*/

bool Sound::PlayWaveFile()
{
	HRESULT result;
	
	result = m_secondaryBuffer1->SetCurrentPosition(0);
	if (FAILED(result))
		return false;

	result = m_secondaryBuffer1->SetVolume(DSBVOLUME_MAX);
	if (FAILED(result))
		return false;

	result = m_secondaryBuffer1->Play(0, 0, DSBPLAY_LOOPING);
	if (FAILED(result))
		return false;

	return true;
}
