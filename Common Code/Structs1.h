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



// Structures

extern struct CallTableStruct1
{
	char    callname[41];           // Sub & Func names can be upto 40 chars long 
	short   pjt_filenum;
} CallTable1;   

extern struct CallTableStruct2
{
	UCHAR   calltype;               // 1=Sub  2=Func
	char    params[10];
	long    par_vars[10];
	long    location;
	long	VarMemSz;
	short	returntype;
} CallTable2;   

extern struct VarTableStruct1
{
	char    varname[41];            // Var names can be upto 40 chars long
} VarTable1;    

extern struct VarTableStruct2
{
	short   modnum;
	UCHAR   vartype;                // 1=int
	UCHAR   dims;                   // up to 16 dimensions
	long    dimsize[16];
	long	offset;
} VarTable2;    

extern struct LabelTableStruct
{
	char    labelname[41];            // Label names can be upto 40 chars long
	UCHAR   status;
	short   modnum;
	//long    location;     Taken out & placed in own buffer
} LabelTable;     

extern struct SelectTableStruct
{
	short   casenum;            
	short	status;
	long    value;		
	long    location;     
} SelectTable;     

extern struct GlobalDataStruct
{
	long    NumOfVars, NumOfCalls, NumOfLabels, NumOfProps, CodePtr;
	struct  CallTableStruct1 *CallTable1Page[100];  // Upto 100 Pages of 100 calls each
	struct  CallTableStruct2 *CallTable2Page[100];  // Upto 100 Pages of 100 calls each
	short   Num_CTP;
	struct  VarTableStruct1 *VarTable1Page[100];    // Upto 100 Pages of 100 varnames each
	struct  VarTableStruct2 *VarTable2Page[100];    // Upto 100 Pages of 100 varnames each
	struct  LabelTableStruct  *LabelTablePage[100];   // Upto 100 Pages of 100 Labels each
	long    *LabelLocPage[100];     
	struct  ObjectPropsStruct1 *PropTable1Page[100];    // Upto 100 Pages of 100 props each
	struct  ObjectPropsStruct2 *PropTable2Page[100];    // Upto 100 Pages of 100 props each
	UCHAR   *CodeTablePage[100];            // Upto 100 Pages of 16K each
	UCHAR   *StringTablePage[100];          // Upto 100 Pages of 4K each
	short   Num_VTP;
	short   Num_LTP;
	short   Num_CDTP;
	short   Num_STP;
	short   Num_PTP;
	UCHAR   StartFlag;
	UCHAR   CompMode;
	UCHAR   DimMode;
	long    CodeCtr;
	struct  SelectTableStruct *SelectTablePage[100];   // Upto 100 Pages of 100 Select cases each
	short   Num_SCTP;
	long	NumOfSelects;
} GlobalData;   

extern struct ObjectPropsStruct1
{
	Str63	name;
	Str63	proc;
	short	filenum;
} PropTable1;

extern struct ObjectPropsStruct2
{
	short	FormID;
	short	CtlID;  // -1 if it is a form definition itself
	short	CtlType;
	Str255	text;
	short	left, top, width, height;
	short	procID;
	short	visible, value, enabled;
	Str63	fontname;
	short	fontsize, fontstyle;
	short	opt14, opt15, opt16, opt17, opt18, opt19, opt20, opt21;
	RGBColor BackRGB, ForeRGB;	
} PropTable2;

short compile_log_create();

short compile_log_update(short errno, char *special);
