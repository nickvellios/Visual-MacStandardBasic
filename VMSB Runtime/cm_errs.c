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
#include        <Quickdraw.h>
#include        <QDOffscreen.h>

#define         UCHAR   unsigned char
#define            kInFront    (WindowPtr) -1    /* put the new window in front */

void StrToStr255(char *st1, Str255 st2);

void CloseItAllUp();

void QT_CloseDown();

short SPR_SpriteEnd();

unsigned char *GetErrMsg();

void ErrHandler(short ErrNum, Str255 AddMsg);

short LastErrorNumber = 0, ErrorSub = 0, ErrorCommand = 0;
extern Handle codeHandle, hdrHandle, tablesHandle;
unsigned char *ErrMsgs[] =
        {
                "\p",
                "\p1001 - Memory allocation error",
                "\p1002 - Buffer not allocated",
                "\p1003 - Buffer element out of range",
                "\p1004 - Parameter out of range",
                "\p1005 - Value out of range",
                "\p1006 - Invalid parameter specified",
                "\p1007 - Array element out of range",
                "\p1008 - Window could not be created",
                "\p1009 - Window not found",
                "\p1010 - Control could not be created",
                "\p1011 - Control not found",
                "\p1012 - File could not be created",
                "\p1013 - File not found",
                "\p1014 - File is locked",
                "\p1015 - Invalid File",
                "\p1016 - Movie not found",
                "\p1017 - Menu error",
                "\p1018 - Sound resource was not found #",
                "\p1019 - Picture resource was not found #",
                "\p1020 - Icon resource was not found #",
                "\p1021 -  resource was not found",
                "\p1022 - Serial port error",
                "\p1023 - Loop nesting  error",
                "\p1024 - For Next nesting error",
                "\p1025 - Argument syntax error",
                "\p1026 - Serial port not open",
                "\p1027 - Invalid Serial port #",
                "\p1028 - Passing value to sub or function",
                "\p1029 - Internal call stack error",
                "\p1030 - Internal Execution Pointer in invalid location",
                "\p1031 - Return string allocation pointer error",
                "\p1032 - Maximum of 500 menu items at once",
                "\p1033 - Maximum of 50 menus at once",
                "\p1034 - QuickTime movie driver not loaded",
                "\p1035 - Maximum of 10 concurrent movies",
                "\p1036 - Serial port I/O error",
                "\p1037 - Sound Manager not loaded",
                "\p1038 - Maximum of 100 concurrent sounds can be loaded",
                "\p1039 - Sound play error",
                "\p1040 - Maximum of 500 concurrent image buffers can be loaded",
                "\p1041 - Sprite image was not found",
                "\p1042 - Internal error - parameter mismatch",
                "\p1043 - PictureBegin must be before PictureDone",
                "\p1044 - Expecting a PictureDone command",
                "\p1045 - There is no picture is to save",
                "\p1046 - Maximum of 500 control items at once",
                "\p1047 - Maximum of 100 picture resources at once",
                "\p1048 - String resource was not found",
                "\p1049 - Empty Palette specified",
                "\p1050 - Invalid Date specified",
                "\p1051 - Array element out of range, less than 1",
        };

void ErrHandler(short ErrNum, Str255 AddMsg) {
    short a, ln;
    DialogPtr md;
    UCHAR *uc, errtemp[256];
    Str255 err255;
    LastErrorNumber = ErrNum;
    // Route to Sub, if previously told to
    if (ErrorSub) {
        ErrorCommand = ErrorSub;
        return;
    }
    // No, okay run Alert MessageBox
    uc = ErrMsgs[ErrNum - 1000];
    ln = uc[0];
    uc[ln + 1] = 0;
    ln = AddMsg[0];
    AddMsg[ln + 1] = 0;
    if (AddMsg == nil) sprintf((char *) errtemp, "Error # %s", uc + 1);
    else sprintf((char *) errtemp, "Error # %s%s", uc + 1, AddMsg + 1);
    StrToStr255((char *) errtemp, err255);
    ParamText(err255, nil, nil, nil);
    SysBeep(30);
    md = GetNewDialog(600, 0, (WindowPtr) - 1);
    do {
        ModalDialog(0, &a);
    } while (a != 1);
    DisposeDialog(md);
    CloseItAllUp();
    QT_CloseDown();
    SPR_SpriteEnd();
    HUnlock(codeHandle);
    HUnlock(tablesHandle);
    DisposeHandle(codeHandle);
    DisposeHandle(tablesHandle);
    ExitToShell();
}

unsigned char *GetErrMsg() {
    if (!LastErrorNumber) return (ErrMsgs[0]);
    return (ErrMsgs[LastErrorNumber - 1000]);
}
