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
#include    "Controls.h"
#include    "Quickdraw.h"
#include    "TextEdit.h"
#include    "Files.h"
#include    "ctype.h"
#include    "IDE.h"
#include    <DCon.h>
#include    "stdio.h"
#include    <math.h>

short CreatePropWindow();

short StrPartCmp(char *st1, char *st2);

void StrToStr255(char *st1, Str255 st2);

void Str255Copy(Str255 st1, Str255 st2);

void Str255Cat(Str255 st1, Str255 st2);

short CheckForEdit(WindowPtr window);

short CreateEditorWindow(short createFlag, Str255 title);

void LoadEdit(short ctlNum, short b);

void EditorDispose();

void LoadEditPullDown(short b);

void LoadEditBox(short b, Str255 text2);

short SaveEditBox(short b);

short CloseEditWindow(WindowPtr window);

void SaveFormContents(short fnum, short formNum);

void WriteProps(short fnum, short objecttype, short objnum);

short LoadFormContents(short formNum);

short SaveSourceFile(WindowPtr window);

short SaveProject();

short LoadSourceFile(short fddi, Str255 fname);

void CreateNewProc(short tp, short b);

void LoadGlobal(short b);

short DoEditCheck(short b);

void TextFix(WindowPtr window);

short FindFormOrEdit(WindowPtr window);

void IDEmemErr();

short IsFileOpen(Str63 fname1);

void EditPager(short WinNum, short partCode);

short IsFormEdit(WindowPtr window);

short FindTEwindow();

short IsModuleEdit(WindowPtr window);

pascal void ScrollProcTE(ControlHandle theControl, short partCode);

void zsplit();

short znext_line();

extern char *zbuffer, zline[512];
extern short zline_pos, zline_len;
extern long zbuf_pos, zsource_len, ztstart;
extern Str255 zparm1, zparm2;
extern MenuHandle FormsSubMenu;
extern MenuHandle ModulesSubMenu;
extern MenuHandle ResourcesSubMenu;
extern WindowPtr PropWin;
extern TEHandle ActiveTE;
short EditorFlag = 0;
extern short FormCtr, ModuleCtr, SaveAsFlag;
aDocumentRecord appr1;
struct EditProcStruct {
    Handle ProcHandle;
    short EditWinNum;
    long size;
}
        EditProc[1000];
// Var Declares
struct EditWinStruct  // 0-19 for forms - 20-99 reserved for modules
{
    Str63 FileName;
    WindowPtr theWindow;
    TEHandle te;
    ControlHandle theControl;        //scrollbar
    ControlHandle dropList;        //dropList
    MenuHandle theMenu;
    short status;
    Rect theRect;
    short ht;
    short FormNum;
    short change;
    short editFlag;   // 1=New Object // 2=New Ind. Proc  // 3 existing proc
    short currentProc;
    FSSpec fs;
    Handle GlobalVar;
    long GlobalVarSize;
}
        EditWin[100];
short GWN = 0, GWHt = 0;
extern struct FormWins {
    short WinType;
    Rect loc;
    short ctlSelected;
    short status;
} Forms[20];
extern WindowPtr FormWin[20];
extern FSSpec PjtFS;
extern Rect MasterRect;
extern short CurrentForm;
extern struct IntControlStruct {
    ControlHandle ctl;
    TEHandle te;
    ListHandle lt;            // Thats a lowercase L in lt
    PicHandle pic;
    Rect theRect;
    short WinNum;
    short status;
    short subcode;
    MenuHandle theMenu;
    short lastItem;
} CtlData[200];
extern struct ObjectPropsStruct {
    Str63 name;
    Str255 text;
    short left, top, width, height;
    Str63 proc;
    short visible, value, enabled;
    Str63 fontname;
    short fontsize, fontstyle;
    short opt14, opt15, opt16, opt17, opt18, opt19, opt20, opt21;
    RGBColor BackRGB, ForeRGB;
} ObjectProps[200], FormProps[20];

// -------------------------------------------------------------------------------------
short IsFileOpen(Str63 fname1) {
    short a;
    for (a = 0; a < 100; a++) {
        if (EditWin[a].status >= 2)
            if (EqualString(fname1, EditWin[a].FileName, false, false)) return a + 1;
    }
    return 0;
}

void EditorDispose() {
    short a;
    //if( !EditorFlag || ActiveTE==nil ) { EditorFlag=0; return;}
    a = FindTEwindow();
    EditorFlag = 0;
    if (a == -1) return;
    TEDeactivate(ActiveTE);
    ActiveTE = nil;
}

void ProcEditInit() {
    short a;
    for (a = 0; a < 1000; a++) {
        EditProc[a].ProcHandle = nil;
        EditProc[a].size = 0;
    }
}

short FindFormOrEdit(WindowPtr window) {
    short a;
    if (window == nil) return -1;
    for (a = 0; a < 20; a++) if (window == FormWin[a]) return a;
    for (a = 0; a < 100; a++) if (EditWin[a].status && window == EditWin[a].theWindow) return a;
    return -1;
}

short FindTEwindow() {
    short a;
    if (ActiveTE == nil) return -1;
    for (a = 0; a < 100; a++) if (EditWin[a].status && ActiveTE == EditWin[a].te) return a;
    return -1;
}

short ModuleEditorCreate(short cf, Str255 title) {
    short a, b = -1;
    CurrentForm = -1;
    b = CreateEditorWindow(cf, title);
    if (b == -1) return b;
    EditWin[b].FormNum = -1;
    EditWin[b].status = 1;   // indicates new  status 1=new 2=loaded
    EditWin[b].change = 1;
    EditWin[b].editFlag = 2;  // New ind proc
    EditWin[b].currentProc = -1;
    EditWin[b].GlobalVar = nil;
    EditWin[b].GlobalVarSize = 0;
    SelectWindow(EditWin[b].theWindow);
    SetPort(EditWin[b].theWindow);
    SetControlValue(EditWin[b].dropList, 0);
    ActiveTE = EditWin[b].te;
    TEActivate(ActiveTE);
    EditorFlag = 1;
    return (b);
}

void FormEditorCall(short ctlNum) {
    Str255 text1;
    long tl;
    short a, b = -1, pos, flag1 = 0;
    Rect theRect;
    // Check if there is already a edit window open for this form
    for (a = 0; a < 100; a++) {
        if (EditWin[a].theWindow != nil && CurrentForm == EditWin[a].FormNum) {
            b = a;
            break;
        }
    }
    if (b >= 0) {
        SelectWindow(EditWin[b].theWindow);
        SetPort(EditWin[b].theWindow);
        ActiveTE = EditWin[b].te;
        a = SaveEditBox(b);
        if (a == -1) return;
        flag1++;
    } else {
        a = CreateEditorWindow(0, FormProps[CurrentForm].name);
        b = a;
        EditWin[b].FormNum = CurrentForm;
        SelectWindow(EditWin[b].theWindow);
        SetPort(EditWin[b].theWindow);
        LoadEditPullDown(b);
    }
    ActiveTE = EditWin[b].te;
    TEActivate(ActiveTE);
    EditorFlag = 1;
    if (ctlNum == -2 && flag1) return;
    if (CurrentForm >= 0) LoadEdit(ctlNum, b);
}

void LoadEditPullDown(short dd) {
    short a, b, c, d, noi = 3, t;
    Str255 text1, text2;
    char *cp;
    //f = EditWin[b].FormNum;
    for (a = 0; a < 1000; a++) {
        if (EditProc[a].ProcHandle != nil) {
            if (EditProc[a].EditWinNum == dd) {
                HLock(EditProc[a].ProcHandle);
                cp = (*EditProc[a].ProcHandle);
                c = 1;
                if (cp[0] == 'S' || cp[0] == 's') {
                    t = 4;
                    if (cp[1] == 't' || cp[1] == 'T') t = 0;
                } else t = 9;
                for (b = t; b < 41 + t; b++) {
                    if (cp[b] == '(' || cp[b] == ' ' || cp[b] == 13) break;
                    else text1[c++] = cp[b];
                }
                text1[0] = c - 1;
                HUnlock(EditProc[a].ProcHandle);
                for (c = 4; c <= noi; c++) {
                    GetMenuItemText(EditWin[dd].theMenu, c, text2);
                    d = RelString(text2, text1, false, false);
                    if (d == 1) {
                        c--;
                        break;
                    }
                }
                InsertMenuItem(EditWin[dd].theMenu, text1, c);
                noi++;
            }
        }
    }
    c = CountMItems(EditWin[dd].theMenu);
    SetControlMaximum(EditWin[dd].dropList, c);
}

void LoadEdit(short ctlNum, short b) {
    Str255 text1, text2;
    long tl;
    short pos, createFlag = 1, noi, c, d, e = -1;
    Rect theRect;
    //TESetSelect( 0, 32767, ActiveTE );
    if (ctlNum == -2) {
        createFlag = 0;
    } else {
        if (ctlNum == -1) {
            if (FormProps[CurrentForm].proc[0] && !EqualString(FormProps[CurrentForm].proc, "\pNone", false, false)) {
                Str255Copy(FormProps[CurrentForm].proc, text2);
            } else {
                Str255Copy(FormProps[CurrentForm].name, text2);
                Str255Cat("\p_Event", text2);
                Str255Copy(text2, FormProps[CurrentForm].proc);
            }
        } else {
            if (ObjectProps[ctlNum].proc[0] && !EqualString(ObjectProps[ctlNum].proc, "\pNone", false, false)) {
                Str255Copy(ObjectProps[ctlNum].proc, text2);
            } else {
                Str255Copy(ObjectProps[ctlNum].name, text2);
                if (CtlData[ctlNum].status == 3 || CtlData[ctlNum].status == 10) Str255Cat("\p_Event", text2);
                else Str255Cat("\p_Click", text2);
                Str255Copy(text2, ObjectProps[ctlNum].proc);
            }
        }
    }
    if (PropWin != nil) {
        SetPort(PropWin);
        EraseRect(&PropWin->portRect);
        InvalRect(&PropWin->portRect);
    }
    SetPort(EditWin[b].theWindow);
    noi = CountMItems(EditWin[b].theMenu);
    if (ctlNum == -2) {
        if (noi >= 4) {
            c = 4;
            GetMenuItemText(EditWin[b].theMenu, c, text2);
        } else c = 0;
    } else {
        for (c = 4; c <= noi; c++) {
            GetMenuItemText(EditWin[b].theMenu, c, text1);
            if (EqualString(text2, text1, false, false)) {
                createFlag = 0;
                break;
            }
        }
    }
    if (createFlag) {
        // AddToEditDropList
        for (c = 4; c <= noi; c++) {
            GetMenuItemText(EditWin[b].theMenu, c, text1);
            d = RelString(text1, text2, false, false);
            if (d == 1) {
                c--;
                break;
            }
        }
        InsertMenuItem(EditWin[b].theMenu, text2, c);
        SetControlMaximum(EditWin[b].dropList, noi + 1);
        SetControlValue(EditWin[b].dropList, c + 1);
        EditWin[b].FormNum = CurrentForm;
        EditWin[b].change = 1;
        EditWin[b].editFlag = 1;  // New object
        EditWin[b].currentProc = -1;
        Str255Copy("\pSub ", text1);
        Str255Cat(text2, text1);
        Str255Cat("\p( )\x0d    \x0d\x0d\x0d", text1);
        pos = text1[0] - 3;
        Str255Cat("\pEndSub\x0d", text1);
        TESetSelect(0, 0, ActiveTE);
        tl = (long) text1[0];
        TESetText(text1 + 1, tl, ActiveTE);
        //TEInsert( text1+1, tl, ActiveTE );
        //*****(*ActiveTE)->clikStuff = 255;
        TESetSelect(pos, pos, ActiveTE);
        //(*ActiveTE)->clikStuff = 0;
        theRect = EditWin[b].theWindow->portRect;
        InvalRect(&theRect);
    } else {
        // Load existing module
        if (c > 0) LoadEditBox(b, text2);
        SetControlValue(EditWin[b].dropList, c);
    }
}

void LoadEditBox(short b, Str255 text2) {
    short a, c, f, t;
    long g, pos;
    Str255 text1;
    char *cp;
    Rect theRect;
    f = EditWin[b].FormNum;
    for (a = 0; a < 1000; a++) {
        if (EditProc[a].ProcHandle != nil)
            if (EditProc[a].EditWinNum == b) {
                HLock(EditProc[a].ProcHandle);
                cp = (*EditProc[a].ProcHandle);
                c = 1;
                if (cp[0] == 'S' || cp[0] == 's') {
                    t = 4;
                    if (cp[1] == 't' || cp[1] == 'T') t = 0;
                } else t = 9;
                for (g = t; g < 41 + t; g++) {
                    if (cp[g] == '(' || cp[g] == ' ' || cp[g] == 13) break;
                    else text1[c++] = cp[g];
                }
                text1[0] = c - 1;
                for (pos = 0; pos < 260; pos++) if (cp[pos] == 13) break;
                pos++;
                for (pos = pos; pos < 260; pos++) if (cp[pos] != ' ') break;
                if (EqualString(text1, text2, false, false)) {
                    //ppp(*ActiveTE)->clikStuff = 255;
                    TESetSelect(0, 32767, ActiveTE);
                    TEDelete(ActiveTE);
                    TESetSelect(0, 0, ActiveTE);
                    //pos = EditProc[a].size;
                    TESetText(cp, EditProc[a].size, ActiveTE);
                    //TEInsert( cp, EditProc[a].size, ActiveTE );
                    TECalText(ActiveTE);
                    TEUpdate(&(EditWin[b].theWindow->portRect), ActiveTE);
                    //ppp(*ActiveTE)->clikStuff = 255;
                    TESetSelect(pos, pos, ActiveTE);
                    TEUpdate(&(EditWin[b].theWindow->portRect), ActiveTE);
                    //(*ActiveTE)->clikStuff = 0;
                    theRect = EditWin[b].theWindow->portRect;
                    OffsetRect(&theRect, -theRect.left, -theRect.top);
                    InvalRect(&theRect);
                    HUnlock(EditProc[a].ProcHandle);
                    EditWin[b].editFlag = 3;  // 0 Already has a buffer established
                    EditWin[b].currentProc = a;
                    return;
                }
                HUnlock(EditProc[a].ProcHandle);
            }
    }
}

void DoQCheck() {
    short a, b = 0;
    for (a = 0; a < 1000; a++) if (EditProc[a].ProcHandle != nil) b++;
    dprintf("# of procedures is %i\n", b);
}

short SaveEditBox(short b) {
    short a, f, flag1 = 0;
    Handle htemp;
    f = GetControlValue(EditWin[b].dropList);
    if (f == 3) {
        // Global
        if (EditWin[b].GlobalVar != nil) DisposeHandle(EditWin[b].GlobalVar);
        htemp = (Handle) TEGetText(ActiveTE);
        f = HandToHand(&htemp);
        if (f) IDEmemErr();
        // Bobby check for mem error
        EditWin[b].GlobalVar = htemp;
        EditWin[b].GlobalVarSize = (*ActiveTE)->teLength;
        EditWin[b].editFlag == 3;
        EditWin[b].currentProc = 9999;
        return 1;
    }
    f = DoEditCheck(b);
    if (f == -1) {
        return -1;
    } else if (!f) {
        a = GetControlValue(EditWin[b].dropList);
        if (a >= 4) {    // Delete sub/function
            DeleteMenuItem(EditWin[b].theMenu, a);
            a = EditWin[b].currentProc;
            if (a >= 0) {
                DisposeHandle(EditProc[a].ProcHandle);
                EditProc[a].ProcHandle = nil;
                EditProc[a].size = 0;
                EditProc[a].EditWinNum = -1;
            }
        }
        return 0;
    } else // OK
    {
        if (EditWin[b].editFlag == 0) {
            {
                SysBeep(30);
                dprintf("Error @ " __FILE__" l. %d\n", __LINE__);
            }
            return 1;
        }
        if (EditWin[b].editFlag == 1 || EditWin[b].editFlag == 2) {
            // New Object or Ind Proc so find open editProc
            //printf("Creating new proc %i\n", EditWin[b].editFlag);
            for (a = 0; a < 1000; a++) {
                if (EditProc[a].ProcHandle == nil) {
                    flag1 = 1;
                    htemp = (Handle) TEGetText(ActiveTE);
                    f = HandToHand(&htemp);
                    if (f) IDEmemErr();
                    // Bobby check for mem error
                    EditProc[a].ProcHandle = htemp;
                    EditProc[a].size = (*ActiveTE)->teLength;
                    EditProc[a].EditWinNum = b;
                    EditWin[b].editFlag = 3;
                    EditWin[b].currentProc = a;
                    break;
                }
            }
        } else {
            a = EditWin[b].currentProc;
            if (EditProc[a].ProcHandle != nil) DisposeHandle(EditProc[a].ProcHandle);  // this should always happen
            htemp = (Handle) TEGetText(ActiveTE);
            f = HandToHand(&htemp);
            if (f) IDEmemErr();
            // Bobby check for mem error
            EditProc[a].ProcHandle = htemp;
            EditProc[a].size = (*ActiveTE)->teLength;
            EditProc[a].EditWinNum = b;
        }
    }
    return 1;
}

short DoEditCheck(short b) {
    long textLen, offset = 0, ft;
    Handle chnd;
    short a, c, ch, ct = 0, ctr, Flag1, noi, x;
    char text1[512];
    Str255 text2, text3;
    chnd = TEGetText(ActiveTE);
    textLen = (*ActiveTE)->teLength;
    if (!textLen) return (0);   // 0=empty
    // Step 1 - Remove all leading spacing on first line
    Flag1 = 0;
    while (!Flag1 && offset < textLen) {
        ch = *((*chnd) + offset);
        if (ch == ' ' || ch == 9) offset++;  // if char = space or tab
        else Flag1++;
    }
    if (offset > 0) {        // Remove leading spaces
        TESetSelect(0, offset, ActiveTE);
        TEDelete(ActiveTE);
        textLen = (*ActiveTE)->teLength;  // get new len
        if (!textLen) return (0);   // 0=empty
        offset = 0;
    }
    offset = 0;
    while (offset < textLen && offset < 512) {
        ch = *((*chnd) + offset); // Grab first character
        if (ch == 13) break;
        else text1[offset++] = ch;
    }
    text1[offset] = 0;
    if (StrPartCmp(text1, "Sub ")) {
        ct = 1;
        // Insure proper caps
        *((*chnd)) = 'S';
        *((*chnd) + 1) = 'u';
        *((*chnd) + 2) = 'b';
        a = 4;
        while (*((*chnd) + a) == ' ') a++;
        if (a != 4) {
            TESetSelect(4, a, ActiveTE);  // Remove extra spacing between sub and name
            TEDelete(ActiveTE);
            textLen = (*ActiveTE)->teLength;
            a = 4;
        }
        if (!isalpha(*((*chnd) + a))) ct = 0;  // not a valid function name
        else {
            while (a < textLen && *((*chnd) + a) != '(' && *((*chnd) + a) != 13) a++;
            if (*((*chnd) + a) != '(') {
                TESetSelect(a, a, ActiveTE);
                TEInsert("( )", 3, ActiveTE);
                textLen = (*ActiveTE)->teLength;
            }
            a = 1;
            c = 4;
            while (*((*chnd) + c) != '(') {
                text2[a++] = *((*chnd) + c);
                c++;
            } // Copy proc name to str255
            text2[0] = a - 1;
        }
    } else if (StrPartCmp(text1, "Function ")) {
        ct = 2;
        // Insure proper caps
        *((*chnd)) = 'F';
        *((*chnd) + 1) = 'u';
        *((*chnd) + 2) = 'n';
        *((*chnd) + 3) = 'c';
        *((*chnd) + 4) = 't';
        *((*chnd) + 5) = 'i';
        *((*chnd) + 6) = 'o';
        *((*chnd) + 7) = 'n';
        a = 9;
        while (*((*chnd) + a) == ' ') a++;
        if (a != 9) {
            TESetSelect(9, a, ActiveTE);  // Remove extra spacing between sub and name
            TEDelete(ActiveTE);
            textLen = (*ActiveTE)->teLength;
            a = 9;
        }
        if (!isalpha(*((*chnd) + a))) ct = 0;  // not a valid function name
        else {
            while (a < textLen && *((*chnd) + a) != '(' && *((*chnd) + a) != ' ' && *((*chnd) + a) != 13) a++;
            x = a;
            while (*((*chnd) + a) == ' ') a++;
            if (a != x) {
                TESetSelect(x, a, ActiveTE);  // Remove extra spacing between sub and name
                TEDelete(ActiveTE);
                textLen = (*ActiveTE)->teLength;
                a = x;
            }
            if (*((*chnd) + a) != '(') {
                TESetSelect(a, a, ActiveTE);
                TEInsert("( ) ", 4, ActiveTE);
                textLen = (*ActiveTE)->teLength;
            }
            a = 1;
            c = 9;
            while (*((*chnd) + c) != '(') {
                text2[a++] = *((*chnd) + c);
                c++;
            } // Copy proc name to str255
            text2[0] = a - 1;
        }
    } else if (StrPartCmp(text1, "Start")) {
        ct = 3;
        Str255Copy("\pStart", text2);
    }
    if (!ct) {
        // Improper header
        {
            SysBeep(30);
            dprintf("Error @ " __FILE__" l. %d\n", __LINE__);
        }
        TESetSelect(0, offset, ActiveTE);
        TEUpdate(&(EditWin[b].theWindow->portRect), ActiveTE);
        return -1;
    }
    // Ok so far now check last line for endsub or endfunction, if it is not there, add it
    // First get last line into text1
    offset = textLen - 1;
    while (offset > 0 && *((*chnd) + offset) == 13) offset--; //skip over all CR's
    while (offset > 0) {
        ch = *((*chnd) + offset); // let's find start of last line
        if (ch == 13) break;
        else offset--;
    }
    offset++;
    a = 0;
    while (offset < textLen && a < 512) {
        ch = *((*chnd) + offset); // copying line into text1
        if (ch == 13) break;
        else {
            text1[a++] = ch;
            offset++;
        }
    }
    text1[a] = 0;
    if (ct == 1) {  //Sub
        if (!StrPartCmp(text1, "EndSub")) {
            TESetSelect(32767, 32767, ActiveTE);
            Str255Copy("\p\x0d", text3);
            Str255Cat("\pEndSub\x0d", text3);
            ft = (long) text3[0];
            TEInsert(text3 + 1, ft, ActiveTE);
            TEUpdate(&(EditWin[b].theWindow->portRect), ActiveTE);
        }
    } else if (ct == 2) {  //Function
        if (!StrPartCmp(text1, "EndFunction")) {
            TESetSelect(32767, 32767, ActiveTE);
            Str255Copy("\p\x0d", text3);
            Str255Cat("\pEndFunction\x0d", text3);
            ft = (long) text3[0];
            TEInsert(text3 + 1, ft, ActiveTE);
            TEUpdate(&(EditWin[b].theWindow->portRect), ActiveTE);
        }
    } else if (ct == 3) {  //Function
        if (!StrPartCmp(text1, "Finish")) {
            TESetSelect(32767, 32767, ActiveTE);
            Str255Copy("\p\x0d", text3);
            Str255Cat("\pFinish\x0d", text3);
            ft = (long) text3[0];
            TEInsert(text3 + 1, ft, ActiveTE);
            TEUpdate(&(EditWin[b].theWindow->portRect), ActiveTE);
        }
    }
    a = GetControlValue(EditWin[b].dropList);
    if (a <= 2) {    // AddToEditDropList
        noi = CountMItems(EditWin[b].theMenu);
        for (c = 4; c <= noi; c++) {
            GetMenuItemText(EditWin[b].theMenu, c, text3);
            a = EqualString(text3, text2, false, false);
            if (a) return 1;
            a = RelString(text3, text2, false, false);
            if (a == 1) {
                c--;
                break;
            }
        }
        InsertMenuItem(EditWin[b].theMenu, text2, c);
        SetControlMaximum(EditWin[b].dropList, noi + 1);
        SetControlValue(EditWin[b].dropList, c + 1);
    } else {
        // check for change
        GetMenuItemText(EditWin[b].theMenu, a, text3);
        c = EqualString(text3, text2, false, false);
        if (!c) SetMenuItemText(EditWin[b].theMenu, a, text2);  // replace
    }
    return 1;
}

void IDEmemErr() {
    dprintf("A memory error occurred\n");
    {
        SysBeep(30);
        dprintf("Error @ " __FILE__" l. %d\n", __LINE__);
    }
}

pascal void ScrollProcTE(ControlHandle theControl, short partCode) {
    short cmax, cmin, cvalue, b, c;
    //Rect			theRect;
    cvalue = GetControlValue(theControl);
    cmin = GetControlMinimum(theControl);
    cmax = GetControlMaximum(theControl);
    switch (partCode) {
        case 20: // inUpButton:
            b = TEGetHeight(0, 0, EditWin[GWN].te);
            TEPinScroll(0, b, EditWin[GWN].te);
            cvalue--;
            if (cvalue >= cmin) SetControlValue(theControl, cvalue);
            break;
        case 21: //inDownButton:
            b = TEGetHeight(0, 0, EditWin[GWN].te);
            TEPinScroll(0, -b, EditWin[GWN].te);
            cvalue++;
            if (cvalue <= cmax) SetControlValue(theControl, cvalue);
            break;
        case 22: //inPageUp:
            b = GWHt;
            c = (b / TEGetHeight(0, 0, EditWin[GWN].te)) - 1;
            b = c * TEGetHeight(0, 0, EditWin[GWN].te);
            TEPinScroll(0, b, EditWin[GWN].te);
            cvalue -= c;
            if (cvalue < cmin) cvalue = cmin;
            SetControlValue(theControl, cvalue);
            break;
        case 23: //inPageDown:
            b = GWHt;
            c = (b / TEGetHeight(0, 0, EditWin[GWN].te)) - 1; // = # lines viewable
            b = c * TEGetHeight(0, 0, EditWin[GWN].te);;
            TEPinScroll(0, -b, EditWin[GWN].te);
            cvalue += c;
            if (cvalue > cmax) cvalue = cmax;
            SetControlValue(theControl, cvalue);
            break;
    }
}

void EditPager(short WinNum, short partCode) {
    short cmax, cmin, cvalue, b, c;
    Rect tr;
    ControlHandle theControl;
    if (WinNum == -1) return;
    theControl = EditWin[WinNum].theControl;
    tr = (**EditWin[WinNum].te).viewRect;
    cvalue = GetControlValue(theControl);
    cmin = GetControlMinimum(theControl);
    cmax = GetControlMaximum(theControl);
    switch (partCode) {
        case 1: //inPageUp:
            b = GWHt = tr.bottom - tr.top;;
            c = (b / TEGetHeight(0, 0, EditWin[WinNum].te)) - 1;
            b = c * TEGetHeight(0, 0, EditWin[WinNum].te);
            TEPinScroll(0, b, EditWin[WinNum].te);
            cvalue -= c;
            if (cvalue < cmin) cvalue = cmin;
            SetControlValue(theControl, cvalue);
            break;
        case 2: //inPageDown:
            b = GWHt;
            c = (b / TEGetHeight(0, 0, EditWin[WinNum].te)) - 1; // = # lines viewable
            b = c * TEGetHeight(0, 0, EditWin[WinNum].te);;
            TEPinScroll(0, -b, EditWin[WinNum].te);
            cvalue += c;
            if (cvalue > cmax) cvalue = cmax;
            SetControlValue(theControl, cvalue);
            break;
    }
}

void InitEditorWindows() {
    short a;
    for (a = 0; a < 100; a++) {
        EditWin[a].theWindow == nil;
        EditWin[a].status = 0;
    }
}

short CalcTextView(short cnum)   // Returns lines viewable
{
    short a, b, c;
    a = TEGetHeight(0, 0, EditWin[cnum].te);
    b = (**EditWin[cnum].te).viewRect.bottom - (**EditWin[cnum].te).viewRect.top;
    //if( !a ) { {SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);}printf( "CalcTextView\n"); getchar(); return 0; }
    if (!a) { return 0; }
    c = b / a;    // c now equals # of line viewable
    return c;
}

void ChangeScrollBar(short cnum) {
    short a, b, c;
    a = fabs((**EditWin[cnum].te).destRect.top - (**EditWin[cnum].te).viewRect.top);
    b = TEGetHeight(0, 0, EditWin[cnum].te);
    //if( !b ) { {SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);}printf( "ChangeScrollBar1\n"); getchar(); return ; }
    if (!b) { return; }
    c = (a / b) + 1;
    //if( c<1 ) { {SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);}printf( "ChangeScrollBar2 %i\n", c); getchar(); return ; }
    if (c < 1) { return; }
    SetControlValue(EditWin[cnum].theControl, c);
}

void TextFix(WindowPtr window) {
    short a, cnum;
    cnum = -1;
    for (a = 0; a < 100; a++)
        if (EditWin[a].status && EditWin[a].theWindow == window) {
            cnum = a;
            break;
        }
    if (cnum == -1) return;
    a = (**EditWin[cnum].te).nLines - CalcTextView(cnum);
    if (a <= 0) {
        SetControlMaximum(EditWin[cnum].theControl, 1);
        HiliteControl(EditWin[cnum].theControl, 255);  // Disable scrollbar
    } else {
        HiliteControl(EditWin[cnum].theControl, 0);  // Enable scrollbar
        SetControlMaximum(EditWin[cnum].theControl, a + 1);
        ChangeScrollBar(cnum);
    }
    return;
}

void SetEdWinStatus(short wn, short st) {
    EditWin[wn].status = st;
}

// Create a editor window
short CreateEditorWindow(short createFlag, Str255 title) {
    short a, b = -1, err, g, f;
    Rect tempRect, tr, theRect;
    RGBColor rgb;
    Pattern thePat;
    WindowPtr window;
    RGBColor black1, white1;
    Str255 title2;
    char title3[65];
    Str255Copy(title, title2);
    if (CurrentForm == -1) {
        for (a = 20; a < 100; a++)
            if (!EditWin[a].status) {
                b = a;
                break;
            }
        if (b == -1) return -1;
        if (createFlag) {
            ModuleCtr++;
            sprintf(title3, "Module%i", ModuleCtr);
            StrToStr255(title3, EditWin[a].FileName);
            StrToStr255(title3, title2);
        }
    } else b = CurrentForm;
    // Now let's create the window itself
    f = b * 4;
    SetRect(&tempRect, 60 + f, 140 + f, 560 + f, 400 + f);
    EditWin[b].theWindow = NewCWindow(nil, &tempRect, title2, true, documentProc, (WindowPtr)(-1), true, 0);
    if (EditWin[b].theWindow == nil) return -1;
    // EditWin[b].status =1;
    EditWin[b].theRect = tempRect;
    EditWin[b].FormNum = CurrentForm;
    if (createFlag) EditWin[b].change = 0;
    EditWin[b].editFlag = 0;
    //EditWin[b].GlobalVar = nil;
    //EditWin[b].GlobalVarSize = 0;
    // Now let's fill it's busty innards
    SelectWindow(EditWin[b].theWindow);
    SetPort(EditWin[b].theWindow);
    black1.red = 0;
    black1.green = 0;
    black1.blue = 0;
    white1.red = 0xffff;
    white1.green = 0xffff;
    white1.blue = 0xffff;
    RGBForeColor(&black1);
    RGBBackColor(&white1);
    tr.top = 31;
    tr.left = 0;
    tr.right = tempRect.right - tempRect.left - 15;
    tr.bottom = tempRect.bottom - tempRect.top - 15;
    TextFont(4); //22 );
    //EditWin[b].te = TEStyleNew( &tr, &tr);   undo won't work right with this
    EditWin[b].te = TENew(&tr, &tr);
    //EditWin[b].te = TENew( &tr, &tr);
    if (EditWin[b].te == nil) {
        IDEmemErr();
        return -1;
    }
    // Adjust
    EditWin[b].ht = TEGetHeight(0, 0, EditWin[b].te);
    if (!EditWin[b].ht) {
        {
            SysBeep(30);
            dprintf("Error @ " __FILE__" l. %d\n", __LINE__);
        }
    }
    a = (tr.bottom - tr.top) / TEGetHeight(0, 0, EditWin[b].te);
    tr.bottom = 31 + (a * TEGetHeight(0, 0, EditWin[b].te));
    (*EditWin[b].te)->viewRect = tr;
    (*EditWin[b].te)->destRect = tr;
    ActiveTE = EditWin[b].te;
    TEActivate(ActiveTE);
    TEAutoView(true, EditWin[b].te);
    TESetSelect(0, 32767, EditWin[b].te);
    TESelView(EditWin[b].te);
    ///*tr.right = tempRect.right - tempRect.left-15;
    //tr.bottom = tempRect.bottom-tempRect.top-15;
    //tempRect = tr;
    //tempRect.left = tempRect.right;
    //tempRect.right = tempRect.left + 15;
    window = EditWin[b].theWindow;
    theRect = window->portRect;
    OffsetRect(&theRect, -theRect.left, -theRect.top);
    theRect.top = 31;
    theRect.bottom -= 14;
    theRect.right++;
    theRect.left = theRect.right - 16;
    EditWin[b].theControl = NewControl(EditWin[b].theWindow, &theRect, "\p", TRUE, 0, 0, 1, 16, 0);
    SetControlMinimum(EditWin[b].theControl, 1);
    SetControlMaximum(EditWin[b].theControl, 1);
    SetControlValue(EditWin[b].theControl, 1);
    //g = CalcTextPages( b,  tr.bottom - tr.top);
    //SetControlMaximum(EditWin[b].theControl, (** EditWin[b].te).nLines - CalcTextView( b,  tempRect.bottom - tempRect.top ) );  //-1
    //if(!g)	HiliteControl( EditWin[b].theControl, 255);  // Disable scrollbar
    TextFix(window);
    rgb.red = 0xb000;
    rgb.green = 0xb000;
    rgb.blue = 0xb000;
    RGBForeColor(&rgb);
    GetIndPattern(&thePat, 0, 1);
    theRect = window->portRect;
    a = theRect.right - theRect.left; //-15;
    SetRect(&tempRect, 0, 0, a, 30);
    FillRect(&tempRect, &thePat);
    // Create droplist
    EditWin[b].theMenu = NewMenu(11000 + b, "\pMenu");
    InsertMenu(EditWin[b].theMenu, -1);
    //SetRect(&tempRect, 4, 4, 260, 4+25);
    SetRect(&tempRect, 4, 4, 214, 4 + 25);
    EditWin[b].dropList = NewControl(EditWin[b].theWindow, &tempRect, "\p", false, 0, 11000 + b, 0,/*popupMenuProc*/400,
                                     0);
    AppendMenu(EditWin[b].theMenu, "\pCreate Sub Procedure");
    AppendMenu(EditWin[b].theMenu, "\pCreate Function Procedure");
    AppendMenu(EditWin[b].theMenu, "\pGlobal Variables");
    SetControlMaximum(EditWin[b].dropList, 3);
    SetItemMark(EditWin[b].theMenu, 1, 0xc8);
    SetItemMark(EditWin[b].theMenu, 2, 0xc8);
    SetItemMark(EditWin[b].theMenu, 3, 0xd7);
    ShowControl(EditWin[b].dropList);
    RGBForeColor(&black1);
//	DrawControls(window);
    if (appr1.gHaveAppearanceMgr) {
        SetThemeWindowBackground(EditWin[b].theWindow, kThemeActiveDialogBackgroundBrush, true);
    }
    return b;
}

short UpdateEditWin(WindowPtr window) {
    short a, b, c;
    GrafPtr savePort;
    Rect theRect, tempRect;
    Pattern thePat;
    RGBColor rgb, oldrgb;
    for (a = 0; a < 100; a++) {
        if (EditWin[a].status && EditWin[a].theWindow == window) {
            GetPort(&savePort);
            SetPortWindowPort(window);
            BeginUpdate(window);
            //theRect = EditWin[a].theRect;
            theRect = window->portRect;
            OffsetRect(&theRect, -theRect.left, -theRect.top);
            EraseRect(&theRect);
            TEUpdate(&theRect, EditWin[a].te);
            TextFix(window);
            DrawGrowIcon(window);
            rgb.red = 0xb000;
            rgb.green = 0xb000;
            rgb.blue = 0xb000;
            GetForeColor(&oldrgb);
            RGBForeColor(&rgb);
            GetIndPattern(&thePat, 0, 1);
            theRect = window->portRect;
            b = theRect.right - theRect.left; //-15;
            SetRect(&tempRect, 0, 0, b, 30);
            FillRect(&tempRect, &thePat);
            rgb.red = 0xd000;
            rgb.green = 0xd000;
            rgb.blue = 0xd000;
            RGBForeColor(&rgb);
            MoveTo(0, 30);
            LineTo(0, 0);
            LineTo(b, 0);
            MoveTo(1, 29);
            LineTo(1, 1);
            LineTo(b - 1, 1);
            rgb.red = 0x7fff;
            rgb.green = 0x7fff;
            rgb.blue = 0x7fff;
            RGBForeColor(&rgb);
            MoveTo(1, 29);
            LineTo(b - 1, 29);
            LineTo(b - 1, 0);
            MoveTo(0, 30);
            LineTo(b, 30);
            LineTo(b, 0);
            RGBForeColor(&oldrgb);
            DrawControls(window);
            EndUpdate(window);
            // restore the old graphics port
            SetPort(savePort);
            return 1;
        }
    }
    {
        SetThemeWindowBackground(window, kThemeActiveDialogBackgroundBrush, true);
    }
    return 0; // No, a form window didn't request an update
}

short DoEditGrow(Point hitPt, WindowRef window) {
    short a, b;
    Rect sizeRect, theRect, tr, tempRect;
    long newSize;
    GrafPtr savePort;
    a = CheckForEdit(window);
    if (a >= 0) {
        GetPort(&savePort);
        SetPortWindowPort(window);
        SetRect(&sizeRect, 50, 50, 2000, 2000); //kMinWindowWidth, kMinWindowHeight, SHRT_MAX, SHRT_MAX );
        newSize = GrowWindow(window, hitPt, &sizeRect);
        if (!newSize) {
            SetPort(savePort);
            return;
        }
        //	for some reason, GrowWindow( ) returns a long value,
        theRect = window->portRect;  //EditWin[a].theRect;
        theRect.right = theRect.left + LoWord(newSize);
        theRect.bottom = theRect.top + HiWord(newSize);
        EditWin[a].theRect = theRect;
        SizeWindow(window, LoWord(newSize), HiWord(newSize), true);
        // Now adjust textbox
        theRect = window->portRect;  //EditWin[a].theRect;
        tr.top = 31;
        tr.left = 0;
        tr.right = theRect.right - theRect.left - 15;
        tr.bottom = theRect.bottom - theRect.top - 15;
        // Adjust
        b = (tr.bottom - tr.top) / EditWin[a].ht;
        tr.bottom = 31 + (b * EditWin[a].ht);
        (*EditWin[a].te)->viewRect = tr;
        (*EditWin[a].te)->destRect = tr;
        TEUpdate(&theRect, EditWin[a].te);
        TECalText(EditWin[a].te);
        TESelView(EditWin[a].te);
        OffsetRect(&theRect, -theRect.left, -theRect.top);
        theRect.top = 31;
        theRect.bottom -= 15;
        theRect.left = theRect.right - 15;
        MoveControl(EditWin[a].theControl, theRect.left, theRect.top);
        SizeControl(EditWin[a].theControl, 16, theRect.bottom - theRect.top + 1);
        EraseRect(&window->portRect);
        InvalRect(&window->portRect);
        //DrawGrowIcon( window );
        SetPort(savePort);
        return a;
    }
    return -1;
}

short CloseEditWindow(WindowPtr window) {
    short a, b;
    a = CheckForEdit(window);
    if (a == -1) return -1;
    b = SaveEditBox(a);
    if (b == -1) return -1;
    if (a >= 20) {    // This is a module so check for save
        if (EditWin[a].change) {
            ParamText(EditWin[a].FileName, "\pclosing", nil, nil);
            //SetCursor( &qd.arrow );
            b = Alert(131, nil);
            // exit if the user canceled the alert box
            if (b == 2) return -2;
            if (b == 1) SaveSourceFile(EditWin[a].theWindow);
            // if b==3 then don't save
        }
    }
    ActiveTE = nil;
    TEDeactivate(EditWin[a].te);
    TEDispose(EditWin[a].te);
    DisposeWindow(EditWin[a].theWindow);
    DeleteMenu(11000 + a);
    DisposeMenu(EditWin[a].theMenu);
    EditWin[a].theWindow = nil;
    EditWin[a].te = nil;
    EditWin[a].theMenu = nil;
    if (a < 20)  // is this editor belong to a form
    {    // yes  OK select the form
        b = EditWin[a].FormNum;
        SelectWindow(FormWin[b]);
        SetPort(FormWin[b]);
    } else {
        EditWin[a].GlobalVar = nil;
        EditWin[a].GlobalVarSize = 0;
        // Module so remove proc from memory
        for (b = 0; b < 1000; b++) {
            if (EditProc[b].ProcHandle != nil) {
                if (EditProc[b].EditWinNum == a) {
                    DisposeHandle(EditProc[b].ProcHandle);
                    EditProc[a].ProcHandle = nil;
                    EditProc[b].size = 0;
                    EditProc[b].EditWinNum = -1;
                }
            }
        }
        EditWin[a].status = 0;
        for (b = 0; b < 20; b++) {
            if (FormWin[b] != nil) {
                SelectWindow(FormWin[b]);
                SetPort(FormWin[b]);
                break;
            }
        }
        if (PropWin != nil) {
            SetPort(PropWin);
            EraseRect(&PropWin->portRect);
            InvalRect(&PropWin->portRect);
        }
    }
    EditorFlag = 0;
    return a;
}

short IsFormEdit(WindowPtr window) {
    short a;
    for (a = 0; a < 20; a++) if (EditWin[a].status && EditWin[a].theWindow == window) return a;
    return -1;
}

short IsModuleEdit(WindowPtr window) {
    short a;
    for (a = 20; a < 100; a++) if (EditWin[a].status && EditWin[a].theWindow == window) return a;
    return -1;
}

short CheckForEdit(WindowPtr window) {
    short a, b = -1;
    //GrafPtr 	oldPort;
    Rect tempRect, tr;
    for (a = 0; a < 100; a++)
        if (EditWin[a].status && EditWin[a].theWindow == window) {
            b = a;
            break;
        }
    if (b == -1) return -1;
    SelectWindow(window);
    SetPort(window);
    CurrentForm = EditWin[b].FormNum;
    if (ActiveTE != EditWin[b].te) {
        ActiveTE = EditWin[b].te;
        TEActivate(ActiveTE);
    }
    //TESelView( ActiveTE );
    //DrawGrowIcon( window );
    return b;
}

short CheckEditMouse(short WinNum, Point pt) {
    short b, c, ct, dt;
    Rect theRect, tr;
    WindowPtr window;
    ControlHandle theControl;
    Str255 text1;
#ifdef    powerc
                                                                                                                            //ControlActionUPP	scrollbarUPP, tescrollUPP;
     ControlActionUPP	tescrollUPP;
     //scrollbarUPP = NewControlActionProc( ScrollProc );
     tescrollUPP = NewControlActionProc( ScrollProcTE );
#endif
    window = EditWin[WinNum].theWindow;
    theRect = window->portRect;
    //theRect = EditWin[WinNum].theRect;
    tr.top = 31;
    tr.left = 0;
    tr.right = theRect.right - theRect.left - 15;
    tr.bottom = theRect.bottom - theRect.top - 15; //46;
    if (PtInRect(pt, &tr)) {
        TEClick(pt, false, EditWin[WinNum].te);
        TEUpdate(&((*EditWin[WinNum].te)->viewRect), EditWin[WinNum].te);
        //ChangeScrollBar( WinNum);
        TextFix(window);
        return 1;
    }
    ct = FindControl(pt, window, &theControl);
    if (ct) {
        GWN = WinNum;
        switch (ct) {
            case 129: //inThumb:
                ct = GetControlValue(theControl);
                dt = TrackControl(theControl, pt, nil);
                c = ct - GetControlValue(theControl);
                b = c * EditWin[GWN].ht;
                TEPinScroll(0, b, EditWin[GWN].te);
                break;
            case 20: //inUpButton:
            case 21: //inDownButton:
            case 22: //inPageUp:
            case 23: //inPageDown:
                GWHt = tr.bottom - tr.top;
#ifdef powerc
                dt = TrackControl( theControl, pt, tescrollUPP );
#else
                dt = TrackControl(theControl, pt, ScrollProcTE);
#endif
                break;
            default:
                dt = SaveEditBox(WinNum);
                if (dt == -1) return 0;
                dt = TrackControl(theControl, pt, NULL);
#warning Need a UPP here
                if (!dt) return 0;
                c = GetControlValue(theControl);
                SetItemMark(EditWin[WinNum].theMenu, 1, 0xc8);
                SetItemMark(EditWin[WinNum].theMenu, 2, 0xc8);
                SetItemMark(EditWin[WinNum].theMenu, 3, 0xd7);
                if (c && c <= 3) {
                    if (c <= 2) CreateNewProc(c, WinNum);
                    else {
                        if (EditWin[WinNum].GlobalVar == nil) CreateNewProc(3, WinNum);
                        else LoadGlobal(WinNum);
                    }
                } else {
                    GetMenuItemText(EditWin[WinNum].theMenu, c, text1);
                    LoadEditBox(WinNum, text1);
                }
                break;
        }
    }
    return 0;
}

void CreateNewProc(short tp, short b) {
    short pos;
    long tl;
    Rect theRect;
    Str255 text1;
    EditWin[b].change = 1;
    EditWin[b].editFlag = 1;  // New object
    EditWin[b].currentProc = -1;
    switch (tp) {
        case 1:
            Str255Copy("\pSub ( )\x0d    \x0d\x0d\x0d", text1);
            Str255Cat("\pEndSub\x0d", text1);
            pos = 4;
            break;
        case 2:
            Str255Copy("\pFunction ( ) As Long\x0d    \x0d\x0d\x0d", text1);
            Str255Cat("\p    ExitFunction 0\x0d", text1);
            Str255Cat("\pEndFunction\x0d", text1);
            pos = 9;
            break;
        case 3:
            Str255Copy("\p    Dim \x0d", text1);
            pos = 8;
            break;
    }
    TESetSelect(0, 0, ActiveTE);
    tl = (long) text1[0];
    TESetText(text1 + 1, tl, ActiveTE);
    //*****(*ActiveTE)->clikStuff = 255;
    TESetSelect(pos, pos, ActiveTE);
    theRect = EditWin[b].theWindow->portRect;
    InvalRect(&theRect);
}

void LoadGlobal(short b) {
    short a, c, f, t;
    long g, pos;
    Str255 text1;
    char *cp;
    Rect theRect;
    if (EditWin[b].GlobalVar == nil) return;
    HLock(EditWin[b].GlobalVar);
    cp = (*EditWin[b].GlobalVar);
    TESetSelect(0, 32767, ActiveTE);
    pos = 0;
    TESetText(cp, EditWin[b].GlobalVarSize, ActiveTE);
    //*****(*ActiveTE)->clikStuff = 255;
    TESetSelect(pos, pos, ActiveTE);
    theRect = EditWin[b].theWindow->portRect;
    OffsetRect(&theRect, -theRect.left, -theRect.top);
    InvalRect(&theRect);
    HUnlock(EditWin[b].GlobalVar);
    EditWin[b].editFlag = 3;  // Already has a buffer established
    EditWin[b].currentProc = 9999;
}

void MarkChangeTE() {
    short a;
    for (a = 0; a < 100; a++)
        if (ActiveTE == EditWin[a].te) {
            EditWin[a].change = 1;
            break;
        }
}

short SaveSourceFile(WindowPtr window) {
    short a, fref, fn, dd, noi, z = 0, looper = 0, cc, typer = 0;
    char *cp, cr[] = "\x0d", hdr1[] = "MacStandardBasic Form Version 4.1\x0d\x0d";
    char hdr2[] = "MacStandardBasic Module Version 4.1\x0d\x0d";
    long bl;
    StandardFileReply sfr;
    Str255 text1;
    DialogPtr theDialog;
    if (CurrentForm == -1) {
        // Edit window
        dd = -1;
        for (a = 20; a < 100; a++) {
            if (EditWin[a].status && window == EditWin[a].theWindow) {
                dd = a;
                break;
            }
        }
        if (dd == -1) {
            {
                SysBeep(30);
                dprintf("Error @ " __FILE__" l. %d\n", __LINE__);
            }
            return -1;
        }
    } else {
        dd = CurrentForm;
    }
    fn = EditWin[dd].FormNum;
    //if( fn)
    if (dd < 20) {   // Save form
        typer = 1;
        if (EditWin[dd].status < 2 || SaveAsFlag) {
            while (!looper) {
                StandardPutFile("\pEnter Form Filename", FormProps[dd].name, &sfr);
                if (sfr.sfGood == true) {
                    if (sfr.sfFile.vRefNum != PjtFS.vRefNum || sfr.sfFile.parID != PjtFS.parID) {
                        //{SysBeep(30); dprintf("Error @ " __FILE__" l. %d", __LINE__);}
                        theDialog = GetNewDialog(280, nil, (WindowPtr) - 1L);
                        ModalDialog(0, &cc);
                        DisposeDialog(theDialog);
                    } else {
                        EditWin[dd].fs.vRefNum = sfr.sfFile.vRefNum;
                        EditWin[dd].fs.parID = sfr.sfFile.parID;
                        Str255Copy(sfr.sfFile.name, EditWin[dd].fs.name);
                        Str255Copy(sfr.sfFile.name, EditWin[dd].FileName);
                        if (sfr.sfReplacing) FSpDelete(&EditWin[dd].fs);
                        if (EditWin[dd].status < 2) {
                            noi = CountMItems(FormsSubMenu);
                            for (a = 1; a <= noi; a++) {
                                GetMenuItemText(FormsSubMenu, a, text1);
                                if (EqualString(text1, EditWin[dd].FileName, false, false)) {
                                    z = 1;
                                    break;
                                }
                            }
                            if (!z) AppendMenu(FormsSubMenu, sfr.sfFile.name);
                            SaveProject();
                        }
                        looper++;
                    }
                } else { return 0; }
            }
        } else {
            a = FSMakeFSSpec(PjtFS.vRefNum, PjtFS.parID, EditWin[dd].FileName, &EditWin[dd].fs);
            if (a != -43) FSpDelete(&EditWin[dd].fs);
        }
    } else {   // Save module
        typer = 2;
        if (EditWin[dd].status < 2 || SaveAsFlag) {
            while (!looper) {
                StandardPutFile("\pEnter Module Filename", EditWin[dd].FileName, &sfr);
                if (sfr.sfGood == true) {
                    if (sfr.sfFile.vRefNum != PjtFS.vRefNum || sfr.sfFile.parID != PjtFS.parID) {
                        //{SysBeep(30); dprintf("Error @ " __FILE__" l. %d", __LINE__);}
                        theDialog = GetNewDialog(280, nil, (WindowPtr) - 1L);
                        ModalDialog(0, &cc);
                        DisposeDialog(theDialog);
                    } else {
                        EditWin[dd].fs.vRefNum = sfr.sfFile.vRefNum;
                        EditWin[dd].fs.parID = sfr.sfFile.parID;
                        Str255Copy(sfr.sfFile.name, EditWin[dd].fs.name);
                        Str255Copy(sfr.sfFile.name, EditWin[dd].FileName);
                        if (sfr.sfReplacing) FSpDelete(&EditWin[dd].fs);
                        if (EditWin[dd].status < 2) {
                            noi = CountMItems(ModulesSubMenu);
                            for (a = 1; a <= noi; a++) {
                                GetMenuItemText(ModulesSubMenu, a, text1);
                                if (EqualString(text1, EditWin[dd].FileName, false, false)) {
                                    z = 1;
                                    break;
                                }
                            }
                            if (!z) AppendMenu(ModulesSubMenu, sfr.sfFile.name);
                            SaveProject();
                        }
                        looper++;
                    }
                } else { return 0; }
            }
        } else {
            a = FSMakeFSSpec(PjtFS.vRefNum, PjtFS.parID, EditWin[dd].FileName, &EditWin[dd].fs);
            if (a != -43) FSpDelete(&EditWin[dd].fs);
        }
    }
    if (typer == 1) FSpCreate(&EditWin[dd].fs, 'VmSB', 'FORM', 0);
    else FSpCreate(&EditWin[dd].fs, 'VmSB', 'TEXT', 0);
    FSpOpenDF(&EditWin[dd].fs, fsWrPerm, &fref);
    if (dd < 20) {
        bl = 35;
        FSWrite(fref, &bl, hdr1);
    }  // Write header
    else {
        bl = 37;
        FSWrite(fref, &bl, hdr2);
    }// Write header
    EditWin[dd].status = 2;
    EditWin[dd].change = 0;
    if (EditWin[dd].FormNum >= 0) SaveFormContents(fref, dd);
    // Write Global if it exists
    if (EditWin[dd].GlobalVar != nil) {
        HLock(EditWin[dd].GlobalVar);
        bl = 7;
        FSWrite(fref, &bl, "Global\x0d");
        cp = (*EditWin[dd].GlobalVar);
        bl = EditWin[dd].GlobalVarSize;
        FSWrite(fref, &bl, cp);
        bl = 1;
        FSWrite(fref, &bl, cr);       // Add extra CR
        bl = 11;
        FSWrite(fref, &bl, "EndGlobal\x0d\x0d");
        HUnlock(EditWin[dd].GlobalVar);
    }
    for (a = 0; a < 1000; a++) {
        if (EditProc[a].ProcHandle != nil) {
            if (EditProc[a].EditWinNum == dd) {
                HLock(EditProc[a].ProcHandle);
                cp = (*EditProc[a].ProcHandle);
                bl = EditProc[a].size;
                FSWrite(fref, &bl, cp);
                HUnlock(EditProc[a].ProcHandle);
                bl = 1;
                FSWrite(fref, &bl, cr);       // Add extra CR
            }
        }
    }
    FSClose(fref);
    return 1;
}

short LoadSourceFile(short fddi, Str255 fname) {
    short a, b, fref, fn, ModuleFlag = 1, dd = -1, f, ctr = 0, z;
    char *cp, cr[] = "\x0d", hdr1[] = "MacStandardBasic Form Version 4.1";
    long bl, t1, t2;
    Handle hbuf, htemp;
    FSSpec fs;
    Str255 text1;
    a = IsFileOpen(fname);
    if (a) {
        /*a--;
		if( a<20 )  // is this editor belong to a form
		{	// yes  OK select the form
			//b = EditWin[a].FormNum;
			SelectWindow( FormWin[a] );
			SetPort( FormWin[a] );
		}*/
        return 0;
    }
    //fn = EditWin[dd].FormNum;
    switch (fddi) {
        case 1:  // Form
            for (a = 0; a < 20; a++)
                if (FormWin[a] == nil) {
                    dd = a;
                    break;
                }
            if (dd == -1) return -1;
            a = FSMakeFSSpec(PjtFS.vRefNum, PjtFS.parID, fname, &EditWin[dd].fs);
            if (a == -43) return -1;
            break;
        case 2:    // Module
            dd = ModuleEditorCreate(0, fname);
            if (dd == -1) return -1;
            a = FSMakeFSSpec(PjtFS.vRefNum, PjtFS.parID, fname, &EditWin[dd].fs);
            if (a == -43) return -1;
            break;
    }
    FSpOpenDF(&EditWin[dd].fs, fsRdPerm, &fref);
    GetEOF(fref, &bl);
    zsource_len = bl;
    hbuf = NewHandle(bl);
    if (hbuf == nil) {
        FSClose(fref);
        return -1;
    }
    HLock(hbuf);
    zbuffer = (*hbuf);
    FSRead(fref, &bl, zbuffer);       // Write header
    FSClose(fref);
    zbuf_pos = 0;
    znext_line();
    if (StrPartCmp(zline, "MacStandardBasic Form")) {
        ModuleFlag = 0;
        LoadFormContents(dd);
        CreatePropWindow();
        if (PropWin != nil) {
            SetPort(PropWin);
            EraseRect(&PropWin->portRect);
            InvalRect(&PropWin->portRect);
        }
        SelectWindow(FormWin[dd]);
        SetPort(FormWin[dd]);
        a = 0;  // znext_line was done in LoadFormContents
    } else {
        if (!StrPartCmp(zline, "MacStandardBasic Module")) zbuf_pos = 0;  // Reset because old V2 code file
        a = znext_line();
    }
    while (!a) {
        //printf("~%s\n", zline );
        if (StrPartCmp(zline, "Start") || StrPartCmp(zline, "Sub ") || StrPartCmp(zline, "Function ") ||
            StrPartCmp(zline, "Global")) {
            if (StrPartCmp(zline, "Global")) {
                t1 = zbuf_pos;
                a = znext_line();
            } else {
                t1 = ztstart;
                a = znext_line();
            }  // Start, Sub and Func
            b = 0;
            while (!b) {
                //printf("~  %s\n", zline );
                if (StrPartCmp(zline, "Finish") || StrPartCmp(zline, "EndSub") || StrPartCmp(zline, "EndFunction")) {
                    for (z = 0; z < 1000; z++) {
                        if (EditProc[z].ProcHandle == nil) {
                            t2 = zbuf_pos - t1;
                            //htemp = (Handle) TEGetText( ActiveTE );
                            f = PtrToHand((zbuffer + t1), &htemp, t2);
                            if (f) IDEmemErr();
                            // Bobby check for mem error
                            EditProc[z].ProcHandle = htemp;
                            EditProc[z].size = (short) t2;;
                            EditProc[z].EditWinNum = dd;
                            ctr++;
                            EditWin[dd].editFlag = 3;
                            EditWin[dd].status = 2;
                            break;
                        }
                    }
                    b = 1;
                } else if (StrPartCmp(zline, "EndGlobal")) {
                    if (EditWin[dd].GlobalVar == nil) {
                        //t2 = zbuf_pos - t1;
                        t2 = ztstart - t1;
                        f = PtrToHand((zbuffer + t1), &htemp, t2);
                        if (f || htemp == nil) IDEmemErr();
                        // Bobby check for mem error
                        EditWin[dd].GlobalVar = htemp;
                        EditWin[dd].GlobalVarSize = t2;
                        EditWin[dd].editFlag = 3;
                        EditWin[dd].status = 2;
                    }
                    //else {SysBeep(30); dprintf("Error @ " __FILE__" l. %d", __LINE__);}
                    b = 1;
                } else {
                    if (a) b++;
                    else a = znext_line();
                }
            }
        } else a = 1;
        if (!a) a = znext_line();
    }
    HUnlock(hbuf);
    DisposeHandle(hbuf);
    zbuffer = nil;
    Str255Copy(EditWin[dd].fs.name, EditWin[dd].FileName);
    EditWin[dd].change = 0;
    //getchar();
    if (fddi == 2 && ctr) {
        LoadEditPullDown(dd);
        SetControlValue(EditWin[dd].dropList, 0);
        a = CountMenuItems(EditWin[dd].theMenu);
        if (a >= 4) {
            SetControlValue(EditWin[dd].dropList, 4);
            GetMenuItemText(EditWin[dd].theMenu, 4, text1);
            LoadEditBox(dd, text1);
        }
    }
    return 0;
}
