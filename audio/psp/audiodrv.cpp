// --------------------------------------------------------------------------
// ``Open Sound System (OSS)'' specific audio driver interface.
// --------------------------------------------------------------------------

#include "audiodrv.h"
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspaudiolib.h>
#include <pspaudio.h>
#include <pspdisplay.h>
#include <pspctrl.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>


audioDriver::audioDriver()
{
	// Reset everything.
	errorString = "None";
	frequency = 0;
	channels = 0;
	encoding = 0;
	precision = 0;
    swapEndian = false;
	audioHd = (-1);
    blockSize = PSP_NUM_SAMPLES * 4;
}

bool audioDriver::IsThere()
{
	return 1;
}

bool audioDriver::Open(udword inFreq, int inPrecision, int inChannels,
					   int inFragments, int inFragBase)
{
	frequency = inFreq;
	precision = inPrecision;
	channels = inChannels;
	fragments = inFragments;
	fragSizeBase = inFragBase;
    //pspAudioInit();
    channel = sceAudioChReserve(PSP_AUDIO_NEXT_CHANNEL,PSP_NUM_SAMPLES,PSP_AUDIO_FORMAT_STEREO);

    return true;
}

// Close an opened audio device, free any allocated buffers and
// reset any variables that reflect the current state.
void audioDriver::Close()
{
    sceAudioChRelease(channel);
}

void audioDriver::Play(ubyte* pBuffer, int bufferSize)
{
      sceAudioOutputPannedBlocking(channel, PSP_AUDIO_VOLUME_MAX, PSP_AUDIO_VOLUME_MAX, pBuffer);
}
