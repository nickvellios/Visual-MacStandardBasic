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

#define         UCHAR   unsigned char
extern TEHandle ActiveTE;
extern short PrintFlag;
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
extern struct CtlStruct {
    ControlHandle theControl;
    TEHandle te;
    ListHandle lt;            // Thats a lowercase L in lt
    PicHandle pic;
    Rect theRect;
    short WinNum;
    UCHAR status;
    short subcode;
    MenuHandle theMenu;
    short lastItem;
    short CtlID;
    short opt1, opt2;
    RGBColor BackRGB, ForeRGB;
    short enabled;
} CtlData[500];

short WhichControl();

void TextFix(short a);

short FindCtlID(short id);

short FindFormID(short id);

void StoreInt(short varnum, long VarOffset, short Value);

void StoreLong(short varnum, long VarOffset, long Value);

void StoreSingle(short varnum, long VarOffset, float Value);

void StoreDouble(short varnum, long VarOffset, short double Value);

void StoreStr(short varnum, unsigned char *, long VarOffset, long);

short pass_assign(short vartype, short varnum);

float get_singlearg_val();

short double get_doublearg_val();

short FreeReturnStr();

void RStrToStr255(UCHAR *st1, long slen, Str255 st2);

long ArrayCalc(short varnum);

long grab_intvar(short varnum);

long grab_longvar(short varnum);

short cmd_drawtext();

short cmd_drawfont();

short cmd_drawsize();

short cmd_drawstyle();

short cmd_moveto();

short cmd_point();

short cmd_line();

short cmd_lineto();

short cmd_box();

short cmd_circle();

short cmd_ellipse();

short cmd_boxfill();

short cmd_circlefill();

short cmd_ellipsefill();

short cmd_pensize();

short cmd_penpattern();

short cmd_penmode();

short cmd_pennormal();

short cmd_setcolor();

short cmd_backcolor();

short cmd_setRGBcolor();

short cmd_backRGBcolor();

short cmd_stretchline();

short cmd_stretchbox();

short cmd_stretchoval();

short cmd_stretchcircle();

short cmd_drawpicture();

short cmd_spriteframe();

short cmd_spritedraw();

short cmd_spritecopy();

short cmd_spritedrawsize();

short cmd_spritecopysize();

short cmd_spritebegin();

short cmd_spritedone();

short cmd_spriteend();

short cmd_spriterelease();

short cmd_spriteclear();

short cmd_spritedisplay();

short cmd_spriteload();

short cmd_spritedup();

short cmd_sndload();

short cmd_sndplay();

short cmd_sndunload();

short cmd_delay();

short cmd_geturl();

// From cm_spr.c
short SPR_SpriteFrame(short WinNum);

short SPR_SpriteLoad(short ResID);

short SPR_SpriteDrawCopy(short imageNum, short x, short y, short priority, short tmode);

short SPR_SpriteDrawCopySize(short imageNum, short x, short y, short width, short height, short priority, short tmode);

short SPR_SpriteBegin(short FrameID);

short SPR_SpriteDone();

short SPR_SpriteEnd();

short SPR_SpriteDisplay(short frameNum);

short SPR_SpriteClear(short imageNum);

short SPR_SpriteRelease(short imageNum);

short SPR_SpriteDup(short image1, short image2);

// From cm_gfx.c
short GFX_Moveto(short WinNum, short x1, short y1);

short GFX_Point(short WinNum, short x1, short y1);

short GFX_Line(short WinNum, short x1, short y1, short x2, short y2);

short GFX_Lineto(short WinNum, short x1, short y1);

short GFX_Box(short WinNum, short x1, short y1, short width, short ht, short fill);

short GFX_Circle(short WinNum, short x1, short y1, short theradius, short fill);

short GFX_Ellipse(short WinNum, short x1, short y1, short width, short ht, short fill);

short GFX_Update(short WinNum, short update);

short GFX_SetColor(short WinNum, short color, short bgFlag);

short GFX_SetRGBColor(short WinNum, long red, long green, long blue, short bgFlag);

short GFX_DrawText(short WinNum, short x1, short y1, UCHAR *txt1);

short GFX_DrawFont(short WinNum, short x1);

short GFX_DrawSize(short WinNum, short x1);

short GFX_DrawStyle(short WinNum, short x1);

short GFX_PenSize(short WinNum, short penWidth, short penHt);

short GFX_PenPattern(short WinNum, short pat);

short GFX_PenMode(short WinNum, short penMode);

short GFX_PenNormal(short WinNum);

short GFX_Picture(short WinNum, short x1, short y1, short w, short ht, short ResID);

// From cm_sound.c
short SND_Load(short theResID);

void SND_Unload(short sndNum);

short SND_Play(short sndNum);

// From cm_net.c
short NET_GetURL(char *url, Str255 filename, short pbox);

long GetLabelLoc(short labelnum);

long get_longarg_val();

long grab_longvar(short varnum);

unsigned char *get_stringarg_val();

void ErrHandler(short ErrNum, Str255 AddMsg);

extern short LastErrorNumber, ErrorSub;
extern short *GIP, GIP2, GIP3, GIP4, GIP5, JS_Ctr, LineLen, ITemp;
extern short VR_Int, *VR_IntPtr;
extern long VR_Long, *VR_LongPtr, DimLong, VR_Long2;
extern float VR_Single, *VR_SinglePtr;
extern short double VR_Double, *VR_DoublePtr;
extern UCHAR *VR_StrPtr;
extern UCHAR *CodeBuf;
extern long CodePos, CodeLen, NLP;
extern short current_module_num, CallStackPtr, RunModCtr, LoopStackPtr, NextFlag;
extern UCHAR *ReturnStr, ReturnStrFlag, EventsFlag;
extern long ReturnStrLen;
extern char MSB_SpeechAvail;
extern short MSB_QTAvail;
extern THPrint PrintHandle;
extern TPPrPort PrinterPort;
extern char PrintActive;

#include        "Structs1.h"

extern struct CallStackStruct {
    long location;
    short module;
    unsigned char *varbuf;
} CallStack[500];
extern struct RunVarStruct // This Data Structure holds runtime info 3.0
{
    struct CallTableStruct1 *CallTable1;
    struct CallTableStruct2 *CallTable2;
    struct VarTableStruct1 *VarTable1;
    struct VarTableStruct2 *VarTable2;
    long *LabelTable;
    unsigned char *StrTable;
    unsigned char *CodeTable;
    unsigned char *GlobalVarSpace;
    struct SelectTableStruct *SelectTable;
    struct ObjectPropsStruct2 *PropTable;
} RunVar;
extern struct RunHdrStruct // This Data Structure holds runtime info
{
    long NumOfVars;
    long NumOfCalls;
    long NumOfLabels;
    long StrLen;
    long CodeLen;
    long CodeStart;        // Where to start
    long GlbVarSz;
    long StartVarSz;
    long CallTableOffset;    // Don't need VarTable Offset because it = 0
    long LabelTableOffset;
    long StrTableOffset;
    long SelectTableOffset;
    long NumOfSelects;
    long NumOfProps;        // New for 3.0
    long PropTableOffset;    // New for 3.0
} RunHdr;
short openPictureFlag = 0;
PicHandle openPictureHandle = nil;
WindowPtr openPictureWindow = nil;

short cmd_picturebegin();

short cmd_picturedone();

short cmd_picturesave();

short FILE_SavePictFile(Str255 filename);

short cmd_picturebegin() {
    short b = 0, c, d, *e;
    GrafPtr old;
    Rect r;
    short a, v[4];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in PictureBegin command");
            return 0;
        }
    }
    for (a = b; a < 2; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = get_longarg_val();
                break;
            case 250:
                v[a] = (long) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in PictureBegin command");
                break;
        }
    }
    if (openPictureFlag) {
        ErrHandler(1044, nil);
        return (0);
    }
    if (openPictureHandle != nil) KillPicture(openPictureHandle);
    SetRect(&r, 0, 0, v[0], v[1]);
    GetPort(&old);
    if (openPictureWindow != nil) DisposeWindow(openPictureWindow);
    openPictureWindow = NewCWindow(nil, &r, "\p", false, 2, nil, false, 0L);
    SetPort(openPictureWindow);
    openPictureHandle = OpenPicture(&r);
    openPictureFlag = 1;
    ClipRect(&r);
    return (0);
}

short cmd_picturedone() {
    if (!openPictureFlag) {
        ErrHandler(1043, nil);
        return (0);
    }
    SetPort(openPictureWindow);
    ClosePicture();
    openPictureFlag = 0;
    return (0);
}

short cmd_picturesave() {
    Str255 txt1;
    if (CodeBuf[CodePos] == 249) {
        CodePos++;
        get_stringarg_val(); // ignore using it for now
    }
    RStrToStr255(ReturnStr, ReturnStrLen, txt1);
    FreeReturnStr();
    if (openPictureFlag) {
        ErrHandler(1044, nil);
        return (0);
    }
    if (openPictureHandle == nil) {
        ErrHandler(1045, nil);
        return (0);
    }
    FILE_SavePictFile(txt1);
    return (0);
}

short cmd_drawtext() {
    short a, b = 0, c, d, *e;
    short v[4];
    Str255 txt1;
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in DrawText command");
            return 0;
        }
    }
    for (a = b; a < 3; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = get_longarg_val();
                break;
            case 250:
                v[a] = (long) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in DrawText command");
                break;
        }
    }
    // Now get title string
    if (CodeBuf[CodePos] == 249) {
        CodePos++;
        get_stringarg_val(); // ignore using it for now
    }
    RStrToStr255(ReturnStr, ReturnStrLen, txt1);
    FreeReturnStr();
    a = GFX_DrawText(v[0], v[1], v[2], txt1);
    return (0);
}

short cmd_drawfont() {
    short a, b = 0, c, d, *e;
    long v[3];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in DrawFont command");
            return 0;
        }
    }
    for (a = b; a < 2; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = get_longarg_val();
                break;
            case 250:
                v[a] = (long) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in DrawFont command");
                break;
        }
    }
    a = GFX_DrawFont(v[0], v[1]);
    return (0);
}

short cmd_drawsize() {
    short a, b = 0, c, d, *e;
    long v[3];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in DrawSize command");
            return 0;
        }
    }
    for (a = b; a < 2; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = get_longarg_val();
                break;
            case 250:
                v[a] = (long) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in DrawSize command");
                break;
        }
    }
    a = GFX_DrawSize(v[0], v[1]);
    return (0);
}

short cmd_drawstyle() {
    short a, b = 0, c, d, *e;
    long v[3];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in DrawStyle command");
            return 0;
        }
    }
    for (a = b; a < 2; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = get_longarg_val();
                break;
            case 250:
                v[a] = (long) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in DrawStyle command");
                break;
        }
    }
    a = GFX_DrawStyle(v[0], v[1]);
    return (0);
}

short cmd_moveto() {
    short a, b = 0, c, d, *e;
    long v[3];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in MoveTo command");
            return 0;
        }
    }
    for (a = b; a < 3; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = get_longarg_val();
                break;
            case 250:
                v[a] = (long) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in MoveTo command");
                break;
        }
    }
    a = GFX_Moveto(v[0], v[1], v[2]);
    return (0);
}

short cmd_lineto() {
    short a, b = 0, c, d, *e;
    long v[3];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in LineTo command");
            return 0;
        }
    }
    for (a = b; a < 3; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = get_longarg_val();
                break;
            case 250:
                v[a] = (long) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in LineTo command");
                break;
        }
    }
    a = GFX_Lineto(v[0], v[1], v[2]);
    return (0);
}

short cmd_point() {
    short a, b = 0, c, d, *e;
    long v[3];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in Point command");
            return 0;
        }
    }
    for (a = b; a < 3; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = get_longarg_val();
                break;
            case 250:
                v[a] = (long) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in Point command");
                break;
        }
    }
    a = GFX_Point(v[0], v[1], v[2]);
    return (0);
}

short cmd_line() {
    short a, b = 0, c, d, *e;
    long v[5];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in Line command");
            return 0;
        }
    }
    for (a = b; a < 5; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = get_longarg_val();
                break;
            case 250:
                v[a] = (long) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in Line command");
                break;
        }
    }
    a = GFX_Line(v[0], v[1], v[2], v[3], v[4]);
    return (0);
}

short cmd_box() {
    short a, b = 0, c, d, *e;
    long v[5];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in Box command");
            return 0;
        }
    }
    for (a = b; a < 5; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = get_longarg_val();
                break;
            case 250:
                v[a] = (long) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in Box command");
                break;
        }
    }
    a = GFX_Box(v[0], v[1], v[2], v[3], v[4], 0);
    return (0);
}

short cmd_boxfill() {
    short a, b = 0, c, d, *e;
    long v[7];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in BoxFill command");
            return 0;
        }
    }
    for (a = b; a < 5; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = get_longarg_val();
                break;
            case 250:
                v[a] = (long) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in BoxFill command");
                break;
        }
    }
    a = GFX_Box(v[0], v[1], v[2], v[3], v[4], 1);
    return (0);
}

short cmd_circle() {
    short a, b = 0, c, d, *e;
    long v[5];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in Circle command");
            return 0;
        }
    }
    for (a = b; a < 4; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = get_longarg_val();
                break;
            case 250:
                v[a] = (long) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in Circle command");
                break;
        }
    }
    a = GFX_Circle(v[0], v[1], v[2], v[3], 0);
    return (0);
}

short cmd_circlefill() {
    short a, b = 0, c, d, *e;
    long v[5];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in CircleFill command");
            return 0;
        }
    }
    for (a = b; a < 4; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = get_longarg_val();
                break;
            case 250:
                v[a] = (long) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in CircleFill command");
                break;
        }
    }
    a = GFX_Circle(v[0], v[1], v[2], v[3], 1);
    return (0);
}

short cmd_ellipse() {
    short a, b = 0, c, d, *e;
    long v[5];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in Ellipse command");
            return 0;
        }
    }
    for (a = b; a < 5; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = get_longarg_val();
                break;
            case 250:
                v[a] = (long) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in Ellipse command");
                break;
        }
    }
    a = GFX_Ellipse(v[0], v[1], v[2], v[3], v[4], 0);
    return (0);
}

short cmd_ellipsefill() {
    short a, b = 0, c, d, *e;
    long v[7];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in EllipseFill command");
            return 0;
        }
    }
    for (a = b; a < 5; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = get_longarg_val();
                break;
            case 250:
                v[a] = (long) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in EllipseFill command");
                break;
        }
    }
    a = GFX_Ellipse(v[0], v[1], v[2], v[3], v[4], 1);
    return (0);
}

short cmd_drawpicture() {
    short a, b = 0, c, d, *e;
    long v[6];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in DrawPicture command");
            return 0;
        }
    }
    for (a = b; a < 6; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = get_longarg_val();
                break;
            case 250:
                v[a] = (long) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in DrawPicture command");
                break;
        }
    }
    a = GFX_Picture(v[0], v[1], v[2], v[3], v[4], v[5]);
    return (0);
}

short cmd_setcolor() {
    short a, b = 0, c, d, *e;
    long v[3];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in SetColor command");
            return 0;
        }
    }
    for (a = b; a < 2; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = get_longarg_val();
                break;
            case 250:
                v[a] = (long) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in SetColor command");
                break;
        }
    }
    a = GFX_SetColor(v[0], v[1], 0);
    return (0);
}

short cmd_backcolor() {
    short a, b = 0, c, d, *e;
    long v[3];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in BackColor command");
            return 0;
        }
    }
    for (a = b; a < 2; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = get_longarg_val();
                break;
            case 250:
                v[a] = (long) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in BackColor command");
                break;
        }
    }
    a = GFX_SetColor(v[0], v[1], 1);
    return (0);
}

short cmd_backRGBcolor() {
    short a, b = 0, c, d, *e;
    long v[6];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in BackColorRGB command");
            return 0;
        }
    }
    for (a = b; a < 4; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = get_longarg_val();
                break;
            case 250:
                v[a] = (long) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in BackColorRGB command");
                break;
        }
    }
    a = GFX_SetRGBColor(v[0], v[1], v[2], v[3], 1);
    return (0);
}

short cmd_setRGBcolor() {
    short a, b = 0, c, d, *e;
    long v[6];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in SetColorRGB command");
            return 0;
        }
    }
    for (a = b; a < 4; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = get_longarg_val();
                break;
            case 250:
                v[a] = (long) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in SetColorRGB command");
                break;
        }
    }
    a = GFX_SetRGBColor(v[0], v[1], v[2], v[3], 0);
    return (0);
}

short cmd_pensize() {
    short a, b = 0, c, d, *e;
    short v[4];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in PenSize command");
            return 0;
        }
    }
    for (a = b; a < 3; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = (short) get_longarg_val();
                break;
            case 250:
                v[a] = (short) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in PenSize command");
                break;
        }
    }
    a = GFX_PenSize(v[0], v[1], v[2]);
    return (0);
}

short cmd_penpattern() {
    short a, b = 0, c, d, *e;
    short v[4];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in PenPattern command");
            return 0;
        }
    }
    for (a = b; a < 2; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = (short) get_longarg_val();
                break;
            case 250:
                v[a] = (short) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in PenPattern command");
                break;
        }
    }
    a = GFX_PenPattern(v[0], v[1]);
    return (0);
}

short cmd_penmode() {
    short a, b = 0, c, d, *e;
    short v[4];
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in PenMode command");
            return 0;
        }
    }
    for (a = b; a < 2; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = (short) get_longarg_val();
                break;
            case 250:
                v[a] = (short) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in PenMode command");
                break;
        }
    }
    a = GFX_PenMode(v[0], v[1]);
    return (0);
}

short cmd_pennormal() {
    short a, b = 0, c, d, *e;
    short v;
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v = FindFormID(d);
        if (v == -1) {
            ErrHandler(1009, "\p in PenNormal command");
            return 0;
        }
    } else {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v = (short) get_longarg_val();
                break;
            case 250:
                v = (short) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in PenNormal command");
                break;
        }
    }
    a = GFX_PenNormal(v);
    return (0);
}

short cmd_stretchline() {
    short varnum1, varnum2, it1, it2, wn, x1, y1, x2 = 0, y2 = 0;
    long v[4], Voffset1 = 0, Voffset2 = 0;
    PenState ps;
    Point pt;
    short a, b = 0, c, d, *e;
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in StretchLine command");
            return 0;
        }
    }
    for (a = b; a < 3; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = (short) get_longarg_val();
                break;
            case 250:
                v[a] = (short) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in StretchLine command");
                break;
        }
    }
    // GIP is the Var num
    GIP = (short *) (CodeBuf + CodePos);
    varnum1 = *GIP;
    CodePos += 2;
    if (RunVar.VarTable2[varnum1].dims) Voffset1 = ArrayCalc(varnum1);
    it1 = RunVar.VarTable2[varnum1].vartype;
    // GIP is the Var num
    GIP = (short *) (CodeBuf + CodePos);
    varnum2 = *GIP;
    CodePos += 2;
    if (RunVar.VarTable2[varnum2].dims) Voffset2 = ArrayCalc(varnum2);
    it2 = RunVar.VarTable2[varnum2].vartype;
    wn = v[0];
    if (WinData[wn].status) {
        // Now draw stretch line
        SetPort(WinData[wn].theWindow);
        GetPenState(&ps);
        PenNormal();
        PenMode(patXor);
        x1 = v[1];
        y1 = v[2];
        x2 = x1;
        y2 = y1;
        MoveTo(x1, y1);
        LineTo(x2, y2);
        while (StillDown()) {
            GetMouse(&pt);
            if (pt.h != x2 || pt.v != y2) {
                MoveTo(x1, y1);
                LineTo(x2, y2);
                x2 = pt.h;
                y2 = pt.v;
                MoveTo(x1, y1);
                LineTo(x2, y2);
            }
        }
        MoveTo(x1, y1);
        LineTo(x2, y2);
        SetPenState(&ps);
    }
    switch (it1) {
        case 1:
            StoreInt(varnum1, Voffset1, (short) x2);
            break;
        case 2:
            StoreLong(varnum1, Voffset1, (long) x2);
            break;
        case 3:
            StoreSingle(varnum1, Voffset1, (float) x2);
            break;
        case 4:
            StoreDouble(varnum1, Voffset1, (short double) x2);
            break;
    }
    switch (it2) {
        case 1:
            StoreInt(varnum2, Voffset2, (short) y2);
            break;
        case 2:
            StoreLong(varnum2, Voffset2, (long) y2);
            break;
        case 3:
            StoreSingle(varnum2, Voffset2, (float) y2);
            break;
        case 4:
            StoreDouble(varnum2, Voffset2, (short double) y2);
            break;
    }
    return (0);
}

short cmd_stretchbox() {
    short a, varnum1, varnum2, it1, it2, wn, x1, y1, x2 = 0, y2 = 0;
    long v[4], Voffset1 = 0, Voffset2 = 0;
    PenState ps;
    Point pt;
    Rect theRect;
    short b = 0, c, d, *e;
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in StretchBox command");
            return 0;
        }
    }
    for (a = b; a < 3; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = (short) get_longarg_val();
                break;
            case 250:
                v[a] = (short) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in StretchBox command");
                break;
        }
    }
    // GIP is the Var num
    GIP = (short *) (CodeBuf + CodePos);
    varnum1 = *GIP;
    CodePos += 2;
    if (RunVar.VarTable2[varnum1].dims) Voffset1 = ArrayCalc(varnum1);
    it1 = RunVar.VarTable2[varnum1].vartype;
    // GIP is the Var num
    GIP = (short *) (CodeBuf + CodePos);
    varnum2 = *GIP;
    CodePos += 2;
    if (RunVar.VarTable2[varnum2].dims) Voffset2 = ArrayCalc(varnum2);
    it2 = RunVar.VarTable2[varnum2].vartype;
    wn = v[0];
    if (WinData[wn].status) {
        // Now draw stretch box
        SetPort(WinData[wn].theWindow);
        GetPenState(&ps);
        PenNormal();
        PenMode(patXor);
        x1 = v[1];
        y1 = v[2];
        x2 = x1;
        y2 = y1;
        SetRect(&theRect, x1, y1, x2, y2);
        FrameRect(&theRect);
        while (StillDown()) {
            GetMouse(&pt);
            if (pt.h != x2 || pt.v != y2) {
                SetRect(&theRect, x1, y1, x2, y2);
                FrameRect(&theRect);
                x2 = pt.h;
                y2 = pt.v;
                SetRect(&theRect, x1, y1, x2, y2);
                FrameRect(&theRect);
            }
        }
        SetRect(&theRect, x1, y1, x2, y2);
        FrameRect(&theRect);
        SetPenState(&ps);
    }
    switch (it1) {
        case 1:
            StoreInt(varnum1, Voffset1, (short) x2 - x1);
            break;
        case 2:
            StoreLong(varnum1, Voffset1, (long) x2 - x1);
            break;
        case 3:
            StoreSingle(varnum1, Voffset1, (float) x2 - x1);
            break;
        case 4:
            StoreDouble(varnum1, Voffset1, (short double) x2 - x1);
            break;
    }
    switch (it2) {
        case 1:
            StoreInt(varnum2, Voffset2, (short) y2 - y1);
            break;
        case 2:
            StoreLong(varnum2, Voffset2, (long) y2 - y1);
            break;
        case 3:
            StoreSingle(varnum2, Voffset2, (float) y2 - y1);
            break;
        case 4:
            StoreDouble(varnum2, Voffset2, (short double) y2 - y1);
            break;
    }
    return (0);
}

short cmd_stretchoval() {
    short a, varnum1, varnum2, it1, it2, wn, x1, y1, x2 = 0, y2 = 0;
    long v[4], Voffset1 = 0, Voffset2 = 0;
    PenState ps;
    Point pt;
    Rect theRect;
    short b = 0, c, d, *e;
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in StretchEllipse command");
            return 0;
        }
    }
    for (a = b; a < 3; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = (short) get_longarg_val();
                break;
            case 250:
                v[a] = (short) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in StretchEllipse command");
                break;
        }
    }
    // GIP is the Var num
    GIP = (short *) (CodeBuf + CodePos);
    varnum1 = *GIP;
    CodePos += 2;
    if (RunVar.VarTable2[varnum1].dims) Voffset1 = ArrayCalc(varnum1);
    it1 = RunVar.VarTable2[varnum1].vartype;
    // GIP is the Var num
    GIP = (short *) (CodeBuf + CodePos);
    varnum2 = *GIP;
    CodePos += 2;
    if (RunVar.VarTable2[varnum2].dims) Voffset2 = ArrayCalc(varnum2);
    it2 = RunVar.VarTable2[varnum2].vartype;
    wn = v[0];
    if (WinData[wn].status) {
        // Now draw stretch oval
        SetPort(WinData[wn].theWindow);
        GetPenState(&ps);
        PenNormal();
        PenMode(patXor);
        x1 = v[1];
        y1 = v[2];
        x2 = x1;
        y2 = y1;
        SetRect(&theRect, x1, y1, x2, y2);
        FrameOval(&theRect);
        while (StillDown()) {
            GetMouse(&pt);
            if (pt.h != x2 || pt.v != y2) {
                SetRect(&theRect, x1, y1, x2, y2);
                FrameOval(&theRect);
                x2 = pt.h;
                y2 = pt.v;
                SetRect(&theRect, x1, y1, x2, y2);
                FrameOval(&theRect);
            }
        }
        SetRect(&theRect, x1, y1, x2, y2);
        FrameOval(&theRect);
        SetPenState(&ps);
    }
    switch (it1) {
        case 1:
            StoreInt(varnum1, Voffset1, (short) x2 - x1);
            break;
        case 2:
            StoreLong(varnum1, Voffset1, (long) x2 - x1);
            break;
        case 3:
            StoreSingle(varnum1, Voffset1, (float) x2 - x1);
            break;
        case 4:
            StoreDouble(varnum1, Voffset1, (short double) x2 - x1);
            break;
    }
    switch (it2) {
        case 1:
            StoreInt(varnum2, Voffset2, (short) y2 - y1);
            break;
        case 2:
            StoreLong(varnum2, Voffset2, (long) y2 - y1);
            break;
        case 3:
            StoreSingle(varnum2, Voffset2, (float) y2 - y1);
            break;
        case 4:
            StoreDouble(varnum2, Voffset2, (short double) y2 - y1);
            break;
    }
    return (0);
}

short cmd_stretchcircle() {
    short a, varnum1, it1, wn, x, y, rad, lasth;
    long v[4], Voffset1 = 0;
    PenState ps;
    Point pt;
    Rect theRect;
    short b = 0, c, d, *e;
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v[0] = FindFormID(d);
        if (v[0] == -1) {
            ErrHandler(1009, "\p in StretchCircle command");
            return 0;
        }
    }
    for (a = b; a < 3; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = (short) get_longarg_val();
                break;
            case 250:
                v[a] = (short) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in StretchCircle command");
                break;
        }
    }
    // GIP is the Var num
    GIP = (short *) (CodeBuf + CodePos);
    varnum1 = *GIP;
    CodePos += 2;
    if (RunVar.VarTable2[varnum1].dims) Voffset1 = ArrayCalc(varnum1);
    it1 = RunVar.VarTable2[varnum1].vartype;
    wn = v[0];
    if (WinData[wn].status) {
        // Now draw stretch oval
        SetPort(WinData[wn].theWindow);
        GetPenState(&ps);
        PenNormal();
        PenMode(patXor);
        rad = 0;
        x = v[1];
        y = v[2];
        lasth = x;
        SetRect(&theRect, x, y, x, y);
        FrameOval(&theRect);
        while (StillDown()) {
            GetMouse(&pt);
            if (pt.h != lasth) {
                SetRect(&theRect, x - rad, y - rad, x + rad, y + rad);
                FrameOval(&theRect);
                lasth = pt.h;
                rad = pt.h - x;
                SetRect(&theRect, x - rad, y - rad, x + rad, y + rad);
                FrameOval(&theRect);
            }
        }
        SetRect(&theRect, x - rad, y - rad, x + rad, y + rad);
        FrameOval(&theRect);
        SetPenState(&ps);
    }
    switch (it1) {
        case 1:
            StoreInt(varnum1, Voffset1, (short) rad);
            break;
        case 2:
            StoreLong(varnum1, Voffset1, (long) rad);
            break;
        case 3:
            StoreSingle(varnum1, Voffset1, (float) rad);
            break;
        case 4:
            StoreDouble(varnum1, Voffset1, (short double) rad);
            break;
    }
    return (0);
}

// ################################# Sprite Commands ####################################
short cmd_spriteframe() {
    short varnum;
    long v, Voffset = 0;
    short a, b = 0, c, d, *e;
    // GIP is the Var num
    GIP = (short *) (CodeBuf + CodePos);
    varnum = *GIP;
    CodePos += 2;
    if (RunVar.VarTable2[varnum].dims) Voffset = ArrayCalc(varnum);
    c = CodeBuf[CodePos++];
    if (c == 2) {
        b = 1; // skip first param & find #
        e = (short *) (CodeBuf + CodePos);
        d = *e;
        CodePos += 2;
        v = FindFormID(d);
        if (v == -1) {
            ErrHandler(1009, "\p in SpriteFrame command");
            return 0;
        }
    } else {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v = (short) get_longarg_val();
                break;
            case 250:
                v = (short) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in SpriteFrame command");
                break;
        }
    }
    a = SPR_SpriteFrame(v);
    ITemp = RunVar.VarTable2[varnum].vartype;
    switch (ITemp) {
        case 1:
            VR_Int = a;
            StoreInt(varnum, Voffset, VR_Int);
            break;
        case 2:
            VR_Long = (long) a;
            StoreLong(varnum, Voffset, VR_Long);
            break;
        case 3:
            VR_Single = (float) a;
            StoreSingle(varnum, Voffset, VR_Single);
            break;
        case 4:
            VR_Double = (short double) a;
            StoreDouble(varnum, Voffset, VR_Double);
            break;
    }
    return (0);
}

short cmd_spriteload() {
    short a, varnum;
    long v, Voffset = 0;
    // GIP is the Var num
    GIP = (short *) (CodeBuf + CodePos);
    varnum = *GIP;
    CodePos += 2;
    if (RunVar.VarTable2[varnum].dims) Voffset = ArrayCalc(varnum);
    switch (CodeBuf[CodePos++]) {
        case 251:
            v = (short) get_longarg_val();
            break;
        case 250:
            v = (short) get_doublearg_val();
            break;
        default:
            ErrHandler(1042, "\p in SpriteLoad command");
            break;
    }
    a = SPR_SpriteLoad(v);
    ITemp = RunVar.VarTable2[varnum].vartype;
    switch (ITemp) {
        case 1:
            VR_Int = a;
            StoreInt(varnum, Voffset, VR_Int);
            break;
        case 2:
            VR_Long = (long) a;
            StoreLong(varnum, Voffset, VR_Long);
            break;
        case 3:
            VR_Single = (float) a;
            StoreSingle(varnum, Voffset, VR_Single);
            break;
        case 4:
            VR_Double = (short double) a;
            StoreDouble(varnum, Voffset, VR_Double);
            break;
    }
    return (0);
}

short cmd_spritedup() {
    short a;
    short v[3];
    for (a = 0; a < 2; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = (short) get_longarg_val();
                break;
            case 250:
                v[a] = (short) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in SpriteDup command");
                break;
        }
    }
    a = SPR_SpriteDup(v[0], v[1]);
    return (0);
}

short cmd_spritedraw() {
    short a;
    short v[5];
    for (a = 0; a < 4; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = (short) get_longarg_val();
                break;
            case 250:
                v[a] = (short) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in SpriteDraw command");
                break;
        }
    }
    a = SPR_SpriteDrawCopy(v[0], v[1], v[2], v[3], transparent);
    return (0);
}

short cmd_spritecopy() {
    short a;
    short v[5];
    for (a = 0; a < 4; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = (short) get_longarg_val();
                break;
            case 250:
                v[a] = (short) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in SpriteCopy command");
                break;
        }
    }
    a = SPR_SpriteDrawCopy(v[0], v[1], v[2], v[3], srcCopy);
    return (0);
}

short cmd_spritedrawsize() {
    short a;
    short v[8];
    for (a = 0; a < 6; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = (short) get_longarg_val();
                break;
            case 250:
                v[a] = (short) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in SpriteDrawSize command");
                break;
        }
    }
    a = SPR_SpriteDrawCopySize(v[0], v[1], v[2], v[3], v[4], v[5], transparent);
    return (0);
}

short cmd_spritecopysize() {
    short a;
    short v[8];
    for (a = 0; a < 6; a++) {
        switch (CodeBuf[CodePos++]) {
            case 251:
                v[a] = (short) get_longarg_val();
                break;
            case 250:
                v[a] = (short) get_doublearg_val();
                break;
            default:
                ErrHandler(1042, "\p in SpriteCopySize command");
                break;
        }
    }
    a = SPR_SpriteDrawCopySize(v[0], v[1], v[2], v[3], v[4], v[5], srcCopy);
    return (0);
}

short cmd_spritebegin() {
    short a;
    long v;
    switch (CodeBuf[CodePos++]) {
        case 251:
            v = (short) get_longarg_val();
            break;
        case 250:
            v = (short) get_doublearg_val();
            break;
        default:
            ErrHandler(1042, "\p in SpriteBegin command");
            break;
    }
    a = SPR_SpriteBegin(v);
    return (0);
}

short cmd_spritedone() {
    short a;
    a = SPR_SpriteDone();
    return (0);
}

short cmd_spritedisplay() {
    short a;
    long v;
    switch (CodeBuf[CodePos++]) {
        case 251:
            v = (short) get_longarg_val();
            break;
        case 250:
            v = (short) get_doublearg_val();
            break;
        default:
            ErrHandler(1042, "\p in SpriteDisplay command");
            break;
    }
    a = SPR_SpriteDisplay(v);
    return (0);
}

short cmd_spriteend() {
    short a;
    a = SPR_SpriteEnd();
    return (0);
}

short cmd_spriterelease() {
    short a;
    long v;
    switch (CodeBuf[CodePos++]) {
        case 251:
            v = (short) get_longarg_val();
            break;
        case 250:
            v = (short) get_doublearg_val();
            break;
        default:
            ErrHandler(1042, "\p in SpriteUnload command");
            break;
    }
    a = SPR_SpriteRelease(v);
    return (0);
}

short cmd_spriteclear() {
    short a;
    long v;
    switch (CodeBuf[CodePos++]) {
        case 251:
            v = (short) get_longarg_val();
            break;
        case 250:
            v = (short) get_doublearg_val();
            break;
        default:
            ErrHandler(1042, "\p in SpriteClear command");
            break;
    }
    a = SPR_SpriteClear(v);
    return (0);
}

short cmd_sndload() {
    short a, varnum;
    long v, Voffset = 0;
    // GIP is the Var num
    GIP = (short *) (CodeBuf + CodePos);
    varnum = *GIP;
    CodePos += 2;
    if (RunVar.VarTable2[varnum].dims) Voffset = ArrayCalc(varnum);
    switch (CodeBuf[CodePos++]) {
        case 251:
            v = (short) get_longarg_val();
            break;
        case 250:
            v = (short) get_doublearg_val();
            break;
        default:
            ErrHandler(1042, "\p in SndLoad command");
            break;
    }
    a = SND_Load(v);
    ITemp = RunVar.VarTable2[varnum].vartype;
    switch (ITemp) {
        case 1:
            VR_Int = a;
            StoreInt(varnum, Voffset, VR_Int);
            break;
        case 2:
            VR_Long = (long) a;
            StoreLong(varnum, Voffset, VR_Long);
            break;
        case 3:
            VR_Single = (float) a;
            StoreSingle(varnum, Voffset, VR_Single);
            break;
        case 4:
            VR_Double = (short double) a;
            StoreDouble(varnum, Voffset, VR_Double);
            break;
    }
    return (0);
}

short cmd_sndplay() {
    short a;
    long v;
    switch (CodeBuf[CodePos++]) {
        case 251:
            v = (short) get_longarg_val();
            break;
        case 250:
            v = (short) get_doublearg_val();
            break;
        default:
            ErrHandler(1042, "\p in SndPlay command");
            break;
    }
    a = SND_Play(v);
    return (0);
}

short cmd_sndunload() {
    long v;
    switch (CodeBuf[CodePos++]) {
        case 251:
            v = (short) get_longarg_val();
            break;
        case 250:
            v = (short) get_doublearg_val();
            break;
        default:
            ErrHandler(1042, "\p in SndUnload command");
            break;
    }
    SND_Unload(v);
    return (0);
}

short cmd_delay() {
    long v;
    unsigned long m;
    switch (CodeBuf[CodePos++]) {
        case 251:
            v = (short) get_longarg_val();
            break;
        case 250:
            v = (short) get_doublearg_val();
            break;
        default:
            ErrHandler(1042, "\p in Delay command");
            break;
    }
    Delay(v, &m);
    return (0);
}
