//
// SIDPLAY (simple console front end)
// Copyright (C) Michael Schwendt.
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include <ctype.h>
#include <iomanip.h>
#include <fstream.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspaudio.h>
#include <pspaudiolib.h>

#include <sidplay/player.h>
#include <sidplay/fformat.h>
#include <sidplay/myendian.h>
#include "audiodrv.h"

#define printf  pspDebugScreenPrintf
#define EXIT_ERROR_STATUS (-1)


PSP_MODULE_INFO ("SIDPLAY", 0, 1, 1);

//PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

/* Exit callback */
int
exitCallback (int arg1, int arg2, void *common)
{
	sceKernelExitGame ();
	return 0;
}

/* Callback thread */
int
callbackThread (SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback ("Exit Callback", exitCallback, NULL);
	sceKernelRegisterExitCallback (cbid);
	sceKernelSleepThreadCB ();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int
setupCallbacks (void)
{
	int thid = 0;

	thid = sceKernelCreateThread ("update_thread", callbackThread, 0x11,
				      0xFA0, THREAD_ATTR_USER, 0);
	if (thid >= 0) {
		sceKernelStartThread (thid, 0, 0);
	}
	return thid;
}



char infile[] = "host0:/Chordian.sid";
emuEngine myEmuEngine;
sidTune myTune (infile);
audioDriver myAudio;


void
audioCallback (void *buf, unsigned int len, void *userdata)
{
	//fprintf(stderr,"audioCallback len=%i\n",len);
	sidTune *tune = (sidTune *) userdata;
	sidEmuFillBuffer (myEmuEngine, (*tune), buf, len);
}

int
audio_thread (SceSize args, void *argp)
{
	fprintf (stderr, "audio_thread: start\n");
	int bufSize = myAudio.GetBlockSize ();
	ubyte *buffer;
	buffer = (ubyte *) malloc (bufSize);
	while (1) {
		sidEmuFillBuffer (myEmuEngine, myTune, buffer, bufSize);
		myAudio.Play (buffer, bufSize);
	}
	return 0;
}


int
main (int argc, char *argv[])
{

	SceCtrlData pad;
	pspDebugScreenInit ();
	setupCallbacks ();
	sceCtrlSetSamplingCycle (0);
	sceCtrlSetSamplingMode (PSP_CTRL_MODE_ANALOG);

	// ======================================================================
	// INITIALIZE THE EMULATOR ENGINE
	// ======================================================================
	// emuEngine myEmuEngine;
	if (!myEmuEngine.verifyEndianess ()) {
		fprintf (stderr,
			 "Hardware endianess improperly configured during compilation.");;
		exit (EXIT_ERROR_STATUS);
	}
	struct emuConfig myEmuConfig;
	myEmuEngine.getConfig (myEmuConfig);

	printf ("SIDPLAY   Music player and C64 SID chip emulator\n");

	myEmuConfig.frequency = 44100;
	myEmuConfig.channels = 2;
	myEmuConfig.bitsPerSample = SIDEMU_16BIT;
	myEmuConfig.sampleFormat = SIDEMU_SIGNED_PCM;
	myEmuConfig.sidChips = 1;
	myEmuConfig.clockSpeed = SIDTUNE_CLOCK_NTSC;
	myEmuConfig.memoryMode = MPU_BANK_SWITCHING;
	myEmuConfig.emulateFilter = true;
	myEmuConfig.filterFs = SIDEMU_DEFAULTFILTERFS;
	myEmuConfig.filterFm = SIDEMU_DEFAULTFILTERFM;
	myEmuConfig.filterFt = SIDEMU_DEFAULTFILTERFT;
	myEmuConfig.mos8580 = false;
	myEmuConfig.measuredVolume = true;
	myEmuConfig.autoPanning = SIDEMU_NONE;
	myEmuConfig.forceSongSpeed = true;
	myEmuConfig.digiPlayerScans = 500;


	uword selectedSong = 0;
	uword fragments = 32;
	uword fragSizeBase = 12;

	//sidTune myTune( infile );
	struct sidTuneInfo mySidInfo;
	myTune.getInfo (mySidInfo);
	if (!myTune) {
		fprintf (stderr, "ERROR: %s\n", mySidInfo.statusString);
		exit (EXIT_ERROR_STATUS);
	}
	else {
		printf ("File format  : %s\n", mySidInfo.formatString);
		printf ("Filenames    : ");
		if (mySidInfo.dataFileName)
			printf ("%s", mySidInfo.dataFileName);
		else
			printf ("(None)");
		printf (", ");
		if (mySidInfo.infoFileName)
			printf ("%s", mySidInfo.infoFileName);
		else
			printf ("(None)");
		printf ("\n");
		printf ("Condition    : %s\n", mySidInfo.statusString);
		printf ("--------------------------------------------------\n");
		if (mySidInfo.numberOfInfoStrings == 3) {
			printf ("Name         : %s\n", mySidInfo.nameString);
			printf ("Author       : %s\n",
				mySidInfo.authorString);
			printf ("Copyright    : %s\n",
				mySidInfo.copyrightString);
		}
		else {
			for (int infoi = 0;
			     infoi < mySidInfo.numberOfInfoStrings; infoi++)
				printf ("Description  : %s\n",
					mySidInfo.infoString[infoi]);
		}
		printf ("--------------------------------------------------\n");
		printf ("Load address : $%04x\n", mySidInfo.loadAddr);
		printf ("Init address : $%04x\n", mySidInfo.initAddr);
		printf ("Play address : $%04x\n", mySidInfo.playAddr);
	}

	// ======================================================================
	// CONFIGURE THE AUDIO DRIVER
	// ======================================================================

	// Instantiate the audio driver. The capabilities of the audio driver
	// can override the settings of the SID emulator.
	// audioDriver myAudio;
	// Open() does not accept the "bitsize" value on all platforms, e.g.
	// Sparcstations 5 and 10 tend to be 16-bit only at rates above 8000 Hz.
	if (!(myAudio.Open (myEmuConfig.frequency, myEmuConfig.bitsPerSample,
			    myEmuConfig.channels, fragments, fragSizeBase))) {
		fprintf (stderr, "ERROR: %s\n", myAudio.GetErrorString ());
		exit (EXIT_ERROR_STATUS);
	}

	// ======================================================================
	// CONFIGURE THE EMULATOR ENGINE
	// ======================================================================

	// Configure the SID emulator according to the audio driver settings.
	/*
	   myEmuConfig.frequency = myAudio.GetFrequency();
	   myEmuConfig.bitsPerSample = myAudio.GetSamplePrecision();
	   myEmuConfig.sampleFormat = myAudio.GetSampleEncoding();
	 */
	myEmuEngine.setConfig (myEmuConfig);

	// ======================================================================
	// INITIALIZE THE EMULATOR ENGINE TO PREPARE PLAYING A SIDTUNE
	// ======================================================================

	if (!sidEmuInitializeSong (myEmuEngine, myTune, selectedSong)) {
		fprintf (stderr,
			 "ERROR: SID Emulator Engine components not ready.\n");
		exit (EXIT_ERROR_STATUS);
	}
	// Read out the current settings of the sidtune.
	myTune.getInfo (mySidInfo);
	if (!myTune) {
		fprintf (stderr, "ERROR: %s\n", mySidInfo.statusString);
		exit (EXIT_ERROR_STATUS);
	}
	printf ("Setting song : %i out of %i (default = %i)\n",
		mySidInfo.currentSong, mySidInfo.songs, mySidInfo.startSong);
	printf ("Song speed   : %s\n", mySidInfo.speedString);

	// ======================================================================
	// KEEP UP A CONTINUOUS OUTPUT SAMPLE STREAM
	// ======================================================================

	/*
	   int bufSize = myAudio.GetBlockSize();

	   ubyte* buffer;
	   buffer = (ubyte*)malloc(bufSize);

	   while(1){
	   sceCtrlReadBufferPositive(&pad, 1);
	   if(pad.Buttons & PSP_CTRL_CROSS) {
	   break;
	   }
	   sidEmuFillBuffer(myEmuEngine,myTune,buffer,bufSize);
	   myAudio.Play(buffer,bufSize);
	   }
	 */

	SceUID thid;
	thid = sceKernelCreateThread ("audio_thread", audio_thread, 0x18,
				      0x1000, PSP_THREAD_ATTR_USER, NULL);
	if (thid < 0) {
		fprintf (stderr, "Error, could not create thread\n");
		sceKernelSleepThread ();
	}
	sceKernelStartThread (thid, 0, NULL);

	while (1) {
		sceCtrlReadBufferPositive (&pad, 1);
		if (pad.Buttons & PSP_CTRL_CROSS) {
			break;
		}
		sceKernelDelayThread (100);
	}

	myAudio.Reset ();
	myAudio.Close ();
	sceKernelExitGame ();
	fprintf (stderr, "done..\n");
	return 0;
}
