#pragma once

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>

/*
WaveHeaderType 구조체는 .wav 파일 포맷을 위한 것입니다. 
wav 파일을 로드할 때는 우선 파일의 헤더 정보를 읽어 오디오 데이터를 로드하기 위한 정보들을 가져옵니다.
만약 다른 오디오 파일 포맷을 사용한다면 그에 맞추어 다른 헤더 형식으로 바꿀 수 있습니다.
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
	Initialize와 Shutdown 함수는 이 튜토리얼에서 필요한 모든 것들을 제어합니다.
	Initialize 함수는 DirectSound를 초기화하고 .wav 오디오 파일을 로드하고 재생하는 일까지 합니다.
	Shutdown 함수는 .wav 파일과 DirectSound를 해제합니다.
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

	//이 튜토리얼에서는 하나의 사운드만 로드하므로 하나의 2차 버퍼만을 선언합니다.
	IDirectSoundBuffer8* m_secondaryBuffer1;
};

