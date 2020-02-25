#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <future>
#include <fstream>
#include <condition_variable>
#include <windows.h>
#include <mmsystem.h>
#include <conio.h>
#include <mmsystem.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <dsound.h>

#include "debug.h"
#include "customException.h"

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

/*============================================================================================================================================================================================================*

The code has been modified and collected from the following sources and references down below
	+ http://www.cplusplus.com/forum/beginner/88542/ (doesent work?)
	+ http://www.rastertek.com/dx10tut14.html
	+ https://www.youtube.com/watch?v=qGC3xiliJW8 (fun fact i leaked a key and almost got my ass handed to me on stream lol)

	TODO: what is "Waveform Audio API".

/*============================================================================================================================================================================================================*/

#define DIRECT_SOUND_CREATE(name)						HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);

using namespace std;

void* pSoundManager;

enum SoundError
{
	SoundLoadLibraryError = 1,
	DirectSoundCreationError,
	FailedToCreatePrimarySoundBuffer,
	FailedToCreateSeconarySoundBuffer,
	FailedToSetWavFormat,
	FailedToOpenFile,
	FailedToReadFile,
	WaveFileNotVerified,
	FailedToSetWaveBuffer,
	FailedToLockTheSecondaryBuffer,
	FailedToUnlockTheSecondaryBuffer,
	FailedToCreate3DBuffer,
	FailedToPlay,
};

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

struct WaveData
{
	WaveHeaderType header;
	WAVEFORMATEX wfWaveFormat;
	DSBUFFERDESC descSecondaryBuffer;
	LPDIRECTSOUNDBUFFER lpSecondaryBuffer;
	LPDIRECTSOUND3DBUFFER8 lpSecondary3DBuffer;
	LPDIRECTSOUND3DLISTENER8 lpSecondary3DBufferListener;
	
	unsigned char* waveData;
	unsigned long waveDataSize;
};

class SoundManager
{
private:
	direct_sound_create* DirectSoundCreate;

public:
	HMODULE hDSoundLibrary;
	LPDIRECTSOUND lpdsDirectSound;
	DSBUFFERDESC descPrimaryBuffer;
	LPDIRECTSOUNDBUFFER lpPrimaryBuffer;

	vector<WaveData*> vSounds;

	bool bInialized;

	int Initalize()
	{
		odprintf("Initalizing SoundManager[%p]...", this);

		debugTabIndex++;

		odprintf("Loading the directx sound library .dll ...");
		HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");											//NOTE: There is a much easier way and a much simpler way to implement this but my babyass wants totalteriann control/info 

		if (!DSoundLibrary)
		{
			odprintf("ERROR: Failed to load the sound library!");
			return SoundLoadLibraryError;																	//If we did fail to load return print and return
		}

		// NOTE(casey): Get a DirectSound object! - cooperative
		odprintf("Loading the funtions from the dll...");

		debugTabIndex++;

		odprintf("HRESULT WINAPI DirectSoundCreate(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)");
		DirectSoundCreate = (direct_sound_create*)														//NOTE: HOW THE ACTUAL FUck is this working while watching the tutorial i rewrote this code multiple times
			GetProcAddress(DSoundLibrary, "DirectSoundCreate");

		debugTabIndex--;

		odprintf("Done!");

		if (FAILED(DirectSoundCreate(NULL, &lpdsDirectSound, NULL)))
		{
			odprintf("ERROR: Failed to create DirectSound object!");
			debugTabIndex--;
			return DirectSoundCreationError;
		}

		odprintf("Setting cooperative level...");
		lpdsDirectSound->SetCooperativeLevel(GetConsoleWindow(), DSSCL_PRIORITY);						//TODO: Make some debugging because this may fail?

		odprintf("Setting the buffer description...");													//Set the buffer for the primary and secondary buffers
		ZeroMemory(&descPrimaryBuffer, sizeof(descPrimaryBuffer));										//Clean the buffers with zero

		//Set the primary buffer identities
		descPrimaryBuffer.dwSize = sizeof(descPrimaryBuffer);											//They may change the sizes in l8r versions
		descPrimaryBuffer.dwFlags = DSBCAPS_PRIMARYBUFFER;

		odprintf("Createing the sound buffers...");														//Set the buffer for the primary and secondary buffers
		if (FAILED(lpdsDirectSound->CreateSoundBuffer(&descPrimaryBuffer, &lpPrimaryBuffer, 0)))
		{
			odprintf("ERROR: Failed to create a primary sound buffer!");								//Print the error and return
			debugTabIndex--;
			return FailedToCreatePrimarySoundBuffer;
		}

		pSoundManager = this;

		debugTabIndex--;

		odprintf("Initalized SoundManager[%p]!", this);

		return 0;
	}

	int LoadSoundFile(wstring name, bool bare, bool b3d, bool b2d)
	{
		odprintf("SoundManager[%p].LoadSoundFile(%ls)...", this, name.c_str());
		debugTabIndex++;

		WaveData* wdCurrent = new WaveData();
		long bitrate = 36000;																			//Smallest bitrate
		FILE* filePtr = nullptr;

		odprintf("Opening the file...");																//Set the buffer for the primary and secondary buffers
		int iResult = _wfopen_s(&filePtr, name.c_str(), L"rb");

		if (iResult != 0 || filePtr == nullptr)
		{
			odprintf("ERROR: Failed to open the file!");												//Print the error and return
			debugTabIndex--;
			return FailedToOpenFile;
		}

		odprintf("Reading the wave header struct...");													//Read the wave structure
		iResult = fread(&wdCurrent->header, sizeof(wdCurrent->header), 1, filePtr);						//Call the function

		if (iResult != 1)
		{
			odprintf("ERROR: Failed to read the file!");												//Print the error and return
			debugTabIndex--;
			return FailedToReadFile;
		}

		odprintf("Verifying the wave header struct...");												//Verify the wave structure
		// Check that the chunk ID is the RIFF format.
		if ((wdCurrent->header.chunkId[0] != 'R') || (wdCurrent->header.chunkId[1] != 'I') ||
			(wdCurrent->header.chunkId[2] != 'F') || (wdCurrent->header.chunkId[3] != 'F'))
		{
			odprintf("ERROR: Failed to verify the RIIF format of the wave file!");						//Print the error and return
			debugTabIndex--;
			return WaveFileNotVerified;
		}

		// Check that the file format is the WAVE format.
		if ((wdCurrent->header.format[0] != 'W') || (wdCurrent->header.format[1] != 'A') ||
			(wdCurrent->header.format[2] != 'V') || (wdCurrent->header.format[3] != 'E'))
		{
			odprintf("ERROR: Failed to verify the RIIF format of the wave file!");						//Print the error and return
			debugTabIndex--;
			return WaveFileNotVerified;
		}

		// Check that the sub chunk ID is the fmt format.
		if ((wdCurrent->header.subChunkId[0] != 'f') || (wdCurrent->header.subChunkId[1] != 'm') ||
			(wdCurrent->header.subChunkId[2] != 't') || (wdCurrent->header.subChunkId[3] != ' '))
		{
			odprintf("ERROR: Failed to verify the fmt format of the wave file!");						//Print the error and return
			debugTabIndex--;
			return WaveFileNotVerified;
		}

		// Check that the audio format is WAVE_FORMAT_PCM.
		if (wdCurrent->header.audioFormat != WAVE_FORMAT_PCM)
		{
			odprintf("ERROR: Failed to verify the format of the wave file!");							//Print the error and return
			debugTabIndex--;
			return WaveFileNotVerified;
		}

		odprintf("Verified!");

		odprintf("Setting the .wav format...");															//Set the default wav format
		wdCurrent->wfWaveFormat.wFormatTag = WAVE_FORMAT_PCM;
		wdCurrent->wfWaveFormat.nChannels = wdCurrent->header.numChannels;
		wdCurrent->wfWaveFormat.nSamplesPerSec = wdCurrent->header.sampleRate;
		wdCurrent->wfWaveFormat.wBitsPerSample = wdCurrent->header.bitsPerSample;
		wdCurrent->wfWaveFormat.nBlockAlign = (wdCurrent->wfWaveFormat.nChannels * wdCurrent->wfWaveFormat.wBitsPerSample) / 8;
		wdCurrent->wfWaveFormat.nAvgBytesPerSec = wdCurrent->wfWaveFormat.nSamplesPerSec * wdCurrent->wfWaveFormat.nBlockAlign;
		wdCurrent->wfWaveFormat.cbSize = 0;

		// Move to the beginning of the wave data which starts at the end of the data chunk header.
		odprintf("Reading the file into the buffer[%lu]...", wdCurrent->header.dataSize);				//Set the default wav format
		fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

			wdCurrent->waveData = new unsigned char[wdCurrent->header.dataSize];
			if (!wdCurrent->waveData)
			{
				odprintf("ERROR: Failed to create a char buffers!");										//Print the error and return
				debugTabIndex--;
				return FailedToSetWaveBuffer;
			}

			iResult = fread(wdCurrent->waveData, sizeof(unsigned char), wdCurrent->header.dataSize, filePtr);

			if (iResult != wdCurrent->header.dataSize)
			{
				odprintf("ERROR: Failed to create a char buffers!");										//Print the error and return
				debugTabIndex--;
				return FailedToSetWaveBuffer;
			}

			iResult = fclose(filePtr);
			if (FAILED(iResult))
			{
				odprintf("WARNING: file pointer failed to close! May cause memory leakage\\future errors");	//Print the error and return
			}
		

		odprintf("Setting the buffer description...");													//Set the buffer for the primary and secondary buffers
		ZeroMemory(&wdCurrent->descSecondaryBuffer, sizeof(wdCurrent->descSecondaryBuffer));

		//Set the secondary buffer 
		wdCurrent->descSecondaryBuffer.dwSize = sizeof(wdCurrent->descSecondaryBuffer);
		wdCurrent->descSecondaryBuffer.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
		wdCurrent->descSecondaryBuffer.dwBufferBytes = wdCurrent->header.dataSize;
		wdCurrent->descSecondaryBuffer.dwReserved = 0;
		wdCurrent->descSecondaryBuffer.lpwfxFormat = &wdCurrent->wfWaveFormat;
		wdCurrent->descSecondaryBuffer.guid3DAlgorithm = GUID_NULL;

		odprintf("Creating the secondary sound buffer...");												//Set the default wav format
		if (FAILED(lpdsDirectSound->CreateSoundBuffer(&wdCurrent->descSecondaryBuffer, &wdCurrent->lpSecondaryBuffer, 0)))
		{
			odprintf("ERROR: Failed to create a secondary sound buffer!");								//Print the error and return
			debugTabIndex--;
			return FailedToCreatePrimarySoundBuffer;
		}

		// Copy the wave data into the buffer.
		unsigned char* bufferPtr;																		//The pointer to the DirectSounds buffers
		unsigned long bufferSize;

		odprintf("Copying to the secondary sound buffer");
		iResult = wdCurrent->lpSecondaryBuffer->Lock(0, wdCurrent->header.dataSize, (void**)& bufferPtr, (DWORD*)& bufferSize, NULL, 0, 0);
		if (FAILED(iResult))
		{
			odprintf("ERROR: Failed to lock the secondary buffer!");									//Print the error and return
			debugTabIndex--;
			return FailedToLockTheSecondaryBuffer;
		}

		memcpy(bufferPtr, wdCurrent->waveData, wdCurrent->header.dataSize);

		iResult = wdCurrent->lpSecondaryBuffer->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
		if (FAILED(iResult))
		{
			odprintf("ERROR: Failed to unlock the secondary buffer!");									//Print the error and return
			debugTabIndex--;
			return FailedToUnlockTheSecondaryBuffer;
		}

		odprintf("Coppied!");

		//odprintf("Getting the 3D interface of the buffer!");
		//
		//// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
		//iResult = wdCurrent->lpSecondaryBuffer->QueryInterface(IID_IDirectSound3DBuffer8, (void**) & *wdCurrent->lpSecondary3DBuffer);
		//if (FAILED(iResult))
		//{
		//	odprintf("ERROR: Failed to get the 3D buffer!");											//Print the error and return
		//	debugTabIndex--;
		//	return FailedToCreate3DBuffer;
		//}

		//iResult = lpPrimaryBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)& wdCurrent->lpSecondary3DBufferListener);
		//if (FAILED(iResult))
		//{
		//	odprintf("ERROR: Failed to get the 3D listener!");											//Print the error and return
		//	debugTabIndex--;
		//	return FailedToCreate3DBuffer;
		//}

		odprintf("Cleaning up...");

		if (bare)
		{
			delete[] wdCurrent->waveData;
			wdCurrent->waveDataSize = sizeof(wdCurrent->waveData);
		}

		if (b2d)
		{
			wdCurrent->lpSecondaryBuffer->SetCurrentPosition(0);
			wdCurrent->lpSecondaryBuffer->SetVolume(DSBVOLUME_MAX);
		}
		else
		{
			wdCurrent->lpSecondaryBuffer->Release();
		}

		vSounds.push_back(wdCurrent);

		debugTabIndex--;

		return 0;
	}

	int Cleanup()
	{
		odprintf("Cleaning SoundManager[%p]...", this);

		debugTabIndex++;

		lpPrimaryBuffer->Release();
		lpdsDirectSound->Release();

		for (int i = 0; i < vSounds.size(); i++)
		{
			vSounds[i]->lpSecondaryBuffer->Release();
			delete[] vSounds[i]->waveData;

			delete vSounds[i];

		}

		vSounds.erase(vSounds.begin(), vSounds.end());

		debugTabIndex--;

	}

};

//Needed for easier reading
#define SoundMacro ((SoundManager*)pSoundManager)

#endif