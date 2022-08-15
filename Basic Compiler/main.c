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
#include        <ctype.h>
#include        <math.h>
#include        <SIOUX.h>

#define         UCHAR   unsigned char

void COD(UCHAR *msg);

short FindFormSub(char *cmdname);

short DoFormCallSync();

short ObjectTableFind(Str255 objname, short objtype);

short FormTableFind(Str255 formname);

short compile_log_init();

short create_program_file(UCHAR *fname);

short create_app();

short LoadCompileFS();

short Str255Cpy(UCHAR *st1, UCHAR *st2);

void Str255Cat(Str255 st1, Str255 st2);

short StrPartCmp(char *st1, char *st2);

void Str255Copy(Str255 st1, Str255 st2);

short add_PropTable_page();

void Str255ToStr(Str255 st1, char *st2);

short doendupdates();

short compile_log_update(short errno, char *special);

short ld_nextline(long ln);

short lde(short errno, short lineno);

void InitializeToolbox(void);

short compile_log_update(short errno, char *special);

short compile_logfile_update(char *txt);

void zsplit();

short znext_line();

short load_libdef();

extern char *zbuffer;
extern char zline[260];
extern short zline_pos, zline_len;
extern long zbuf_pos, zsource_len, ztstart;
extern Str255 zparm1, zparm2;
short MasterFormNum = -1, ldlc;
DialogPtr CompileDialog;
WindowPtr CSWindow;
unsigned char *buffer, *project_bp, temp[100], CurrSrc[65], CurrProc[260];
short pjt_ctr, pjt_len, current_module_num, LabelCtr;
extern short whdata;
long StrConCtr, VarSzCtr, GblSzCtr, pjt_pos, bcc, lcc;
short SelectCtr, SelectArray[32], SelectCurr;
short whtype, p2_startflag;
unsigned char ParList[12], DimNum;
long ParVar[12], DimList[12];
char LibraryMode = 3; // Default to FAT    68K=1  PPC=2  FAT=3
short ReturnType;        //Used for adding function returntype to table2
extern long ErrorCtr;
FSSpec CplFS;
short MinMem = 2048, PrefMem = 2048, numOfPjtFiles = 0;
extern short srcFileType;
struct PjtFilesStruct {
    Str63 fname;
    short tp;    // 1= form  2= module 3= resource
    long codeStart;
    short lineCtr;
    short FormID;
} PjtFileList[200];
// Structures
struct CallTableStruct1 {
    char callname[41];           // Sub & Func names can be upto 40 chars long
    short pjt_filenum;
} CallTable1;
struct CallTableStruct2 {
    UCHAR calltype;                       // 1=Sub  2=Func
    char params[10];
    long par_vars[10];
    long location;
    long VarMemSz;
    short returntype;    // Not used with Subs, only Functions
} CallTable2;
struct VarTableStruct1 {
    char varname[41];            // Var names can be upto 40 chars long
} VarTable1;
struct VarTableStruct2 {
    short modnum;
    UCHAR vartype;                // 1=short
    UCHAR dims;                   // up to 16 dimensions
    long dimsize[16];
    long offset;
} VarTable2;
struct LabelTableStruct {
    char labelname[41];            // Label names can be upto 40 chars long
    UCHAR status;
    short modnum;
    //long    location;     Taken out & placed in own buffer
} LabelTable;
struct SelectTableStruct {
    short casenum;
    short status;
    long value;
    long location;
} SelectTable;
struct ObjectPropsStruct1 {
    Str63 name;
    Str63 proc;
    short filenum;
} PropTable1;
struct ObjectPropsStruct2 {
    short FormID;
    short CtlID;  // -1 if it is a form definition itself
    short CtlType;
    Str255 text;
    short left, top, width, height;
    short procID;
    short visible, value, enabled;
    Str63 fontname;
    short fontsize, fontstyle;
    short opt14, opt15, opt16, opt17, opt18, opt19, opt20, opt21;
    RGBColor BackRGB, ForeRGB;
} PropTable2;
/*struct GlobalDataStruct
{
	long    NumOfVars, NumOfCalls, NumOfLabels, CodePtr;
	struct  CallTableStruct1 *CallTable1Page[100];  // Upto 100 Pages of 100 calls each
	struct  CallTableStruct2 *CallTable2Page[100];  // Upto 100 Pages of 100 calls each
	short   Num_CTP;
	struct  VarTableStruct1 *VarTable1Page[100];    // Upto 100 Pages of 100 varnames each
	struct  VarTableStruct2 *VarTable2Page[100];    // Upto 100 Pages of 100 varnames each
	struct  LabelTableStruct *LabelTablePage[100];   // Upto 100 Pages of 100 Labels each
	long    *LabelLocPage[100];
	UCHAR   *CodeTablePage[100];            // Upto 100 Pages of 16K each
	UCHAR   *StringTablePage[100];          // Upto 100 Pages of 4K each
	short   Num_VTP;
	short   Num_LTP;
	short   Num_CDTP;
	short   Num_STP;
	UCHAR   StartFlag;
	UCHAR   CompMode;
	UCHAR   DimMode;
	long    CodeCtr;
	struct  SelectTableStruct *SelectTablePage[100];   // Upto 100 Pages of 100 Select cases each
	short   Num_SCTP;
	long	NumOfSelects;
} GlobalData;   */
struct GlobalDataStruct {
    long NumOfVars, NumOfCalls, NumOfLabels, NumOfProps, CodePtr;
    struct CallTableStruct1 *CallTable1Page[100];  // Upto 100 Pages of 100 calls each
    struct CallTableStruct2 *CallTable2Page[100];  // Upto 100 Pages of 100 calls each
    short Num_CTP;
    struct VarTableStruct1 *VarTable1Page[100];    // Upto 100 Pages of 100 varnames each
    struct VarTableStruct2 *VarTable2Page[100];    // Upto 100 Pages of 100 varnames each
    struct LabelTableStruct *LabelTablePage[100];   // Upto 100 Pages of 100 Labels each
    long *LabelLocPage[100];
    struct ObjectPropsStruct1 *PropTable1Page[100];    // Upto 100 Pages of 100 props each
    struct ObjectPropsStruct2 *PropTable2Page[100];    // Upto 100 Pages of 100 props each
    UCHAR *CodeTablePage[100];            // Upto 100 Pages of 16K each
    UCHAR *StringTablePage[100];          // Upto 100 Pages of 4K each
    short Num_VTP;
    short Num_LTP;
    short Num_CDTP;
    short Num_STP;
    short Num_PTP;
    UCHAR StartFlag;
    UCHAR CompMode;
    UCHAR DimMode;
    long CodeCtr;
    struct SelectTableStruct *SelectTablePage[100];   // Upto 100 Pages of 100 Select cases each
    short Num_SCTP;
    long NumOfSelects;
} GlobalData;
//Externs
struct RunHdrStruct // This Data Structure holds runtime info
{
    long NumOfVars;
    long NumOfCalls;
    long NumOfLabels;
    long StrLen;
    long CodeLen;
    long CodeStart;        // Where to start
    long GlbVarSz;
    long StartVarSz;
    long CallTableOffset;    // Don't need VarTable Offset becuse it = 0
    long LabelTableOffset;
    long StrTableOffset;
    long SelectTableOffset;
    long NumOfSelects;
    long NumOfProps;        // New for 3.0
    long PropTableOffset;    // New for 3.0
} RunHdr;    //ext to pass1.c & pass2.c
UCHAR PassNum;
unsigned char parsed_line[1024], line[260], RawLine[260], label_flag;
short line_pos, pline_pos, line_len;
long source_len, buf_pos, line_ctr;
Str255 AppRunName, Target, TargetDef;

#include "proto1.h"

void getIDEspec();

FSSpec PjtSpec, IDESpec;

void memory_error() {
}

short compile_init() {
    LabelCtr = 0;
    StrConCtr = 0;
    GlobalData.NumOfVars = 0;
    GlobalData.NumOfCalls = 0;
    GlobalData.NumOfLabels = 0;
    GlobalData.NumOfSelects = 0;
    GlobalData.CodePtr = 0;
    GlobalData.NumOfProps = 0;
    GlobalData.Num_LTP = 0;
    GlobalData.Num_VTP = 0;
    GlobalData.Num_CTP = 0;
    GlobalData.Num_CDTP = 0;
    GlobalData.Num_PTP = 0;
    GlobalData.StartFlag = 0;
    GlobalData.CompMode = 0;
    GlobalData.DimMode = 0;
    for (SelectCtr = 0; SelectCtr < 32; SelectCtr++) SelectArray[SelectCtr] = 0;
    SelectCtr = 0;
    Target[0] = 0;  // 4.0 Target Name
    TargetDef[0] = 0;
    return (0);
}

long GrabLongConst() {
    char st[25], stc = 0;
    long result = 0;
    while (line[line_pos] == ' ' && line_pos <= line[0]) line_pos++;
    while (line[line_pos] >= '0' && line[line_pos] <= '9' & stc < 23) st[stc++] = line[line_pos++];
    st[stc] = 0;
    result = atol(st);
    //Now do check
    if (result <= 0) result = -1;
    return (result);
}

void COD(UCHAR *msg) {
    short itype, a, b, c = 1;
    long f;
    Handle hand;
    Rect r;
    Str255 txt;
    SetPort(CSWindow);
    SetRect(&r, 0, 170, 370, 210);
    EraseRect(&r);
    TextSize(14);
    a = StringWidth(msg);
    MoveTo((404 - a) / 2, 190);
    DrawString(msg);
    //Delay(40, &f);
    return;
}

short process_dims(short scope) {
    char varname[42];
    short vartype;
    short a, vct = 0, plc = 0, ct;
    long TempVSC, dimsz;
    for (a = 0; a < 10; a++) ParList[a] = 0;
    while (1) {
        TempVSC = 0;
        // Should only be vars, spaces, commas, and colons
        while (line[line_pos] == ' ' && line_pos <= line[0]) line_pos++;
        // Get Var
        a = 0;
        while (isvalidvar(line[line_pos]) && line_pos <= line[0]) {
            varname[a++] = line[line_pos++];
            if (a > 40) {}// Varname too long...}
        }
        if (a)   // Var here?
        {
            // Store it
            varname[a] = 0;
            vartype = OneOfThem(line[line_pos], "%&!#$");
            if (vartype == 3) vartype = 4;  // New float to double conversion
            if (!vartype) {
                // This means no type was explicitly expressed
                vartype = 2;   //Patch =Long
            } else line_pos++;
            switch (vartype) {
                case 1:         //int
                    if (plc < 10) {
                        ParList[plc] = 'U';
                        ParList[plc + 1] = 0;
                        ParVar[plc++] = GlobalData.NumOfVars;
                    }
                    TempVSC = 2;
                    //printf("Dim int %s\n",varname);
                    break;
                case 2:         //long
                    if (plc < 10) {
                        ParList[plc] = 'L';
                        ParList[plc + 1] = 0;
                        ParVar[plc++] = GlobalData.NumOfVars;
                    }
                    TempVSC = 4;
                    //printf("Dim long %s\n",varname);
                    break;
                case 3:         //single
                    if (plc < 10) {
                        ParList[plc] = 'X';
                        ParList[plc + 1] = 0;
                        ParVar[plc++] = GlobalData.NumOfVars;
                    }
                    TempVSC = 4;
                    //printf("Dim single %s\n",varname);
                    break;
                case 4:         //double
                    if (plc < 10) {
                        ParList[plc] = 'D';
                        ParList[plc + 1] = 0;
                        ParVar[plc++] = GlobalData.NumOfVars;
                    }
                    TempVSC = 8;
                    //printf("Dim double %s\n",varname);
                    break;
                case 5:         //string
                    if (plc < 10) {
                        ParList[plc] = 'S';
                        ParList[plc + 1] = 0;
                        ParVar[plc++] = GlobalData.NumOfVars;
                    }
                    TempVSC = 1;
                    break;
            }
            while (line[line_pos] == ' ' && line_pos <= line[0]) line_pos++;
            DimNum = 0;    // Reset to zero
            if (line[line_pos] == '[')  // Is this an array?
            {
                // An array, so find dimensions
                while (line[line_pos] == '[' && line_pos <= line[0]) {
                    line_pos++;
                    dimsz = GrabLongConst();
                    while (line[line_pos] == ' ' && line_pos <= line[0]) line_pos++;
                    if (dimsz == -1 || line[line_pos] != ']') {
                        //printf("ERROR - Array Specification Error for [%s]\n", varname);
                        compile_log_update(144, varname);
                        line_pos = line[0] + 1;
                    } else {
                        line_pos++;  // Skip ]
                        //Ok store in temp area
                        DimList[DimNum++] = dimsz;
                    }
                    while (line[line_pos] == ' ' && line_pos <= line[0]) line_pos++;
                }
                dimsz = TempVSC;
                if (vartype != 5) {
                    for (ct = 0; ct < DimNum; ct++) {
                        // Let's calc total size of array
                        TempVSC *= DimList[ct];
                    }
                } else {
                    for (ct = 0; ct < DimNum - 1; ct++) {
                        // Let's calc total size of array
                        TempVSC *= DimList[ct];
                    }
                    DimList[DimNum - 1] += 4;   // Add 4 byte long length
                    TempVSC *= DimList[DimNum - 1];
                }
            }
            // Special case for string with max size not specified
            if (vartype == 5 && !DimNum) {
                DimList[DimNum++] = 104; //Default = 100+4Byte len  //was 256 pre 8/14/96 RMH
                TempVSC = 104;
            }
            a = AddToVarTable(varname, vartype);
            //printf("[RS%li]", TempVSC);
            if (a < 0) // Error ?
            {
                //printf( "Dim Var Error\n" );
                compile_log_update(145, nil);
            } else {
                VarSzCtr += TempVSC;
            }
        } else {
            while (line[line_pos] == ' ' && line_pos <= line[0]) line_pos++;
            return (vct);
        }
        //Next one?
        while (line[line_pos] == ' ' && line_pos <= line[0]) line_pos++;
        if (line[line_pos] == ',') {
            line_pos++;
        } else {
            //printf("\n");
            return (vct);
        }
    }
}

void compile_error() {
    // This function loads next file into buffer
    // Returns 0 if none available or 1 if it has one
    FSSpec source_fp;
    short rn;
    OSErr error;
    char fname[50];
    strcpy(fname, (char *) "ERROR.LOG");
    error = FSMakeFSSpec(0, 0, (UCHAR *) fname, &source_fp);
    if (error) return;
    error = FSpOpenDF(&source_fp, fsRdWrShPerm, &rn);
    if (error) return;
    error = FSClose(rn);
    if (error) return;
    return;
}

//#include "datalst1.c"
struct cmd_list    //These are built-in cmds, there will also be user defined subs
{
    short num;
    char name[20];
    char params[10];
} cmd_list[800];
struct fun_list {
    short num;
    char name[20];
    char params[10];
    char ret;        //#11 is return (actually10)
} func_list[800];

short compile_command(void) {
    // This function retrieve cmd, finds # or not error.
    // moves line_pos
    short a, looper;
    char cmdtext[45];
    // Ignore any leading spaces
    while (line[line_pos] == ' ' && line_pos <= line[0]) line_pos++;
    if (line_pos > line[0]) {
        // No cmd avail.
        return (-2);
    }
    // Get First Char
    if (line[line_pos] < 'A' || line[line_pos] > 'Z') {
        // Error 1st char should be a A-Z for var or cmd
        //printf("Command Syntax Error\n");
        compile_log_update(101, nil);
        return (-1);
    }
    a = 0;
    while (isvalidvar(line[line_pos]) && line_pos <= line[0]) {
        cmdtext[a++] = line[line_pos++];
    }
    cmdtext[a] = 0;
    // Ok, we have cmdtext
    // Ignore any extra spaces
    while (line[line_pos] == ' ' && line_pos <= line[0]) line_pos++;
    if (line[line_pos] == '=')       // Is this Implied LET
    {
        // Var Assignment
    } else {
        // Ok cmd for sure, find it
        // First look at internal ones
        a = CallTableFind(cmdtext, 1);        // 1 means sub not func
        if (a < 0) {
            return (-3);   // -3 means invalid command, cmd not found
        }
    }
    return (0);
}

short FindFormSub(char *cmdname) {
    short a, b, c;
    struct CallTableStruct1 *BPtr1;
    struct CallTableStruct2 *BPtr2;
    // Then cmp vs. external cmds/functions
    c = 0;
    //for( b=0; b < GlobalData.Num_CDTP; b++ )
    for (b = 0; b < GlobalData.Num_CTP; b++) {
        BPtr1 = GlobalData.CallTable1Page[b];
        BPtr2 = GlobalData.CallTable2Page[b];
        a = 0;
        while (a < 100 && c < GlobalData.NumOfCalls) {
            //printf("Comparing %s with table entry %s\n",cmdname, BPtr1[a].callname);
            if (cmdname[0] == BPtr1[a].callname[0] && BPtr2[a].calltype == 1)
                if (!strcmp(cmdname, BPtr1[a].callname) && BPtr1[a].pjt_filenum == pjt_ctr - 1) {
                    ReturnType = BPtr2[a].returntype;
                    whtype = ReturnType;
                    //printf("[%s] =%i loc=%li\n",cmdname, a, BPtr2[a].location);
                    return (c + 1000);
                }
            a++;
            c++;
        }
    }
    return -1;
}

short DoFormCallSync() {
    short a, b, c, ctr = 0, d, FormID;
    char text1[260];
    struct ObjectPropsStruct1 *BPtr1;
    struct ObjectPropsStruct2 *BPtr2;
    FormID = PjtFileList[pjt_ctr - 1].FormID;
    if (FormID == -1) return 0;
    CurrProc[0] = 0;
    for (b = 0; b < GlobalData.Num_PTP; b++) {
        BPtr1 = GlobalData.PropTable1Page[b];
        BPtr2 = GlobalData.PropTable2Page[b];
        a = 0;
        while (a < 100 && ctr < GlobalData.NumOfProps) {
            //Str255ToStr( BPtr1[a].proc, text1);
            //printf("DoFormCallSync [%s] %i %i\n",text1, BPtr2[a].FormID, FormID);
            if (BPtr2[a].FormID == FormID) {
                if (!BPtr1[a].proc[0] || EqualString("\pNone", BPtr1[a].proc, false, false)) BPtr2[a].procID = 0;
                else {
                    Str255ToStr(BPtr1[a].proc, text1);
                    d = FindFormSub(text1);
                    if (d == -1) {
                        Str255ToStr(BPtr1[a].name, text1);
                        compile_log_update(166, text1);
                        d = 0; //***later error msg
                    } else {
                        BPtr2[a].procID = d;
                    }
                }
            }
            a++;
            ctr++;
        }
    }
}

short CallTableFind(char *cmdname, short PassCallType) {
    // Finds (if possible) cmd in CallTable
    short a = 0, b, c, looper, df = 0, filenum = -1, marker = -1, CallType, dupCheckFlag = 0;
    struct CallTableStruct1 *BPtr1;
    struct CallTableStruct2 *BPtr2;
    char comname[50];
    Str255 formname;
    CallType = PassCallType;
    if (CallType > 1000) {
        CallType -= 1000;
        dupCheckFlag++;
    }
    while (cmdname[a]) if (cmdname[a++] == '.') df++;
    if (df > 1) return -1;
    a = 0;
    if (df) {
        b = 1;
        while (cmdname[a] && cmdname[a] != '.') formname[b++] = cmdname[a++];
        formname[b] = 0;
        formname[0] = b - 1;
        a++; // skip .
        b = 0;
        while (cmdname[a]) comname[b++] = cmdname[a++];
        comname[b] = 0;
        a = FormTableFind(formname);
        if (a == -1) return -1;
        filenum = MasterFormNum;
    } else {
        b = 0;
        while (cmdname[a]) comname[b++] = cmdname[a++];
        comname[b] = 0;
    }
    if (!df) {
        if (CallType == 1) // Sub?
        {
            // First cmp vs. internal
            a = 0;
            while (cmd_list[a].name[0] != '*') //Remember datalst
            {
                if (comname[0] == cmd_list[a].name[0]) {
                    //printf( ">> %c = %c\n", cmdname[0] , cmd_list[a].name[0]);
                    if (!strcmp(comname, cmd_list[a].name)) {
                        whdata = a;
                        return (cmd_list[a].num);
                    }
                }
                a++;
            }
        } else // Ok this is a Function Search
        {
            // First cmp vs. internal
            a = 0;
            while (func_list[a].name[0] != '*') //Remember datalst
            {
                if (comname[0] == func_list[a].name[0])
                    if (!strcmp(comname, func_list[a].name)) {
                        //printf("func = %s\n",cmdname);
                        whdata = a;
                        switch (func_list[a].ret) {
                            case 'U':
                                whtype = 1;
                                break;
                            case 'L':
                                whtype = 2;
                                break;
                            case 'X':
                                whtype = 3;
                                break;
                            case 'D':
                                whtype = 4;
                                break;
                            case 'S':
                                whtype = 5;
                                break;
                        }
                        return (func_list[a].num);
                    }
                a++;
            }
        }
    }
    // Then cmp vs. external cmds/functions
    c = 0;
    //for( b=0; b < GlobalData.Num_CDTP; b++ )
    for (b = 0; b < GlobalData.Num_CTP; b++) {
        BPtr1 = GlobalData.CallTable1Page[b];
        BPtr2 = GlobalData.CallTable2Page[b];
        a = 0;
        while (a < 100 && c < GlobalData.NumOfCalls) {
            //printf("Comparing %s with table entry %s\n",cmdname, BPtr1[a].callname);
            if (comname[0] == BPtr1[a].callname[0] && BPtr2[a].calltype == CallType) {
                if (!strcmp(comname, BPtr1[a].callname)) {
                    ReturnType = BPtr2[a].returntype;
                    whtype = ReturnType;
                    marker = c + 1000;
                    if (!dupCheckFlag) {
                        if (filenum != -1 && filenum == BPtr1[a].pjt_filenum) return (marker);
                        if (filenum == -1 && pjt_ctr - 1 == BPtr1[a].pjt_filenum) return (marker);
                    } else if (pjt_ctr - 1 == BPtr1[a].pjt_filenum) return (marker);  // dup check
                }
            }
            a++;
            c++;
        }
    }
    if (marker >= 0 && !dupCheckFlag) if (filenum == -1) return (marker);
    return (-1);             // No match found
}

/*
short CallTableFind( char *cmdname, short CallType )
{
	// Finds (if possible) cmd in CallTable
	short a, b ,c, looper;
	struct CallTableStruct1 *BPtr1;
	struct CallTableStruct2 *BPtr2;
	if( CallType == 1) // Sub?
	{
		// First cmp vs. internal
		a=0;
		while( cmd_list[a].name[0] != '*' ) //Remember datalst
		{
			if( cmdname[0] == cmd_list[a].name[0] )
			{
				//printf( ">> %c = %c\n", cmdname[0] , cmd_list[a].name[0]);
				if( !strcmp(cmdname, cmd_list[a].name) )
				{
				 whdata=a;  return(cmd_list[a].num);
				}
			}
			a++;
		}
	}
	else // Ok this is a Function Search
	{
		// First cmp vs. internal
		a=0;
		while( func_list[a].name[0] != '*' ) //Remember datalst
		{
			if( cmdname[0] == func_list[a].name[0] )
				if( !strcmp(cmdname, func_list[a].name) )
				{
				    //printf("func = %s\n",cmdname);
					whdata = a;
					switch( func_list[a].ret)
					{
					 	case 'U':
					 		whtype = 1;
					 		break;
					 	case 'L':
					 		whtype = 2;
					 		break;
					 	case 'X':
					 		whtype = 3;
					 		break;
					 	case 'D':
					 		whtype = 4;
					 		break;
					 	case 'S':
					 		whtype = 5;
					 		break;
					}
					return(func_list[a].num);
				}
			a++;
		}
	}
	// Then cmp vs. external cmds/functions
	c = 0;
	for( b=0; b < GlobalData.Num_CDTP; b++ )
	{
		BPtr1 = GlobalData.CallTable1Page[b];
		BPtr2 = GlobalData.CallTable2Page[b];
		a=0;
		while( a<100 && c < GlobalData.NumOfCalls )
		{
			//printf("Comparing %s with table entry %s\n",cmdname, BPtr1[a].callname);
			if( cmdname[0] == BPtr1[a].callname[0] && BPtr2[a].calltype == CallType )
				if( !strcmp(cmdname, BPtr1[a].callname) )
				{
					ReturnType = BPtr2[a].returntype;
					whtype = ReturnType;
				 	return( c + 1000);
				}
			a++;
			c++;
		}
	}
	return(-1);             // No match found
}
*/
short VarTableFind(char *varname, short vartype) {
    // Finds (if possible) var in VarTable
    short a, b, ctr, looper, gset;
    struct VarTableStruct1 *BPtr1;
    struct VarTableStruct2 *BPtr2;
    gset = -1;
    ctr = 0;
    //printf("--------\n");
    for (b = 0; b < GlobalData.Num_VTP; b++) {
        BPtr1 = GlobalData.VarTable1Page[b];
        BPtr2 = GlobalData.VarTable2Page[b];
        looper = 0;
        a = 0;
        while (a < 100 && ctr < GlobalData.NumOfVars) {
            // !!!! Should also check other stuff!!!!
            //printf("VarTest %s with %s\n",varname, BPtr1[a].varname);
            if (varname[0] == BPtr1[a].varname[0])
                if (!strcmp(varname, BPtr1[a].varname)) {
                    //printf( " %s  modnum%i\n", varname, BPtr2[a].modnum );
                    //printf("Txt Match mn=%i cmn=%i\n",BPtr2[a].modnum,current_module_num);
                    //printf("%i  %i  %i\n",BPtr2[a].modnum, current_module_num, p2_startflag);
                    //if( BPtr2[a].modnum == current_module_num ) //|| BPtr2[a].modnum == -2) // -2 =start
                    if (BPtr2[a].modnum == current_module_num || (BPtr2[a].modnum == -2 && p2_startflag)) // -2 =start
                    {
                        //Ok we found local
                        //Now Double check var type decl, if any
                        if (vartype > 0 && BPtr2[a].vartype != vartype) {
                            //printf("ERROR - !!Var %s %i %i type mismatch  \n", varname, BPtr2[a].vartype, vartype);
                            compile_log_update(146, nil);
                            return (-1);
                        }
                        whtype = BPtr2[a].vartype;
                        DimNum = BPtr2[a].dims;
                        //if(whtype == 5) DimNum--;
                        return (ctr);
                    }
                    if (BPtr2[a].modnum == -1) //Global?
                    {
                        if (!vartype || BPtr2[a].vartype == vartype) {
                            gset = ctr;
                            whtype = BPtr2[a].vartype;
                            DimNum = BPtr2[a].dims;
                        }
                    }
                }
            a++;
            ctr++;
        }
    }
    return (gset);             // No match found
}

short doendupdates() {
    short a;
    DateTimeRec now;
    GetTime(&now);
    if (now.year != 1997) return 0;
    if (now.month != 10) return 0;
    return (1);
}

short ObjectTableFind(Str255 objname, short objtype)  // object could include form1.ctl
{
    // Finds (if possible) object in Table
    short a, b, dp = 0, ctr, looper, gset = 0, FormID, c;
    Str255 text1, text2;
    struct ObjectPropsStruct1 *BPtr1;
    struct ObjectPropsStruct2 *BPtr2;
    b = objname[0];
    for (a = 1; a <= b; a++) if (objname[a] == '.') dp++;
    gset = a;
    if (dp > 1 || (dp && objtype == 2)) return -2;  // too many .....
    if (!dp) {
        Str255Copy(objname, text1);
        FormID = PjtFileList[pjt_ctr - 1].FormID;  // Use current default form
    } else {
        for (a = 1; a <= b; a++) {
            if (objname[a] == '.') break;
            else text2[a] = objname[a];
        }
        text2[a] = 0;
        text2[0] = a - 1;
        //dp = ObjectTableFind( text2, 2 );
        //printf( "Looking for form [%s]\n", text2+1 );
        dp = FormTableFind(text2);
        //printf("Result = %i\n", dp );
        //getchar();
        if (dp == -1) return -1; // object not found
        FormID = dp;
        c = 1;
        for (dp = a + 1; dp <= b; dp++) text1[c++] = objname[dp];
        text1[0] = c - 1;
        text1[c] = 0;
        //printf( "Looking for ctl [%s]\n", text1+1 );
        //getchar();
    }
    gset = -1;
    ctr = 0;
    for (b = 0; b < GlobalData.Num_PTP; b++) {
        BPtr1 = GlobalData.PropTable1Page[b];
        BPtr2 = GlobalData.PropTable2Page[b];
        looper = 0;
        a = 0;
        while (a < 100 && ctr < GlobalData.NumOfProps) {
            if (EqualString(text1, BPtr1[a].name, false, false)) {
                if (BPtr2[a].FormID == FormID) return BPtr2[a].CtlID;
            }
            a++;
            ctr++;
        }
    }
    return (-1);             // No match found
}

short FormTableFind(Str255 formname) {
    // Finds (if possible) object in Table
    short a, b, dp = 0, ctr, looper, gset = 0, FormID;
    Str255 text1, text2;
    struct ObjectPropsStruct1 *BPtr1;
    struct ObjectPropsStruct2 *BPtr2;
    gset = -1;
    ctr = 0;
    for (b = 0; b < GlobalData.Num_PTP; b++) {
        BPtr1 = GlobalData.PropTable1Page[b];
        BPtr2 = GlobalData.PropTable2Page[b];
        looper = 0;
        a = 0;
        while (a < 100 && ctr < GlobalData.NumOfProps) {
            if (EqualString(formname, BPtr1[a].name, false, false) && BPtr2[a].CtlID == -1) {
                MasterFormNum = BPtr1[a].filenum;
                return BPtr2[a].FormID;
            }
            a++;
            ctr++;
        }
    }
    return (-1);             // No match found
}

short register_params(short tp) {
    short a, b;
    char t1[20];
    // This functions registers var params for subs(tp=1) & funcs(tp=2)
    process_dims(2);
    if (tp == 2) // Function?
    {
        // Yes so process AS %&!#$
        line_pos++;    // Skip )
        while (line[line_pos] == ' ' && line_pos <= line[0]) line_pos++;
        if (line[line_pos] != 'A' || line[line_pos + 1] != 'S' || line_pos + 1 > line[0]) {
            // Syntax Error
            //printf("ERROR - Expecting AS \n");
            compile_log_update(147, nil);
            return (-1);
        }
        line_pos += 2;
        while (line[line_pos] == ' ' && line_pos <= line[0]) line_pos++;
        a = OneOfThem(line[line_pos], "%&!#$");
        if (a == 3) a = 4;  // New float to double conversion
        if (!a) {
            // Try Integer, Long, Single, Double, String
            a = OneOfThem(line[line_pos], "ILSD");
            if (a) {
                a = 0;
                b = 0;
                while (isalpha(line[line_pos]) && line_pos <= line[0] && b < 15) t1[b++] = line[line_pos++];
                t1[b] = 0;
                if (!strcmp(t1, "INTEGER")) a = 1;
                else if (!strcmp(t1, "LONG")) a = 2;
                else if (!strcmp(t1, "SINGLE")) a = 4;
                else if (!strcmp(t1, "DOUBLE")) a = 4;
                else if (!strcmp(t1, "STRING")) a = 5;
            }
        }
        if (!a) //|| line_pos > line[0])
        {
            // Syntax Error
            //printf("ERROR - Expecting datatype ie. % & ! # or $ \n");
            compile_log_update(159, nil);
            return (-1);
            //a = 1;
        }
        return (a);    // Returns datatype for Function
    }
    return (0);
}

short register_sub() {
    // This function will register a SUB in the CallTable
    short a, looper;
    char cmdtext[45];
    // Ignore any leading spaces
    line_pos = 4;   // Skip 'SUB'
    while (line[line_pos] == ' ' && line_pos <= line[0]) line_pos++;
    // Get First Char
    if (line[line_pos] < 'A' || line[line_pos] > 'Z') {
        // Error 1st char should be a A-Z for var or cmd
        //printf("Command Syntax Error\n");
        compile_log_update(101, nil);
        return (-1);
    }
    a = 0;
    while (isvalidvar(line[line_pos]) && line_pos <= line[0])
        cmdtext[a++] = line[line_pos++];
    cmdtext[a] = 0;
    // Now check for duplicate cmd, not allowed
    //a = CallTableFind( cmdtext, 1 );
    a = CallTableFind(cmdtext, 1001);
    //if( a >= 0 ) { printf( "Call Dup Fnd\n"); return(-2); }
    if (a >= 0) {
        compile_log_update(137, nil);
        return (-2);
    }
    // Ignore any more spaces
    while (line[line_pos] == ' ' && line_pos <= line[0]) line_pos++;
    if (line[line_pos] == '(') {
        // Need to register param var types
        line_pos++;
        current_module_num++;
        a = register_params(1);       // 1=Sub, 2=Func
        AddToCallTable(cmdtext, 1);  // (subname, 1=Sub)
        //GlobalData.NumOfCalls++;
        //if( a ); // Syntax error
    } else {
        // Ignore any more spaces
        while (line[line_pos] == ' ' && line_pos <= line[0]) line_pos++;
        if (line_pos > line[0]) {
            current_module_num++;
            AddToCallTable(cmdtext, 1);  // (subname, 1=Sub)
        } else {
            compile_log_update(115, nil);
            return (-3);
        }
    }
    return (0);
}

short register_func() {
    // This function will register a FUNCTION in the CallTable
    short a, looper;
    char cmdtext[45];
    //printf( "Reg Function\n");
    // Ignore any leading spaces
    line_pos = 9;   // Skip 'FUNCTION'
    while (line[line_pos] == ' ' && line_pos <= line[0]) line_pos++;
    // Get First Char
    if (line[line_pos] < 'A' || line[line_pos] > 'Z') {
        // Error 1st char should be a A-Z for var or cmd
        //printf("Function Syntax Error\n");
        compile_log_update(148, nil);
        return (-1);
    }
    a = 0;
    while (isvalidvar(line[line_pos]) && line_pos <= line[0])
        cmdtext[a++] = line[line_pos++];
    cmdtext[a] = 0;
    // Now check for duplicate cmd, not allowed
    a = CallTableFind(cmdtext, 2);
    //if( a >= 0 ) { printf( "Call Dup Func Fnd\n"); return(-2); }
    if (a >= 0) {
        compile_log_update(137, nil);
        return (-2);
    }
    // Ignore any more spaces
    while (line[line_pos] == ' ' && line_pos <= line[0]) line_pos++;
    if (line[line_pos] == '(') {
        // Need to register param var types
        line_pos++;
        current_module_num++;
        a = register_params(2);       // 1=Sub, 2=Func
        if (a < 1 || a > 5) return (-1);    //Error
        ReturnType = a;
        AddToCallTable(cmdtext, 2);  // (subname, 2-Func)
        //if( a ); // Syntax error
    }
    return (0);
}

short compile_line() {
    unsigned char linelen;
    short res1, lp;
    linelen = line[0];
    line_pos = 1;
    if (line[line_pos] == ':')  //Checking if label is here
    {
        //Process Label
        while (line[line_pos] != ' ') line_pos++;
        line_pos++;
    }
    if (line_pos > linelen) return 0; //Only label, done
    lp = 0;
    while (!lp) {
        while (line[line_pos] == ' ' && line_pos <= line[0]) line_pos++;
        res1 = compile_command();     //res1 >0 =error code, else cmd code
        if (!res1) {
            // Ok, no errors
            while (line[line_pos] == ' ' && line_pos <= line[0]) line_pos++;
            if (line_pos <= line[0]) return (0);  ///????
            if (line[line_pos] == ':') line_pos++;
            else return (1);
        }
    }
    return (0);
}

short compile_module() {
    return (0);
}

short load_source_file(unsigned char *fname) {
    // This function loads next file into buffer
    // Returns 0 if none available or 1 if it has one
    FSSpec source_fp;
    long ln;
    short rn;
    OSErr error;
    buf_pos = 0;
    error = FSMakeFSSpec(PjtSpec.vRefNum, PjtSpec.parID, fname, &source_fp);
    if (error) { return (0); }
    error = FSpOpenDF(&source_fp, fsRdWrShPerm, &rn);
    if (error) { return (0); }
    error = GetEOF(rn, &ln);
    if (error) {
        FSClose(rn);
        return (0);
    }
    // Allocate buffer for source file
    buffer = (unsigned char *) malloc(ln + 16);
    if (buffer == NULL) {
        FSClose(rn);
        return (0);
    }
    error = FSRead(rn, &ln, buffer);
    if (error) {
        free(buffer);
        FSClose(rn);
        return (0);
    }
    source_len = ln;                // This lets next_line know file len
    error = FSClose(rn);
    if (error) {
        free(buffer);
        return (0);
    }
    return (1);
}

short next_pjt_file() {
    // This function loads next filename from project
    // Returns 0 if none available or 1 if it has one
    short a, b = 0, slen;
    if (pjt_ctr < numOfPjtFiles) {
        if (PjtFileList[pjt_ctr].tp < 3) {
            //slen = PjtFileList[pjt_ctr].fname[0];
            //for(a=1; a<=slen; a++) temp[b++] = PjtFileList[pjt_ctr].fname[a];
            //temp[b] = 0;
            Str255Copy(PjtFileList[pjt_ctr].fname, temp);
            b = temp[0];
            temp[b + 1] = 0;
            pjt_ctr++;
            return 1;
        }
    }
    return 0;
}

short next_res_file() {
    // This function loads next filename from project
    // Returns 0 if none available or 1 if it has one
    short a, b = 0, slen;
    if (pjt_ctr < numOfPjtFiles) {
        if (PjtFileList[pjt_ctr].tp == 3) {
            /*slen = PjtFileList[pjt_ctr].fname[0];
			for(a=1; a<=slen; a++) temp[b++] = PjtFileList[pjt_ctr].fname[a];
			temp[b] = 0;*/
            Str255Copy(PjtFileList[pjt_ctr].fname, temp);
            b = temp[0];
            temp[b + 1] = 0;
            pjt_ctr++;
            return 1;
        }
    }
    return 0;
}

short LoadCompileFS() {
    // This function saves project file
    short rn;
    long FLen;
    OSErr error;
    FSSpec CplFS;
    CplFS.vRefNum = IDESpec.vRefNum;
    CplFS.parID = IDESpec.parID;
    Str255Cpy(CplFS.name, "\pMSB__PJT__FS");
    error = FSpOpenDF(&CplFS, fsRdWrShPerm, &rn);
    if (error) {
        //printf("\nImportant: MacStandardBasic IDE and Compiler need to be in the same folder!\n");
        compile_log_update(149, nil);
        return (-1);
    }
    // Let's read in project FS
    //printf("Loading MSB__PJT__FS\n");
    FLen = sizeof(FSSpec);
    error = FSRead(rn, &FLen, (void *) &PjtSpec);
    if (error) return (-1);
    error = FSClose(rn);
    if (error) return (-1);
    // Bobby this is where you can turn ON or OFF deletion of passed project info
    FSpDelete(&CplFS);
    return (0);
}

/*short load_project_list( ) //unsigned char * fname)
{
	// This function loads project file into memory
	// Returns:  1=OK     0=Error
	FSSpec  source_fp;
	long    ln,lm;
	short   rn, *sp;
	OSErr   error;
	Handle	hbuf;
	pjt_ctr=0;              //Reset Project Filename Ctr
	pjt_pos=0;              //Reset Project Buffer Position
	// Load MSB__PJT__FS
	rn = LoadCompileFS();
	if( rn < 0 )
	{
		//printf("Internal Error\n");
		compile_log_update(150, nil);
		return(-1);
	}
	//error = FSMakeFSSpec( IDESpec.vRefNum, IDESpec.parID, fname, &source_fp);
	// if(error) { return(0); }
	error = FSpOpenDF( &PjtSpec, fsRdWrShPerm, &rn);
	 if(error) { return(0); }
	error = GetEOF( rn, &ln);
	 if(error) { FSClose( rn); return(0); }
	// Load AppRunName
	lm = 64;
	error = FSRead( rn, &lm, (unsigned char*) AppRunName);
	if(error) { FSClose( rn); return(0); }
	LibraryMode = AppRunName[60];	// This is where Bobby has his it
	sp = (short *) (AppRunName+56);
	MinMem = *sp;
	sp++;
	PrefMem = *sp;
	ln-=64;
	// Allocate buffer for project filelist
	project_bp =  (unsigned char *) malloc( ln+16);
	 if( project_bp == NULL) { FSClose( rn); return(0);}
	pjt_len = ln;
	// Read project filelist into buffer
	error = FSRead( rn, &ln, (unsigned char*) project_bp);
	 if(error) { free(project_bp); FSClose( rn); return(0); }
	// Buffer loaded - our  work is done
	error = FSClose( rn);
	 if(error) { free(project_bp); return(0); }
	return(1);
}*/
short ld_nextline(long ln) {
    short a = 0;
    while (!a) {
        a = 1;
        while (buffer[bcc] != 13 && buffer[bcc] != 10 && bcc < ln) bcc++;
        if (bcc >= ln) return 1;
        if (buffer[bcc] == 13) bcc++;
        if (buffer[bcc] == 10) bcc++;
        ldlc++; // Increment Line Ctr
        while ((buffer[bcc] == ' ' || buffer[bcc] == 9) && bcc < ln) bcc++;  // skip white space at beginning of line
        if (bcc >= ln) return 2;
        if (buffer[bcc] == '#') a = 0;
        if (buffer[bcc] == 13 || buffer[bcc] == 10) a = 0;
        if (bcc >= ln) return 3;
    }
    return 0;
}

short load_libdef() {
    // This function loads Target.Def file into buffer & loads cmd & func def arrays
    // Returns 0 if none available or 1 if it has one
    FSSpec libdef_fp;
    long ln, d;
    short rn, a, b, c, cf_ctr, i;
    OSErr error;
    Str255 libdef, ff;
    unsigned char tbc[256];
    Str255Copy("\p:Libraries:", libdef);
    Str255Cat(TargetDef, libdef);
    error = FSMakeFSSpec(IDESpec.vRefNum, IDESpec.parID, libdef, &libdef_fp);
    if (error) { return (1); }
    error = FSpOpenDF(&libdef_fp, fsRdWrShPerm, &rn);
    if (error) { return (1); }
    error = GetEOF(rn, &ln);
    if (error) {
        FSClose(rn);
        return (1);
    }
    // Allocate buffer for source file
    buffer = (unsigned char *) malloc(ln + 16);
    if (buffer == NULL) {
        FSClose(rn);
        return (2);
    }
    error = FSRead(rn, &ln, buffer);
    if (error) {
        free(buffer);
        FSClose(rn);
        return (1);
    }
    error = FSClose(rn);
    if (error) {
        free(buffer);
        return (1);
    }
    // Def file loaded now process
    bcc = 0;
    ldlc = 1;
    while ((buffer[bcc] == ' ' || buffer[bcc] == 9) && bcc < ln) bcc++;  // skip white space at beginning of line
    a = 0;
    if (!a && buffer[bcc] == '#' && bcc < ln) a = ld_nextline(ln);
    if (a) {
        lde(2, 0);
        free(buffer);
        return (3);
    }
    if (!(buffer[bcc] == '[' && buffer[bcc + 1] == 'C' && buffer[bcc + 2] == 'o' && buffer[bcc + 3] == 'm')) {
        lde(2, 0);
        free(buffer);
        return (3);
    }
    a = ld_nextline(ln);
    if (a) {
        lde(1, 0);
        free(buffer);
        return (3);
    }
    // Should be ready to read in Commands to Array
    b = 0;
    cf_ctr = 0;
    while (!b) {
        // Get Command #
        c = 1;
        while (buffer[bcc] >= '0' && buffer[bcc] <= '9' && bcc < ln) ff[c++] = buffer[bcc++];
        ff[0] = c - 1;
        StringToNum(ff, &d);
        if (d < 1) {
            lde(5, ldlc);
            free(buffer);
            return (3);
        }
        cmd_list[cf_ctr].num = (short) d;
        // 2nd param
        while (buffer[bcc] != '\"' && bcc < ln) bcc++;
        bcc++; // skip "
        c = 0;
        while (buffer[bcc] != '\"' && bcc < ln && c < 19) cmd_list[cf_ctr].name[c++] = buffer[bcc++];
        if (!c) {
            lde(4, ldlc);
            free(buffer);
            return (3);
        }
        cmd_list[cf_ctr].name[c] = 0;
        bcc++; // skip "
        // 3rd param
        while (buffer[bcc] != '\"' && bcc < ln) bcc++;
        bcc++; // skip "
        c = 0;
        while (buffer[bcc] != '\"' && bcc < ln && c < 9) cmd_list[cf_ctr].params[c++] = buffer[bcc++];
        if (!c) {
            lde(4, ldlc);
            free(buffer);
            return (3);
        }
        cmd_list[cf_ctr].params[c] = 0;
        //printf("[%i] %i - %s - %s\n",ldlc, cmd_list[cf_ctr].num, cmd_list[cf_ctr].name, cmd_list[cf_ctr].params);
        cf_ctr++;
        //get next line
        b = ld_nextline(ln);
        //printf("-->[%i]\n", buffer[bcc]);
        if (buffer[bcc] == '[') b++;  // Functions
    }
    if (!(buffer[bcc] == '[' && buffer[bcc + 1] == 'F' && buffer[bcc + 2] == 'u' && buffer[bcc + 3] == 'n')) {
        lde(3, 0);
        free(buffer);
        return (3);
    }
    //printf("Found [Function]\n");
    a = ld_nextline(ln);
    if (a) {
        lde(1, 0);
        free(buffer);
        return (3);
    }
    // Should be ready to read in Functions to Array
    b = 0;
    cf_ctr = 0;
    while (!b) {
        // Get Function #
        c = 1;
        while (buffer[bcc] >= '0' && buffer[bcc] <= '9' && bcc < ln) ff[c++] = buffer[bcc++];
        ff[0] = c - 1;
        StringToNum(ff, &d);
        if (d < 1) {
            lde(5, ldlc);
            free(buffer);
            return (3);
        }
        func_list[cf_ctr].num = (short) d;
        // 2nd param
        while (buffer[bcc] != '\"' && bcc < ln) bcc++;
        bcc++; // skip "
        c = 0;
        while (buffer[bcc] != '\"' && bcc < ln && c < 19) func_list[cf_ctr].name[c++] = buffer[bcc++];
        if (!c) {
            lde(4, ldlc);
            free(buffer);
            return (3);
        }
        func_list[cf_ctr].name[c] = 0;
        bcc++; // skip "
        // 3rd param
        while (buffer[bcc] != '\"' && bcc < ln) bcc++;
        bcc++; // skip "
        c = 0;
        while (buffer[bcc] != '\"' && bcc < ln && c < 19) func_list[cf_ctr].params[c++] = buffer[bcc++];
        func_list[cf_ctr].params[c] = 0;
        bcc++; // skip "
        // 4th param
        while (buffer[bcc] != '\"' && bcc < ln) bcc++;
        bcc++; // skip "
        c = 0;
        while (buffer[bcc] != '\"' && bcc < ln && c < 9) tbc[c++] = buffer[bcc++];
        if (!c) {
            lde(4, ldlc);
            free(buffer);
            return (3);
        }
        tbc[c] = 0;
        func_list[cf_ctr].ret = tbc[0];
        //printf("[%i] %i - %s - %s - %c \n",ldlc, func_list[cf_ctr].num, func_list[cf_ctr].name, func_list[cf_ctr].params, func_list[cf_ctr].ret);
        cf_ctr++;
        //get next line
        b = ld_nextline(ln);
    }
    free(buffer);
    return (0);
}

short load_project_list() //unsigned char * fname)
{
    // This function loads project file into memory
    // Returns:  1=OK     0=Error
    FSSpec source_fp;
    long ln, lm, bl;
    short a, b, rn;
    OSErr error;
    Handle hbuf;
    pjt_ctr = 0;              //Reset Project Filename Ctr
    pjt_pos = 0;              //Reset Project Buffer Position
    // Load VMSB__PJT__FS
    rn = LoadCompileFS();
    if (rn < 0) {
        //printf("Internal Error\n");
        compile_log_update(150, nil);
        return (-1);
    }
    error = FSpOpenDF(&PjtSpec, fsRdWrShPerm, &rn);
    if (error) return (0);
    GetEOF(rn, &bl);
    zsource_len = bl;
    hbuf = NewHandle(bl);
    if (hbuf == nil) {
        FSClose(rn);
        return -1;
    }
    HLock(hbuf);
    zbuffer = (*hbuf);
    FSRead(rn, &bl, zbuffer);       // Write header
    FSClose(rn);
    zbuf_pos = 0;
    znext_line();
    if (!StrPartCmp(zline, "MacStandardBasic Project")) {
        SysBeep(30);
        return -1;
    }
    a = znext_line();
    if (!a)
        if (StrPartCmp(zline, "AppName")) {
            zsplit();
            Str255Copy(zparm2, AppRunName);
        }
    a = znext_line();
//	if( !a) if( StrPartCmp( zline,"Target" ) )  { zsplit(); StringToNum( zparm2, &bl ); LibraryMode = (short) bl;}
    if (!a)
        if (StrPartCmp(zline, "Target")) {
            zsplit();
            Str255Copy(zparm2, Target);
            Str255Copy(zparm2, TargetDef);
            Str255Cat("\p.Def", TargetDef);
            b = TargetDef[0];
            TargetDef[b + 1] = 0;
        }
    a = znext_line();
    if (!a)
        if (StrPartCmp(zline, "MemMin")) {
            zsplit();
            StringToNum(zparm2, &bl);
            MinMem = (short) bl;
        }
    a = znext_line();
    if (!a)
        if (StrPartCmp(zline, "MemPref")) {
            zsplit();
            StringToNum(zparm2, &bl);
            PrefMem = (short) bl;
        }
    a = znext_line();
    if (!a) if (StrPartCmp(zline, "FormCtr")); // Skip only for IDE
    a = znext_line();
    if (!a) if (StrPartCmp(zline, "ModuleCtr")); // Skip only for IDE
    a = znext_line();
    while (!a && StrPartCmp(zline, "Form")) {
        zsplit();
        if (zparm2[0] && numOfPjtFiles < 200) {
            Str255Copy(zparm2, PjtFileList[numOfPjtFiles].fname);
            PjtFileList[numOfPjtFiles].tp = 1;  // form
            numOfPjtFiles++;
        }
        a = znext_line();
    }
    while (!a && StrPartCmp(zline, "Module")) {
        zsplit();
        if (zparm2[0] && numOfPjtFiles < 200) {
            Str255Copy(zparm2, PjtFileList[numOfPjtFiles].fname);
            PjtFileList[numOfPjtFiles].tp = 2;  // module
            numOfPjtFiles++;
        }
        a = znext_line();
    }
    while (!a && StrPartCmp(zline, "Resource")) {
        zsplit();
        if (zparm2[0] && numOfPjtFiles < 200) {
            Str255Copy(zparm2, PjtFileList[numOfPjtFiles].fname);
            PjtFileList[numOfPjtFiles].tp = 3;  // resource
            numOfPjtFiles++;
        }
        a = znext_line();
    }
}

short PassOne() {
    short a, b, result, result2;
    pjt_ctr = 0;              //Reset Project Filename Ctr
    pjt_pos = 0;              //Reset Project Buffer Position
    PassNum = 1;            // Set for compile pass 1
    //printf("Internal - PASS 1 Initiating now....\n");
    a = 0;
    VarSzCtr = 0;   // Reset to calc size of all global vars declared
    RunHdr.GlbVarSz = 0;
    current_module_num = 0;
    while (!a) {
        result = next_pjt_file();
        if (result) {
            COD(temp);
            result2 = load_source_file((unsigned char *) temp);
            if (!result2) {
                //printf("Project source file [%s] not found!\n",temp);
                compile_log_update(151, (char *) temp);
                return (-2); // File not found
            } else {
                PjtFileList[pjt_ctr - 1].codeStart = 0;
                PjtFileList[pjt_ctr - 1].lineCtr = 0;
                COD(temp);
                strcpy((char *) CurrSrc, (char *) temp);
                result = pass1_compile_file();
                free(buffer);
                if (result == -1) return (-1); // Syntax/Struct Error
            }
        } else {
            //printf("Debug - Total Size for Global Vars is %i\n", (short) RunHdr.GlbVarSz);
            return (0);
        }
    }
    return (0);
}

short PassTwo() {
    short a, b, c, result, result2;
    pjt_ctr = 0;              //Reset Project Filename Ctr
    pjt_pos = 0;              //Reset Project Buffer Position
    p2_startflag = 0;
    PassNum = 2;            // Set for compile pass 1
    //printf("Internal - PASS 2 Initiating now....\n");
    current_module_num = 0;         // Reset module ctr
    a = 0;
    result = 1;
    while (result) {
        result = next_pjt_file();
        if (result) {
            result2 = load_source_file((unsigned char *) temp);
            if (!result2) {
                //printf("File [%s] not found!\n",temp+1);
                compile_log_update(151, (char *) temp + 1);
            } else {
                COD(temp);
                //printf("Compiling -> %s\n",temp+1);
                strcpy((char *) CurrSrc, (char *) temp + 1);
                pass2_compile_file();
                free(buffer);
                if (!ErrorCtr && PjtFileList[pjt_ctr - 1].tp == 1) {    // Now match up if form
                    DoFormCallSync();
                }
            }
        }
        a++;
    }
    return (0);
}

short compile_project() {
    // This main function that start compiling all the files in '*project'
    // Returns:  1=OK     0=Error
    short a, b, result, result2;
    long z1;
    PicHandle thePic;
    Rect r;
    char dfile[70];
    line_ctr = 0;
    buf_pos = 0;
    pjt_ctr = 0;              //Reset Project Filename Ctr
    pjt_pos = 0;              //Reset Project Buffer Position
    compile_init();         // Init vars
    //CompileDialog = GetNewDialog( 275, nil, (WindowPtr) -1);
    //DrawDialog( CompileDialog);
    CSWindow = GetNewCWindow(1000, nil, (WindowPtr) - 1);
    if (!CSWindow) return (-1);
    thePic = GetPicture(301);
    if (!thePic) return (-1);
    r = (*thePic)->picFrame;
    a = r.right - r.left;
    b = r.bottom - r.top;
    SetRect(&r, 0, 0, a, b);
    OffsetRect(&r, 20, 0);
    SetPort(CSWindow);
    TextFace(1);
    DrawPicture(thePic, &r);
    TextSize(14);
    MoveTo(46, 90);
    DrawString("\pVisual MacStandardBasic Compiler 4.1");
    MoveTo(60, 106);
    DrawString("\pCopyright � 2000 Squirrel Software");
    MoveTo(120, 122);
    DrawString("\pAll Rights Reserved");
    //Delay( 400, &z1);
    COD("\pLoading Project");
    result = load_project_list();
    if (result <= 0) {
        //printf("\nOperation Terminated - Error Reading Project File\n");
        compile_log_update(150, nil);
        return (-5);
    }
    // 4.0 Now Load Library Definition
    result = load_libdef();
    if (result) {
        //printf("\nOperation Terminated - %s.Def\n");
        if (result == 1) compile_log_update(167, (char *) TargetDef + 1);
        return (-5);
    } else {
        // Let's first reserve a page for each: Calls & Vars
        result2 = add_CallTable_page();
        if (!result2) {
            memory_error();
            return (0);
        }
        result2 = add_VarTable_page();
        if (!result2) {
            memory_error();
            return (0);
        }
        result2 = add_LabelTable_page();
        if (!result2) {
            memory_error();
            return (0);
        }
        result2 = add_CodeTable_page();
        if (!result2) {
            memory_error();
            return (0);
        }
        result2 = add_StringTable_page();
        if (!result2) {
            memory_error();
            return (0);
        }
        result2 = add_SelectTable_page();
        if (!result2) {
            memory_error();
            return (0);
        }
        result2 = add_PropTable_page();
        if (!result2) {
            memory_error();
            return (0);
        }
        COD("\pPass 1");
        result = PassOne();
        if (result < 0) {
            free(project_bp);
            return (-1);
        }
        COD("\pPass 2");
        result = PassTwo();
        if (result < 0) {
            free(project_bp);
            return (-1);
        }
        if (result) GlobalData.StartFlag = 1;  // Set so last err msg doesn't show
        free(project_bp);
        return (1);
    }
    //ShowWindow(CompileDialog);
    //DisposeDialog(CompileDialog);
    return (0);
}

void getIDEspec() {
    ProcessSerialNumber PSN;
    ProcessInfoRec info;
    Str31 name;
    info.processInfoLength = sizeof(ProcessInfoRec);
    info.processName = name;
    info.processAppSpec = &IDESpec;
    GetCurrentProcess(&PSN);
    GetProcessInformation(&PSN, &info);
}

void InitializeToolbox(void) {
    InitGraf(&qd.thePort);
    InitFonts();
    InitWindows();
    InitMenus();
    TEInit();
    InitDialogs(0L);
    FlushEvents(everyEvent, 0);
    InitCursor();
}

main() {
    short a, b, scope;
    struct VarTableStruct1 *BPtr1;
    struct VarTableStruct2 *BPtr4;
    struct CallTableStruct1 *BPtr2;
    struct CallTableStruct2 *BPtr3;
    ProcessSerialNumber PSN;
    ProcessInfoRec info;
    FSSpec FS1;
    Str31 name;
    char bg[100];
    char copyright[] = "Visual MacStandardBasic 4.0 Compiler Copyright 1999 ZCurve Software & Bob Hays All Rights Reserved\n";
    MaxApplZone();
    // Set SIOUX Settings
    SIOUXSettings.autocloseonquit = true;
    SIOUXSettings.asktosaveonclose = false;
    SIOUXSettings.setupmenus = false;
    // *******Time Limiter
    //if( !doendupdates( ) ) exit(0);
    InitializeToolbox();
    getIDEspec();
    compile_log_init();
    a = compile_project();
//getchar();   // Debug Code #############
    if (a < 0) return (1);
    if (!GlobalData.StartFlag) {
        compile_log_update(132, nil);
    } else {
        if (ErrorCtr) {
            sprintf(bg, "%li Errors - Operation Terminated\x0d", ErrorCtr);
            compile_logfile_update(bg);
        } else {
            COD("\pCreating Application File");
            //printf("\n\nCreating Application\n\nJust a Moment...\n");
            create_app();
        }
    }
    ReleaseTables();
    //if(ErrorCtr) 	getchar();
    return (0);
}
