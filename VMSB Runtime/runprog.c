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
#define            MaxNumOfMenuItems    500
#if powerc
#define			EvCtr 	1000
#else
#define            EvCtr    400
#endif
extern unsigned char parsed_line[1024], line[260], label_flag;
extern short line_pos, pline_pos, line_len;
extern long source_len, buf_pos, line_ctr;
extern char SoundDisposal;
extern Handle codeHandle, tablesHandle;
char DoneFlag, TimersFlag = 0;
extern short ErrorCommand;

void ErrHandler(short ErrNum, Str255 AddMsg);

short doendupdates();

UCHAR *CodeBuf;
long CodePos, CodeLen, NLP, LineStart;
short DimVarCtr, CallStackPtr, LoopStackPtr, NextFlag;
long ReturnStrLen, ReturnStrSize;
UCHAR *ReturnStr, ReturnStrFlag, EventsFlag = 1;
short EventSetting = 1;
long EventCtr = EvCtr;

short pass_assign(short vartype, short varnum);

short RunProgramLoop();

short EventLoop(void);

void CloseItAllUp();

void SoundCleanUp();

struct TimersStruct {
    short status;   // 0=empty  1=disabled  2=enabled
    short cmd;
    long interval;
    long next;
    short WinNum;
} TimersList[50];
struct CallStackStruct {
    long location;
    short module;
    unsigned char *varbuf;
} CallStack[500];
struct WinStruct {
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

short RunProgram();

short RunPrep();

short Load_Exec_File(char *fname);

#include    "Structs1.h"

extern struct cmd_func1       //These are built-in cmds, there will also be user defined subs
{
    short (*func_ptr)();
} cmd_func[200];

struct RunVarStruct // This Data Structure holds runtime info
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
extern struct MenuStruct {
    MenuHandle Menu;
    short subcode;
} MenuData[52];
extern struct MenuItemStruct {
    short menunum;
    short itemnum;
    short subcode;
} MenuItemData[MaxNumOfMenuItems];

short RunProgram() {
    short a;
    CodeBuf = (UCHAR *) *codeHandle;
    CodePos = 0;
    DimVarCtr = 0;
    CallStackPtr = -1;
    LoopStackPtr = -1;
    NextFlag = 0;
    DoneFlag = false;
    ReturnStr = nil;
    ReturnStrLen = 0;
    for (a = 0; a < 100; a++) WinData[a].status = 0;        //Reset all windows
    for (a = 0; a < 500; a++) CtlData[a].status = 0;        //Reset all controls
    for (a = 0; a < 50; a++) MenuData[a].Menu = nil;        //Reset all Menu handles
    for (a = 0; a < MaxNumOfMenuItems; a++) MenuItemData[a].menunum = -1;        //Reset all Menu handles
    for (a = 0; a < 50; a++) TimersList[a].status = 0; // Reset 50 timers
    //Will chg
    RunVar.VarTable2 = (struct VarTableStruct2 *) *tablesHandle;
    RunVar.CallTable2 = (struct CallTableStruct2 *) (*tablesHandle + RunHdr.CallTableOffset);
    RunVar.LabelTable = (long *) (*tablesHandle + RunHdr.LabelTableOffset);
    RunVar.StrTable = (UCHAR *) (*tablesHandle + RunHdr.StrTableOffset);
    RunVar.SelectTable = (struct SelectTableStruct *) (*tablesHandle + RunHdr.SelectTableOffset);
    RunVar.PropTable = (struct ObjectPropsStruct2 *) (*tablesHandle + RunHdr.PropTableOffset);
    //Ok, Set the CodePos to Start Finish
    CodePos = RunHdr.CodeStart;
    CallStackPtr++;
    // Mem Alloc's
    if (RunHdr.GlbVarSz) {
        RunVar.GlobalVarSpace = (unsigned char *) NewPtrClear(RunHdr.GlbVarSz);
        if (RunVar.GlobalVarSpace == nil) {
            ErrHandler(1001, nil);
            return (-1);
        }
    }
    if (RunHdr.StartVarSz) {
        CallStack[CallStackPtr].varbuf = (unsigned char *) NewPtrClear(RunHdr.StartVarSz);
        if (CallStack[CallStackPtr].varbuf == nil) {
            ErrHandler(1001, nil);
            return (-1);
        }
    }
    RunProgramLoop();
    CloseItAllUp();
    return (0);
}

void CloseItAllUp() {
    short a;
    //Releasing Start and Global Mem Space
    if (CallStack[0].varbuf == nil) DisposePtr((Ptr) CallStack[0].varbuf);
    if (RunVar.GlobalVarSpace == nil) DisposePtr((Ptr) RunVar.GlobalVarSpace);
    // Be sure all windows are closed
    for (a = 0; a < 100; a++) {
        if (WinData[a].status > 0)    //Reset all windows
        {
            DisposeWindow(WinData[a].theWindow);
        }
    }
    return;
}

short RunProgramLoop() {
    short a, LL, result, nn = 0, b, c, t;
    short *ip, cmd = 0;
    long ectr, tc;
    a = 0;
    ectr = EvCtr;
    while (!DoneFlag && !a) {
        // Do Sound Disposal Check
        if (SoundDisposal) {
            SoundCleanUp();
        }
        // Check Events
        cmd = 0;
        if (EventsFlag) {
            ectr--;
            if (ectr <= 0) {
                cmd = EventLoop();
                ectr = EventCtr;
            } else { // no event check timers
                if (TimersFlag) {
                    tc = TickCount();
                    for (t = 0; t < 50; t++) // 50 timers max
                        if (TimersList[t].status == 2)
                            if (TimersList[t].next <= tc) {
                                TimersList[t].next = tc + TimersList[t].interval;
                                cmd = TimersList[t].cmd;
                                break;  // break because we can only handle one timer event at a time
                            }
                }
            }
        }
        if (DoneFlag) break;
        if (cmd || ErrorCommand) {    // This is a user defined sub called by an event or the error handler
            if (!cmd && ErrorCommand) {
                cmd = ErrorCommand;
                ErrorCommand = 0;
            }
            cmd -= 1000;
            CallStackPtr++;
            if (CallStackPtr > 499) {
                ErrHandler(1029, nil);
                a++;
            }
            CallStack[CallStackPtr].location = CodePos;        //Return to next cmd
            CallStack[CallStackPtr].module = cmd;        //Return to next cmd
            if (RunVar.CallTable2[cmd].VarMemSz) {    // Allocate memory for local vars
                CallStack[CallStackPtr].varbuf = (unsigned char *) NewPtrClear(RunVar.CallTable2[cmd].VarMemSz);
                if (CallStack[CallStackPtr].varbuf == nil) return (-1);
            } else CallStack[CallStackPtr].varbuf = nil;
            CodePos = RunVar.CallTable2[cmd].location;                // Call sub
        }
        // Run BASIC Code
        LineStart = CodePos;
        ip = (short *) (CodeBuf + CodePos);
        LL = *ip;                       // LL = Line Length
        NLP = CodePos + (long) LL;      // Set NLM to point to next line
        ip++;
        cmd = *ip;                      // Get command code
        CodePos += 4;
        if (LL <= 0) {
            ErrHandler(1030, nil);
            return (0);
        }
        if (cmd < 1000)                 // Internal or user defined
        {
            cmd -= 100;
            result = cmd_func[cmd].func_ptr();
            if (!result) CodePos = NLP;
            if (result == 99) a++;         //End ??
        } else {
            cmd -= 1000;
            CallStackPtr++;
            if (CallStackPtr > 499) {
                ErrHandler(1029, nil);
                //Call Stack Overflow Error
                a++;
            }
            CallStack[CallStackPtr].location = NLP;        //Return to next cmd
            CallStack[CallStackPtr].module = cmd;        //Return to next cmd
            if (RunVar.CallTable2[cmd].VarMemSz) {
                // Allocate memory for local vars
                CallStack[CallStackPtr].varbuf = (unsigned char *) NewPtrClear(RunVar.CallTable2[cmd].VarMemSz);
                if (CallStack[CallStackPtr].varbuf == nil) return (-1);
            } else CallStack[CallStackPtr].varbuf = nil;
            // Now assign passed values to variables (if any)
            b = 0;
            while (RunVar.CallTable2[cmd].params[b]) {
                c = 2;    //default
                switch (RunVar.CallTable2[cmd].params[b]) {
                    case 'U':
                        c = 1;
                        break;
                    case 'L':
                        c = 2;
                        break;
                    case 'X':
                        c = 3;
                        break;
                    case 'D':
                        c = 4;
                        break;
                    case 'S':
                        c = 5;
                        break;
                }
                pass_assign(c, RunVar.CallTable2[cmd].par_vars[b]);
                b++;
            }
            CodePos = RunVar.CallTable2[cmd].location;                // Call sub
        }
        nn++;
    }
    return (0);
}