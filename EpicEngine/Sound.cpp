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
	우선 DirectSound API와 1차 버퍼를 초기화합니다.
	그 다음에는 LoadWaveFile 함수를 호출하여 .wav 파일을 로드하고 그것으로 2차 버퍼를 초기화합니다.
	로드가 완료되면 PlayWaveFile을 호출하여 바로 .wav 파일을 재생합니다.
	*/

	//직접 사운드와 기본 사운드 버퍼 초기화
	result = InitializeDirectSound(hwnd);
	if (!result)
		return false;

	//파동 오디오 파일을 보조 버퍼에 로드하십시오
	result = LoadWaveFile("../EpicEngine/data/sound01.wav", &m_secondaryBuffer1);
	if (!result)
		return false;

	//이제 로드된 웨이브 파일을 재생합니다.
	result = PlayWaveFile();
	if (!result)
		return false;

	return true;
}

/*
Shutdown 함수는 ShutdownWaveFile 함수를 통하여 .
wav 오디오 데이터를 가지고 있는 2차 버퍼를 해제합니다. 
그것이 완료된 후에는 ShutdownDirectSound 함수로 1차 버퍼와 DirectSound 인터페이스를 해제합니다.
*/

void Sound::Shutdown()
{
	ShutdownWaveFile(&m_secondaryBuffer1);
	ShutdownDirectSound();
}
/*
InitializeDirectSound 함수는 DirectSound와 1차 버퍼의 인터페이스에 대한 포인터를 얻어옵니다.
여기서 모든 음향 장치를 조회하여 그 중 특정 장치를 1차 버퍼로 정해 그 포인터를 얻어올 수 있지만,
이 튜토리얼에서는 단순함을 유지하기 위하여 기본 음향 장치에 대한 포인터를 얻어오도록 하였습니다.
*/
bool Sound::InitializeDirectSound(HWND hwnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;

	//다이렉트 사운드 인터페이스 포인터를 기본 음향 장치로 선언합니다.
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if (FAILED(result))
		return false;

	// 기본 사운드 버퍼의 형식을 수정할 수 있도록 협력 레벨을 우선순위로 설정하십시오.
	result = m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(result))
		return false;

	/*
	1차 버퍼에 접근하기 위한 방법을 명세하기 위한 description을 작성해야 합니다.
	이 구조체에서는 dwFlags가 가장 중요합니다.
	이 튜토리얼에서는 1차 버퍼의 볼륨을 조절할 수 있도록 작성합니다.
	다른 기능들도 건드려 볼 수 있지만 단순함을 유지하도록 하겠습니다.
	*/

	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	//기본 사운드 장치에서 기본 사운드 버퍼를 제어하십시오.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if (FAILED(result))
		return false;

	/*
	기본 음향 장치를 1차 버퍼로 하여 제어권을 얻었다면 이 장치의 포맷을 우리의 오디오 파일 포맷으로 바꿔야 합니다.
	고품질의 사운드 품질을 원하기 때문에 비압축 CD 오디오 품질로 설정합니다.
	*/

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	//기본 버퍼를 지정된 파형 형식으로 설정하십시오
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
//LoadWaveFile 함수는 .wav 오디오 파일을 로드하여 그 데이터를 2차 버퍼에 복사합니다.
//다른 포맷으로 하기 원한다면 이 함수를 바꾸거나 그런 기능의 새로운 함수를 만들어 사용할 수 있습니다.
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
	//파형 파일을 이진수로 염.
	h = fopen_s(&filePtr, filename, "rb");
	if (h != 0)
	{
		return false;
	}

	//웨이브 파일의 헤더를 읽음
	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// 청크 ID가 RIFF 형식인지 확인하십시오.
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	// 파일 포맷이 웨이브 인지 확ㄹ인
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	// 하위 청크 ID가 fmt 형식인지 확인하십시오.
	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}

	//오디오 형식이 WAVE_FORMAT_PCM인지 확인하십시오
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}


	//파일이 스테레오 형식으로 기록되었는지 확인한다.
	if (waveFileHeader.numChannels != 2)
	{
		return false;
	}

	// 파형 파일이 44.1KHz의 샘플링 속도로 기록되었는지 확인하십시오.
	if (waveFileHeader.sampleRate != 44100)
	{
		return false;
	}

	if (waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}

	// 데이터 청크 헤더를 확인하십시오.
	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		return false;
	}


	/*
	여기까지 왔다면 웨이브 파일의 헤더가 충분히 검증된 상태이므로 데이터를 복사할 2차 버퍼를 생성할 수 있습니다. 
	1차 버퍼와 비슷하게 우선 웨이브 포맷을 설정하고 버퍼의 description을 작성합니다.
	1차버퍼가 아니기 때문에 dwFlag나 dwBufferBytes는 그 값이 조금 다릅니다.
	*/
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// 파형 파일을 로드할 보조 사운드 버퍼의 버퍼 설명을 설정하십시오.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	/*
	2차 버퍼를 생성하는 방법은 상당히 이상합니다.
	우선 버퍼의 description을 가지고 임시적으로 IDirectSoundBuffer 객체를 만듭니다.
	이것이 성공하면 QueryInterface 함수를 IID_IDirectSoundBuffer8인자와 함께 호출하여
	이 임시 버퍼로 IDirectSoundBuffer8의 2차 버퍼를 만들게 됩니다.
	이것까지 성공하면 이제서야 임시 버퍼를 해제하고 2차 버퍼를 사용하게 됩니다.
	*/

	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 직접 사운드 8 인터페이스에서 버퍼 형식을 테스트하고 보조 버퍼를 생성하십시오.
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if (FAILED(result))
	{
		return false;
	}

	tempBuffer->Release();
	tempBuffer = nullptr;

	/*
	2차 버퍼가 준비되었다면 오디오 파일의 웨이브 데이터를 가져옵니다.
	우선 메모리 버퍼에 불러들여서 필요하다면 적절히 수정을 가합니다.
	그 뒤에 2차 버퍼를 잠궈 접근 권한을 취득한 뒤, memcpy를 사용하여 데이터를 복사하고 잠금을 해제합니다.
	이제 2차 버퍼는 사용할 준비가 되었습니다. 기억해 둘 것은 2차 버퍼를 잠글 때
	두 개의 포인터를 받아 쓴다는 것입니다.

	버퍼가 원형으로 구성되어 있기 때문에 중간부터 버퍼를 쓰게 되는 경우 
	그 지점에서의 버퍼 사이즈를 아는 것이 필요하기 때문입니다.
	
	그래야 버퍼의 범위를 넘어가지 않게 쓰기 작업을 할 수 있습니다.
	이런 구조는 오디오를 재생하는 것과 같은 일에 유용합니다. 
	이 튜토리얼에서는 작업을 간단하게 하기 위해 오디오 파일과 같은 크기의 버퍼를 만들고
	버퍼의 시작부터 파일을 기록하게 합니다.
	*/

	// 데이터 청크 헤더의 끝에서 시작하는 웨이브 데이터의 시작으로 이동하십시오.
	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	//파형 파일 데이터를 저장할 임시 버퍼를 생성하십시오.
	waveData = new unsigned char[waveFileHeader.dataSize];
	if (!waveData)
	{
		return false;
	}

	//새로 생성된 버퍼로 파형 파일 데이터를 읽어보십시오.
	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if (count != waveFileHeader.dataSize)
	{
		return false;
	}

	// 읽기가 완료되면 파일을 닫으십시오.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	//파동 데이터를 쓰려면 보조 버퍼를 잠근다.
	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result))
	{
		return false;
	}

	//버퍼에 파동 데이터 복사
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	//두번째 버퍼에 데이터를 쓴 후 언락
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
PlayWaveFile 함수는 2차 버퍼에 저장되어 있는 오디오 파일을 재생합니다. 
Play 함수에서 1차 버퍼의 오디오와의 믹싱을 자동으로 수행한 뒤 재생합니다.
여기서는 2차 버퍼의 시작 부분으로 시작 위치를 지정하였습니다.
그렇지 않으면 가장 최근에 재생되었던 부분부터 시작하게 됩니다.
그리고 소리를 조정할 수 있게 세팅하였으므로 음량을 최대로 맞춰 봅니다.
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
