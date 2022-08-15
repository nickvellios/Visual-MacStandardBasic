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
#include        <Movies.h>

#define         UCHAR   unsigned char
#define            kInFront    (WindowPtr) -1    /* put the new window in front */
extern short PrintFlag;
struct MovieStruct {
    Movie theMovie;
    char status;
    FSSpec fileSp;
    MovieController theController;
} MovieData[10];
extern struct WinStruct {
    WindowPtr theWindow;
    Rect winRect;    // This is total window size
    UCHAR status;
    short CharX, CharY;
    short GraphX, GraphY;
    short subcode;
    short wintype;
    Handle drawCmds;
    char update;
    short opt1;
    short FormID;
    RGBColor BackRGB, ForeRGB;
    Rect clipRect;
    Rect MinMax;
} WinData[100];
extern short MSB_QTAvail;

void ErrHandler(short ErrNum, Str255 AddMsg);

short QT_Ver();

short QT_Init();

void QT_CloseDown();

short QT_OpenMovie(short WinNum, Str255 MovieFilename, short CtlFlag);

short QT_PlayMovie(short MovieNum);

short QT_ResetMovie(short MovieNum);

short QT_CloseMovie(short MovieNum);

short QT_OffsetMovie(short MovieNum, short x, short y);

short QT_Init() {
    OSErr theError;
    long theResult, m;
    short cc;
    // This sub does the init stuff for QT
    MSB_QTAvail = 0;  //Let's 1st set it to 0
    for (cc = 0; cc < 10; cc++) {
        MovieData[cc].status = 0;
        MovieData[cc].theMovie = nil;
        MovieData[cc].theController = nil;
    }
    theError = Gestalt(gestaltQuickTime, &theResult);
    if (theError || !theResult) return (-1);
    theError = EnterMovies();
    if (theError) return (-1);
    // Change version # from hex to decimal
    m = theResult >> 16;
    MSB_QTAvail = (short) m & 7;
    m >>= 4;
    MSB_QTAvail += (short) (m & 7) * 10;
    m >>= 4;
    MSB_QTAvail += (short) (m & 7) * 100;
    return (0);
}

short QT_Ver() {
    return (MSB_QTAvail);
}

void QT_CloseDown() {
    short a;
    for (a = 0; a < 10; a++) QT_CloseMovie(a);
}

short QT_OpenMovie(short WinNum, Str255 MovieFilename, short CtlFlag) {
    OSErr theError;
    short refNum, ResID = 0, MovieNum = -1, a;
    Str255 MovieName;
    Boolean wasAltered;
    Rect mb;
    if (!MSB_QTAvail) {
        ErrHandler(1034, nil);
        return (-99);
    }
    if (!WinData[WinNum].status) {
        ErrHandler(1009, nil);
        return (-1);
    }
    for (a = 0; a < 10; a++)
        if (!MovieData[a].status) {
            MovieNum = a;
            break;
        }
    if (MovieNum == -1) {
        ErrHandler(1035, nil);
        return (-2);
    }
    theError = FSMakeFSSpec(0, 0, MovieFilename, &MovieData[MovieNum].fileSp);
    if (theError) {
        ErrHandler(1016, nil);
        return (-2);
    }
    theError = OpenMovieFile(&MovieData[MovieNum].fileSp, &refNum, fsRdPerm);
    if (theError) {
        ErrHandler(1016, nil);
        return (-2);
    }
    theError = NewMovieFromFile(&MovieData[MovieNum].theMovie, refNum, &ResID, MovieName, newMovieActive, &wasAltered);
    CloseMovieFile(refNum);    // Got info so I can close it
    SetMovieGWorld(MovieData[MovieNum].theMovie, (CGrafPtr) WinData[WinNum].theWindow, nil);
    GetMovieBox(MovieData[MovieNum].theMovie, &mb);
    MovieData[MovieNum].theController = nil;
    if (CtlFlag & 1) {
        // Ok attach Movie Controller to movie
        MovieData[MovieNum].theController = NewMovieController(MovieData[MovieNum].theMovie, &mb, mcTopLeftMovie);
        MCGetControllerBoundsRect(MovieData[MovieNum].theController, &mb);
    } else {
        MovieData[MovieNum].theController = nil;
        OffsetRect(&mb, -mb.left, -mb.top);
        SetMovieBox(MovieData[MovieNum].theMovie, &mb);
    }
    if (CtlFlag & 2) {
        // Resize window
        SizeWindow(WinData[WinNum].theWindow, mb.right, mb.bottom, true);
    }
    MovieData[MovieNum].status = 1;
    return (MovieNum);
}

short QT_PlayMovie(short MovieNum) {
    Movie theMovie;
    if (!MovieData[MovieNum].status) {
        ErrHandler(1016, nil);
        return (-1);
    }
    theMovie = MovieData[MovieNum].theMovie;
    StartMovie(theMovie);
    return (0);
}

short QT_ResetMovie(short MovieNum) {
    if (!MovieData[MovieNum].status) {
        ErrHandler(1016, nil);
        return (-1);
    }
    GoToBeginningOfMovie(MovieData[MovieNum].theMovie);
    return (0);
}

short QT_OffsetMovie(short MovieNum, short x, short y) {
    Rect mb;
    if (!MovieData[MovieNum].status) {
        ErrHandler(1016, nil);
        return (-1);
    }
    GetMovieBox(MovieData[MovieNum].theMovie, &mb);
    OffsetRect(&mb, x, y);
    SetMovieBox(MovieData[MovieNum].theMovie, &mb);
    if (MovieData[MovieNum].theController != nil) {
        DisposeMovieController(MovieData[MovieNum].theController);
        // Ok attach Movie Controller to movie
        MovieData[MovieNum].theController = NewMovieController(MovieData[MovieNum].theMovie, &mb, mcTopLeftMovie);
        MCGetControllerBoundsRect(MovieData[MovieNum].theController, &mb);
    }
    return (0);
}

short QT_CloseMovie(short MovieNum) {
    if (!MovieData[MovieNum].status) return (-1);
    if (MovieData[MovieNum].theController != nil)
        DisposeMovieController(MovieData[MovieNum].theController);
    DisposeMovie(MovieData[MovieNum].theMovie);
    MovieData[MovieNum].status = 0;
    return (0);
}
