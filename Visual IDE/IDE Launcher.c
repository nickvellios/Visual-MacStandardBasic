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
/* LaunchWithDoc */
#include <Dialogs.h>
#include <QuickDraw.h>
#include <Windows.h>
#include <Menus.h>
#include <Fonts.h>
#include <appleevents.h>
#include <processes.h>
#include <files.h>
#include <StandardFile.h>
#include <Aliases.h>

extern FSSpec PjtFS;

short ResLaunch(unsigned char *rfile);

short ResLaunch(unsigned char *rfile) {
    Boolean tf, ta;
    FSSpec launchApp, fileToLaunch;   /* file spec for the app (launchApp) and the file (fileToLaunch) */
    OSErr myErr;
    LaunchParamBlockRec launchThis;
    AEDesc myAddress;
    ProcessSerialNumber myPSN;
    AEDesc docDesc, launchDesc;
    AEDescList theList;
    AliasHandle withThis;
    //StandardFileReply myReply;
    AppleEvent theEvent;
    InitGraf((Ptr) & qd.thePort);  /* standard setup stuff */
    InitFonts();
    InitWindows();
    InitMenus();
    TEInit();
    InitDialogs(nil);
    InitCursor();
    /* get the app to launch, using the Sys7 versions of Standard File */
    /* which return FSSpecs */
    /* Preset our address descriptor to our PSN */
    /* The Finder is going to change this to whatever is appropriate  */
    /* for the application that eventually gets launched, so I'll */
    /* just fill it with my own address. */
    /* I'm doing this because I have heard sporadic reports that  */
    /* _not_ doing this causes address errors sometimes, I've never had that happen. */
    /* But since it has, you should prefill. */
    GetCurrentProcess(&myPSN);
    /* create the address desc for the event */
    myErr = AECreateDesc(typeProcessSerialNumber, (Ptr) & myPSN, sizeof(ProcessSerialNumber), &myAddress);
    /* get the application to launch */
    //StandardGetFile(nil, -1, nil, &myReply);
    //if (!myReply.sfGood)
    //   return;
    //launchApp = myReply.sfFile;
    /* stuff it in my launch parameter block */
    /* get the file to pass */
    // StandardGetFile(nil, -1, nil, &myReply);
    //if (!myReply.sfGood)
    //    return;
    /* the caller may have already done this, but it doesn't hurt to do it again */
    // fileToLaunch = myReply.sfFile;
    myErr = FSMakeFSSpec(0, 0, "\p:Tools:ResEdit", &launchApp);
    if (myErr) myErr = FSMakeFSSpec(0, 0, "\p:Tools:ResEdit Alias", &launchApp);
    if (myErr) myErr = FSMakeFSSpec(0, 0, "\p:Tools:Resourcerer", &launchApp);
    if (myErr) myErr = FSMakeFSSpec(0, 0, "\p:Tools:Resourcerer Alias", &launchApp);
    if (myErr) {
        Alert(132, nil);
        return (0);
    }
    ResolveAliasFile(&launchApp, true, &tf, &ta);
    launchThis.launchAppSpec = &launchApp;
    myErr = FSMakeFSSpec(PjtFS.vRefNum, PjtFS.parID, rfile, &fileToLaunch);
    if (myErr) {
        Alert(134, nil);
        return (0);
    }
    /* create an appleevent to carry it */
    AECreateAppleEvent(kCoreEventClass, kAEOpenDocuments, &myAddress, kAutoGenerateReturnID, kAnyTransactionID,
                       &theEvent);
    /* create a list for the alaises.  In this case, I only have one, but you still need */
    /* a list */
    AECreateList(nil, 0, false, &theList);
    /* create an alias out of the file spec */
    /* I'm not real sure why I did this, since there is a system coercion handler for */
    /* alias to FSSpec, but I'm paranoid */
    NewAlias(nil, &fileToLaunch, &withThis);
    HLock((Handle) withThis);
    /* now create an alias descriptor */
    AECreateDesc(typeAlias, (Ptr) * withThis, GetHandleSize((Handle) withThis), &docDesc);
    HUnlock((Handle) withThis);
    /* put it in the list */
    AEPutDesc(&theList, 0, &docDesc);
    AEPutParamDesc(&theEvent, keyDirectObject, &theList);
    /* coerce the event from being an event into being appParms */
    /* **** If you just want to send an 'odoc' to an application that is */
    /* already running, you can stop here and do an AESend on theEvent */
    AECoerceDesc(&theEvent, typeAppParameters, &launchDesc);
    HLock((Handle) theEvent.dataHandle);
    /* and stuff it in the parameter block */
    /* This is a little weird, since we're actually moving the event out of the */
    /* AppParameters descriptor.  But it's necessary, the coercison to typeAppParameters */
    /* stuffs the whole appleevent into one AERecord (instead of a AEDesc) so  */
    /* the Finder gets the whole event as one handle.  It can then parse it itself */
    /* to do the sending */
    launchThis.launchAppParameters = (AppParametersPtr) * (launchDesc.dataHandle);
    /* launch the thing */
    launchThis.launchBlockID = extendedBlock;
    launchThis.launchEPBLength = extendedBlockLen;
    launchThis.launchFileFlags = nil;
    launchThis.launchControlFlags = launchContinue + launchNoFileFlags;
    LaunchApplication(&launchThis);
    /* and launch it */
    return (0);
}
