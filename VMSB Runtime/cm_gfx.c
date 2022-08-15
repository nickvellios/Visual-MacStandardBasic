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
//#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <math.h>
#include        <Icons.h>
#include        <Palettes.h>
#include        <QDOffscreen.h>

#define         UCHAR   unsigned char
#define            kInFront    (WindowPtr) -1    /* put the new window in front */

short CTL_ClearControls(short WinNum);

short GFX_GetWinContentSize(short WinNum, Rect *myRect);

short GFX_WinRedraw(short WinNum);

short GFX_CreateWin(short x1, short y1, short x2, short y2, short WinType, Str255, short);

short GFX_CloseWindow(short WinNum);

short GFX_HideWindow(short WinNum);

short GFX_ShowWindow(short WinNum);

short GFX_SizeWindow(short WinNum, short x, short y);

short GFX_MoveWindow(short WinNum, short x, short y);

short GFX_SetWindowTitle(short WinNum, Str255 wtitle);

short GFX_Windowinfo(short WinNum, short parm);

short GFX_Screeninfo(short parm);

short GFX_MsgBox(UCHAR *message, short buttonTypes);

short GFX_ClipWindow(short WinNum, short x, short y, short wide, short ht);

short GFX_WindowOrigin(short WinNum, short x, short y);

short GetWinStatus(short a);

short GFX_GetWinTitle(short a);

short GFX_GetFrontWindow();

short GFX_Moveto(short WinNum, short x1, short y1);

short GFX_Lineto(short WinNum, short x1, short y1);

short GFX_Point(short WinNum, short x1, short y1);

short GFX_Line(short WinNum, short x1, short y1, short x2, short y2);

short GFX_Box(short WinNum, short x1, short y1, short width, short ht, short fill);

short GFX_Circle(short WinNum, short x1, short y1, short theradius, short fill);

short GFX_Ellipse(short WinNum, short x1, short y1, short width, short ht, short fill);

short GFX_Update(short WinNum, short update);

void GFX_AddGfxCommand(short WinNum, short cmd, short p1, short p2, short p3, short p4, short p5, unsigned char *txt);

void DrawGraphics(short WinNum);

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

void GFX_ClearGfx(short WinNum, short flag);

short SetSpritePort(short imagenum);

void GFX_DrawUpdate(short WinNum, short flag);

short GFX_LoadPal(short palNum, short resID, short ent, short opts);

short GFX_WinPal(short WinNum, short palNum);

void ClearPalettes();

short GFX_Picture(short WinNum, short x1, short y1, short w, short ht, short ResID);

short double GFX_GetPixel(short WinNum, short x1, short y1);

short double GFX_RGB(long red, long green, long blue);

extern RGBColor GlobalBackRGB, GlobalForeRGB;
char InlineUpdateFlag = 0;
extern short openPictureFlag;
extern PicHandle openPictureHandle;
extern WindowPtr openPictureWindow;
extern PicHandle Picts[100];
extern UCHAR *ReturnStr, ReturnStrFlag;
extern long ReturnStrLen;

void ErrHandler(short ErrNum, Str255 AddMsg);

long GFX_WinHandle(short v);

short FreeReturnStr();

short Allocate_ReturnStr(long);

struct PalStruct {
    CTabHandle cth;
    PaletteHandle ph;
    short status;
} WinPal[10];
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
PenState GfxWinPS[100];
extern struct SpriteStruct {
    char imageType;
    GWorldPtr theGWPtr;
    PixMapHandle thePixMap;
    Rect theRect;
} SpriteImages[500];
extern THPrint PrintHandle;
extern TPPrPort PrinterPort;
extern char PrintActive;

short GFX_GetWinContentSize(short WinNum, Rect *myRect) {
    Rect theRect;
    WindowPtr WinPtr;
    if (!WinData[WinNum].status) return (0);
    WinPtr = WinData[WinNum].theWindow;
    theRect = WinPtr->portRect;
    *myRect = theRect;
    return (1);
}

short GFX_WinRedraw(short WinNum) {
    Rect theRect;
    WindowPtr WinPtr;
    if (!WinData[WinNum].status) {
        ErrHandler(1009, "\p in WinRedraw command");
        return (-1);
    }
    SetPort(WinData[WinNum].theWindow);
    WinPtr = WinData[WinNum].theWindow;
    theRect = WinPtr->portRect;
    InvalRect(&theRect);
    return (1);
}

long GFX_WinHandle(short v) {
    long result;
    if (!WinData[v].status) return -1;
    result = (long) WinData[v].theWindow;
    return result;
}

short GFX_CreateWin(short x1, short y1, short wide, short ht, short WinType, Str255 wt, short cmd) {
    short a, b, x, y, w, h, Cbox = false, ViewFlag = true, CenterFlag = false, wc = 0;
    Rect tempRect;
    long www;
    // Find Open Window Slot
    b = -1;
    for (a = 0; a < 100; a++)
        if (!WinData[a].status) {
            b = a;
            break;
        }
    if (b == -1) return (-1);    //Couldn't find one
    if (WinType >= 8000) {
        WinType -= 8000;
        wc = 1;
    }
    if (WinType >= 4000) {
        WinType -= 4000;
        CenterFlag = true;
    }
    if (WinType >= 2000) {
        WinType -= 2000;
        ViewFlag = false;
    }
    if (WinType >= 1000) {
        WinType -= 1000;
        Cbox = true;
    }
    if (!CenterFlag) SetRect(&tempRect, x1, y1, x1 + wide, y1 + ht);
    else {    // Center WIndow on screen
        w = GFX_Screeninfo(3) - wide;
        if (w < 0) w = 0;
        h = GFX_Screeninfo(4) - ht;
        if (h < 0) h = 0;
        x = w / 2;
        y = h / 2;
        SetRect(&tempRect, x, y, x + wide, y + ht);
    }
    WinData[b].theWindow = NewCWindow(nil, &tempRect, wt, false, WinType, kInFront, Cbox, 0);
    www = (long) WinData[b].theWindow;
    WinData[b].BackRGB = GlobalBackRGB;
    WinData[b].ForeRGB = GlobalForeRGB;
    WinData[b].status = 1;
    WinData[b].wintype = WinType;
    WinData[b].winRect = tempRect;
    WinData[b].drawCmds = nil;
    WinData[b].update = 1;
    WinData[b].opt1 = wc;
    WinData[b].FormID = -1;        // new 3.0 for forms only - create form sub will chg it
    SetRect(&WinData[b].MinMax, 50, 50, 10000, 10000);
    // Reset cursor locations for text and graphics
    WinData[b].CharX = 0;
    WinData[b].CharY = 0;
    WinData[b].GraphX = 0;
    WinData[b].GraphY = 0;
    WinData[b].subcode = cmd;
    WinData[b].clipRect = WinData[b].theWindow->portRect;
    //Draw some grid points
    SetPort(WinData[b].theWindow);
    RGBBackColor(&GlobalBackRGB);
    RGBForeColor(&GlobalForeRGB);
    EraseRect(&WinData[b].theWindow->portRect);
    if (!WinType || WinType == 8) {
        DrawGrowIcon(WinData[b].theWindow);
        if (!wc) {
            tempRect.right -= 15;
            tempRect.bottom -= 15;
        }
    }
    if (ViewFlag) ShowWindow(WinData[b].theWindow);
    return (b);
}

short GFX_CloseWindow(short WinNum) {
    if (!WinData[WinNum].status) return (-1);
    if (WinData[WinNum].drawCmds != nil) {
        DisposeHandle(WinData[WinNum].drawCmds);
        WinData[WinNum].drawCmds = nil;
    }
    CTL_ClearControls(WinNum);
    WinData[WinNum].status = 0;
    DisposeWindow(WinData[WinNum].theWindow);
    WinData[WinNum].theWindow = nil;
    return (0);
}

short GFX_HideWindow(short WinNum) {
    if (!WinData[WinNum].status) {
        ErrHandler(1009, "\p in WinHide command");
        return (-1);
    }
    HideWindow(WinData[WinNum].theWindow);
    return (0);
}

short GFX_ShowWindow(short WinNum) {
    if (!WinData[WinNum].status) {
        ErrHandler(1009, "\p in WinShow command");
        return (-1);
    }
    SelectWindow(WinData[WinNum].theWindow);
    ShowWindow(WinData[WinNum].theWindow);
    return (0);
}

short GFX_SizeWindow(short WinNum, short x, short y) {
    if (!WinData[WinNum].status) {
        ErrHandler(1009, "\p in WinSize command");
        return (-1);
    }
    WinData[WinNum].winRect.right = WinData[WinNum].winRect.left + x;
    WinData[WinNum].winRect.bottom = WinData[WinNum].winRect.top + y;
    SizeWindow(WinData[WinNum].theWindow, x, y, true);
    WinData[WinNum].clipRect = WinData[WinNum].theWindow->portRect;
    return (0);
}

short GFX_MoveWindow(short WinNum, short x, short y) {
    if (!WinData[WinNum].status) {
        ErrHandler(1009, "\p in WinMove command");
        return (-1);
    }
    OffsetRect(&WinData[WinNum].winRect, -WinData[WinNum].winRect.left, -WinData[WinNum].winRect.top);
    OffsetRect(&WinData[WinNum].winRect, x, y);
    MoveWindow(WinData[WinNum].theWindow, x, y, false);
    return (0);
}

short GFX_WindowOrigin(short WinNum, short x, short y) {
    GrafPtr gp;
    if (!WinData[WinNum].status) {
        ErrHandler(1009, "\p in WinOrigin command");
        return (-1);
    }
    GetPort(&gp);
    SetPort(WinData[WinNum].theWindow);
    SetOrigin(x, y);
    SetPort(gp);
    return (0);
}

void ClearPalettes() {
    short a;
    for (a = 0; a < 10; a++) {
        WinPal[a].status = 0;
        WinPal[a].cth = nil;
        WinPal[a].ph = nil;
    }
}

short GFX_LoadPal(short palNum, short resID, short ent, short opts) {
    palNum--;
    if (palNum < 0 || palNum > 9) {
        ErrHandler(1006, "\p in LoadPalette command");
        return (-1);
    }
    // Check if palette is in use, if so, dispose of it
    if (WinPal[palNum].status) {
        DisposeCTable(WinPal[palNum].cth);
        DisposePalette(WinPal[palNum].ph);
        WinPal[palNum].status = 0;
    }
    WinPal[palNum].cth = GetCTable(resID);
    if (WinPal[palNum].cth == nil) return -1;
    WinPal[palNum].ph = NewPalette(ent, WinPal[palNum].cth, opts, 0);
    if (WinPal[palNum].ph == nil) {
        DisposeCTable(WinPal[palNum].cth);
        return -1;
    }
    WinPal[palNum].status = 1;
    return (0);
}

short GFX_WinPal(short WinNum, short palNum) {
    if (!WinData[WinNum].status) {
        ErrHandler(1009, "\p in WinPalette command");
        return (-1);
    }
    if (!WinPal[palNum].status) {
        ErrHandler(1049, "\p in WinPalette command");
        return (-1);
    }
    palNum--;
    if (palNum < 0 || palNum > 9) {
        ErrHandler(1006, "\p in WinPalette command");
        return (-1);
    }
    NSetPalette(WinData[WinNum].theWindow, WinPal[palNum].ph, pmAllUpdates);
    ActivatePalette(WinData[WinNum].theWindow);
    return (0);
}

short GFX_ClipWindow(short WinNum, short x, short y, short wide, short ht) {
    Rect theRect;
    if (!WinData[WinNum].status) {
        ErrHandler(1009, "\p in WinClip command");
        return (-1);
    }
    SetRect(&theRect, x, y, x + wide, y + ht);
    WinData[WinNum].clipRect = theRect;
    return (0);
}

short GFX_SetWindowTitle(short WinNum, Str255 wtitle) {
    if (!WinData[WinNum].status) {
        ErrHandler(1009, "\p in WinTitle command");
        return (-1);
    }
    SetWTitle(WinData[WinNum].theWindow, wtitle);
    return (0);
}

short GFX_Windowinfo(short WinNum, short parm) {
    short a = 0;
    Rect theRect;
    if (!WinData[WinNum].status) {
        ErrHandler(1009, "\p in WinBorder function");
        return (-1);
    }
    theRect = WinData[WinNum].winRect;
    switch (parm) {
        case 1: // left
            a = theRect.left;
            break;
        case 2: // top
            a = theRect.top;
            break;
        case 3: // width
            a = theRect.right - theRect.left;
            break;
        case 4: // ht
            a = theRect.bottom - theRect.top;
            break;
    }
    return (a);
}

short GFX_Screeninfo(short parm) {
    short a = 0;
    Rect theRect;
    theRect = (**GetMainDevice()).gdRect;
    switch (parm) {
        case 1: // left
            a = theRect.left;
            break;
        case 2: // top
            a = theRect.top;
            break;
        case 3: // width
            a = theRect.right - theRect.left;
            break;
        case 4: // ht
            a = theRect.bottom - theRect.top;
            break;
    }
    return (a);
}

short GFX_MsgBox(UCHAR *message, short buttonTypes) {
    short a = 0;
    DialogPtr md;
    switch (buttonTypes) {
        case 0: // Ok
            ParamText(message, 0, 0, 0);
            md = GetNewDialog(600, 0, (WindowPtr) - 1);
            if (md != nil) {
                do { ModalDialog(0, &a); } while (a != 1);
                DisposeDialog(md);
            }
            break;
        case 1: // Ok and Cancel
            ParamText(message, 0, 0, 0);
            md = GetNewDialog(601, 0, (WindowPtr) - 1);
            if (md != nil) {
                do { ModalDialog(0, &a); } while (a > 2);
                DisposeDialog(md);
                if (a == 2) a = -1;
            }
            break;
        case 2: // Yes & No
            ParamText(message, 0, 0, 0);
            md = GetNewDialog(602, 0, (WindowPtr) - 1);
            if (md != nil) {
                do { ModalDialog(0, &a); } while (a > 2);
                DisposeDialog(md);
                if (a == 2) a = 0;
            }
            break;
        case 3: // Quit & Don't Quit
            ParamText(message, 0, 0, 0);
            md = GetNewDialog(603, 0, (WindowPtr) - 1);
            if (md != nil) {
                do { ModalDialog(0, &a); } while (a > 2);
                DisposeDialog(md);
                if (a == 2) a = 0;
            }
            break;
        case 4: // Yes, No, & Cancel
            ParamText(message, 0, 0, 0);
            md = GetNewDialog(610, 0, (WindowPtr) - 1);
            if (md != nil) {
                do { ModalDialog(0, &a); } while (a > 3);
                DisposeDialog(md);
                if (a == 2) a = -1;
                else if (a == 3) a = 0;
            }
            break;
        case 5: // Save, Don't Save, & Cancel
            ParamText(message, 0, 0, 0);
            md = GetNewDialog(611, 0, (WindowPtr) - 1);
            if (md != nil) {
                do { ModalDialog(0, &a); } while (a > 3);
                DisposeDialog(md);
                if (a == 2) a = -1;
                else if (a == 3) a = 0;
            }
            break;
    }
    return (a);
}

short GFX_DrawText(short WinNum, short x1, short y1, UCHAR *txt1) {
    short a;
    if (WinNum < 2000) {
        if (openPictureFlag) SetPort(openPictureWindow);
        else {
            if (WinData[WinNum].status < 1) {
                ErrHandler(1009, "\p in DrawText command");
                return (-1);
            }
            SetPort(WinData[WinNum].theWindow);
            GFX_AddGfxCommand(WinNum, 11, x1, y1, 0, 0, 0, txt1);
        }
    } else {
        if (WinNum < 2500) {    // Ah send to sprite frame
            WinNum -= 2000;
            a = SetSpritePort(WinNum);
            if (a) return (-1);  //Invalid sprite port
        } else if (WinNum == 30000) // Printer
        {
            if (PrintActive) SetPort(&PrinterPort->gPort);
            else return (-2);  // No print doc open
        } else return (-1);  // Invalid destination
    }
    MoveTo(x1, y1);
    DrawString(txt1);
    return (0);
}

short GFX_DrawFont(short WinNum, short x1) {
    short a;
    if (WinNum < 2000) {
        if (openPictureFlag) SetPort(openPictureWindow);
        else {
            if (WinData[WinNum].status < 1) {
                ErrHandler(1009, "\p in DrawFont command");
                return (-1);
            }
            SetPort(WinData[WinNum].theWindow);
            GFX_AddGfxCommand(WinNum, 12, x1, 0, 0, 0, 0, nil);
        }
    } else {
        if (WinNum < 2500) {    // Ah send to sprite frame
            WinNum -= 2000;
            a = SetSpritePort(WinNum);
            if (a) return (-1);  //Invalid sprite port
        } else if (WinNum == 30000) // Printer
        {
            if (PrintActive) SetPort(&PrinterPort->gPort);
            else return (-2);  // No print doc open
        } else return (-1);  // Invalid destination
    }
    TextFont(x1);
    return (0);
}

short GFX_DrawSize(short WinNum, short x1) {
    short a;
    if (WinNum < 2000) {
        if (openPictureFlag) SetPort(openPictureWindow);
        else {
            if (WinData[WinNum].status < 1) {
                ErrHandler(1009, "\p in DrawSize command");
                return (-1);
            }
            SetPort(WinData[WinNum].theWindow);
            GFX_AddGfxCommand(WinNum, 13, x1, 0, 0, 0, 0, nil);
        }
    } else {
        if (WinNum < 2500) {    // Ah send to sprite frame
            WinNum -= 2000;
            a = SetSpritePort(WinNum);
            if (a) return (-1);  //Invalid sprite port
        } else if (WinNum == 30000) // Printer
        {
            if (PrintActive) SetPort(&PrinterPort->gPort);
            else return (-2);  // No print doc open
        } else return (-1);  // Invalid destination
    }
    TextSize(x1);
    return (0);
}

short GFX_DrawStyle(short WinNum, short x1) {
    short a;
    if (WinNum < 2000) {
        if (openPictureFlag) SetPort(openPictureWindow);
        else {
            if (WinData[WinNum].status < 1) {
                ErrHandler(1009, "\p in DrawStyle command");
                return (-1);
            }
            SetPort(WinData[WinNum].theWindow);
            GFX_AddGfxCommand(WinNum, 14, x1, 0, 0, 0, 0, nil);
        }
    } else {
        if (WinNum < 2500) {    // Ah send to sprite frame
            WinNum -= 2000;
            a = SetSpritePort(WinNum);
            if (a) return (-1);  //Invalid sprite port
        } else if (WinNum == 30000) // Printer
        {
            if (PrintActive) SetPort(&PrinterPort->gPort);
            else return (-2);  // No print doc open
        } else return (-1);  // Invalid destination
    }
    TextFace(x1);
    return (0);
}

short GFX_Moveto(short WinNum, short x1, short y1) {
    short a;
    if (WinNum < 2000) {
        if (openPictureFlag) SetPort(openPictureWindow);
        else {
            if (WinData[WinNum].status < 1) {
                ErrHandler(1009, "\p in MoveTo command");
                return (-1);
            }
            SetPort(WinData[WinNum].theWindow);
            GFX_AddGfxCommand(WinNum, 2, x1, y1, 0, 0, 0, nil);
            WinData[WinNum].GraphX = x1;
            WinData[WinNum].GraphY = y1;
        }
    } else {
        if (WinNum < 2500) {    // Ah send to sprite frame
            WinNum -= 2000;
            a = SetSpritePort(WinNum);
            if (a) return (-1);  //Invalid sprite port
        } else if (WinNum == 30000) // Printer
        {
            if (PrintActive) SetPort(&PrinterPort->gPort);
            else return (-2);  // No print doc open
        } else return (-1);  // Invalid destination
    }
    MoveTo(x1, y1);
    return (0);
}

short GFX_Update(short WinNum, short update) {
    if (WinData[WinNum].status < 1) return (-1);
    if (update) update = 1;
    WinData[WinNum].update = update;
    return (0);
}

short GFX_Point(short WinNum, short x1, short y1) {
    short a;
    if (WinNum < 2000) {
        if (openPictureFlag) SetPort(openPictureWindow);
        else {
            if (WinData[WinNum].status < 1) {
                ErrHandler(1009, "\p in Point command");
                return (-1);
            }
            SetPort(WinData[WinNum].theWindow);
            GFX_AddGfxCommand(WinNum, 1, x1, y1, 0, 0, 0, nil);
            WinData[WinNum].GraphX = x1;
            WinData[WinNum].GraphY = y1;
        }
    } else {
        if (WinNum < 2500) {    // Ah send to sprite frame
            WinNum -= 2000;
            a = SetSpritePort(WinNum);
            if (a) return (-1);  //Invalid sprite port
        } else if (WinNum == 30000) // Printer
        {
            if (PrintActive) SetPort(&PrinterPort->gPort);
            else return (-2);  // No print doc open
        } else return (-1);  // Invalid destination
    }
    MoveTo(x1, y1);
    LineTo(x1, y1);
    return (0);
}

short GFX_Lineto(short WinNum, short x1, short y1) {
    short a;
    if (WinNum < 2000) {
        if (openPictureFlag) SetPort(openPictureWindow);
        else {
            if (WinData[WinNum].status < 1) {
                ErrHandler(1009, "\p in LineTo command");
                return (-1);
            }
            SetPort(WinData[WinNum].theWindow);
            GFX_AddGfxCommand(WinNum, 3, x1, y1, 0, 0, 0, nil);
            MoveTo(WinData[WinNum].GraphX, WinData[WinNum].GraphY);
            WinData[WinNum].GraphX = x1;
            WinData[WinNum].GraphY = y1;
        }
    } else {
        if (WinNum < 2500) {    // Ah send to sprite frame
            WinNum -= 2000;
            a = SetSpritePort(WinNum);
            if (a) return (-1);  //Invalid sprite port
        } else if (WinNum == 30000) // Printer
        {
            if (PrintActive) SetPort(&PrinterPort->gPort);
            else return (-2);  // No print doc open
        } else return (-1);  // Invalid destination
    }
    LineTo(x1, y1);
    return (0);
}

short GFX_Line(short WinNum, short x1, short y1, short x2, short y2) {
    short a;
    if (WinNum < 2000) {
        if (openPictureFlag) SetPort(openPictureWindow);
        else {
            if (WinData[WinNum].status < 1) {
                ErrHandler(1009, "\p in Line command");
                return (-1);
            }
            SetPort(WinData[WinNum].theWindow);
            GFX_AddGfxCommand(WinNum, 4, x1, y1, x2, y2, 0, nil);
            WinData[WinNum].GraphX = x2;
            WinData[WinNum].GraphY = y2;
        }
    } else {
        if (WinNum < 2500) {    // Ah send to sprite frame
            WinNum -= 2000;
            a = SetSpritePort(WinNum);
            if (a) return (-1);  //Invalid sprite port
        } else if (WinNum == 30000) // Printer
        {
            if (PrintActive) SetPort(&PrinterPort->gPort);
            else return (-2);  // No print doc open
        } else return (-1);  // Invalid destination
    }
    MoveTo(x1, y1);
    LineTo(x2, y2);
    return (0);
}

short GFX_Box(short WinNum, short x1, short y1, short width, short ht, short fill) {
    short a, b = 1;
    Rect tempRect;
    Pattern thePat;
    SetRect(&tempRect, x1, y1, x1 + width, y1 + ht);
    if (WinNum < 2000) {
        if (openPictureFlag) {
            b = 0;
            SetPort(openPictureWindow);
        } else {
            if (WinData[WinNum].status < 1) {
                ErrHandler(1009, "\p in Box command");
                return (-1);
            }
            SetPort(WinData[WinNum].theWindow);
        }
    } else {
        b = 0;
        if (WinNum < 2500) {    // Ah send to sprite frame
            WinNum -= 2000;
            a = SetSpritePort(WinNum);
            if (a) return (-1);  //Invalid sprite port
        } else if (WinNum == 30000) // Printer
        {
            if (PrintActive) SetPort(&PrinterPort->gPort);
            else return (-2);  // No print doc open
        } else return (-1);  // Invalid destination
    }
    if (!fill) {
        FrameRect(&tempRect);
        if (b) GFX_AddGfxCommand(WinNum, 5, x1, y1, width, ht, 0, nil);
    } else {
        if (b) GFX_AddGfxCommand(WinNum, 6, x1, y1, width, ht, fill, nil);
        GetIndPattern(&thePat, 0, fill);
        FillRect(&tempRect, &thePat);
    }
    return (0);
}

short GFX_Circle(short WinNum, short x1, short y1, short theradius, short fill) {
    short a, b = 1;
    Rect tempRect;
    Pattern thePat;
    SetRect(&tempRect, x1 - theradius, y1 - theradius, x1 + theradius, y1 + theradius);
    if (WinNum < 2000) {
        if (openPictureFlag) {
            b = 0;
            SetPort(openPictureWindow);
        } else {
            if (WinData[WinNum].status < 1) {
                ErrHandler(1009, "\p in Circle command");
                return (-1);
            }
            SetPort(WinData[WinNum].theWindow);
        }
    } else {
        b = 0;
        if (WinNum < 2500) {    // Ah send to sprite frame
            WinNum -= 2000;
            a = SetSpritePort(WinNum);
            if (a) return (-1);  //Invalid sprite port
        } else if (WinNum == 30000) // Printer
        {
            if (PrintActive) SetPort(&PrinterPort->gPort);
            else return (-2);  // No print doc open
        } else return (-1);  // Invalid destination
    }
    if (!fill) {
        if (b) GFX_AddGfxCommand(WinNum, 7, x1, y1, theradius, 0, 0, nil);
        FrameOval(&tempRect);
    } else {
        if (b) GFX_AddGfxCommand(WinNum, 8, x1, y1, theradius, fill, 0, nil);
        GetIndPattern(&thePat, sysPatListID, fill);
        FillOval(&tempRect, &thePat);
    }
    return (0);
}

short GFX_Ellipse(short WinNum, short x1, short y1, short width, short ht, short fill) {
    short a, b = 1;
    Rect tempRect;
    Pattern thePat;
    SetRect(&tempRect, x1, y1, x1 + width, y1 + ht);
    if (WinNum < 2000) {
        if (openPictureFlag) {
            b = 0;
            SetPort(openPictureWindow);
        } else {
            if (WinData[WinNum].status < 1) {
                ErrHandler(1009, "\p in Ellipse command");
                return (-1);
            }
            SetPort(WinData[WinNum].theWindow);
        }
    } else {
        b = 0;
        if (WinNum < 2500) {    // Ah send to sprite frame
            WinNum -= 2000;
            a = SetSpritePort(WinNum);
            if (a) return (-1);  //Invalid sprite port
        } else if (WinNum == 30000) // Printer
        {
            if (PrintActive) SetPort(&PrinterPort->gPort);
            else return (-2);  // No print doc open
        } else return (-1);  // Invalid destination
    }
    if (!fill) {
        if (b) GFX_AddGfxCommand(WinNum, 9, x1, y1, width, ht, 0, nil);
        FrameOval(&tempRect);
    } else {
        if (b) GFX_AddGfxCommand(WinNum, 10, x1, y1, width, ht, fill, nil);
        GetIndPattern(&thePat, 0, fill);
        FillOval(&tempRect, &thePat);
    }
    return (0);
}

short GFX_Picture(short WinNum, short x1, short y1, short w, short ht, short ResID) {
    short a, b = 1, w2, h2;
    Rect tempRect, tempRect2, r;
    PicHandle thePict;
    SetRect(&tempRect, x1, y1, x1 + w, y1 + ht);
    if (WinNum < 2000) {
        if (openPictureFlag) {
            b = 0;
            SetPort(openPictureWindow);
        } else {
            if (WinData[WinNum].status < 1) {
                ErrHandler(1009, "\p in DrawPicture command");
                return (-1);
            }
            SetPort(WinData[WinNum].theWindow);
        }
    } else {
        b = 0;
        if (WinNum < 2500) {    // Ah send to sprite frame
            WinNum -= 2000;
            a = SetSpritePort(WinNum);
            if (a) return (-1);  //Invalid sprite port
        } else if (WinNum == 30000) // Printer
        {
            if (PrintActive) SetPort(&PrinterPort->gPort);
            else return (-2);  // No print doc open
        } else return (-1);  // Invalid destination
    }
    if (ResID < 30000) thePict = GetPicture(ResID);
    else {
        a = ResID - 30000;
        if (a < 0 || a > 99) return (-1);
        thePict = Picts[a];
    }
    if (thePict == nil) return (-1);
    if (b) GFX_AddGfxCommand(WinNum, 21, x1, y1, w, ht, ResID, nil);
    if (!w && !ht) {
        // Draw picture in original size
        tempRect2 = (*thePict)->picFrame;
        w2 = tempRect2.right - tempRect2.left;
        h2 = tempRect2.bottom - tempRect2.top;
        SetRect(&r, x1, y1, x1 + w2, y1 + h2);
        DrawPicture(thePict, &r);
    } else {
        // Draw picture in set size
        SetRect(&r, x1, y1, x1 + w, y1 + ht);
        DrawPicture(thePict, &r);
    }
    return (0);
}

short GFX_PenSize(short WinNum, short penWidth, short penHt) {
    short a;
    if (WinNum < 2000) {
        if (openPictureFlag) SetPort(openPictureWindow);
        else {
            if (WinData[WinNum].status < 1) {
                ErrHandler(1009, "\p in PenSize command");
                return (-1);
            }
            SetPort(WinData[WinNum].theWindow);
            GFX_AddGfxCommand(WinNum, 17, penWidth, penHt, 0, 0, 0, nil);
        }
    } else {
        if (WinNum < 2500) {    // Ah send to sprite frame
            WinNum -= 2000;
            a = SetSpritePort(WinNum);
            if (a) return (-1);  //Invalid sprite port
        } else if (WinNum == 30000) // Printer
        {
            if (PrintActive) SetPort(&PrinterPort->gPort);
            else return (-2);  // No print doc open
        } else return (-1);  // Invalid destination
    }
    PenSize(penWidth, penHt);
    return (0);
}

short GFX_PenPattern(short WinNum, short pat) {
    short a;
    Pattern thePat;
    if (WinNum < 2000) {
        if (openPictureFlag) SetPort(openPictureWindow);
        else {
            if (WinData[WinNum].status < 1) {
                ErrHandler(1009, "\p in PenPattern command");
                return (-1);
            }
            SetPort(WinData[WinNum].theWindow);
            GFX_AddGfxCommand(WinNum, 18, pat, 0, 0, 0, 0, nil);
        }
    } else {
        if (WinNum < 2500) {    // Ah send to sprite frame
            WinNum -= 2000;
            a = SetSpritePort(WinNum);
            if (a) return (-1);  //Invalid sprite port
        } else if (WinNum == 30000) // Printer
        {
            if (PrintActive) SetPort(&PrinterPort->gPort);
            else return (-2);  // No print doc open
        } else return (-1);  // Invalid destination
    }
    GetIndPattern(&thePat, sysPatListID, pat);
    PenPat(&thePat);
    return (0);
}

short GFX_PenMode(short WinNum, short penMode) {
    short a;
    if (WinNum < 2000) {
        if (openPictureFlag) SetPort(openPictureWindow);
        else {
            if (WinData[WinNum].status < 1) {
                ErrHandler(1009, "\p in PenMode command");
                return (-1);
            }
            SetPort(WinData[WinNum].theWindow);
            GFX_AddGfxCommand(WinNum, 19, penMode, 0, 0, 0, 0, nil);
        }
    } else {
        if (WinNum < 2500) {    // Ah send to sprite frame
            WinNum -= 2000;
            a = SetSpritePort(WinNum);
            if (a) return (-1);  //Invalid sprite port
        } else if (WinNum == 30000) // Printer
        {
            if (PrintActive) SetPort(&PrinterPort->gPort);
            else return (-2);  // No print doc open
        } else return (-1);  // Invalid destination
    }
    PenMode(penMode);
    return (0);
}

short GFX_PenNormal(short WinNum) {
    short a;
    if (WinNum < 2000) {
        if (openPictureFlag) SetPort(openPictureWindow);
        else {
            if (WinData[WinNum].status < 1) {
                ErrHandler(1009, "\p in PenNormal command");
                return (-1);
            }
            SetPort(WinData[WinNum].theWindow);
            GFX_AddGfxCommand(WinNum, 20, 0, 0, 0, 0, 0, nil);
        }
    } else {
        if (WinNum < 2500) {    // Ah send to sprite frame
            WinNum -= 2000;
            a = SetSpritePort(WinNum);
            if (a) return (-1);  //Invalid sprite port
        } else if (WinNum == 30000) // Printer
        {
            if (PrintActive) SetPort(&PrinterPort->gPort);
            else return (-2);  // No print doc open
        } else return (-1);  // Invalid destination
    }
    PenNormal();
    return (0);
}

void GFX_ClearGfx(short WinNum, short flag) {
    if (WinData[WinNum].drawCmds != nil) {
        DisposeHandle(WinData[WinNum].drawCmds);
        WinData[WinNum].drawCmds = nil;
        if (!flag) GFX_WinRedraw(WinNum);
    }
}

void GFX_DrawUpdate(short WinNum, short flag) {
    if (WinData[WinNum].status > 0) {
        if (!flag) WinData[WinNum].update = 0;
        else WinData[WinNum].update = 1;
    }
}

short OLD_WinNum = -1, OLD_cmd = -1, OLD_p1 = -1, OLD_p2 = -1;
short reentryFlag = 0;

void GFX_AddGfxCommand(short WinNum, short cmd, short p1, short p2, short p3, short p4, short p5, unsigned char *txt) {
    UCHAR *cptr;
    short *sptr, s, c;
    long *pptr, bsize = 14096, *lptr, blen = 14, mm = 0;
    if (InlineUpdateFlag || !WinData[WinNum].update || openPictureFlag) return;
    // Command summary
    // 1 = Point	2 = Move To  3 = Lineto		4 = Line	5 = Box		6 = BoxFill
    // 7 = Circle	8 = CircleFill		9 = Ellipse			10 = EllipseFIll
    // 11 = DrawText	12 = DrawFont	13 = DrawSize		14 = DrawStyle
    // 15 = Set FG Color	16 = Set FG RGB Color
    // 17 = Set PenSize		18 = Set PenPat		19 = Set PenMode    20 = PenNormal
    // 21 = DrawPicture
    // Header = 0-3>bsize 4-7>blen 8-13>fontstuff
    // Let's check if this is a repeat of old moveto, point or line
    if (cmd <= 3) {
        if (WinNum == OLD_WinNum && cmd == OLD_cmd)
            if (p1 == OLD_p1 && p2 == OLD_p2) return;
    }
    OLD_WinNum = WinNum;
    OLD_cmd = cmd;
    OLD_p1 = p1;
    OLD_p2 = p2;
    reentryFlag = 1;
    if (WinData[WinNum].drawCmds == nil) {
        // Need to allocate some new space
        WinData[WinNum].drawCmds = NewHandle(bsize);
        if (WinData[WinNum].drawCmds == nil) {
            ErrHandler(1001, "\p");
            return;
        }
        HLock(WinData[WinNum].drawCmds);
        pptr = (long *) (*WinData[WinNum].drawCmds);
        lptr = pptr + 1;
        *pptr = bsize;
        *lptr = blen;
        sptr = (short *) (*WinData[WinNum].drawCmds);
        sptr += 4;
        s = (WinData[WinNum].theWindow)->txFont;
        *sptr = s;
        sptr++;
        s = (WinData[WinNum].theWindow)->txSize;
        *sptr = s;
        sptr++;
        s = (WinData[WinNum].theWindow)->txFace;
        *sptr = s;
        sptr++;
        // Above was replaced with GetPenState
        GetPenState(&GfxWinPS[WinNum]);
    } else {
        HLock(WinData[WinNum].drawCmds);
        pptr = (long *) (*WinData[WinNum].drawCmds);
        lptr = pptr + 1;
        bsize = *pptr;
        blen = *lptr;
        if (blen + 200 > bsize) {
            // Lets make it larger
            bsize += 14096;
            HUnlock(WinData[WinNum].drawCmds);
            SetHandleSize(WinData[WinNum].drawCmds, bsize);
            if (MemError()) {
                ErrHandler(1001, "\p");
                return;
            }
            HLock(WinData[WinNum].drawCmds);
            pptr = (long *) (*WinData[WinNum].drawCmds);
            *pptr = bsize;
            lptr = pptr + 1;
            mm = bsize;
        }
    }
    cptr = (UCHAR *) (*WinData[WinNum].drawCmds);
    cptr += blen;
    sptr = (short *) cptr;
    *sptr = cmd;
    sptr++;  // command
    blen += 2;
    switch (cmd) {
        case 1: //  Point
        case 2: //  MoveTo
        case 3: //  LineTo
        case 17://  PenSize
            *sptr = p1;
            sptr++;
            *sptr = p2;
            sptr++;
            blen += 4;
            *lptr = blen;
            break;
        case 12: //  DrawFont
        case 13: //  DrawSize
        case 14: //  DrawStyle
        case 15: //  Set FG Color
        case 18: //  PenPattern
        case 19: //  PenMode
            *sptr = p1;
            sptr++;
            blen += 2;
            *lptr = blen;
            break;
        case 16: // Set FG RGB Clor
            *sptr = p1;
            sptr++;
            *sptr = p2;
            sptr++;
            *sptr = p3;
            sptr++;
            blen += 6;
            *lptr = blen;
            break;
        case 4: //  Line
        case 5: //  Box
        case 7: //  Circle
        case 9: //  Ell
            *sptr = p1;
            sptr++;
            *sptr = p2;
            sptr++;
            *sptr = p3;
            sptr++;
            *sptr = p4;
            sptr++;
            blen += 8;
            *lptr = blen;
            break;
        case 6: //  BoxFill
        case 8: //  CircleFill
        case 10: // EllFill
        case 21: // DrawPicture
            *sptr = p1;
            sptr++;
            *sptr = p2;
            sptr++;
            *sptr = p3;
            sptr++;
            *sptr = p4;
            sptr++;
            *sptr = p5;
            sptr++;
            blen += 10;
            *lptr = blen;
            break;
        case 11: // DrawText
            *sptr = p1;
            sptr++;
            *sptr = p2;
            sptr++;
            blen += 4;
            s = txt[0];  //Get length
            if (blen + s + 200 > bsize) {
                // Lets make it larger
                bsize += 14096;
                HUnlock(WinData[WinNum].drawCmds);
                SetHandleSize(WinData[WinNum].drawCmds, bsize);
                HLock(WinData[WinNum].drawCmds);
                pptr = (long *) (*WinData[WinNum].drawCmds);
                *pptr = bsize;
                lptr = pptr + 1;
            }
            cptr = (UCHAR *) (*WinData[WinNum].drawCmds);
            cptr += blen;
            for (c = 0; c <= s; c++) cptr[c] = txt[c];
            blen += s + 1;
            *lptr = blen;
            break;
        case 20: // PenNormal
            *lptr = blen;
            break;
    }
    HUnlock(WinData[WinNum].drawCmds);
    BeginUpdate(WinData[WinNum].theWindow);
    EndUpdate(WinData[WinNum].theWindow);
    reentryFlag = 0;
}

void DrawGraphics(short WinNum) {
    UCHAR *cptr;
    short tfont, tsize, tstyle, *sptr, cmd;
    short p1, p2, p3, p4, p5;
    long blen, *lptr, ctr;
    Str255 txt1;
    PenState PStemp;
    if (!WinData[WinNum].status) return;   // Not a valid window, shouldn't happen
    if (WinData[WinNum].drawCmds == nil) return;   // No Graphics to draw
    InlineUpdateFlag = 1;
    // Let's save first and then restore at end
    tfont = (WinData[WinNum].theWindow)->txFont;
    tsize = (WinData[WinNum].theWindow)->txSize;
    tstyle = (WinData[WinNum].theWindow)->txFace;
    SetPort(WinData[WinNum].theWindow);
    GetPenState(&PStemp);
    HLock(WinData[WinNum].drawCmds);
    lptr = (long *) (*WinData[WinNum].drawCmds);
    lptr++;
    blen = *lptr;
    sptr = (short *) (*WinData[WinNum].drawCmds);
    (WinData[WinNum].theWindow)->txFont = sptr[4];
    (WinData[WinNum].theWindow)->txSize = sptr[5];
    (WinData[WinNum].theWindow)->txFace = sptr[6];
    SetPenState(&GfxWinPS[WinNum]);
    ctr = 14;
    cptr = (UCHAR *) ((*WinData[WinNum].drawCmds) + ctr);
    sptr = (short *) cptr;
    while (ctr < blen) {
        cmd = *sptr;
        sptr++;
        ctr += 2;
        switch (cmd) {
            case 1:
                p1 = *sptr;
                sptr++;
                ctr += 2;
                p2 = *sptr;
                sptr++;
                ctr += 2;
                GFX_Point(WinNum, p1, p2);
                break;
            case 2:
                p1 = *sptr;
                sptr++;
                ctr += 2;
                p2 = *sptr;
                sptr++;
                ctr += 2;
                GFX_Moveto(WinNum, p1, p2);
                break;
            case 3:
                p1 = *sptr;
                sptr++;
                ctr += 2;
                p2 = *sptr;
                sptr++;
                ctr += 2;
                GFX_Lineto(WinNum, p1, p2);
                break;
            case 4:
                p1 = *sptr;
                sptr++;
                ctr += 2;
                p2 = *sptr;
                sptr++;
                ctr += 2;
                p3 = *sptr;
                sptr++;
                ctr += 2;
                p4 = *sptr;
                sptr++;
                ctr += 2;
                GFX_Line(WinNum, p1, p2, p3, p4);
                break;
            case 5:
            case 6:
                p1 = *sptr;
                sptr++;
                ctr += 2;
                p2 = *sptr;
                sptr++;
                ctr += 2;
                p3 = *sptr;
                sptr++;
                ctr += 2;
                p4 = *sptr;
                sptr++;
                ctr += 2;
                if (cmd == 5) p5 = 0;
                else {
                    p5 = *sptr;
                    sptr++;
                    ctr += 2;
                }
                GFX_Box(WinNum, p1, p2, p3, p4, p5);
                break;
            case 7:
            case 8:
                p1 = *sptr;
                sptr++;
                ctr += 2;
                p2 = *sptr;
                sptr++;
                ctr += 2;
                p3 = *sptr;
                sptr++;
                ctr += 2;
                if (cmd == 7) p4 = 0;
                else {
                    p4 = *sptr;
                    sptr++;
                    ctr += 2;
                }
                GFX_Circle(WinNum, p1, p2, p3, p4);
                break;
            case 9:
            case 10:
                p1 = *sptr;
                sptr++;
                ctr += 2;
                p2 = *sptr;
                sptr++;
                ctr += 2;
                p3 = *sptr;
                sptr++;
                ctr += 2;
                p4 = *sptr;
                sptr++;
                ctr += 2;
                if (cmd == 9) p5 = 0;
                else {
                    p5 = *sptr;
                    sptr++;
                    ctr += 2;
                }
                GFX_Ellipse(WinNum, p1, p2, p3, p4, p5);
                break;
            case 11:
                p1 = *sptr;
                sptr++;
                ctr += 2;
                p2 = *sptr;
                sptr++;
                ctr += 2;
                cptr = (UCHAR *) (*WinData[WinNum].drawCmds) + ctr;
                p3 = cptr[0];    // Len
                for (p4 = 0; p4 <= p3; p4++) {
                    txt1[p4] = cptr[p4];
                    ctr++;
                }
                GFX_DrawText(WinNum, p1, p2, txt1);
                break;
            case 12:
                p1 = *sptr;
                sptr++;
                ctr += 2;
                GFX_DrawFont(WinNum, p1);
                break;
            case 13:
                p1 = *sptr;
                sptr++;
                ctr += 2;
                GFX_DrawSize(WinNum, p1);
                break;
            case 14:
                p1 = *sptr;
                sptr++;
                ctr += 2;
                GFX_DrawStyle(WinNum, p1);
                break;
            case 15:
                p1 = *sptr;
                sptr++;
                ctr += 2;
                GFX_SetColor(WinNum, p1, 0);
                break;
            case 16:
                p1 = *sptr;
                sptr++;
                ctr += 2;
                p2 = *sptr;
                sptr++;
                ctr += 2;
                p3 = *sptr;
                sptr++;
                ctr += 2;
                GFX_SetRGBColor(WinNum, p1, p2, p3, 0);
                break;
            case 17:
                p1 = *sptr;
                sptr++;
                ctr += 2;
                p2 = *sptr;
                sptr++;
                ctr += 2;
                GFX_PenSize(WinNum, p1, p2);
                break;
            case 18:
                p1 = *sptr;
                sptr++;
                ctr += 2;
                GFX_PenPattern(WinNum, p1);
                break;
            case 19:
                p1 = *sptr;
                sptr++;
                ctr += 2;
                GFX_PenMode(WinNum, p1);
                break;
            case 20:
                GFX_PenNormal(WinNum);
                break;
            case 21:
                p1 = *sptr;
                sptr++;
                ctr += 2;
                p2 = *sptr;
                sptr++;
                ctr += 2;
                p3 = *sptr;
                sptr++;
                ctr += 2;
                p4 = *sptr;
                sptr++;
                ctr += 2;
                p5 = *sptr;
                sptr++;
                ctr += 2;
                GFX_Picture(WinNum, p1, p2, p3, p4, p5);
                break;
        }
    }
    HUnlock(WinData[WinNum].drawCmds);
    // Now Restore
    (WinData[WinNum].theWindow)->txFont = tfont;
    (WinData[WinNum].theWindow)->txSize = tsize;
    (WinData[WinNum].theWindow)->txFace = tstyle;
    SetPenState(&PStemp);
    InlineUpdateFlag = 0;
}

short GFX_SetColor(short WinNum, short color, short bgflag) {
    short a, b = 1, c = 0;
    RGBColor rgb;
    if (color < 1 || color > 8) {
        ErrHandler(1004, "\p in SetColor command");
        return (-1);
    }
    if (WinNum < 2000) {
        if (openPictureFlag) {
            b = 0;
            SetPort(openPictureWindow);
        } else {
            if (WinData[WinNum].status < 1) {
                if (!bgflag) ErrHandler(1009, "\p in SetColor command");
                else ErrHandler(1009, "\p in BackColor command");
                return (-1);
            }
            SetPort(WinData[WinNum].theWindow);
        }
    } else {
        b = 0;
        if (WinNum < 2500) {    // Ah send to sprite frame
            WinNum -= 2000;
            a = SetSpritePort(WinNum);
            if (a) return (-1);  //Invalid sprite port
            c++;
        } else if (WinNum == 30000) // Printer
        {
            if (PrintActive) SetPort(&PrinterPort->gPort);
            else return (-2);  // No print doc open
        } else return (-1);  // Invalid destination
    }
    switch (color) {
        case 1:
            rgb.red = 0;
            rgb.green = 0;
            rgb.blue = 0;
            break;
        case 2:
            rgb.red = 0xfc00;
            rgb.green = 0xf37d;
            rgb.blue = 0x052f;
            break;
        case 3:
            rgb.red = 0xf2d7;
            rgb.green = 0x0856;
            rgb.blue = 0x84ec;
            break;
        case 4:
            rgb.red = 0xdd6b;
            rgb.green = 0x08c2;
            rgb.blue = 0x06a2;
            break;
        case 5:
            rgb.red = 0x0241;
            rgb.green = 0xab54;
            rgb.blue = 0xeaff;
            break;
        case 6:
            rgb.red = 0;
            rgb.green = 0x8000;
            rgb.blue = 0x11b0;
            break;
        case 7:
            rgb.red = 0;
            rgb.green = 0;
            rgb.blue = 0xd400;
            break;
        case 8:
            rgb.red = 0xffff;
            rgb.green = 0xffff;
            rgb.blue = 0xffff;
            break;
    }
    if (!bgflag) {
        RGBForeColor(&rgb);
        if (WinNum < 2000) GFX_AddGfxCommand(WinNum, 15, color, 0, 0, 0, 0, nil);
    } else {
        RGBBackColor(&rgb);
        if (c) EraseRect(&SpriteImages[WinNum].theRect);
        else {
            // Now redraw window
            if (WinNum < 2000 && b) {
                WinData[WinNum].BackRGB = rgb;
                GFX_WinRedraw(WinNum);
            }
        }
    }
    return (0);
}

short GFX_SetRGBColor(short WinNum, long red, long green, long blue, short bgflag) {
    short a, b = 1;
    RGBColor rgb;
    if (WinNum < 2000) {
        if (openPictureFlag) {
            b = 0;
            SetPort(openPictureWindow);
        } else {
            if (WinData[WinNum].status < 1) {
                if (!bgflag) ErrHandler(1009, "\p in SetColorRGB command");
                else ErrHandler(1009, "\p in BackColorRGB command");
                return (-1);
            }
            SetPort(WinData[WinNum].theWindow);
        }
    } else {
        b = 0;
        if (WinNum < 2500) {    // Ah send to sprite frame
            WinNum -= 2000;
            a = SetSpritePort(WinNum);
            if (a) return (-1);  //Invalid sprite port
            rgb.red = red;
            rgb.green = green;
            rgb.blue = blue;
            RGBBackColor(&rgb);
            EraseRect(&SpriteImages[WinNum].theRect);
        } else if (WinNum == 30000) // Printer
        {
            if (PrintActive) SetPort(&PrinterPort->gPort);
            else return (-2);  // No print doc open
        } else return (-1);  // Invalid destination
    }
    rgb.red = (unsigned short) red;
    rgb.green = (unsigned short) green;
    rgb.blue = (unsigned short) blue;
    if (!bgflag) {
        RGBForeColor(&rgb);
        if (WinNum < 2000) GFX_AddGfxCommand(WinNum, 16, red, green, blue, 0, 0, nil);
    } else {
        WinData[WinNum].BackRGB = rgb;
        // Now redraw window
        if (WinNum < 2000 && b) GFX_WinRedraw(WinNum);
    }
    return (0);
}

short GetWinStatus(short a) {
    if (WinData[a].status) return 1;
    return 0;
}

short double GFX_RGB(long red, long green, long blue) {
    short double a, b, result = 0;
    a = (short double) red;
    b = 4294967296;
    a = a * b;
    result = a;
    a = (short double) green;
    b = 65536;
    a = a * b;
    result += a;
    a = (short double) blue;
    result += a;
    return result;
}

short double GFX_GetPixel(short WinNum, short x1, short y1) {
    short a;
    short double ad, b, result = 0;
    RGBColor r1;
    if (WinNum < 2000) {
        if (!openPictureFlag) {
            if (WinData[WinNum].status < 1) {
                ErrHandler(1009, "\p in GetPixel function");
                return (-1);
            }
            SetPort(WinData[WinNum].theWindow);
        }
    } else {
        if (WinNum < 2500) {    // Ah send to sprite frame
            WinNum -= 2000;
            a = SetSpritePort(WinNum);
            if (a) return (-1);  //Invalid sprite port
        } else if (WinNum == 30000) // Printer
        {
            if (PrintActive) SetPort(&PrinterPort->gPort);
            else return (-2);  // No print doc open
        } else return (-1);  // Invalid destination
    }
    GetCPixel(x1, y1, &r1);
    ad = (short double) r1.red;
    b = 4294967296;
    ad = ad * b;
    result = ad;
    ad = (short double) r1.green;
    b = 65536;
    ad = ad * b;
    result += ad;
    ad = (short double) r1.blue;
    result += ad;
    return result;
}

short GFX_GetWinTitle(short a) {
    short b;
    long slen, c;
    Str255 txt1;
    if (!WinData[a].status) {
        ReturnStrLen = 0;
        return -1;
    }
    GetWTitle(WinData[a].theWindow, txt1);
    // Allocate ReturnStr Space
    b = Allocate_ReturnStr(txt1[0] + 16);
    if (b) return (-1); // Buffer allocation error
    b = 1;
    for (c = 0; c <= txt1[0]; c++) ReturnStr[c] = txt1[b++];
    ReturnStrLen = txt1[0];
    ReturnStr[ReturnStrLen] = 0;
    return (0);
}

short GFX_GetFrontWindow() {
    short a, b = -1;
    long slen, c;
    Str255 txt1;
    WindowPtr window;
    window = FrontWindow();
    for (a = 0; a < 100; a++)
        if (WinData[a].status && window == WinData[a].theWindow) {
            b = a;
            break;
        }
    if (b == -1) {
        ReturnStrLen = 0;
        return -1;
    }
    GetWTitle(WinData[b].theWindow, txt1);
    // Allocate ReturnStr Space
    a = Allocate_ReturnStr(txt1[0] + 16);
    if (a) return (-1); // Buffer allocation error
    a = 1;
    for (c = 0; c <= txt1[0]; c++) ReturnStr[c] = txt1[a++];
    ReturnStrLen = txt1[0];
    ReturnStr[ReturnStrLen] = 0;
    return (0);
}
