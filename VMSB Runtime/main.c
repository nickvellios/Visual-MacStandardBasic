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
#include		<SIOUX.h>
#include		<Quickdraw.h>
#include		<Speech.h>
#define         UCHAR   unsigned char

short create_program_file( UCHAR * fname);
short create_app();
extern short PrintFlag;
short  ConFlag = 1;

void  InitializeToolbox( void );
short ReadCodeResources();
void IsSpeechAvailable();
short QT_Init();
void QT_CloseDown();
void WhichSoundMgrVer();
void FILE_Init();
void SPR_InitSprites();
short SPR_SpriteEnd();
void  MemInit();
void InitSoundMgr();
void SoundCleanUp();
void ClearPalettes();
short doendupdates( );
void StartAppearance(void);

Boolean gHaveAppearance=false;

Handle	codeHandle, hdrHandle, tablesHandle;
THPrint	PrintHandle=nil;
TPPrPort	PrinterPort;
char PrintActive;
extern short	PrintFlag;


unsigned char *buffer, *project_bp, temp[100], CurrSrc[65];
short 		pjt_ctr, pjt_pos, pjt_len, current_module_num, LabelCtr;
extern 		whdata;
long 		StrConCtr, VarSzCtr, GblSzCtr;
short 		whtype;
unsigned char ParList[12], DimNum;
long ParVar[12], DimList[12];
FSSpec  pgm_fp;

short	CmdKeys[256];

// Structures

struct CallTableStruct1
{
	char    callname[41];           // Sub & Func names can be upto 40 chars long 
	short     pjt_filenum;
} CallTable1;   

struct CallTableStruct2
{
	UCHAR   calltype;                       // 1=Sub  2=Func
	char    params[10];
	long    par_vars[10];
	long    location;
	long	VarMemSz;
	short	returntype;	// Not used with Subs, only Functions
} CallTable2;   

struct VarTableStruct1
{
	char    varname[41];            // Var names can be upto 40 chars long
} VarTable1;    

struct VarTableStruct2
{
	short     modnum;
	UCHAR   vartype;                // 1=short
	UCHAR   dims;                   // up to 16 dimensions
	long    dimsize[16];
	long	offset;
} VarTable2;    

struct LabelTableStruct
{
	char    labelname[41];            // Label names can be upto 40 chars long
	UCHAR   status;
} LabelTable;     

struct SelectTableStruct
{
	short   casenum;            
	short	status;
	long    value;		
	long    location;     
} SelectTable;     

struct ObjectPropsStruct2  // new 3.0 stuff
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


struct RunHdrStruct // This Data Structure holds runtime info for 4.0
{       
	long    NumOfVars;
	long    NumOfCalls;
	long    NumOfLabels;
	long    StrLen;
	long    CodeLen;
	long    CodeStart;		// Where to start
	long	GlbVarSz;
	long	StartVarSz;
	long	CallTableOffset;	// Don't need VarTable Offset because it = 0
	long	LabelTableOffset;
	long	StrTableOffset;
	long	SelectTableOffset;
	long	NumOfSelects;
	long	NumOfProps;   		// New for 3.0
	long	PropTableOffset;    // New for 3.0

} RunHdr;	

struct CtlStruct
{
   	ControlHandle	theControl;
   	TEHandle		te;
   	ListHandle		lt;			// Thats a lowercase L in lt
	PicHandle		pic;
	Rect			theRect;
   	short			WinNum;
	UCHAR			status;
	short			subcode;
	MenuHandle		theMenu;
	short			lastItem;
	short			CtlID;
	short			opt1, opt2;
	RGBColor 		BackRGB, ForeRGB;	
	short			enabled;
} CtlData[500];   

extern struct RunVarStruct // This Data Structure holds runtime info 4.0
{       
	struct  CallTableStruct1 *CallTable1;  
	struct  CallTableStruct2 *CallTable2;  
	struct  VarTableStruct1 *VarTable1;    
	struct  VarTableStruct2 *VarTable2;    
	long	*LabelTable;   
	unsigned char * StrTable;
	unsigned char * CodeTable;
	unsigned char * GlobalVarSpace;
	struct  SelectTableStruct *SelectTable;  
	struct  ObjectPropsStruct2 *PropTable;  
} RunVar;


short line_pos,pline_pos,line_len;
long source_len, buf_pos, line_ctr;

//#include "proto1.h"

char MSB_SpeechAvail=0;
short  MSB_QTAvail=0;
short	MSB_SndMgrVer=0;
TEHandle  ActiveTE = nil;
void memory_error();
short RunProgram();

void memory_error()
{
}


void  InitializeToolbox( void )
{
   InitGraf( &qd.thePort );
   InitFonts();
   InitWindows();
   InitMenus();
   TEInit();
   InitDialogs( 0L );
   FlushEvents( everyEvent, 0 );
   InitCursor();
   
   TEFromScrap();
 
}

void IsSpeechAvailable()
{
	OSErr	theError;
	long	theResult;
	
	MSB_SpeechAvail = 0;
	theError = Gestalt( gestaltSpeechAttr, &theResult );

	if(  theError || !theResult ) return;
	if( theResult & ( 1 << gestaltSpeechMgrPresent ) > 0 ) MSB_SpeechAvail = 1;
	return;
}


short ReadCodeResources()
{

	// Load Code	
	codeHandle = GetIndResource( 'MSBC', 1 );

	// Load Code	
	tablesHandle = GetIndResource( 'MSBT', 1 );

	// Load Header
	hdrHandle = GetIndResource( 'MSBH', 1 );

	// Copy contents over then release resource
	HLock( hdrHandle );
	BlockMoveData( (UCHAR*) *hdrHandle, (UCHAR*) &RunHdr, sizeof( RunHdr ) );
	HUnlock( hdrHandle );
	DisposeHandle( hdrHandle );

	DetachResource( codeHandle );
	DetachResource( tablesHandle );
	
	HLock( codeHandle );
	HLock( tablesHandle );
	return(0);

}


main()
{
	char	copyright[]="Visual MacStandardBasic Copyright 2000 Squirrel Software - All Rights Reserved\n";
	short	a;
	GrafPtr		OldGPort;
	
	MaxApplZone();
	// Set SIOUX Settings
	SIOUXSettings.autocloseonquit = true;
	SIOUXSettings.asktosaveonclose = false;
	SIOUXSettings.setupmenus = false;
	
	InitializeToolbox();
	StartAppearance();
	IsSpeechAvailable();  // Check for speech
	QT_Init();	// Check for QT
	MemInit();
	ClearPalettes();
	InitSoundMgr();
	FILE_Init();
	SPR_InitSprites();
	
	ReadCodeResources();

	for(a=0;a<256; a++) CmdKeys[a] = 0;  // Reset cmd keys

	GetDateTime( (unsigned long*) (&qd.randSeed) );
	//qd.randSeed = TickCount(); 
	GetPort( &OldGPort);
	PrintHandle = (THPrint) NewHandle(sizeof(TPrint));
	PrOpen();
	//PrintDefault(PrintHandle);
	PrintActive = false;
	
	RunProgram();
	
	QT_CloseDown();
	SPR_SpriteEnd();
	SoundCleanUp();
	HUnlock( codeHandle );
	HUnlock( tablesHandle );
	DisposeHandle( codeHandle );
	DisposeHandle( tablesHandle );
	PrClose();
	SetPort( OldGPort);
	if( ConFlag && PrintFlag )
	{
		printf("\n Press any key to end >");
		getchar();
	}
	
	return(0);
}

void StartAppearance(void)
{
	OSStatus err;
	long response;
	
	gHaveAppearance = false;
	
	if (!(err = Gestalt (gestaltAppearanceAttr, &response))) {
		gHaveAppearance = response & (1 << gestaltAppearanceExists) ? true : false;
	}
	else if (err == gestaltUndefSelectorErr)
	{
		gHaveAppearance = false;
	}
	if (!gHaveAppearance) return;

	err = RegisterAppearanceClient();
}