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
#include        <Icons.h>
#include        <Lists.h>
#include        <TextEdit.h>

#define         UCHAR   unsigned char
#define            kInFront    (WindowPtr) -1    /* put the new window in front */
UCHAR ListDoubleClick = false;
extern UCHAR *ReturnStr, ReturnStrFlag;
extern long ReturnStrLen;
extern TEHandle ActiveTE;
extern char TimersFlag;

short CTL_Add(ControlHandle Ctl, short WinNum, UCHAR status, short cmd, Rect *theRect);

short TE_Add(TEHandle te, short WinNum, UCHAR status, short cmd, Rect *theRect);

short LT_Add(ListHandle lt, short WinNum, UCHAR status, short cmd, Rect *theRect);

short Pic_Add(PicHandle pic, short WinNum, UCHAR status, short cmd, Rect *theRect);

short Icon_Add(short WinNum, UCHAR status, short cmd, Rect *theRect);

short FreeReturnStr();

short Allocate_ReturnStr(long);

void TextFix(short a);

void ChangeScrollBar(short dr);

short GFX_WinRedraw(short WinNum);

short CTL_ClearControls(short WinNum);

long CTL_GetHandle(short a);

short CTL_Remove(short a);

short CTL_Hide(short a);

short CTL_Show(short a);

short CTL_Disable(short a);

short CTL_Enable(short a);

short CTL_Size(short a, short w, short h);

short CTL_Move(short a, short x, short y);

short CTL_Hilite(short a);

short CTL_Align(short a, short way);

short CTL_Timer(short WinNum, long interval, short enabled, short cmd);

short CTL_DropList(short WinNum, short x1, short y1, short width, short cmd);

short CTL_Button(short WinNum, short x1, short y1, short x2, short y2, char *, short);

short CTL_CheckBox(short WinNum, short x1, short y1, short x2, short y2, char *, short);

short CTL_Radio(short WinNum, short x1, short y1, short wide, short ht, short group, char *btext, short cmd);

short CTL_ScrollBar(short WinNum, short x1, short y1, short x2, short y2, short);

short
CTL_Textbox(short WinNum, short x1, short y1, short wide, short ht, short opts, short opts2, char *btext, short cmd);

short CTL_List(short WinNum, short x1, short y1, short x2, short y2, short option, short cmd);

short CTL_Label(short WinNum, short x1, short y1, short width, short height, char *btext, short cmd);

short CTL_Picture(short WinNum, short ResID, short x1, short y1, short x2, short y2, short cmd);

short CTL_GetText(short a);

short CTL_SetText(short a, short appendFlag);

short CTL_ListAdd(short ListNum, unsigned char *buf, long buflen);

short CTL_LgIcon(short WinNum, short ResID, short x1, short y1, short width, short height, short cmd);

short CTL_ListCount(short ListNum);

short CTL_ListItem(short ListNum, short item);

short CTL_ListClear(short ListNum);

short CTL_ListRemove(short ListNum, short item);

short CTL_TextboxRO(short ctl, short ro);

short CTL_SetTextSelect(short ctl, short start, short end);

short CTL_TextSelect(short ctl);

short CTL_GetTextStart(short ctl);

short CTL_GetTextEnd(short ctl);

short CTL_TextLen(short ctl);

short CTL_IsListSelected(short ListNum, short item);

short CTL_SelectCount(short ListNum);

short CTL_ListSelect(short ListNum, short item);

short CTL_ListDeselect(short ListNum, short item);

long CTL_GetValue(short a);

short CTL_SetValue(short a, long b);

short CTL_Border(short ctl, short dm);

short CTL_NextFreeCtl();

short CTL_ScrollSet(short ctl, short min, short max, short pager);

short GetCtlStatus(short a);

void ErrHandler(short ErrNum, Str255 AddMsg);

void StrToStr255(char *st1, Str255 st2);

short CalcTextPages(short cnum);

short CalcTextView(short cnum);

extern short ControlVisibleFlag;
extern char TimersFlag;
extern struct TimersStruct {
    short status;   // 0=empty  1=disabled  2=enabled
    short cmd;
    long interval;
    long next;
    short WinNum;
} TimersList[50];
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
// Status =     1 Button
//				2 Pulldown
//				3 Checkbox
//				4 Radio
//				5 Scrollbar
//			    6 scrollbar for Textbox (# before)
//			   10 Textbox
//			   11 Textbox w/scrollbar
//			   12 Label
//			   15 List
//			   20 Picture
//			   25 LIcon
//			   30 Timer
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
extern long ReturnStrLen;
extern PicHandle Picts[100];

short CTL_Add(ControlHandle Ctl, short WinNum, UCHAR status, short cmd, Rect *theRect) {
    short a;
    for (a = 0; a < 500; a++) {
        if (!CtlData[a].status) {
            CtlData[a].theControl = Ctl;
            CtlData[a].WinNum = WinNum;
            CtlData[a].status = status;
            CtlData[a].subcode = cmd;
            CtlData[a].theRect = *theRect;
            return (a);
        }
    }
    ErrHandler(1046, nil);
    return (-1);
}

short CTL_NextFreeCtl() {
    short a;
    for (a = 0; a < 500; a++) {
        if (!CtlData[a].status) return a;
    }
    return (-1);
}

short CTL_Remove(short a) {
    short b, c;
    if (CtlData[a].status) {
        CtlData[a].WinNum = -1;
        CtlData[a].subcode = 0;
        switch (CtlData[a].status) {
            case 1: // Button
            case 2: // pulldown
            case 3: // checkbox
            case 4: // radio
            case 5: // scrollbar
                DisposeControl(CtlData[a].theControl);
                CtlData[a].theControl = nil;
                break;
            case 10:    // Textbox
            case 12:    // Label
                if (ActiveTE == CtlData[a].te) {
                    TEDeactivate(ActiveTE);
                    ActiveTE = nil;
                }
                TEDispose(CtlData[a].te);
                CtlData[a].te = nil;
                break;
            case 11:    // TextBox w/scrollbar
                if (ActiveTE == CtlData[a].te) {
                    TEDeactivate(ActiveTE);
                    ActiveTE = nil;
                }
                TEDispose(CtlData[a].te);
                CtlData[a].te = nil;
                DisposeControl(CtlData[a + 1].theControl);
                CtlData[a + 1].theControl = nil;
                CtlData[a + 1].status = 0;
                break;
            case 15: // List Box
                LDispose(CtlData[a].lt);
                CtlData[a].lt = nil;
                break;
            case 30:
                b = CtlData[a].subcode;
                TimersList[b].status = 0;
                b = 0;
                for (c = 0; c < 50; c++) // check timers for window
                    if (TimersList[c].status > 1) b++;
                if (!b) TimersFlag = 0;
                break;
        }
        CtlData[a].status = 0;
        return (1);
    }
    return (0);
}

short CTL_ClearControls(short WinNum) {
    short a, b = 0;
    if (WinData[WinNum].status < 1) return (0);
    for (a = 0; a < 500; a++) {
        if (CtlData[a].status) {
            if (CtlData[a].WinNum == WinNum) {
                CtlData[a].WinNum = -1;
                CtlData[a].subcode = 0;
                switch (CtlData[a].status) {
                    case 1: // Button
                    case 2: // pulldown
                    case 3: // checkbox
                    case 4: // radio
                    case 5: // scrollbar
                    case 6: // tescrollbar
                        DisposeControl(CtlData[a].theControl);
                        CtlData[a].theControl = nil;
                        break;
                    case 10:    // Textbox
                    case 11:    // TextBox w/scrollbar
                    case 12:    // Label
                        if (ActiveTE == CtlData[a].te) {
                            TEDeactivate(ActiveTE);
                            ActiveTE = nil;
                        }
                        TEDispose(CtlData[a].te);
                        CtlData[a].te = nil;
                        break;
                    case 15: // List Box
                        LDispose(CtlData[a].lt);
                        CtlData[a].lt = nil;
                        break;
                    case 30:
                        b = CtlData[a].subcode;
                        TimersList[b].status = 0;
                        break;
                }
                CtlData[a].status = 0;
            }
        }
    }
    b = 0;
    for (a = 0; a < 50; a++) // clear timers for window
        if (TimersList[a].status > 1) b++;
    if (!b) TimersFlag = 0;
    GFX_WinRedraw(WinNum);
    return (0);
}

short CTL_Hide(short a) {
    if (CtlData[a].status) {
        switch (CtlData[a].status) {
            case 1: // Button
            case 2: // pulldown
            case 3: // checkbox
            case 4: // radio
            case 5: // scrollbar
                HideControl(CtlData[a].theControl);
                break;
            case 10:    // Textbox
            case 12:    // Label
            case 120:   // Picture
            case 125:    // Icon
                CtlData[a].status += 100;
                break;
            case 11:    // Textbox w/scrollbar
                CtlData[a].status = 111;
                HideControl(CtlData[a + 1].theControl);
                break;
            case 15: // List Box
                LSetDrawingMode(false, CtlData[a].lt);
                break;
        }
        return (1);
    }
    return (0);
}

short CTL_Show(short a) {
    if (CtlData[a].status) {
        switch (CtlData[a].status) {
            case 1: // Button
            case 2: // pulldown
            case 3: // checkbox
            case 4: // radio
            case 5: // scrollbar
                ShowControl(CtlData[a].theControl);
                break;
            case 110:    // Textbox
            case 112:    // Label
            case 120:   // Picture
            case 125:    // Icon
                CtlData[a].status -= 100;
                break;
            case 111:    // Textbox w/scrollbar
                CtlData[a].status = 11;
                ShowControl(CtlData[a + 1].theControl);
                break;
            case 15: // List Box
                LSetDrawingMode(true, CtlData[a].lt);
                break;
        }
        return (1);
    }
    return (0);
}

short CTL_Disable(short a) {
    short b, c;
    if (CtlData[a].status) {
        switch (CtlData[a].status) {
            case 1: // Button
            case 2: // pulldown
            case 3: // checkbox
            case 4: // radio
            case 5: // scrollbar
                HiliteControl(CtlData[a].theControl, 255);
                break;
            case 10:    // Textbox
            case 12:    // Label
                if (CtlData[a].te == ActiveTE) ActiveTE = nil;
                TEDeactivate(CtlData[a].te);
                break;
            case 11:    // Textbox w/scrollbar
                if (CtlData[a].te == ActiveTE) ActiveTE = nil;
                TEDeactivate(CtlData[a].te);
                HiliteControl(CtlData[a + 1].theControl, 255);
                break;
            case 15: // List Box
                LActivate(false, CtlData[a].lt);
                HiliteControl((**CtlData[a].lt).vScroll, 255);
                break;
            case 30:
                b = CtlData[a].subcode;
                TimersList[b].status = 1;
                b = 0;
                for (c = 0; c < 50; c++) // check timers for window
                    if (TimersList[c].status > 1) b++;
                if (!b) TimersFlag = 0;
                break;
        }
        CtlData[a].enabled = 0;
        return (1);
    }
    return (0);
}

short CTL_Enable(short a) {
    short b;
    if (CtlData[a].status) {
        switch (CtlData[a].status) {
            case 1: // Button
            case 2: // pulldown
            case 3: // checkbox
            case 4: // radio
            case 5: // scrollbar
                HiliteControl(CtlData[a].theControl, 0);
                break;
            case 10:    // Textbox
            case 12:    // Label
                TEActivate(CtlData[a].te);
                break;
            case 11:    // Textbox w/scrollbar
                TEActivate(CtlData[a].te);
                HiliteControl(CtlData[a + 1].theControl, 0);
                break;
            case 15: // List Box
                LActivate(true, CtlData[a].lt);
                HiliteControl((**CtlData[a].lt).vScroll, 0);
                break;
            case 30:
                b = CtlData[a].subcode;
                TimersList[b].status = 2;
                TimersList[b].next = TickCount() + TimersList[b].interval;
                TimersFlag = 1;
                break;
        }
        CtlData[a].enabled = 1;
        return (1);
    }
    return (0);
}

short CTL_Size(short a, short w, short h) {
    if (CtlData[a].status) {
        switch (CtlData[a].status) {
            case 1: // Button
            case 2: // pulldown
            case 3: // checkbox
            case 4: // radio
            case 5: // scrollbar
                SizeControl(CtlData[a].theControl, w, h);
                break;
            case 10:    // Textbox
            case 12:    // Label
                (*CtlData[a].te)->viewRect.right = (*CtlData[a].te)->viewRect.left + w;
                (*CtlData[a].te)->viewRect.bottom = (*CtlData[a].te)->viewRect.top + h;
                CtlData[a].theRect = (*CtlData[a].te)->viewRect;
                TEUpdate(&CtlData[a].theRect, CtlData[a].te);
                break;
            case 11:    // Textbox w/scrollbar
                CtlData[a].theRect.right = CtlData[a].theRect.left + (w - 16);
                CtlData[a].theRect.bottom = CtlData[a].theRect.top + h;
                (*CtlData[a].te)->viewRect.right = (*CtlData[a].te)->viewRect.left + (w - 16);
                (*CtlData[a].te)->viewRect.bottom = (*CtlData[a].te)->viewRect.top + h;
                (*CtlData[a].te)->destRect = (*CtlData[a].te)->viewRect;
                TEUpdate(&CtlData[a].theRect, CtlData[a].te);
                MoveControl(CtlData[a + 1].theControl, (*CtlData[a].te)->viewRect.right,
                            (*CtlData[a].te)->viewRect.top);
                SizeControl(CtlData[a + 1].theControl, 16, h);
                break;
            case 15: // List Box
                LSize(w, h, CtlData[a].lt);
                break;
        }
        return (1);
    }
    return (0);
}

short CTL_Move(short a, short x, short y) {
    short temp, zx, zy;
    Rect r;
    if (CtlData[a].status) {
        switch (CtlData[a].status) {
            case 1: // Button
            case 2: // pulldown
            case 3: // checkbox
            case 4: // radio
            case 5: // scrollbar
                MoveControl(CtlData[a].theControl, x, y);
                break;
            case 10:    // Textbox
            case 12:    // Label
                temp = (*CtlData[a].te)->viewRect.right - (*CtlData[a].te)->viewRect.left;
                (*CtlData[a].te)->viewRect.left = x;
                (*CtlData[a].te)->viewRect.right = x + temp;
                temp = (*CtlData[a].te)->viewRect.bottom - (*CtlData[a].te)->viewRect.top;
                (*CtlData[a].te)->viewRect.top = y;
                (*CtlData[a].te)->viewRect.bottom = y + temp;
                break;
            case 11:    // Textbox w/scrollbar
                temp = (*CtlData[a].te)->viewRect.right - (*CtlData[a].te)->viewRect.left;
                (*CtlData[a].te)->viewRect.left = x;
                (*CtlData[a].te)->viewRect.right = x + temp;
                temp = (*CtlData[a].te)->viewRect.bottom - (*CtlData[a].te)->viewRect.top;
                (*CtlData[a].te)->viewRect.top = y;
                (*CtlData[a].te)->viewRect.bottom = y + temp;
                MoveControl(CtlData[a + 1].theControl, (*CtlData[a].te)->viewRect.right - 16, y);
                break;
            case 15: // List Box
                temp = (*CtlData[a].lt)->rView.right - (*CtlData[a].lt)->rView.left;
                (*CtlData[a].lt)->rView.left = x;
                (*CtlData[a].lt)->rView.right = x + temp;
                temp = (*CtlData[a].lt)->rView.bottom - (*CtlData[a].lt)->rView.top;
                (*CtlData[a].lt)->rView.top = y;
                (*CtlData[a].lt)->rView.bottom = y + temp;
                break;
            case 20:
                r = CtlData[a].theRect;
                InvalRect(&r);
                zx = x - r.left;
                zy = y - r.top;
                OffsetRect(&r, zx, zy);
                CtlData[a].theRect = r;
                LSize(r.right - r.left, r.bottom - r.top, CtlData[a].lt);
                InvalRect(&r);
                //GFX_WinRedraw( CtlData[a].WinNum);
                break;
        }
        return (1);
    }
    return (0);
}

short CTL_Align(short a, short way) {
    if (way == 2) way = -1;
    if (way < 0 || way > 2) way = 0;
    if (CtlData[a].status) {
        switch (CtlData[a].status) {
            case 10:    // Textbox
            case 11:    // Textbox w/scrollbar
            case 12:    // Label
                TESetAlignment(-1, CtlData[a].te);
                break;
        }
        return (1);
    }
    return (0);
}

short CTL_Hilite(short a) {
    Rect r;
    PenState ps;
    if (CtlData[a].status) {
        GetPenState(&ps);
        r = CtlData[a].theRect;
        InsetRect(&r, -4, -4);
        PenSize(3, 3);
        FrameRoundRect(&r, 20, 20);
        SetPenState(&ps);
    }
    return (0);
}

short TE_Add(TEHandle te, short WinNum, UCHAR status, short cmd, Rect *theRect) {
    short a, TEflag = 0;
    ControlHandle Ctl;
    Rect tRect;
    if (status == 10 || status == 12) {
        for (a = 0; a < 500; a++) {
            if (!CtlData[a].status) {
                CtlData[a].te = te;
                CtlData[a].WinNum = WinNum;
                CtlData[a].status = status;
                CtlData[a].subcode = cmd;
                CtlData[a].theRect = *theRect;
                CtlData[a].lastItem = 0;
                if (!TEflag && status == 10) {
                    ActiveTE = CtlData[a].te;
                    TEActivate(ActiveTE);
                    TESetSelect(0, 32767, CtlData[a].te);
                    TESelView(CtlData[a].te);
                }
                return (a);
            } else {
                if (CtlData[a].WinNum == WinNum && (CtlData[a].status == 10)) TEflag++;
            }
        }
    } else {
        for (a = 0; a < 499; a++) {
            if (!CtlData[a].status && !CtlData[a + 1].status) {
                CtlData[a].te = te;
                CtlData[a].WinNum = WinNum;
                CtlData[a].status = 11;
                CtlData[a].subcode = cmd;
                CtlData[a].theRect = *theRect;
                CtlData[a].lastItem = 0;
                if (!TEflag) {
                    ActiveTE = CtlData[a].te;
                    TEActivate(ActiveTE);
                    TESetSelect(0, 32767, CtlData[a].te);
                    TESelView(CtlData[a].te);
                }
                a++;
                tRect = *theRect;
                tRect.left = tRect.right;
                tRect.right = tRect.left + 16;
                Ctl = NewControl(WinData[WinNum].theWindow, &tRect, "\p", TRUE, 0, 0, 1, 16, 0);
                CtlData[a].theControl = Ctl;
                CtlData[a].WinNum = WinNum;
                CtlData[a].status = 6;
                CtlData[a].subcode = cmd;
                CtlData[a].theRect = tRect;
                a--;
                return (a);
            } else {
                if (CtlData[a].WinNum == WinNum && (CtlData[a].status == 10 || CtlData[a].status == 11)) TEflag++;
            }
        }
    }
    ErrHandler(1046, nil);
    return (-1);
}

short LT_Add(ListHandle lt, short WinNum, UCHAR status, short cmd, Rect *theRect) {
    short a;
    for (a = 0; a < 500; a++) {
        if (!CtlData[a].status) {
            CtlData[a].lt = lt;
            CtlData[a].WinNum = WinNum;
            CtlData[a].status = status;
            CtlData[a].subcode = cmd;
            CtlData[a].theRect = *theRect;
            return (a);
        }
    }
    ErrHandler(1046, nil);
    return (-1);
}

short Pic_Add(PicHandle pic, short WinNum, UCHAR status, short cmd, Rect *theRect) {
    short a;
    for (a = 0; a < 500; a++) {
        if (!CtlData[a].status) {
            CtlData[a].pic = pic;
            CtlData[a].WinNum = WinNum;
            CtlData[a].status = status;
            CtlData[a].subcode = cmd;
            CtlData[a].theRect = *theRect;
            CtlData[a].lastItem = 0;
            return (a);
        }
    }
    ErrHandler(1046, nil);
    return (-1);
}

short Icon_Add(short WinNum, UCHAR status, short cmd, Rect *theRect) {
    short a;
    for (a = 0; a < 500; a++) {
        if (!CtlData[a].status) {
            CtlData[a].WinNum = WinNum;
            CtlData[a].status = status;
            CtlData[a].subcode = cmd;
            CtlData[a].theRect = *theRect;
            CtlData[a].lastItem = 0;
            return (a);
        }
    }
    ErrHandler(1046, nil);
    return (-1);
}

short CTL_DropList(short WinNum, short x1, short y1, short width, short cmd) {
    Rect tempRect;
    ControlHandle Ctl;
    short a = 0, cn = 0;
    MenuHandle theMenu1;
    if (WinData[WinNum].status < 1) return (-1);
    SetRect(&tempRect, x1, y1, x1 + width, y1 + 20);
    cn = CTL_NextFreeCtl();
    if (cn >= 0) {
        cn += 10000;
        theMenu1 = NewMenu(cn, "\pMenu");
        InsertMenu(theMenu1, -1);
    }
    Ctl = NewControl(WinData[WinNum].theWindow, &tempRect, "\p", true, 0, cn, 0,/*popupMenuProc*/400, 0);
    if (!ControlVisibleFlag) HideControl(Ctl);
    a = CTL_Add(Ctl, WinNum, 2, cmd, &tempRect);
    CtlData[a].theMenu = theMenu1;
    CtlData[a].lastItem = 0;
    CtlData[a].CtlID = -1; // Will be changed if loaded by Form Loader
    CtlData[a].enabled = 1;
    return (a);
}

short CTL_Button(short WinNum, short x1, short y1, short wide, short ht, char *btext, short cmd) {
    Rect tempRect;
    ControlHandle Ctl;
    Str255 txt;
    short a;
    if (WinData[WinNum].status < 1) return (-1);
    SetRect(&tempRect, x1, y1, x1 + wide, y1 + ht);
    SetPort(WinData[WinNum].theWindow);
    StrToStr255(btext, txt);
    Ctl = NewControl(WinData[WinNum].theWindow, &tempRect, txt, ControlVisibleFlag, 0, 0, 1, 0, 0);
    if (!ControlVisibleFlag) HideControl(Ctl);
    a = CTL_Add(Ctl, WinNum, 1, cmd, &tempRect);
    CtlData[a].CtlID = -1; // Will be changed if loaded by Form Loader
    CtlData[a].enabled = 1;
    return (a);
}

short CTL_CheckBox(short WinNum, short x1, short y1, short wide, short ht, char *btext, short cmd) {
    Rect tempRect;
    ControlHandle Ctl;
    Str255 txt;
    short a;
    if (WinData[WinNum].status < 1) return (-1);
    SetRect(&tempRect, x1, y1, x1 + wide, y1 + ht);
    SetPort(WinData[WinNum].theWindow);
    StrToStr255(btext, txt);
    Ctl = NewControl(WinData[WinNum].theWindow, &tempRect, txt, ControlVisibleFlag, 0, 0, 1, 1, 0);
    if (!ControlVisibleFlag) HideControl(Ctl);
    a = CTL_Add(Ctl, WinNum, 3, cmd, &tempRect);
    CtlData[a].CtlID = -1; // Will be changed if loaded by Form Loader
    CtlData[a].enabled = 1;
    return (a);
}

short CTL_Radio(short WinNum, short x1, short y1, short wide, short ht, short group, char *btext, short cmd) {
    Rect tempRect;
    ControlHandle Ctl;
    Str255 txt;
    short a;
    if (WinData[WinNum].status < 1) return (-1);
    SetRect(&tempRect, x1, y1, x1 + wide, y1 + ht);
    SetPort(WinData[WinNum].theWindow);
    StrToStr255(btext, txt);
    Ctl = NewControl(WinData[WinNum].theWindow, &tempRect, txt, ControlVisibleFlag, 0, 0, 1, 2, 0);
    if (!ControlVisibleFlag) HideControl(Ctl);
    a = CTL_Add(Ctl, WinNum, 4, cmd, &tempRect);
    if (a >= 0) CtlData[a].lastItem = group;
    CtlData[a].CtlID = -1; // Will be changed if loaded by Form Loader
    CtlData[a].enabled = 1;
    return (a);
}

short CTL_ScrollBar(short WinNum, short x1, short y1, short wide, short ht, short cmd) {
    Rect tempRect;
    ControlHandle Ctl;
    short a;
    if (WinData[WinNum].status < 1) return (-1);
    SetRect(&tempRect, x1, y1, x1 + wide, y1 + ht);
    SetPort(WinData[WinNum].theWindow);
    Ctl = NewControl(WinData[WinNum].theWindow, &tempRect, "\pScroll", ControlVisibleFlag, 0, 0, 1, 16, 0);
    if (!ControlVisibleFlag) HideControl(Ctl);
    SetControlMinimum(Ctl, 1);
    SetControlMaximum(Ctl, 100);
    a = CTL_Add(Ctl, WinNum, 5, cmd, &tempRect);
    CtlData[a].lastItem = 10;
    CtlData[a].CtlID = -1; // Will be changed if loaded by Form Loader
    CtlData[a].enabled = 1;
    return (a);
}

short CTL_ScrollSet(short ctl, short min, short max, short pager) {
    if (CtlData[ctl].status != 5) return (-1);
    SetControlMinimum(CtlData[ctl].theControl, min);
    SetControlMaximum(CtlData[ctl].theControl, max);
    CtlData[ctl].lastItem = pager;
    return (0);
}

short
CTL_Textbox(short WinNum, short x1, short y1, short wide, short ht, short opts, short opts2, char *btext, short cmd) {
    // opts 1 = scroll bar
    Rect tempRect, tr;
    TEHandle te;
    short a, b = 10, g, tl;
    RGBColor black1, white1;
    if (WinData[WinNum].status < 1) return (-1);
    if (opts & 1) {
        b++;  // set for scrollbars
        SetRect(&tempRect, x1, y1, x1 + wide - 16, y1 + ht);
    } else SetRect(&tempRect, x1, y1, x1 + wide, y1 + ht);
    SetPort(WinData[WinNum].theWindow);
    tr = tempRect;
    if ((opts & 1) == 0 || opts2) tr.right = 2000;
    EraseRect(&tempRect);
    te = TEStyleNew(&tr, &tempRect);
    TEAutoView(true, te);
    InsetRect(&tempRect, -1, -1);
    tr = tempRect;
    if (opts & 4) {
        tr.right++;
        FrameRect(&tr);
    }
    a = TE_Add(te, WinNum, b, cmd, &tempRect);
    if (a >= 0) {
        for (tl = 0; tl < 256; tl++) if (!btext[tl]) break;
        TEInsert(btext, tl, te);
        if (opts & 1) {
            g = CalcTextPages(a);
            SetControlMinimum(CtlData[a + 1].theControl, 0);
            SetControlMaximum(CtlData[a + 1].theControl, (**CtlData[a].te).nLines - CalcTextView(a));
            if (!g) HiliteControl(CtlData[a + 1].theControl, 255);  // Disable scrollbar
        }
        CtlData[a].lastItem = opts;  // Read Only Textbox
    }
    if (opts & 4) CtlData[a].opt2 = 0;
    else CtlData[a].opt2 = 1;
    CtlData[a].CtlID = -1; // Will be changed if loaded by Form Loader
    black1.red = 0;
    black1.green = 0;
    black1.blue = 0;
    white1.red = 0xffff;
    white1.green = 0xffff;
    white1.blue = 0xffff;
    CtlData[a].ForeRGB = black1;
    CtlData[a].BackRGB = white1;
    CtlData[a].enabled = 1;
    return (a);
}

short CalcTextPages(short cnum) {
    short a, b, c;
    a = TEGetHeight(0, 0, CtlData[cnum].te);
    if (!a) return (0);
    b = CtlData[cnum].theRect.bottom - CtlData[cnum].theRect.top;
    c = b / a;    // c now equals # of line viewable
    a = (**CtlData[cnum].te).nLines;
    if (c >= a) return (0);
    b = (a / c);
    return (b);
}

short CalcTextView(short cnum)   // Returns lines viewable
{
    short a, b, c;
    a = TEGetHeight(0, 0, CtlData[cnum].te);
    if (!a) return (0);
    b = CtlData[cnum].theRect.bottom - CtlData[cnum].theRect.top;
    c = b / a;    // c now equals # of line viewable
    return (c);
}

short CTL_List(short WinNum, short x1, short y1, short wide, short ht, short option, short cmd) {
    Rect tempRect, bounds;
    ListHandle theList;
    Point CellSz;
    short a;
    RGBColor black1, white1;
    if (WinData[WinNum].status < 1) return (-1);
    SetRect(&tempRect, x1, y1, x1 + wide, y1 + ht);
    SetRect(&bounds, 0, 0, 0, 0);
    CellSz.h = 0;
    CellSz.v = 0;
    SetPort(WinData[WinNum].theWindow);
    theList = LNew(&tempRect, &bounds, CellSz, 0, WinData[WinNum].theWindow, ControlVisibleFlag, false, false, true);
    if (!ControlVisibleFlag) LSetDrawingMode(false, theList);
    a = LAddColumn(1, 0, theList);
    if (ControlVisibleFlag && !(option & 2)) FrameRect(&tempRect);
    a = LT_Add(theList, WinNum, 15, cmd, &tempRect);
    if (a >= 0) CtlData[a].lastItem = option;
    CtlData[a].CtlID = -1; // Will be changed if loaded by Form Loader
    black1.red = 0;
    black1.green = 0;
    black1.blue = 0;
    white1.red = 0xffff;
    white1.green = 0xffff;
    white1.blue = 0xffff;
    CtlData[a].ForeRGB = black1;
    CtlData[a].BackRGB = white1;
    CtlData[a].enabled = 1;
    return (a);
}

short CTL_Label(short WinNum, short x1, short y1, short width, short height, char *btext, short cmd) {
    Rect tempRect;
    TEHandle te;
    short a, tl;
    long strlen, *lp;
    WindowPtr wp;
    RGBColor black1;
    if (WinData[WinNum].status < 1) return (-1);
    lp = (long *) btext;
    strlen = *lp;
    SetPort(WinData[WinNum].theWindow);
    wp = WinData[WinNum].theWindow;
    SetRect(&tempRect, x1, y1, x1 + width, y1 + height);
    te = TEStyleNew(&tempRect, &tempRect);
    a = TE_Add(te, WinNum, 12, cmd, &tempRect);
    // Put text in box (if any)
    for (tl = 0; tl < 256; tl++) if (!btext[tl]) break;
    if (a >= 0 && tl >= 0) TEInsert(btext, tl, te);
    CtlData[a].CtlID = -1; // Will be changed if loaded by Form Loader
    black1.red = 0;
    black1.green = 0;
    black1.blue = 0;
    CtlData[a].ForeRGB = black1;
    CtlData[a].BackRGB = WinData[WinNum].BackRGB;
    CtlData[a].enabled = 1;
    return (a);
}

short CTL_Picture(short WinNum, short x1, short y1, short wide, short ht, short ResID, short cmd) {
    Rect tempRect, r;
    PicHandle thePict;
    short a, w, h;
    if (!WinData[WinNum].status) return (-1);
    if (ResID < 30000) thePict = GetPicture(ResID);
    else {
        a = ResID - 30000;
        if (a < 0 || a > 99) return (-1);
        thePict = Picts[a];
    }
    if (thePict == nil) return (-1);
    tempRect = (*thePict)->picFrame;
    w = tempRect.right - tempRect.left;
    h = tempRect.bottom - tempRect.top;
    SetPort(WinData[WinNum].theWindow);
    if (!wide && !ht) {
        // Draw picture in original size
        SetRect(&r, x1, y1, x1 + w, y1 + h);
        DrawPicture(thePict, &r);
    } else {
        // Draw picture in original size
        SetRect(&r, x1, y1, x1 + wide, y1 + ht);
        DrawPicture(thePict, &r);
    }
    a = Pic_Add(thePict, WinNum, 20, cmd, &r);
    if (a >= 0) CtlData[a].lastItem = ResID;
    CtlData[a].CtlID = -1; // Will be changed if loaded by Form Loader
    CtlData[a].enabled = 1;
    return (a);
}

short CTL_LgIcon(short WinNum, short x1, short y1, short width, short height, short ResID, short cmd) {
    Rect r;
    short a = 0;
    OSErr err;
    if (!WinData[WinNum].status) return (-1);
    SetPort(WinData[WinNum].theWindow);
    SetRect(&r, x1, y1, x1 + width, y1 + height); // 32x32
    SetResLoad(true);
    err = PlotIconID(&r, atAbsoluteCenter, ttNone, ResID);
    if (err) ErrHandler(1020, nil);
    a = Icon_Add(WinNum, 25, cmd, &r);
    if (a >= 0) CtlData[a].lastItem = ResID;
    CtlData[a].CtlID = -1; // Will be changed if loaded by Form Loader
    CtlData[a].enabled = 1;
    return (a);
}

short CTL_Timer(short WinNum, long interval, short enabled, short cmd) {
    short a, b;
    for (a = 0; a < 50; a++) {
        if (!TimersList[a].status) {
            if (!enabled) TimersList[a].status = 1;
            else {
                TimersList[a].status = 2;  //enabled
                TimersFlag = 1;
            }
            TimersList[a].interval = interval;
            TimersList[a].WinNum = WinNum;
            TimersList[a].cmd = cmd;
            TimersList[a].next = TickCount() + interval;
            for (b = 0; b < 500; b++)
                if (!CtlData[b].status) {
                    CtlData[b].status = 30; //timer status
                    CtlData[b].WinNum = WinNum;
                    CtlData[b].subcode = a;
                    break;
                }
            return b;
        }
    }
    return (-1);
}

long CTL_GetValue(short a) {
    long b = 0;
    short c;
    if (CtlData[a].status) {
        switch (CtlData[a].status) {
            case 1: // Controls
            case 2:
            case 3:
            case 4:
            case 5:
                b = (long) GetControlValue(CtlData[a].theControl);
                break;
            case 15:
                b = CTL_ListCount(a);
                if (b > 0) {
                    for (c = 0; c <= b; c++) if (CTL_IsListSelected(a, c)) return (c);
                }
                break;
            case 20: //Picture
            case 25: // Icon
                b = CtlData[a].lastItem;
                break;
            case 30: // timer
                c = CtlData[a].subcode;
                b = TimersList[c].interval;
                break;
        }
    }
    return (b);
}

short CTL_SetValue(short a, long b) {
    short w, c;
    if (CtlData[a].status) {
        switch (CtlData[a].status) {
            case 1: // Controls
            case 2:
            case 3:
            case 4:
            case 5:
                SetControlValue(CtlData[a].theControl, (short) b);
                break;
            case 15:
                CTL_ListSelect(a, b);
                break;
            case 20: //Picture
                if (b < 30000) CtlData[a].pic = GetPicture((short) b);
                else {
                    c = b - 30000;
                    if (a < 0 || a > 99) return (-1);
                    CtlData[a].pic = Picts[c];
                }
                CtlData[a].lastItem = (short) b;
                w = CtlData[a].WinNum;
                SetPort(WinData[w].theWindow);
                InvalRect(&CtlData[a].theRect);
                break;
            case 25: // Icon
                CtlData[a].lastItem = (short) b;
                break;
            case 30: // timer
                c = CtlData[a].subcode;
                TimersList[c].interval = b;
                break;
        }
    }
    return (0);
}

short CTL_GetText(short a) {
    short b;
    long slen, c;
    Str255 txt1;
    CharsHandle chdl;
    if (CtlData[a].status) {
        switch (CtlData[a].status) {
            case 2:  // DropList
                b = GetControlValue(CtlData[a].theControl);
                CTL_ListItem(a, b);
                break;
            case 1: // Controls
            case 3:
            case 4:
            case 5:
                GetControlTitle(CtlData[a].theControl, txt1);
                // Allocate ReturnStr Space
                b = Allocate_ReturnStr(txt1[0] + 16);
                if (b) return (-1); // Buffer allocation error
                b = 1;
                for (c = 0; c <= txt1[0]; c++) ReturnStr[c] = txt1[b++];
                ReturnStrLen = txt1[0];
                ReturnStr[ReturnStrLen] = 0;
                break;
            case 10: // Textbox, Label
            case 11:
            case 12:
                slen = (*CtlData[a].te)->teLength;
                chdl = TEGetText(CtlData[a].te);
                // Allocate ReturnStr Space
                b = Allocate_ReturnStr(slen + 16);
                if (b) return (-1); // Buffer allocation error
                for (c = 0; c < slen; c++) ReturnStr[c] = *((*chdl) + c);
                ReturnStrLen = slen;
                ReturnStr[ReturnStrLen] = 0;
                break;
            case 15:
                b = CTL_ListCount(a);
                if (b > 0) {
                    for (c = 0; c <= b; c++) {
                        if (CTL_IsListSelected(a, c)) {
                            CTL_ListItem(a, c);
                            return (1);
                        }
                    }
                }
                break;
        }
        return (1);
    }
    return (0);
}

short CTL_SetText(short a, short appendFlag) {
    long x;
    Str255 txt1;
    TextStyle tes;
    if (CtlData[a].status) {
        x = CtlData[a].WinNum;
        SetPort(WinData[x].theWindow);
        switch (CtlData[a].status) {
            case 1: // Controls
            case 2:
            case 3:
            case 4:
            case 5:
                StrToStr255((char *) ReturnStr, txt1);
                SetControlTitle(CtlData[a].theControl, txt1);
                break;
            case 10: // Textbox, Label
            case 11:
            case 12:
                RGBForeColor(&CtlData[a].ForeRGB);
                RGBBackColor(&CtlData[a].BackRGB);
                if (!appendFlag) {
                    TESetSelect(0, 32767, CtlData[a].te);
                    TEDelete(CtlData[a].te);
                }
                if (appendFlag != 2) TESetSelect(32767, 32767, CtlData[a].te);
                for (x = 0; x < ReturnStrLen; x++) {
                    if (ReturnStr[x] == 10) ReturnStr[x] = 32;
                }
                TEInsert(ReturnStr, ReturnStrLen, CtlData[a].te);
                if (appendFlag != 2) TESetSelect(32767, 32767, CtlData[a].te);
                TESelView(CtlData[a].te);
                if (CtlData[a].status == 11) TextFix(a);
                break;
        }
        return (1);
    }
    return (0);
}

short CTL_ListAdd(short ListNum, unsigned char *buf, long buflen) {
    short a;
    Point cSize;
    Str255 txt;
    if (!(CtlData[ListNum].status == 2 || CtlData[ListNum].status == 15)) return (-1);
    if (CtlData[ListNum].status == 15) {
        // Listbox
        a = CtlData[ListNum].WinNum;
        SetPort(WinData[a].theWindow);
        RGBForeColor(&CtlData[ListNum].ForeRGB);
        RGBBackColor(&CtlData[ListNum].BackRGB);
        a = LAddRow(1, 30000, CtlData[ListNum].lt);
        SetPt(&cSize, 0, a);
        LSetCell(buf, (short) buflen, cSize, CtlData[ListNum].lt);
        LDraw(cSize, CtlData[ListNum].lt);
        InvalRect(&CtlData[ListNum].theRect);
    } else {
        StrToStr255((char *) buf, txt);
        a = CtlData[ListNum].WinNum;
        SetPort(WinData[ListNum].theWindow);
        AppendMenu(CtlData[ListNum].theMenu, txt);
    }
    return (0);
}

short CTL_ListClear(short ListNum) {
    short a, b;
    if (!(CtlData[ListNum].status == 2 || CtlData[ListNum].status == 15)) return (-1);
    if (CtlData[ListNum].status == 15) {
        a = CtlData[ListNum].WinNum;
        SetPort(WinData[a].theWindow);
        RGBForeColor(&CtlData[ListNum].ForeRGB);
        RGBBackColor(&CtlData[ListNum].BackRGB);
        LDelRow(0, 0, CtlData[ListNum].lt);
    } else {
        a = CountMItems(CtlData[ListNum].theMenu);
        for (b = a; b > 0; b--) DeleteMenuItem(CtlData[ListNum].theMenu, b);
    }
    InvalRect(&CtlData[ListNum].theRect);
    return (0);
}

short CTL_ListRemove(short ListNum, short item) {
    short a, b;
    if (!(CtlData[ListNum].status == 2 || CtlData[ListNum].status == 15)) return (-1);
    if (CtlData[ListNum].status == 15) {
        item--;
        b = CTL_ListCount(ListNum);
        if (!b || item < 0 || item > b) return (0);
        a = CtlData[ListNum].WinNum;
        SetPort(WinData[a].theWindow);
        RGBForeColor(&CtlData[ListNum].ForeRGB);
        RGBBackColor(&CtlData[ListNum].BackRGB);
        LDelRow(1, item, CtlData[ListNum].lt);
    } else {
        DeleteMenuItem(CtlData[ListNum].theMenu, item);
    }
    InvalRect(&CtlData[ListNum].theRect);
    return (0);
}

short CTL_IsListSelected(short ListNum, short item) {
    short a, b;
    Point cl;
    if (CtlData[ListNum].status != 15) return (-1);
    item--;
    b = CTL_ListCount(ListNum);
    if (item >= b) return (0);
    SetPt(&cl, 0, item);
    a = LGetSelect(0, &cl, CtlData[ListNum].lt);
    if (a) return (1);
    return (0);
}

short CTL_ListSelect(short ListNum, short item) {
    short a, b;
    Point cl;
    if (CtlData[ListNum].status != 15) return (-1);
    item--;
    b = CTL_ListCount(ListNum);
    if (item >= b) return (0);
    RGBForeColor(&CtlData[ListNum].ForeRGB);
    RGBBackColor(&CtlData[ListNum].BackRGB);
    if ((CtlData[ListNum].lastItem & 1) == 0) {
        for (a = 0; a < b; a++) {
            SetPt(&cl, 0, a);
            LSetSelect(false, cl, CtlData[ListNum].lt);
        }
    }
    SetPt(&cl, 0, item);
    LSetSelect(true, cl, CtlData[ListNum].lt);
    InvalRect(&CtlData[ListNum].theRect);
    return (0);
}

short CTL_ListDeselect(short ListNum, short item) {
    short b;
    Point cl;
    if (CtlData[ListNum].status != 15) return (-1);
    item--;
    b = CTL_ListCount(ListNum);
    if (item >= b) return (0);
    RGBForeColor(&CtlData[ListNum].ForeRGB);
    RGBBackColor(&CtlData[ListNum].BackRGB);
    SetPt(&cl, 0, item);
    LSetSelect(false, cl, CtlData[ListNum].lt);
    InvalRect(&CtlData[ListNum].theRect);
    return (0);
}

short CTL_SelectCount(short ListNum) {
    short a, b, c, ctr = 0;
    Point cl;
    if (CtlData[ListNum].status != 15) return (-1);
    b = CTL_ListCount(ListNum);
    for (c = 0; c <= b; c++) {
        SetPt(&cl, 0, c);
        a = LGetSelect(0, &cl, CtlData[ListNum].lt);
        if (a) ctr++;
    }
    return (ctr);
}

short CTL_ListItem(short ListNum, short item) {
    short a, b, c, ln, d;
    Point theCell;
    char txt[258];
    if (!(CtlData[ListNum].status == 2 || CtlData[ListNum].status == 15)) return (-1);
    if (CtlData[ListNum].status == 15) {
        b = CTL_ListCount(ListNum);
        item--;
        if (!b || item < 0 || item >= b) return (0);
        a = CtlData[ListNum].WinNum;
        SetPort(WinData[a].theWindow);
        ln = 255;
        SetPt(&theCell, 0, item);
        LGetCell(txt, &ln, theCell, CtlData[ListNum].lt);
        txt[ln] = 0;
        b = Allocate_ReturnStr(ln + 16);
        if (b) return (-1); // Buffer allocation error
        for (c = 0; c < ln; c++) ReturnStr[c] = txt[c];
        ReturnStrLen = ln;
        ReturnStr[ReturnStrLen] = 0;
    } else {
        GetMenuItemText(CtlData[ListNum].theMenu, item, (UCHAR *) txt);
        d = txt[0];
        // Allocate ReturnStr Space
        b = Allocate_ReturnStr(d + 16);
        if (b) return (-1); // Buffer allocation error
        for (c = 0; c < d; c++) ReturnStr[c] = txt[c + 1];
        ReturnStrLen = d;
        ReturnStr[ReturnStrLen] = 0;
    }
    return (0);
}

short CTL_ListCount(short ListNum) {
    short a;
    Rect theRect;
    if (!(CtlData[ListNum].status == 2 || CtlData[ListNum].status == 15)) return (-1);
    if (CtlData[ListNum].status == 15) {
        theRect = (**CtlData[ListNum].lt).dataBounds;
        return (theRect.bottom);
    }
    a = CountMItems(CtlData[ListNum].theMenu);
    return (a);
}

void TextFix(short a) {
    short b, c;
    b = CalcTextPages(a);
    c = (**CtlData[a].te).nLines - CalcTextView(a);
    SetControlMaximum(CtlData[a + 1].theControl, c);
    if (!b) HiliteControl(CtlData[a + 1].theControl, 255);  // Disable scrollbar
    else HiliteControl(CtlData[a + 1].theControl, 0);  // Disable scrollbar
    ChangeScrollBar(a);
}

short CTL_TextboxRO(short ctl, short ro) {
    if (CtlData[ctl].status != 10 && CtlData[ctl].status != 11) return (-1);
    if (ro) ro = 1;
    CtlData[ctl].lastItem = ro;
    return (0);
}

short CTL_SetTextSelect(short ctl, short start, short end) {
    if (CtlData[ctl].status < 10 || CtlData[ctl].status > 12) return (-1);
    if (ActiveTE != nil) TEDeactivate(ActiveTE);
    ActiveTE = CtlData[ctl].te;
    TEActivate(ActiveTE);
    TESetSelect(start, end, CtlData[ctl].te);
    TESelView(CtlData[ctl].te);
    return (0);
}

short CTL_GetTextStart(short ctl) {
    short a;
    if (CtlData[ctl].status != 10 && CtlData[ctl].status != 11) return (-1);
    a = (*CtlData[ctl].te)->selStart;
    return (a);
}

short CTL_GetTextEnd(short ctl) {
    short a;
    if (CtlData[ctl].status != 10 && CtlData[ctl].status != 11) return (-1);
    a = (*CtlData[ctl].te)->selEnd;
    return (a);
}

short CTL_TextLen(short ctl) {
    short a;
    if (CtlData[ctl].status != 10 && CtlData[ctl].status != 11) return (-1);
    a = (*CtlData[ctl].te)->teLength;
    return (a);
}

short CTL_Border(short ctl, short dm) {
    short a = 0;
    if (!CtlData[ctl].status) return (0);
    switch (dm) {
        case 1:
            a = CtlData[ctl].theRect.left;
            break;
        case 2:
            a = CtlData[ctl].theRect.top;
            break;
        case 3:
            a = CtlData[ctl].theRect.right - CtlData[ctl].theRect.left;
            break;
        case 4:
            a = CtlData[ctl].theRect.bottom - CtlData[ctl].theRect.top;
            break;
    }
    return (a);
}

short CTL_TextSelect(short ctl) {
    short slen, a, b, c;
    CharsHandle chdl;
    if (CtlData[ctl].status != 10 && CtlData[ctl].status != 11) return (-1);
    a = (*CtlData[ctl].te)->selStart;
    slen = (*CtlData[ctl].te)->selEnd - (*CtlData[ctl].te)->selStart;
    if (slen < 0) slen = 0;
    chdl = TEGetText(CtlData[ctl].te);
    // Allocate ReturnStr Space
    b = Allocate_ReturnStr(slen + 16);
    if (b) return (-1); // Buffer allocation error
    for (c = 0; c < slen; c++) ReturnStr[c] = *((*chdl) + c + a);
    ReturnStrLen = slen;
    ReturnStr[ReturnStrLen] = 0;
    return (0);
}

long CTL_GetHandle(short a) {
    long result = -1;
    if (CtlData[a].status) {
        switch (CtlData[a].status) {
            case 1: // Button
            case 2: // pulldown
            case 3: // checkbox
            case 4: // radio
            case 5: // scrollbar
            case 6: // scrollbar
                result = (long) CtlData[a].theControl;
                break;
            case 10:    // Textbox
            case 11:    // TextBox w/scrollbar
            case 12:    // Label
                result = (long) CtlData[a].te;
                break;
            case 15: // List Box
                result = (long) CtlData[a].lt;
                break;
        }
    }
    return result;
}

short GetCtlStatus(short a) {
    if (CtlData[a].status) return 1;
    return 0;
}
