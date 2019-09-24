#pragma once

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>

/*
WaveHeaderType ����ü�� .wav ���� ������ ���� ���Դϴ�. 
wav ������ �ε��� ���� �켱 ������ ��� ������ �о� ����� �����͸� �ε��ϱ� ���� �������� �����ɴϴ�.
���� �ٸ� ����� ���� ������ ����Ѵٸ� �׿� ���߾� �ٸ� ��� �������� �ٲ� �� �ֽ��ϴ�.
*/

class Sound
{
private:
	struct WaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};


public:
	Sound();
	Sound(const Sound&);
	~Sound();

public:
	/*
	Initialize�� Shutdown �Լ��� �� Ʃ�丮�󿡼� �ʿ��� ��� �͵��� �����մϴ�.
	Initialize �Լ��� DirectSound�� �ʱ�ȭ�ϰ� .wav ����� ������ �ε��ϰ� ����ϴ� �ϱ��� �մϴ�.
	Shutdown �Լ��� .wav ���ϰ� DirectSound�� �����մϴ�.
	*/

	bool Initialize(HWND);
	void Shutdown();

private:
	bool InitializeDirectSound(HWND);
	void ShutdownDirectSound();

	bool LoadWaveFile(char*, IDirectSoundBuffer8**);
	void ShutdownWaveFile(IDirectSoundBuffer8**);

	bool PlayWaveFile();	

private:
	IDirectSound8* m_DirectSound;
	IDirectSoundBuffer* m_primaryBuffer;

	//�� Ʃ�丮�󿡼��� �ϳ��� ���常 �ε��ϹǷ� �ϳ��� 2�� ���۸��� �����մϴ�.
	IDirectSoundBuffer8* m_secondaryBuffer1;
};

