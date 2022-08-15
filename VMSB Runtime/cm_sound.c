//
//  Visual MacStandardBasic
//
//  Copyright 1999 Nick Vellios. All rights reserved.
//
//  This code is licensed under the "Take a kid fishing or hunting license"
//  See website or email below for details.
//
//  nick@vellios.com
//  http://www.Vellios.com
//
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <math.h>
#include        <Speech.h>
#include        <Sound.h>

#define         UCHAR   unsigned char
#define            kInFront    (WindowPtr) -1    /* put the new window in front */
extern char MSB_SpeechAvail;
extern short MSB_SndMgrVer;

void InitSoundMgr();

short SPK_Ver();

short SPK_Busy();

short SND_Say(Str255 txt);

void ErrHandler(short ErrNum, Str255 AddMsg);

short SND_Say(Str255 txt) {
    if (MSB_SpeechAvail) SpeakString(txt);
    return (0);
}

short SPK_Ver() {
    return ((short) MSB_SpeechAvail);
}

short SPK_Busy() {
    if (MSB_SpeechAvail) return ((short) SpeechBusy());
    else return (0);
}

pascal void SoundChannelCallback(SndChannelPtr, SndCommand);

OSErr InstallCallbackCommand(SndChannelPtr);

void CleanUpSoundIfFinished(void);

void PlaySoundResourceAsynch(short);

void AnimateWhileSoundPlays(void);

void InitializeToolbox(void);

void OpenDisplayWindow(void);

SndChannelPtr OpenOneAsynchSoundChannel(void);

OSErr DisposeOneSoundChannel(SndChannelPtr);

void SoundCleanUp();

SndChannelPtr MSB_SoundChannel[100];
Handle MSB_SoundHandle[100];
char MSB_SoundStatus[100];
char SoundDisposal = 0;

short SND_IsPlaying(short sndNum);

short SND_Load(short theResID);

void SND_Unload(short sndNum);

short SND_Play(short sndNum);

short SND_Ver();

void InitSoundMgr() {
    short a;
    NumVersion nm;
    for (a = 0; a < 100; a++) {
        MSB_SoundHandle[a] = nil;
        MSB_SoundStatus[a] = 0;
        MSB_SoundChannel[a] = nil;
    }
    nm = SndSoundManagerVersion();
    MSB_SndMgrVer = nm.majorRev * 100;
    return;
}

//____________________________________________________________
short SND_Ver() {
    return (MSB_SndMgrVer);
}

// Load sound from resource to memory
short SND_Load(short theResID) {
    short a, b = -1;
    if (!MSB_SndMgrVer) {
        ErrHandler(1037, nil);
        return (-1);
    }
    for (a = 0; a < 100; a++)
        if (MSB_SoundHandle[a] == nil) {
            b = a;
            break;
        }
    if (b == -1) {
        ErrHandler(1038, nil);
        return (-1);
    }
    MSB_SoundHandle[b] = GetResource('snd ', theResID);
    if (MSB_SoundHandle[b] == nil) return (-1);
    DetachResource(MSB_SoundHandle[b]);
    HLock(MSB_SoundHandle[b]);
    return (b);
}

void SND_Unload(short sndNum) {
    if (MSB_SoundHandle[sndNum] == nil) return;
    HUnlock(MSB_SoundHandle[sndNum]);
    ReleaseResource(MSB_SoundHandle[sndNum]);
    MSB_SoundHandle[sndNum] = nil;
    return;
}

short SND_Play(short sndNum) {
    short a, b = -1;
    OSErr theError;
    SndCallBackUPP theCallBackUPP;
    if (!MSB_SndMgrVer) {
        ErrHandler(1037, nil);
        return (-1);
    }
    for (a = 0; a < 100; a++)
        if (MSB_SoundChannel[a] == nil) {
            b = a;
            break;
        }
    if (b == -1) {
        ErrHandler(1038, nil);
        return (-1);
    }
    theCallBackUPP = NewSndCallBackProc(SoundChannelCallback);
    theError = SndNewChannel(&MSB_SoundChannel[b], 0, 0, theCallBackUPP);
    if (MSB_SoundChannel[b] == nil) {
        ErrHandler(1018, nil);
        return (-1);
    }
    MSB_SoundStatus[b] = 1;
    theError = SndPlay(MSB_SoundChannel[b], (SndListHandle) MSB_SoundHandle[sndNum], true);
    if (theError == noErr) theError = InstallCallbackCommand(MSB_SoundChannel[b]);
    else ErrHandler(1039, nil);
    return (b);
}

short SND_IsPlaying(short sndNum) {
    return (MSB_SoundStatus[sndNum]);
}

//____________________________________________________________
OSErr InstallCallbackCommand(SndChannelPtr theChannel) {
    OSErr theError;
    SndCommand theCommand;
    theCommand.cmd = callBackCmd;
    theCommand.param1 = 0;
#ifndef powerc
    theCommand.param2 = SetCurrentA5();
#else
    theCommand.param2 = 0;
#endif
    theError = SndDoCommand(theChannel, &theCommand, true);
    return (theError);
}

//____________________________________________________________
pascal void SoundChannelCallback(SndChannelPtr theChannel, SndCommand theCommand) {
    short a;
    long theA5;
#ifndef powerc
    theA5 = SetA5(theCommand.param2);
#endif
    for (a = 0; a < 100; a++) {
        if (MSB_SoundChannel[a] == theChannel) {
            MSB_SoundStatus[a] = 2;
            SoundDisposal = 1;
            break;
        }
    }
#ifndef powerc
    theA5 = SetA5(theA5);
#endif
}

//____________________________________________________________
OSErr DisposeOneSoundChannel(SndChannelPtr theChannel) {
    OSErr theError;
    theError = SndDisposeChannel(theChannel, true);
    DisposePtr((Ptr) theChannel);
    return (theError);
}

//____________________________________________________________
void SoundCleanUp() {
    short a;
    for (a = 0; a < 100; a++) {
        if (MSB_SoundStatus[a] == 2) {
            DisposeOneSoundChannel(MSB_SoundChannel[a]);
            MSB_SoundChannel[a] = nil;
            MSB_SoundStatus[a] = 0;
        }
    }
    SoundDisposal = 0;
}
