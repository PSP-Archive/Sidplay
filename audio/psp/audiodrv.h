// --------------------------------------------------------------------------
// ``Open Sound System (OSS)'' specific audio driver interface.
// --------------------------------------------------------------------------

#ifndef AUDIODRV_H
#define AUDIODRV_H


#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sidplay/emucfg.h>

#define PSP_NUM_SAMPLES 1024


class audioDriver
{
	
 public:  // --------------------------------------------------------- public

    audioDriver();
  
	bool IsThere();

	bool Open(udword freq, int precision, int channels,
			  int fragments, int fragBase);
	
	void Close();
	
	void Play(ubyte* buffer, int bufferSize);

	bool Reset()
	{
        return 1;
	}
	
	int GetAudioHandle()
	{
		return audioHd;
	}
	
	udword GetFrequency()
    { 
		return frequency;
    }
	
	int GetChannels()
    {
		return channels;
    }
	
	int GetSamplePrecision()
    {
		return precision;
    }
	
	int GetSampleEncoding()
    {
		return encoding;
    }
	
	int GetBlockSize()
	{
		return blockSize;
	}
	
	int GetFragments()
    {
		return fragments;
    }
	
	int GetFragSizeBase()
	{
		return fragSizeBase;
	}
	
	const char* GetErrorString()
	{
		return errorString;
	}
			
 private:  // ------------------------------------------------------- private

    static const char AUDIODEVICE[];
	int audioHd;
	
	char* errorString;

	int blockSize;
	int fragments;
	int fragSizeBase;
	
	udword frequency;
	
	// These are constants/enums from ``libsidplay/include/emucfg.h''.
	int encoding;
	int precision;
	int channels;
    int channel; 
    bool swapEndian;
};


#endif  // AUDIODRV_H
