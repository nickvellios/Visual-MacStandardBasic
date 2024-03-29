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
#include <SIOUX.h> 
#include "SIOUXGlobals.h"
#include "SIOUXMenus.h"
#include "SIOUXWindows.h"
#include        <Palettes.h>
#define         UCHAR   unsigned char

extern ConFlag;
extern TEHandle	ActiveTE;
extern short	CmdKeys[256];
extern struct CtlStruct
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

extern struct WinStruct
{
   	WindowPtr	theWindow;
   	Rect		winRect;	// This is total window size
	UCHAR		status;
	short		CharX, CharY;
	short		GraphX, GraphY;
	short		subcode;
	short		wintype;
	Handle		drawCmds;
	char		update;
	short		opt1;
	short		FormID;
	RGBColor 	BackRGB, ForeRGB;	
   	Rect		clipRect;	
   	Rect		MinMax;	
} WinData[100];   

short  WhichControl( );
void TextFix( short a );

void StoreInt( short varnum, long VarOffset, short Value);
void StoreLong( short varnum, long VarOffset, long Value);
void StoreSingle( short varnum, long VarOffset, float Value);
void StoreDouble( short varnum, long VarOffset, short double Value);
void StoreStr( short varnum, unsigned char *,long VarOffset, long);
short pass_assign( short vartype, short varnum);
float get_singlearg_val();
short double get_doublearg_val();
short FreeReturnStr();
void RStrToStr255( UCHAR * st1, long slen, Str255 st2);
long ArrayCalc( short varnum );
long grab_intvar(short varnum);
long grab_longvar(short varnum);
float grab_singlevar(short varnum);
short double grab_doublevar(short varnum);

short cmd_dummy();
short cmd_none();
short cmd_beep();
short cmd_let();
short cmd_print();
short cmd_input();
short cmd_console();
short cmd_end();
short cmd_goto();
short cmd_gosub();
short cmd_return();
short cmd_for();
short cmd_next();
short cmd_do();
short cmd_exitloop();
short cmd_loop();
short cmd_if();
short cmd_else();
short cmd_endif();
short cmd_exitsub();
short cmd_exitfunc();
short cmd_window();
short cmd_closewindow();
short cmd_alertbox();
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
short cmd_droplist();
short cmd_button();
short cmd_checkbox();
short cmd_radio();
short cmd_scrollbar();
short cmd_textbox();
short cmd_textboxro();
short cmd_list();
short cmd_listadd();
short cmd_listremove();
short cmd_listclear();
short cmd_label();
short cmd_pict();
short cmd_licon();
short cmd_timer();
short cmd_setcolor();
short cmd_backcolor();
short cmd_setRGBcolor();
short cmd_backRGBcolor();
short cmd_hilite();
short cmd_stretchline();	
short cmd_stretchbox();	
short cmd_stretchoval();	
short cmd_stretchcircle();		
short cmd_picturebegin();		
short cmd_picturedone();		
short cmd_picturesave();
short cmd_drawupdate();
short cmd_setdepth();
short cmd_winorigin();
short cmd_drawpicture();

short cmd_addmenu();
short cmd_addapple();
short cmd_adddesk();
short cmd_addmenuitem();
short cmd_menufonts();
short cmd_menublank();
short cmd_menuclear();
short cmd_menudelete();
short cmd_menudeleteitem();
short cmd_menudisable();
short cmd_menuenable();
short cmd_menuinsertitem();
short cmd_menumark();
short cmd_menuprepare();
short cmd_menuredraw();
short cmd_menutext();
short cmd_submenu();

short cmd_select();
short cmd_case();
short cmd_endcase();
short cmd_default();

short cmd_movewindow();
short cmd_sizewindow();
short cmd_windowtitle();
short cmd_hidewindow();
short cmd_showwindow();
short cmd_redrawwindow();
short cmd_clipwindow();
short cmd_cleargraphics();
short cmd_clearcontrols();
short cmd_clearwindow();
short cmd_loadpal();
short cmd_winpal();

short cmd_say();
short cmd_sndload();
short cmd_sndplay();	
short cmd_sndunload();		
short cmd_delay();		

short cmd_printopen();		
short cmd_printclose();		
short cmd_newpage();		


short cmd_ctlremove();
short cmd_ctlhide();
short cmd_ctlshow();
short cmd_ctldisable();
short cmd_ctlenable();
short cmd_ctlsize();
short cmd_ctlmove();
short cmd_ctltext();
short cmd_ctllist();
short cmd_ctlalign();
short cmd_textselect();	
short cmd_listselect();	
short cmd_listdeselect();	
short cmd_setvalue();		
short cmd_cut();		
short cmd_copy();		
short cmd_paste();		
short cmd_clear();		
short cmd_scrollset();

short cmd_qtopen();
short cmd_qtplay();
short cmd_qtclose();
short cmd_qtmove();

short cmd_seropen();
short cmd_serclose();
short cmd_serwrite();
short cmd_serset();

short cmd_filecreate();
short cmd_fileopen();
short cmd_fileclose();
short cmd_filewrite();		
short cmd_filesetloc();		
short cmd_filedelete();
short cmd_filerename();
short cmd_filelock();
short cmd_fileunlock();
short cmd_filewritebuffer();
short cmd_filecopy();
short cmd_createfolder();

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

short cmd_setcursor();
short cmd_showcursor();
short cmd_hidecursor();
short cmd_clearerror();
short cmd_onerror();
short cmd_events();
short cmd_poke();		
short cmd_pokeint();		
short cmd_pokelong();		

short cmd_memalloc();
short cmd_memfree();	
short cmd_memresize();	
short cmd_memfill();	
short cmd_memint();	
short cmd_memlong();	
short cmd_memsingle();	
short cmd_memdouble();	
short cmd_memstring();	

short cmd_setcmdkey();
short cmd_formshow();
short cmd_conmsg();
short cmd_conclear();
short cmd_contitle();

// From cm_forms.c
short FORM_Create( short FormID );

short FindCtlID( short id );
short FindFormID( short id );


// From cm_gfx.c
short GFX_CreateWin(short x1, short y1, short x2, short y2, short WinType, Str255, short);
short GFX_CloseWindow( short WinNum);
short GFX_HideWindow( short WinNum);
short GFX_ShowWindow( short WinNum);
short GFX_SizeWindow(short WinNum, short x, short y);
short GFX_MoveWindow(short WinNum, short x, short y);
short GFX_SetWindowTitle(short WinNum, Str255 wtitle);
short GFX_WinRedraw( short WinNum );
short GFX_ClipWindow(short WinNum, short x, short y, short wide, short ht);
void  GFX_ClearGfx( short WinNum, short flag);
void  GFX_DrawUpdate( short WinNum, short flag );
short GFX_LoadPal(short palNum, short resID, short ent, short opts);
short GFX_WinPal(short WinNum, short palNum);
short GFX_WindowOrigin(short WinNum, short x, short y);


// From cm_ctls.c
short	CTL_DropList(short WinNum, short x1, short y1, short width, short cmd );
short	CTL_Button(short WinNum, short x1, short y1, short x2, short y2, char *, short );
short	CTL_CheckBox(short WinNum, short x1, short y1, short x2, short y2, char *, short );
short	CTL_Radio(short WinNum, short x1, short y1, short wide, short ht, short group, char* btext, short cmd   );
short	CTL_ScrollBar(short WinNum, short x1, short y1, short x2, short y2, short );
//short	CTL_Textbox(short WinNum, short x1, short y1, short x2, short y2, short opts, char* btext, short cmd );
short	CTL_Textbox(short WinNum, short x1, short y1, short wide, short ht, short opts, short opts2, char* btext, short cmd );
short	CTL_List(short WinNum, short x1, short y1, short x2, short y2, short option,  short cmd );
short   CTL_ListAdd( short ListNum, unsigned char *buf, long buflen );
short   CTL_ListClear( short ListNum );
short   CTL_ListRemove( short ListNum, short item );
short	CTL_Label( short WinNum, short x1, short y1, short width, short height, char* btext, short cmd );
short	CTL_Picture( short WinNum, short ResID, short x1, short y1, short x2, short y2, short cmd  );
short   CTL_SetTextSelect( short ctl, short start, short end );
short   CTL_ListSelect( short ListNum, short item );
short   CTL_ListDeselect( short ListNum, short item );
short	CTL_SetValue( short a, long b );
short 	CTL_Align( short a, short way );
short 	CTL_ClearControls( short WinNum );
short	CTL_Timer( short WinNum, long interval, short enabled, short cmd );

short CTL_Remove( short a );
short CTL_Hide( short a );
short CTL_Show( short a );
short CTL_Disable( short a );
short CTL_Enable( short a );
short CTL_Size( short a, short w, short h );
short CTL_Move( short a, short x, short y );
short CTL_SetText( short a, short appendFlag );
short CTL_LgIcon( short WinNum, short ResID, short x1, short y1, short width, short height, short cmd );
short CTL_Hilite( short a );
short CTL_TextboxRO( short ctl, short ro );
short CTL_ScrollSet(short ctl, short min, short max, short pager   );

// From cm_menu.c
short MENU_Add( short where, char * btext, short cmd );
short MENU_AddApple( );
short MENU_AddItem( short menu,  char * btext, short cmd, short cmdchar );
short MENU_InsertItem( short menu, short where, char * btext, short cmd, short cmdchar );
short MENU_AddDesk( );
short MENU_Delete( short menu );
short MENU_DeleteItem( short menu, short where );
void MENU_Blank( );
void MENU_Redraw(  );
void MENU_Clear(  );
short MENU_Disable( short menu, short where );
short MENU_Enable( short menu, short where );
short MENU_SetMark( short menu, short where, short mark);
short MENU_SetText( short menu, short where, char * btext);
short MENU_SubMenu( short menu, short where, short cmd );
short MENU_AddFonts( short menu );


// From cm_sound.c
short SND_Say( Str255 txt );
void  PlaySoundResourceAsynch( short );

// From cm_qt.c
short QT_OpenMovie( short WinNum, Str255 MovieFilename, short CtlFlag);
short QT_PlayMovie( short MovieNum );
short QT_ResetMovie( short MovieNum );
short QT_CloseMovie( short MovieNum );
short QT_OffsetMovie(short MovieNum, short x, short y);

// From cm_ser.c
short SER_BaudConfig( long baud, short databits, short stopbits, short parity );
long SER_CharAvail(short portNum);
short SER_Open( short portNum, short parms );
short SER_SetParm( short portNum, short parms );
short SER_Write( short portNum, unsigned char *buf, long buflen );
long SER_Read( short portNum, unsigned char *buf, long numToRead );
short SER_HandShake( short portNum, unsigned char *buf );
short SER_Close( short portNum );

// From cm_file.c
short FILE_CreateFileData( Str255 filename, long creator, long filetype, long script );
short FILE_OpenFileData( short perm, Str255 filename );
short FILE_CloseFile( short filenum );
short FILE_SetLoc(short filenum, long location);
short FILE_Write( short filenum );
short FILE_Delete( Str255 filename );
short FILE_Rename( Str255 filename1, Str255 filename2 );
short FILE_Lock( Str255 filename );
short FILE_Unlock( Str255 filename );
short FILE_WriteBuffer( short filenum, short bufferID, long Start, long WriteLen );
short FILE_Copy( Str255 filename1, Str255 filename2 );
short FILE_CreateFolder( Str255 filename );


// From cm_buf.c
short MemAlloc( long sz);
void  MemFree( long bufferID );
short MemResize( long bufferID, long newsz );
void  MemFill( long bufferID, long start, long nob, short value );
short MemInt( long bufferID, long location, short value );
short MemLong( long bufferID, long location, long value );
short MemSingle( long bufferID, long location, short double value );
short MemDouble( long bufferID, long location, short double value );
short MemString( long bufferID, long location );

long GetLabelLoc( short labelnum );
long get_longarg_val();
long grab_longvar(short varnum);
unsigned char *get_stringarg_val();
extern RGBColor 	GlobalBackRGB, GlobalForeRGB;	

void ErrHandler( short ErrNum, Str255 AddMsg);
extern short	LastErrorNumber, ErrorSub;

short     *GIP, GIP2, GIP3, GIP4, GIP5, JS_Ctr, LineLen,ITemp;
short  	VR_Int, *VR_IntPtr;
long 	VR_Long, *VR_LongPtr, DimLong, VR_Long2;
float 	VR_Single, *VR_SinglePtr;
short double VR_Double, *VR_DoublePtr;
UCHAR 	*VR_StrPtr;
short		ExitLoopFlag=0;

extern long 		Long_Function_Result;
extern short double	Double_Function_Result;

extern UCHAR    *CodeBuf;
extern long     CodePos, CodeLen, NLP;
extern short 	current_module_num, CallStackPtr, RunModCtr, LoopStackPtr, NextFlag;
extern UCHAR	*ReturnStr, ReturnStrFlag, EventsFlag;
extern long		ReturnStrLen;
extern char MSB_SpeechAvail;
extern short	MSB_QTAvail;
extern THPrint	PrintHandle;
extern TPPrPort	PrinterPort;
extern char 	PrintActive;
char			PassAssignFlag=0;
char			ConsoleFlag=1;
short			PrintFlag = 0;
extern short 	EventSetting;
extern long		EventCtr;
#include        "Structs1.h"

struct  stack1
{
	short   module;
	short   LL;
	long    retpt;
}       JmpStack[1000];
	

#include "CmdList.h"

extern struct   CallStackStruct
{
	long    location;
	short   module;
	unsigned char * varbuf;                         
} CallStack[500];

extern struct RunVarStruct // This Data Structure holds runtime info
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


extern struct RunHdrStruct // This Data Structure holds runtime info
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

short FindCtlID( short id )
{
	short a;
	for( a=0; a<500; a++ )
	{
	   if( CtlData[a].status && CtlData[a].CtlID == id ) return a;
	
	}
	return -1; 
}

short FindFormID( short id )
{
	short a;
	
	for( a=0; a<100; a++ )  if( WinData[a].status && WinData[a].FormID == id ) return a;
	return -1; 
}


long GetLabelLoc( short labelnum )
{
	long    result;

	result = RunVar.LabelTable[labelnum];          // location
	
	return(result);
}

void StoreInt( short varnum, long VarOffset, short Value)
{

	if( RunVar.VarTable2[varnum].modnum == -1)  // Global ?
		VR_IntPtr = (short*) (RunVar.GlobalVarSpace +  RunVar.VarTable2[varnum].offset + VarOffset);
	else // Local 
		VR_IntPtr = (short*) (CallStack[ CallStackPtr].varbuf +  RunVar.VarTable2[varnum].offset + VarOffset);
	
	*VR_IntPtr = Value;
	
	return;
}

void StoreLong( short varnum, long VarOffset, long Value)
{

	if( RunVar.VarTable2[varnum].modnum == -1)  // Global ?
		VR_LongPtr = (long*) (RunVar.GlobalVarSpace +  RunVar.VarTable2[varnum].offset + VarOffset);
	else // Local 
		VR_LongPtr = (long*) (CallStack[ CallStackPtr].varbuf +  RunVar.VarTable2[varnum].offset + VarOffset);
	
	*VR_LongPtr = Value;
	
	return;
}

void StoreSingle(short varnum, long VarOffset, float Value)
{
	float *sp;
	
	if( RunVar.VarTable2[varnum].modnum == -1)  // Global ?
	{
		sp = (float*) (RunVar.GlobalVarSpace +  RunVar.VarTable2[varnum].offset + VarOffset);
	}
	else 
	{
		sp = (float*) (CallStack[ CallStackPtr].varbuf +  RunVar.VarTable2[varnum].offset + VarOffset);
		//Storing value in Local
	}
	*sp = Value;
	return;
}

void StoreDouble(short varnum, long VarOffset, short double Value)
{
	if( RunVar.VarTable2[varnum].modnum == -1)  // Global ?
	{
		VR_DoublePtr = (short double*) (RunVar.GlobalVarSpace +  RunVar.VarTable2[varnum].offset + VarOffset);
		//Storing value in Global
	}
	else 
	{
		VR_DoublePtr = (short double*) (CallStack[ CallStackPtr].varbuf +  RunVar.VarTable2[varnum].offset + VarOffset);
		//Storing value in Local
	}
	*VR_DoublePtr = Value;
	return;
}

void StoreStr( short varnum, unsigned char * strval, long VarOffset, long stlen)
{
	//short a;
	if( RunVar.VarTable2[varnum].modnum == -1)  // Global or Local ?
	{
		//Global
		VR_StrPtr = (unsigned char*) (RunVar.GlobalVarSpace +  RunVar.VarTable2[varnum].offset + VarOffset);
	}
	else
	{
		//Local
		VR_StrPtr = (unsigned char*) (CallStack[ CallStackPtr].varbuf +  RunVar.VarTable2[varnum].offset + VarOffset);
	}

	// Get max len of string
	VR_Int = RunVar.VarTable2[varnum].dims;
	VR_Long = RunVar.VarTable2[varnum].dimsize[ VR_Int-1 ] - 4; //Max Len (-4 is prefix space for len)
	if( VR_Long > stlen) VR_Long = stlen;  // Less than max

	// copy string
	memcpy(	VR_StrPtr+4, strval, VR_Long);

	// set str var len
	VR_LongPtr = (long *) VR_StrPtr;
	*VR_LongPtr = VR_Long;		//Store 4byte String Len
	
	VR_StrPtr = (unsigned char*) (CallStack[ CallStackPtr].varbuf +  RunVar.VarTable2[varnum].offset);
	return;
}

short cmd_submenu()
{
	short a, cmd, varnum;
	long  Xoffset=0, v[3];
	

	// GIP is the Var num
	GIP=(short *) (CodeBuf+CodePos);
	varnum = *GIP; 
	CodePos+=2;

	if( RunVar.VarTable2[varnum].dims )
		Xoffset = ArrayCalc( varnum );	

	for(a=0; a<2;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (long) get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in SubMenu command");
				break;
		}
	}
	if(v[0]<0 || v[0]> 49)
	{
		ErrHandler( 1017, "\pin SubMenu command");
		return(-1);
	}
	
	GIP=(short *) (CodeBuf+CodePos);
	cmd = *GIP; 

	a = MENU_SubMenu( v[0], v[1], cmd );

	ITemp = RunVar.VarTable2[varnum].vartype;
	switch(ITemp)
	{
		case 1:
			VR_Int = a;
			StoreInt(varnum, Xoffset, VR_Int);
			break;
		case 2:
			VR_Long = (long) a;
			StoreLong(varnum, Xoffset, VR_Long);
			break;
		case 3:
			VR_Single = (float) a;
			StoreSingle(varnum, Xoffset, VR_Single);
			break;
		case 4:
			VR_Double = (short double) a;
			StoreDouble(varnum, Xoffset, VR_Double);
			break;
	}
	
	return(0);
}

short cmd_setcmdkey()
{
	short v, cmd;
	

	switch(CodeBuf[CodePos++])
	{
		case 251:
			v = (short) get_longarg_val();
			break;
		case 250:
			v = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in SetCmdKey command");
			break;
	}
	if(v<0 || v> 255)
	{
		ErrHandler( 1006, "\pin SetCmdKey command");
		return(-1);
	}
	
	GIP=(short *) (CodeBuf+CodePos);
	cmd = *GIP; 

	CmdKeys[v] = cmd;
	
	return(0);
}


short cmd_dummy()
{
	return(0);
}

short cmd_none()
{
	return(0);
}

short cmd_beep()
{
	SysBeep(30);
	return(0);
}

long ArrayCalc( short varnum )
{
	short a, b, t,dims, mpl;
	long	sz1, sz2, result=0;
	
	// This is an array variable  ; Reminder Base is always 1 
	sz1 = 0;
	dims = RunVar.VarTable2[varnum].dims;
	t = RunVar.VarTable2[varnum].vartype;
	if( t == 5 ) dims--;
	for(a=0; a< dims; a++ )
	{
		mpl =1;
		if( CodeBuf[CodePos] == 250 || CodeBuf[CodePos] == 251) CodePos++;
		sz2 = get_longarg_val();
		if( sz2<=0 || sz2 > RunVar.VarTable2[varnum].dimsize[a] )
		{
			// Out of range, !!! Generate error
			if( sz2<= 0 ) ErrHandler( 1051, "\p in assignment variable");
			else ErrHandler( 1007, "\p in assignment variable");
			return( -1 );
		}
		
		// Fix for 3.03
		for(b=a+1; b< dims; b++ ) mpl *= RunVar.VarTable2[varnum].dimsize[b];

		if (a+1 < dims ) sz1 += (sz2-1) * mpl; 
		else sz1 += (sz2-1);  // Last dimension is special
	}
	
	switch(t)
	{
		case 1:
			result = sz1 * 2;	// times 2 bytes
			break;
		case 2:
		case 3:
			result = sz1 * 4;	// times 4 bytes
			break;
		case 4:
			result = sz1 * 8;	// times 8 bytes
			break;
		case 5:
			result = sz1 * RunVar.VarTable2[varnum].dimsize[dims]; //Should already have +4 added to it
			break;
	}
	return( result );
}

short cmd_let()
{
	
	short	varnum;
	long	Voffset = 0;
	float	fv;
	short double dv;

	// GIP is the Var num
	GIP=(short *) (CodeBuf+CodePos);
	varnum = *GIP; 
	CodePos+=2;

	ITemp = RunVar.VarTable2[varnum].vartype;
	if( ITemp <=4 )
	{
		if( RunVar.VarTable2[varnum].dims )	Voffset = ArrayCalc( varnum );	
		CodePos++;
		switch(ITemp)
		{
			case 1:
				if( CodeBuf[CodePos-1] == 251) VR_Int = (short) get_longarg_val();
				else VR_Int = (short) get_doublearg_val();
				StoreInt( varnum, Voffset, VR_Int);
				break;
			case 2:
				if( CodeBuf[CodePos-1] == 251) VR_Long = get_longarg_val();
				else VR_Long = (long) get_doublearg_val();
				StoreLong( varnum, Voffset, VR_Long);
				break;
			case 3:
				dv = get_doublearg_val();
				fv = (float) dv;
				StoreSingle( varnum, Voffset, fv);
				break;
			case 4:
				dv = get_doublearg_val();
				StoreDouble( varnum, Voffset, dv);
				break;
		}
	}
	else
	{
		if( RunVar.VarTable2[varnum].dims>1 ) Voffset = ArrayCalc( varnum );	
		
		if( CodeBuf[CodePos] == 249 )  // String?
		{
				CodePos++;
				get_stringarg_val();
				StoreStr( varnum, ReturnStr, Voffset, ReturnStrLen );
				FreeReturnStr(); 
		}
	}
	return(0);
}

short pass_assign( short vartype, short varnum)
{
	ITemp = vartype;
	if( CodeBuf[CodePos] >=249 && CodeBuf[CodePos] <= 251)
	{
		CodePos++;
		PassAssignFlag=1;
		switch(ITemp)
		{
			case 1:
				VR_Int = (short) get_longarg_val();
				StoreInt( varnum, 0, VR_Int);
				break;
			case 2:
				VR_Long = get_longarg_val();
				StoreLong( varnum, 0, VR_Long);
				break;
			case 3:
				VR_Single = (float) get_doublearg_val();
				StoreSingle( varnum, 0, VR_Single);
				break;
			case 4:
				VR_Double = get_doublearg_val();
				StoreDouble( varnum, 0, VR_Double);
				break;
			case 5:
				get_stringarg_val();
				StoreStr( varnum, ReturnStr, 0,  ReturnStrLen );
				FreeReturnStr(); 
				break;
		}
		PassAssignFlag=0;
	}
	else { ErrHandler( 1028, "\p"); return(99);}
	return(0);
}

short cmd_print()
{
	short y,z, lf=1;
	short double	 	d;
	char temp[64];
	GrafPtr		oldPort;

	GetPort( &oldPort);
	while ( (CodeBuf[CodePos] >= 249 && CodeBuf[CodePos] <= 251) || CodeBuf[CodePos] == 255 )
	{
		switch( CodeBuf[CodePos] )
		{
			case 251:	// Long Arg
			case 250:	// Single/Double Arg
				CodePos++;
				d = get_doublearg_val();
				if(ConsoleFlag)
				{
					y = sprintf( temp, "%13.13f", d);
					if( y>0)
					{
						for( z=y-1; z>=0; z--)
						{
							if(temp[z]=='.') {temp[z]=0; break;}
							if(temp[z]=='0') temp[z]=0;
							else break;
						}
					}
					printf("%s",temp);
				}
				break;
			case 249:
				CodePos++;
				get_stringarg_val();
				if(ConsoleFlag) printf("%s",ReturnStr); 
				FreeReturnStr(); 
				break;
			case 255:
				CodePos++;
				lf=0;
				break;
		}
	}
	if(ConsoleFlag)
	{
		if( lf ) printf("\n");
		if( !PrintFlag)	GetDateTime( (unsigned long*) (&qd.randSeed) );

		PrintFlag = 1; 
	}
	SetPort( oldPort);
	return(0);
}

short cmd_input()
{
	
	short	varnum, buflen;
	long	Voffset = 0;
	float	fv;
	short double dv;
	char 	buf[402];

	// GIP is the Var num
	GIP=(short *) (CodeBuf+CodePos);
	varnum = *GIP; 
	CodePos+=2;

	ITemp = RunVar.VarTable2[varnum].vartype;
	
	gets( buf);
	for( buflen=0; buflen<400; buflen++) if( !buf[buflen] ) break;
	
	if( ITemp <=4 )
	{
		if( RunVar.VarTable2[varnum].dims )	Voffset = ArrayCalc( varnum );	
		CodePos++;
		switch(ITemp)
		{
			case 1:
				VR_Int = (short) atol( buf);
				StoreInt( varnum, Voffset, VR_Int);
				break;
			case 2:
				VR_Long = (long) atol( buf );
				StoreLong( varnum, Voffset, VR_Long);
				break;
			case 3:
				dv = atof( buf);
				fv = (float) atof( buf );
				StoreSingle( varnum, Voffset, fv);
				break;
			case 4:
				dv = atof( buf );
				StoreDouble( varnum, Voffset, dv);
				break;
		}
	}
	else
	{
		if( RunVar.VarTable2[varnum].dims>1 ) Voffset = ArrayCalc( varnum );	
		
		StoreStr( varnum, (unsigned char *) buf, Voffset, buflen );
		FreeReturnStr(); 
	}
	return(0);
}


short cmd_end()
{
	return(99);
}

short cmd_goto()
{
	short	wtg;
	GIP=(short *) (CodeBuf+CodePos);
	wtg = *GIP;                    //Redirect
	CodePos = GetLabelLoc(wtg);
	return(1);
} 

short cmd_gosub()
{
	short	wtg;
	GIP=(short *) (CodeBuf+CodePos);
	JmpStack[JS_Ctr].retpt = CodePos+3;
	JmpStack[JS_Ctr].LL = LineLen;
	JmpStack[JS_Ctr++].module = current_module_num;
	wtg = *GIP;                    //Redirect
	CodePos = GetLabelLoc(wtg);

	return(1);
} 

short cmd_return()
{

	if( JS_Ctr>1 )
	{
		// No call so skip right by it
		// Move past to next cmd/line
	}       

	if( JmpStack[JS_Ctr-1].module == current_module_num)
	{
		JS_Ctr--;
		CodePos = JmpStack[JS_Ctr].retpt;
		LineLen = JmpStack[JS_Ctr].LL;
		return(1);
	}
	else
	{
		// No call left, so skip right by it
		// Move past to next cmd/line
	}       
	return(0);
} 

short cmd_for()
{
	short a;
	long    v[8], jmpr, ValOffset,VL, *pl;
	short double	dv[8], *dpl, dd;
	VR_LongPtr = (long *) (CodeBuf+CodePos);
	jmpr = *VR_LongPtr;
	CodePos+=4;

	// GIP is the Var num
	GIP=(short *) (CodeBuf+CodePos);
	GIP2 = *GIP; 
	CodePos+=2;

	VL = GIP2;
	ITemp = RunVar.VarTable2[VL].vartype;
	
	//This is local memory var space reserved for this for (16 bytes- 2 Longs or 2 Short Doubles)
	VR_LongPtr = (long *) (CodeBuf+CodePos);
	pl = (long *) (CodeBuf+CodePos);
	ValOffset = *VR_LongPtr;
	CodePos+=4;



	if( !NextFlag )
	{
	    switch(ITemp)
	    {
		case 1:
		case 2:
			v[2] = 1;       // Default for STEP
			for(a=0;a<3;a++)
			{
				if( CodeBuf[CodePos] == 251) { CodePos++; v[a] = get_longarg_val(); }
				else if( CodeBuf[CodePos] == 250) { CodePos++; v[a] = (long) get_doublearg_val(); }
			}

			// Check direction & set up LoopStack
			VR_Long = v[0];
			if( ITemp == 1 ) StoreInt( GIP2, 0, (short) VR_Long);
			else StoreLong( GIP2, 0, VR_Long);
			
			if( v[2] >= 0 )
			{
				if( v[0] > v[1] ) CodePos = jmpr;
			}
			else if( v[0] < v[1] ) CodePos = jmpr;
			
			VL = CallStackPtr;
			VR_LongPtr = (long*) (CallStack[ VL ].varbuf + ValOffset );
			pl = (long*) (CallStack[ VL ].varbuf + ValOffset );
			*pl = v[1];
			VR_LongPtr++;
			pl++;
			*pl=v[2];
			break;
		case 3:
		case 4:
			dv[2] = 1;       // Default for STEP
			for(a=0;a<3;a++)
			{
				if( CodeBuf[CodePos] == 251) { CodePos++; dv[a] = (short double) get_longarg_val(); }
				else if( CodeBuf[CodePos] == 250) { CodePos++; dv[a] = get_doublearg_val(); }
			}
			// Check direction & set up LoopStack
			VR_Double = dv[0];
			if( ITemp == 4 ) StoreDouble( GIP2, 0, VR_Double);
			else  StoreSingle( GIP2, 0, (float) VR_Double);
			
			if( dv[2] >= 0 )
			{
				if( dv[0] > dv[1] ) CodePos = jmpr;
			}
			else if( dv[0] < dv[1] ) CodePos = jmpr;
			
			VL = CallStackPtr;
			VR_DoublePtr = (short double*) (CallStack[ VL ].varbuf + ValOffset );
			dpl = (short double*) (CallStack[ VL ].varbuf + ValOffset );
			*dpl = dv[1];
			VR_DoublePtr++;
			dpl++;
			*dpl=dv[2];
			break;
	    }

	}
	else
	{
		if( ITemp <= 2 )
		{
			NextFlag = 0;
			VR_LongPtr = (long*) (CallStack[ CallStackPtr].varbuf +  ValOffset + 4);
			VR_Long2 = *VR_LongPtr;

			if( ITemp == 1 )
			{
				VR_Long = grab_intvar( GIP2 ) + VR_Long2;
				StoreInt( GIP2, 0, (short) VR_Long);
			}
			else
			{
				VR_Long = grab_longvar( GIP2 ) + VR_Long2;
				StoreLong( GIP2, 0, VR_Long);
			}

			if( VR_Long2 >= 0)
			{
	   			VR_LongPtr = (long*) (CallStack[ CallStackPtr].varbuf +  ValOffset);
				if( VR_Long > *VR_LongPtr) { CodePos = jmpr;  return(1); }
			}
			else
			{
	   			VR_LongPtr = (long*) (CallStack[ CallStackPtr].varbuf +  ValOffset);
				if( VR_Long < *VR_LongPtr) { CodePos = jmpr; return(1); }
			}
		}
		else
		{
			NextFlag = 0;
			VR_DoublePtr = (short double*) (CallStack[ CallStackPtr].varbuf +  ValOffset );
			VR_DoublePtr++;
			dd = *VR_DoublePtr;
			if(ITemp == 3 )
			{
				VR_Double = grab_singlevar( GIP2 ) + dd;
				StoreSingle( GIP2, 0, (float) VR_Double );
			}
			else
			{
				VR_Double = grab_doublevar( GIP2 ) + dd;
				StoreDouble( GIP2, 0, VR_Double );
			}
			if( dd >= 0)
			{
	   			VR_DoublePtr = (short double*) (CallStack[ CallStackPtr].varbuf +  ValOffset);
				if( VR_Double > *VR_DoublePtr) { CodePos = jmpr;  return(1); }
			}
			else
			{
	   			VR_DoublePtr = (short double*) (CallStack[ CallStackPtr].varbuf +  ValOffset);
				if( VR_Double < *VR_DoublePtr) { CodePos = jmpr; return(1); }
			}
		}
	}
	return(0);
} 

short cmd_next()
{

	// GIP is the Var num
	CodePos+=2;

	VR_LongPtr = (long *) (CodeBuf+CodePos);
	NextFlag=1;
	CodePos = *VR_LongPtr;
	return(1);
} 


short cmd_do()
{
	long    jmpr, g;
	UCHAR	tp;
	
	
	VR_LongPtr = (long *) (CodeBuf+CodePos);
	jmpr = *VR_LongPtr;
	CodePos+=4;

	tp = CodeBuf[CodePos++];
	
	if(CodeBuf[CodePos++] != 251)
	{
		CodePos = jmpr;
		return(1);
	}
	
	if( tp ==3 )
	{
		//Do While Statement
		g = get_longarg_val();
		if( !g || ExitLoopFlag==1)
		{
			ExitLoopFlag = 0;
			CodePos = jmpr;
			return(1);
		}
	}
	else
	{
		//Do Until Statement
		g = get_longarg_val();
		if( g  || ExitLoopFlag==1)
		{
			ExitLoopFlag = 0;
			CodePos = jmpr;
			return(1);
		}
	}
	ExitLoopFlag = 0;
	return(0);
} 

short cmd_exitloop()
{

	ExitLoopFlag=1;
	VR_LongPtr = (long *) (CodeBuf+CodePos);
	CodePos = *VR_LongPtr;
	return(1);
} 

short cmd_loop()
{

	VR_LongPtr = (long *) (CodeBuf+CodePos);
	CodePos = *VR_LongPtr;
	return(1);
} 

short cmd_if()
{
	long    jmpr, g;
	short double	d;
	
	VR_LongPtr = (long *) (CodeBuf+CodePos);
	jmpr = *VR_LongPtr;
	CodePos+=4;

	switch( CodeBuf[CodePos++])
	{
		case  250:
			d = get_doublearg_val();
			if( !d )
			{
				// Not true so jump to past else or endif
				CodePos = jmpr;
				return(1);
			}
			break;
		case 251:
			g = get_longarg_val();
			if( !g )
			{
				// Not true so jump to past else or endif
				CodePos = jmpr;
				return(1);
			}
			break;
	}
	return(0);
} 

short cmd_else()
{

	// if you hit an ELSE jump past endif
	VR_LongPtr = (long *) (CodeBuf+CodePos);
	CodePos = *VR_LongPtr;
	return(1);
} 

short cmd_endif()
{
	return(0);
} 

short cmd_exitsub()
{

			if( CallStackPtr ==-1 ) 
			{
				ErrHandler( 1029, "\pin ExitSub command");
				return(99);
			}
			
			CodePos = CallStack[ CallStackPtr ].location;    
			if( CallStack[ CallStackPtr ].varbuf != nil )
			{
				DisposePtr( (Ptr) CallStack[ CallStackPtr ].varbuf);
				//Releasing Sub Memory
			}
			CallStackPtr--;
			//Exiting Sub
			return(1);
}

short cmd_exitfunc()
{
	long	larg;
	short double	darg;

			if( CallStackPtr ==-1 ) 
			{
				ErrHandler( 1029, "\p in ExitFunction command");
				return(99);
			}
		
			switch(CodeBuf[CodePos++])
			{
				case 251:
					larg = get_longarg_val();
					Long_Function_Result = larg;
					Double_Function_Result = (short double) larg;
					break;
				case 250:
					darg = get_doublearg_val();
					Long_Function_Result = (long) darg;
					Double_Function_Result = darg;
					break;
				case 249:
					get_stringarg_val();
					break;
				default:
					ErrHandler( 1042, "\p in ExitFunction command");
					break;
			}

			CodePos = CallStack[ CallStackPtr ].location;        
			if( CallStack[ CallStackPtr ].varbuf != nil )
			{
				DisposePtr( (Ptr) CallStack[ CallStackPtr ].varbuf);
				//Releasing Function Memory
			}
			CallStackPtr--;
			return(99);
}

short cmd_window()
{
	short a, cmd, varnum;
	long    v[6], Voffset=0;
	Str255	wt;
	RGBColor	rgb;

	// GIP is the Var num
	GIP=(short *) (CodeBuf+CodePos);
	varnum = *GIP; 
	CodePos+=2;
	
	if( RunVar.VarTable2[varnum].dims )	Voffset = ArrayCalc( varnum );	

	for(a=0;a<5;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in Window command");
				break;
		}
	}
	// Now get title string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}
	GIP=(short *) (CodeBuf+CodePos);
	cmd = *GIP; 
	
	RStrToStr255( ReturnStr, ReturnStrLen, wt);
	FreeReturnStr();

	rgb.red = 0xffff;	rgb.green = 0xffff;		rgb.blue = 0xffff;  ///white
	GlobalBackRGB = rgb;
	rgb.red = 0;	rgb.green = 0;		rgb.blue = 0;  ///black
	GlobalForeRGB = rgb;
	
	a = GFX_CreateWin( v[0], v[1], v[2], v[3], v[4], wt, cmd );
	FreeReturnStr(); 
 
	if(a<0) ErrHandler( 1008, "\p");

	ITemp = RunVar.VarTable2[ varnum].vartype;
	
	switch(ITemp)
	{
		case 1:
			VR_Int = a;
			StoreInt( varnum, Voffset, VR_Int);
			break;
		case 2:
			VR_Long = (long) a;
			StoreLong( varnum, Voffset, VR_Long);
			break;
		case 3:
			VR_Single = (float) a;
			StoreSingle( varnum, Voffset, VR_Single);
			break;
		case 4:
			VR_Double = (short double) a;
			StoreDouble( varnum, Voffset, VR_Double);
			break;
	}
	return(0);
}

short cmd_closewindow()
{
	short a, b=0, c,d,*e;
	short    v;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindFormID( d );
		if( v == -1 ) {ErrHandler( 1009, "\p in WinClose command"); return 0 ; } 
	}
	else
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v = (short) get_longarg_val();
				break;
			case 250:
				v = (short) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in WinClose command");
				break;
		}
	}
	a = GFX_CloseWindow(  v ); 
	return(0);
}

short cmd_alertbox()
{
	short    v;
	switch(CodeBuf[CodePos++])
	{
		case 251:
			v = (short) get_longarg_val();
			break;
		case 250:
			v = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in AlertBox command");
			break;
	}
	Alert(  v, nil ); 
	return(0);
}


short cmd_redrawwindow()
{
	short a,b=0,c,d,*e;
	short    v;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindFormID( d );
		if( v == -1 ) {ErrHandler( 1009, "\p in WinRedraw command"); return 0 ; } 
	}
	else
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v = (short) get_longarg_val();
				break;
			case 250:
				v = (short) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in WinRedraw command");
				break;
		}
	}
	a = GFX_WinRedraw(  v ); 
	if(a<0) { ErrHandler( 1009, "\pin WinRedraw command"); return(99); }

	return(0);
}


short cmd_hidewindow()
{
	short a,b=0,c,d,*e;
	short    v;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindFormID( d );
		if( v == -1 ) {ErrHandler( 1009, "\p in WinHide command"); return 0 ; } 
	}
	else
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v = (short) get_longarg_val();
				break;
			case 250:
				v = (short) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in WinHide command");
				break;
		}
	}
	a = GFX_HideWindow(  v ); 
	if(a<0) { ErrHandler( 1009, "\pin WinHide command"); return(99); }
	return(0);
}

short cmd_cleargraphics()
{
	short a, b=0, c,d,*e;
	short    v;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindFormID( d );
		if( v == -1 ) {ErrHandler( 1009, "\p in ClearGraphics command"); return 0 ; } 
	}
	else
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v = (short) get_longarg_val();
				break;
			case 250:
				v = (short) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in ClearGraphics command");
				break;
		}
	}
	GFX_ClearGfx( v, 0 );
	return(0);
}

short cmd_clearcontrols()
{
	short a, b=0, c,d,*e;
	short    v;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindFormID( d );
		if( v == -1 ) {ErrHandler( 1009, "\p in ClearControls command"); return 0 ; } 
	}
	else
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v = (short) get_longarg_val();
				break;
			case 250:
				v = (short) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in ClearControls command");
				break;
		}
	}
	CTL_ClearControls( v );
	return(0);
}

short cmd_clearwindow()
{
	short a, b=0, c,d,*e;
	short    v;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindFormID( d );
		if( v == -1 ) {ErrHandler( 1009, "\p in ClearWindow command"); return 0 ; } 
	}
	else
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v = (short) get_longarg_val();
				break;
			case 250:
				v = (short) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in ClearWindow command");
				break;
		}
	}
	GFX_ClearGfx( v, 1 );
	CTL_ClearControls( v );
	return(0);
}

short cmd_setdepth()
{
	short    v;

	switch(CodeBuf[CodePos++])
	{
		case 251:
			v = (short) get_longarg_val();
			break;
		case 250:
			v = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in SetDepth command");
			break;
	}

	SetDepth(GetMainDevice(), v, 1, 1);

	return(0);
}

short cmd_conmsg()
{
	short    v;

	switch(CodeBuf[CodePos++])
	{
		case 251:
			ConFlag = (short) get_longarg_val();
			break;
		case 250:
			ConFlag = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in ConMsg command");
			break;
	}

	return(0);
}


short cmd_drawupdate()
{
	short a, b=0, c,d,*e;
	short    v[3];

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindFormID( d );
		if( v[0] == -1 ) {ErrHandler( 1009, "\p in DrawUpdate command"); return 0 ; } 
	}

	for(a=b;a<2;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (short) get_longarg_val();
				break;
			case 250:
				v[a] = (short) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in DrawUpdate command");
				break;
		}
	}
	GFX_DrawUpdate( v[0], v[1] );
	return(0);
}

short cmd_showwindow()
{
	short a, b=0, c,d,*e;
	short    v;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindFormID( d );
		if( v == -1 ) {ErrHandler( 1009, "\p in WinShow command"); return 0 ; } 
	}
	else
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v = (short) get_longarg_val();
				break;
			case 250:
				v = (short) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in WinShow command");
				break;
		}
	}
	a = GFX_ShowWindow(  v ); 
	if(a<0) { ErrHandler( 1009, "\pin WinShow command"); return(99); }
	return(0);
}

short cmd_movewindow()
{
	short a, b=0, c,d,*e;
	short    v[4];

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindFormID( d );
		if( v[0] == -1 ) {ErrHandler( 1009, "\p in WinMove command"); return 0 ; } 
	}

	for(a=b;a<3;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in WinMove command");
				break;
		}
	}
	a = GFX_MoveWindow( v[0],  v[1],  v[2] ); 
	if(a<0) { ErrHandler( 1009, "\pin WinMove command"); return(99); }
	return(0);
}

short cmd_winorigin()
{
	short a, b=0, c,d,*e;
	short    v[4];

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindFormID( d );
		if( v[0] == -1 ) {ErrHandler( 1009, "\p in WinOrigin command"); return 0 ; } 
	}
	
	for(a=b;a<3;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in WinOrigin command");
				break;
		}
	}
	a = GFX_WindowOrigin(v[0],  v[1],  v[2]);
	if(a<0) { ErrHandler( 1009, "\pin WinOrigin command"); return(99); }
	return(0);
}




short cmd_sizewindow()
{
	short a, b=0, c,d,*e;
	short    v[4];

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindFormID( d );
		if( v[0] == -1 ) {ErrHandler( 1009, "\p in WinSize command"); return 0 ; } 
	}
	
	for(a=b;a<3;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in WinSize command");
				break;
		}
	}
	a = GFX_SizeWindow(  v[0], v[1], v[2] ); 
	if(a<0) { ErrHandler( 1009, "\pin WinSize command"); return(99); }
	return(0);
}

short cmd_clipwindow()
{
	short a, b=0, c,d,*e;
	short    v[6];

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindFormID( d );
		if( v[0] == -1 ) {ErrHandler( 1009, "\p in WinClip command"); return 0 ; } 
	}

	for(a=b;a<5;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in WinClip command");
				break;
		}
	}
	a = GFX_ClipWindow(  v[0], v[1], v[2], v[3], v[4] ); 
	if(a<0) { ErrHandler( 1009, "\pin WinClip command"); return(99); }
	return(0);
}

short cmd_loadpal()
{
	short a;
	short    v[4];
	
	for(a=0;a<4;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in LoadPalette command");
				break;
		}
	}
	a = GFX_LoadPal(  v[0], v[1], v[2], v[3] ); 
	if(a<0) { ErrHandler( 1009, "\pin LoadPalette command"); return(99); }
	return(0);
}

short cmd_winpal()
{
	short a, b=0, c,d,*e;
	short    v[4];

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindFormID( d );
		if( v[0] == -1 ) {ErrHandler( 1009, "\p in WinPalette command"); return 0 ; } 
	}

	for(a=b;a<2;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in WinPalette command");
				break;
		}
	}
	a = GFX_WinPal(  v[0], v[1] ); 
	if(a<0) { ErrHandler( 1009, "\pin WinPalette command"); return(99); }
	return(0);
}


short cmd_windowtitle()
{
	short a, b=0, c,d,*e;
	long    v;
	Str255	wt;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindFormID( d );
		if( v == -1 ) {ErrHandler( 1009, "\p in WinTitle command"); return 0 ; } 
	}
	else
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v = get_longarg_val();
				break;
			case 250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in WinTitle command");
				break;
		}
	}
	// Now get title string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}
	else ErrHandler( 1042, "\p in WinTitle command");

	RStrToStr255( ReturnStr, ReturnStrLen, wt);
	FreeReturnStr();
	
	a = GFX_SetWindowTitle( (short) v, wt);
	if(a<0) { ErrHandler( 1009, "\pin WinTitle command"); return(99); }
	return(0);
}



short cmd_droplist()
{
	short b=0, c,d,*e;
	short a, cmd, varnum;
	long    v[5], Voffset=0;

	// GIP is the Var num
	GIP=(short *) (CodeBuf+CodePos);
	varnum = *GIP; 
	CodePos+=2;
	
	if( RunVar.VarTable2[varnum].dims )	Voffset = ArrayCalc( varnum );	

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindFormID( d );
		if( v[0] == -1 ) {ErrHandler( 1009, "\p in DropList command"); return 0 ; } 
	}

	for(a=b;a<4;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in DropList command");
				break;
		}
	}

	GIP=(short *) (CodeBuf+CodePos);
	cmd = *GIP; 

	a = CTL_DropList( v[0], v[1], v[2], v[3], cmd  );

	ITemp = RunVar.VarTable2[varnum].vartype;
	
	switch(ITemp)
	{
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
	return(0);

}


short cmd_button()
{
	short a, cmd, varnum;
	long    v[5], Voffset=0;
	short b=0, c,d,*e;

	// GIP is the Var num
	GIP=(short *) (CodeBuf+CodePos);
	varnum = *GIP; 
	CodePos+=2;
	
	if( RunVar.VarTable2[varnum].dims )	Voffset = ArrayCalc( varnum );	

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindFormID( d );
		if( v[0] == -1 ) {ErrHandler( 1009, "\p in Button command"); return 0 ; } 
	}
	
	for(a=b;a<5;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in Button command");
				break;
		}
	}
	// Now get text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}

	GIP=(short *) (CodeBuf+CodePos);
	cmd = *GIP; 

	a = CTL_Button( v[0], v[1], v[2], v[3], v[4],(char*) ReturnStr, cmd  );
	FreeReturnStr(); 

	ITemp = RunVar.VarTable2[varnum].vartype;
	
	switch(ITemp)
	{
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
	return(0);

}

short cmd_checkbox()
{
	short a, cmd, varnum;
	long    v[5], Voffset=0;
	short b=0, c,d,*e;

	// GIP is the Var num
	GIP=(short *) (CodeBuf+CodePos);
	varnum = *GIP; 
	CodePos+=2;

	if( RunVar.VarTable2[varnum].dims )	Voffset = ArrayCalc( varnum );	

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindFormID( d );
		if( v[0] == -1 ) {ErrHandler( 1009, "\p in CheckBox command"); return 0 ; } 
	}

	for(a=b;a<5;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in CheckBox command");
				break;
		}
	}
	// Now get text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}
	GIP=(short *) (CodeBuf+CodePos);
	cmd = *GIP; 

	a = CTL_CheckBox( v[0], v[1], v[2], v[3], v[4], (char*) ReturnStr, cmd   );
	FreeReturnStr(); 

	ITemp = RunVar.VarTable2[varnum].vartype;
	
	switch(ITemp)
	{
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
	return(0);

}

short cmd_radio()
{
	short a, cmd, varnum;
	long    v[7], Voffset=0;
	short b=0, c,d,*e;

	// GIP is the Var num
	GIP=(short *) (CodeBuf+CodePos);
	varnum = *GIP; 
	CodePos+=2;

	if( RunVar.VarTable2[varnum].dims )	Voffset = ArrayCalc( varnum );	

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindFormID( d );
		if( v[0] == -1 ) {ErrHandler( 1009, "\p in Radio command"); return 0 ; } 
	}

	for(a=b;a<6;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in Radio command");
				break;
		}
	}
		// Now get attrib string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}

	GIP=(short *) (CodeBuf+CodePos);
	cmd = *GIP; 

	a = CTL_Radio( v[0], v[1], v[2], v[3], v[4], v[5], (char*) ReturnStr, cmd    );
	FreeReturnStr(); 

	ITemp = RunVar.VarTable2[varnum].vartype;
	
	switch(ITemp)
	{
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
	return(0);

}

short cmd_timer()
{
	short a, cmd, varnum;
	long    v[5], Voffset=0;
	short b=0, c,d,*e;

	// GIP is the Var num
	GIP=(short *) (CodeBuf+CodePos);
	varnum = *GIP; 
	CodePos+=2;

	if( RunVar.VarTable2[varnum].dims )	Voffset = ArrayCalc( varnum );	

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindFormID( d );
		if( v[0] == -1 ) {ErrHandler( 1009, "\p in Timer command"); return 0 ; } 
	}

	for(a=b;a<3;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in Timer command");
				break;
		}
	}
	GIP=(short *) (CodeBuf+CodePos);
	cmd = *GIP; 

	a = CTL_Timer( (short) v[0], v[1], (short) v[2], cmd );

	ITemp = RunVar.VarTable2[varnum].vartype;
	
	switch(ITemp)
	{
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
	return(0);

}


short cmd_scrollbar()
{
	short a, cmd, varnum;
	long    v[5], Voffset=0;
	short b=0, c,d,*e;

	// GIP is the Var num
	GIP=(short *) (CodeBuf+CodePos);
	varnum = *GIP; 
	CodePos+=2;

	if( RunVar.VarTable2[varnum].dims )	Voffset = ArrayCalc( varnum );	

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindFormID( d );
		if( v[0] == -1 ) {ErrHandler( 1009, "\p in ScrollBar command"); return 0 ; } 
	}

	for(a=b;a<5;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in ScrollBar command");
				break;
		}
	}
	GIP=(short *) (CodeBuf+CodePos);
	cmd = *GIP; 

	a = CTL_ScrollBar( v[0], v[1], v[2], v[3], v[4], cmd ); 

	ITemp = RunVar.VarTable2[varnum].vartype;
	
	switch(ITemp)
	{
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
	return(0);

}

short cmd_scrollset()
{
	short a, b=0, c,d,*e;
	long    v[5];

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1;
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindCtlID( d );
		if( v[0] == -1 ) {ErrHandler( 1011, "\p in ScrollSet command"); return 0 ; } 
	}

	for(a=b;a<4;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in ScrollSet command");
				break;
		}
	}

	a = CTL_ScrollSet( v[0], v[1], v[2], v[3]); 

	return(0);

}


short cmd_textbox()
{
	short a, cmd, varnum;
	long    v[7], Voffset=0;
	short b=0, c,d,*e;
	
	// GIP is the Var num
	GIP=(short *) (CodeBuf+CodePos);
	varnum = *GIP; 
	CodePos+=2;

	if( RunVar.VarTable2[varnum].dims )	Voffset = ArrayCalc( varnum );	

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindFormID( d );
		if( v[0] == -1 ) {ErrHandler( 1009, "\p in TextBox command"); return 0 ; } 
	}
	
	for(a=b;a<6;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in TextBox command");
				break;
		}
	}
	// Now get attrib string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}

	GIP=(short *) (CodeBuf+CodePos);
	cmd = *GIP; 

	a = CTL_Textbox( v[0], v[1], v[2], v[3], v[4], v[5], 0 ,(char*) ReturnStr, cmd  ); 
	FreeReturnStr(); 

	ITemp = RunVar.VarTable2[varnum].vartype;
	
	switch(ITemp)
	{
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
	return(0);

}

short cmd_list()
{
	short a, cmd, varnum;
	long    v[6], Voffset = 0;
	short b=0, c,d,*e;
	
	// GIP is the Var num
	GIP=(short *) (CodeBuf+CodePos);
	varnum = *GIP; 
	CodePos+=2;

	if( RunVar.VarTable2[varnum].dims )	Voffset = ArrayCalc( varnum );	

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindFormID( d );
		if( v[0] == -1 ) {ErrHandler( 1011, "\p in ListBox command"); return 0 ; } 
	}

	for(a=b;a<6;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in ListBox command");
				break;
		}
	}

	GIP=(short *) (CodeBuf+CodePos);
	cmd = *GIP; 

	a = CTL_List( v[0], v[1], v[2], v[3], v[4], v[5], cmd  ); 

	ITemp = RunVar.VarTable2[varnum].vartype;
	
	switch(ITemp)
	{
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
	return(0);

}

short cmd_listadd()
{
	short a, b=0, c,d,*e;
	short    v;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1;
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindCtlID( d );
		if( v == -1 ) {ErrHandler( 1011, "\p in ListAdd command"); return 0 ; } 
	}
	else
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v = get_longarg_val();
				break;
			case 250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in ListAdd command");
				break;
		}
	}	
	// Now get title string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}

	a = CTL_ListAdd( v,  ReturnStr, ReturnStrLen);
	FreeReturnStr();
	return(0);
}

short cmd_listclear()
{
	short a,c,d,*e;
	long    v;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindCtlID( d );
		if( v == -1 ) {ErrHandler( 1011, "\p in ListClear command"); return 0 ; } 
	}
	else
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v = get_longarg_val();
				break;
			case 250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in ListClear command");
				break;
		}
	}	
	CTL_ListClear( v );
	return(0);
}

short cmd_listremove()
{
	short a, b=0, c,d,*e;
	short    v[3];
	

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1;
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindCtlID( d );
		if( v[0] == -1 ) {ErrHandler( 1011, "\p in ListRemove command"); return 0 ; } 
	}

	for(a=b; a<2;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in ListRemove command");
				break;
		}
	}
	CTL_ListRemove( v[0], v[1] );
	return(0);
}

short cmd_textboxro()
{
	short a, b=0, c,d,*e;
	short    v[3];
	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1;
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindCtlID( d );
		if( v[0] == -1 ) {ErrHandler( 1011, "\p in TextBoxRO command"); return 0 ; } 
	}

	for(a=b; a<2;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in TextBoxRO command");
				break;
		}
	}
	CTL_TextboxRO( v[0], v[1] );
	return(0);
}

short cmd_label()
{
	short a, cmd, varnum;
	long    v[5], Voffset=0;
	short b=0, c,d,*e;

	// GIP is the Var num
	GIP=(short *) (CodeBuf+CodePos);
	varnum = *GIP; 
	CodePos+=2;

	if( RunVar.VarTable2[varnum].dims )	Voffset = ArrayCalc( varnum );	

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindFormID( d );
		if( v[0] == -1 ) {ErrHandler( 1011, "\p in Label command"); return 0 ; } 
	}

	for(a=b;a<5;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in Label command");
				break;
		}
	}
	// Now get text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}

	GIP=(short *) (CodeBuf+CodePos);
	cmd = *GIP; 

	a = CTL_Label( v[0], v[1], v[2], v[3], v[4],(char*) ReturnStr, cmd ); 
	FreeReturnStr(); 

	ITemp = RunVar.VarTable2[varnum].vartype;
	
	switch(ITemp)
	{
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

	return(0);

}

short cmd_pict()
{
	short a, cmd, varnum;
	long    v[7], Voffset=0;
	short b=0, c,d,*e;

	// GIP is the Var num
	GIP=(short *) (CodeBuf+CodePos);
	varnum = *GIP; 
	CodePos+=2;
	
	if( RunVar.VarTable2[varnum].dims )	Voffset = ArrayCalc( varnum );	

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindFormID( d );
		if( v[0] == -1 ) {ErrHandler( 1011, "\p in Picture command"); return 0 ; } 
	}

	for(a=b;a<6;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in Picture command");
				break;
		}
	}
	
	GIP=(short *) (CodeBuf+CodePos);
	cmd = *GIP; 

	a = CTL_Picture( v[0],  v[1], v[2], v[3], v[4], v[5],  cmd  );

	ITemp = RunVar.VarTable2[varnum].vartype;
	
	switch(ITemp)
	{
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
	return(0);

}

short cmd_licon()
{
	short a, cmd, varnum;
	long    v[7], Voffset=0;
	short b=0, c,d,*e;

	// GIP is the Var num
	GIP=(short *) (CodeBuf+CodePos);
	varnum = *GIP; 
	CodePos+=2;
	
	if( RunVar.VarTable2[varnum].dims )	Voffset = ArrayCalc( varnum );	

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindFormID( d );
		if( v[0] == -1 ) {ErrHandler( 1011, "\p in Icon command"); return 0 ; } 
	}

	for(a=b;a<6;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in Icon command");
				break;
		}
	}
	
	GIP=(short *) (CodeBuf+CodePos);
	cmd = *GIP; 

	a = CTL_LgIcon( v[0], v[1], v[2], v[3], v[4], v[5], cmd  );

	ITemp = RunVar.VarTable2[varnum].vartype;
	
	switch(ITemp)
	{
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
	return(0);

}



short cmd_addmenu()
{
	short a, cmd;
	long    v;

	switch(CodeBuf[CodePos++])
	{
		case 251:
			v = get_longarg_val();
			break;
		case 250:
			v = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in MenuAdd command");
			break;
	}
	if(v<0 || v > 50)
	{
		ErrHandler( 1004, "\p in MenuAdd command");
		return(-1);
	}
	
	// Now get attrib string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}
	else ErrHandler( 1042, "\p in MenuAdd command");

	GIP=(short *) (CodeBuf+CodePos);
	cmd = *GIP; 

	a = MENU_Add( v, (char*) ReturnStr, cmd ); 
	FreeReturnStr(); 

	return(0);

}

short cmd_addapple()
{
	MENU_AddApple( );
	return(0);
}

short cmd_addmenuitem()
{
	short a, cmd, cmdchar=0;
	long    v, rsl;
	UCHAR	*rs;

	switch(CodeBuf[CodePos++])
	{
		case 251:
			v = get_longarg_val();
			break;
		case 250:
			v = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in MenuAddItem command");
			break;
	}
	if(v<0 || v> 50)
	{
		ErrHandler( 1004, "\pin AddMenuItem command");
		return(99);
	}
	
	// Now get text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); 
	}
	
	rs  = ReturnStr;
	rsl = ReturnStrLen;
	ReturnStr = nil;
	ReturnStrLen = 0;
	
	// Now get cmd letter string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); 
	}
	if( ReturnStrLen && ReturnStr[0] != ' ' )  cmdchar = ReturnStr[0];
	
	FreeReturnStr(); 
	ReturnStr = rs;
	ReturnStrLen = rsl;
	
	GIP=(short *) (CodeBuf+CodePos);
	cmd = *GIP; 

	a = MENU_AddItem( v, (char*) ReturnStr, cmd, cmdchar ); 
	FreeReturnStr(); 

	return(0);

}

short cmd_adddesk()
{
	MENU_AddDesk( );
	return(0);
}

short cmd_select()
{
	short casenum, stype=0;
	long startpoint, def=-1, lm, ctr1, ctr2, ctr3;

	GIP=(short *) (CodeBuf+CodePos);
	casenum = *GIP; 
	CodePos+=2;

	VR_LongPtr=(long *) (CodeBuf+CodePos);
	startpoint = *VR_LongPtr; 
	CodePos+=4;
	if( CodeBuf[CodePos] >= 249 )
	{
		CodePos++;
		if( RunVar.SelectTable[startpoint].status == 3 )
		{
			stype=3;
			get_stringarg_val();
		}
		else
		{
			stype=1;
			VR_Long = get_longarg_val();
		}
	}
	else ErrHandler( 1042, "\p in Select command");
	
	for(ctr1=startpoint+1; ctr1<= RunHdr.NumOfSelects-1; ctr1++)	//Goes from
	{
		if( casenum == RunVar.SelectTable[ctr1].casenum)
		{
			switch( RunVar.SelectTable[ctr1].status )
			{
				case 12:		// Long Constant
					if( RunVar.SelectTable[ctr1].value == VR_Long )
					{
						CodePos = RunVar.SelectTable[ctr1].location;
						return(1);
					}
					break;
				case 13:        //Long Var
					if( grab_longvar( (short) RunVar.SelectTable[ctr1].value) == VR_Long )
					{
						CodePos = RunVar.SelectTable[ctr1].location;
						return(1);
					}
					break;
				case 14:        //Int Var
					if( grab_intvar( (short) RunVar.SelectTable[ctr1].value) == VR_Long )
					{
						CodePos = RunVar.SelectTable[ctr1].location;
						return(1);
					}
					break;
				case 15:        //String Const
					lm = RunVar.SelectTable[ctr1].value;
					VR_Long = (long) RunVar.StrTable[lm];

					if( VR_Long == ReturnStrLen)  // Check if len is the same
					{
						VR_Long = 0;
						ctr2=lm+1;						
						for(ctr3=0; ctr3<ReturnStrLen; ctr3++)
						{	
							if( RunVar.StrTable[ctr2] != ReturnStr[ctr3] ) 
							{
								VR_Long++;
								break;
							} 
							ctr2++;
						} 
						if( !VR_Long )
						{
							CodePos = RunVar.SelectTable[ctr1].location;
							FreeReturnStr();
							return(1);
						}
					}
					else
					{
					 	//Case len doesn't match
					}
					break;
				case 16:        //String Var
					lm = RunVar.SelectTable[ctr1].value;
					if( RunVar.VarTable2[lm].modnum == -1)  // Global ?
					    VR_StrPtr = (UCHAR*) (RunVar.GlobalVarSpace +  RunVar.VarTable2[lm].offset);
					else
					    VR_StrPtr = (UCHAR*) (CallStack[ CallStackPtr].varbuf +  RunVar.VarTable2[lm].offset);
					VR_LongPtr = (long*) VR_StrPtr;
					VR_Long = *VR_LongPtr;
					VR_StrPtr+=4;
					if( VR_Long == ReturnStrLen)  // Check if len is the same
					{
						VR_Long = 0;
						for(ctr2=0; ctr2<ReturnStrLen; ctr2++)
						{
							if( VR_StrPtr[ctr2] != ReturnStr[ctr2] ) 
							{
								VR_Long++;
							} 
						} 
						if( !VR_Long )
						{
							CodePos = RunVar.SelectTable[ctr1].location;
							FreeReturnStr();
							return(1);
						}
					}
					break;
			}

			// !!!Need to check strings and vars for both str and #'s

			if( RunVar.SelectTable[ctr1].status == 50 )		// Hit default
			{
				def = RunVar.SelectTable[ctr1].location;
			}

			if( RunVar.SelectTable[ctr1].status == 99 )		// Hit Endcase with no match
			{
				if( def == -1 ) CodePos = RunVar.SelectTable[ctr1].location;
				else  CodePos = def; 
				if( stype ==3) FreeReturnStr(); 
				return(1);
			}
		}
	}
	
	if( stype ==3) FreeReturnStr(); 
	return(0); 
}

short cmd_case()
{
	short casenum;
	long ctr1;
	
	GIP=(short *) (CodeBuf+CodePos);
	casenum = *GIP;
	for(ctr1=RunHdr.NumOfSelects-1; ctr1>=0; ctr1--)	//Goes from
	{
		if( casenum == RunVar.SelectTable[ctr1].casenum)
		{
			if( RunVar.SelectTable[ctr1].status == 99 )
			{
				CodePos = RunVar.SelectTable[ctr1].location;
				return(1);
			}
		}
	}
	return(0);  
}

short cmd_endcase()	// Just skip past an endcase
{
	return(0);
}

short cmd_default()	// Need to find the endcase
{
	short casenum;
	long ctr1;

	GIP=(short *) (CodeBuf+CodePos);
	casenum = *GIP; 
	for(ctr1=RunHdr.NumOfSelects-1; ctr1>=0; ctr1--)	//Goes from
	{
		if( casenum == RunVar.SelectTable[ctr1].casenum)
		{
			if( RunVar.SelectTable[ctr1].status == 99 )
			{
				CodePos = RunVar.SelectTable[ctr1].location;
				return(1);
			}
		}
	}
	return(0);  // It it is down here then there is a problem
}

short cmd_say()
{
	short a;
	Str255	speechtext;
	
	if( !MSB_SpeechAvail ) return(0);

	// Now get snd string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}
	else ErrHandler( 1042, "\p in Say command");

	RStrToStr255( ReturnStr, ReturnStrLen, speechtext);
	FreeReturnStr();
	
	a = SND_Say( speechtext);
	return(0);
}

short cmd_ctlremove()
{
	short a,c,d,*e;
	long    v;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindCtlID( d );
		if( v == -1 ) {ErrHandler( 1011, "\p in CtlRemove command"); return 0 ; } 
	}
	else
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v = get_longarg_val();
				break;
			case 250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in CtlRemove command");
				break;
		}
	}	
	a = CTL_Remove( v ); 
	return(0);
}

short cmd_hilite()
{
	short a,c,d,*e;
	long    v;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindCtlID( d );
		if( v == -1 ) {ErrHandler( 1011, "\p in CtlHilite command"); return 0 ; } 
	}
	else
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v = get_longarg_val();
				break;
			case 250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in CtlHilite command");
				break;
		}
	}	
	a = CTL_Hilite( v ); 
	return(0);
}

short cmd_ctlhide()
{
	short a,c,d,*e;
	long    v;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindCtlID( d );
		if( v == -1 ) {ErrHandler( 1011, "\p in CtlHide command"); return 0 ; } 
	}
	else
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v = get_longarg_val();
				break;
			case 250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in CtlHide command");
				break;
		}
	}	
	a = CTL_Hide( v ); 
	return(0);
}

short cmd_ctlshow()
{
	short a,c,d,*e;
	long    v;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindCtlID( d );
		if( v == -1 ) {ErrHandler( 1011, "\p in CtlShow command"); return 0 ; } 
	}
	else
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v = get_longarg_val();
				break;
			case 250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in CtlShow command");
				break;
		}
	}	
	a = CTL_Show( v ); 
	return(0);
}

short cmd_ctldisable()
{
	short a,c,d,*e;
	long    v;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindCtlID( d );
		if( v == -1 ) {ErrHandler( 1011, "\p in CtlDisable command"); return 0 ; } 
	}
	else
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v = get_longarg_val();
				break;
			case 250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in CtlDisable command");
				break;
		}
	}
	
	a = CTL_Disable( v ); 
	return(0);
}

short cmd_ctlenable()
{
	short a,c,d,*e;
	long    v;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindCtlID( d );
		if( v == -1 ) {ErrHandler( 1011, "\p in CtlEnable command"); return 0 ; } 
	}
	else
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v = get_longarg_val();
				break;
			case 250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in CtlEnable command");
				break;
		}
	}
	
	a = CTL_Enable( v ); 
	return(0);
}

short cmd_ctlsize()
{
	short a, b=0,c,d,*e ;
	long    v[4];
	
	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindCtlID( d );
		if( v[0] == -1 ) {ErrHandler( 1011, "\p in CtlSize command"); return 0 ; } 
	}

	for(a=b;a<3;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in CtlSize command");
				break;
		}
	}
	a = CTL_Size( v[0], v[1], v[2] ); 
	return(0);
}

short cmd_ctlmove()
{
	short a, b=0, c, d, *e;
	long    v[4];

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindCtlID( d );
		if( v[0] == -1 ) {ErrHandler( 1011, "\p in CtlMove command"); return 0 ; } 
	}

	for(a=b;a<3;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in CtlMove command");
				break;
		}
	}
	a = CTL_Move( v[0], v[1], v[2] ); 
	
	return(0);
}

short cmd_ctlalign()
{
	short a, b =0, c,d,*e;
	long    v[3];
	
	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindCtlID( d );
		if( v[0] == -1 ) {ErrHandler( 1011, "\p in CtlAlign command"); return 0 ; } 
	}
	
	for(a=b;a<2;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in CtlAlign command");
				break;
		}
	}
	a = CTL_Align( v[0], v[1] ); 
	return(0);
}


short cmd_ctltext()
{
	short    v[3],c,d,*e;
	
	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindCtlID( d );
		if( v[0] == -1 ) {ErrHandler( 1011, "\p in CtlText command"); return 0 ; } 
	}
	else
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[0] = (short) get_longarg_val();
				break;
			case 250:
				v[0] = (short) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in CtlText command");
				break;
		}
	}
	// Now get title string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}

	switch(CodeBuf[CodePos++])
	{
		case 251:
			v[1] = (short) get_longarg_val();
			break;
		case 250:
			v[1] = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in CtlText command");
			break;
	}

	CTL_SetText( v[0], v[1] );
	FreeReturnStr();
	return(0);
}


short cmd_qtopen()
{
	short 	a, varnum;
	long    v[5], Voffset=0;
	Str255	fname;
	short b=0, c,d,*e;

	// GIP is the Var num
	GIP=(short *) (CodeBuf+CodePos);
	varnum = *GIP; 
	CodePos+=2;

	if( RunVar.VarTable2[varnum].dims )	Voffset = ArrayCalc( varnum );	

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindFormID( d );
		if( v[0] == -1 ) {ErrHandler( 1011, "\p in WinClose command"); return 0 ; } 
	}

	for(a=b;a<2;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MovieOpen command");
				break;
		}
	}
	// Now get movie filename string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}

	RStrToStr255( ReturnStr, ReturnStrLen, fname);
	a = QT_OpenMovie( v[0], fname , v[1]  );
	FreeReturnStr(); 
	
	if(a < 0 )  ErrHandler( 1016, "\p in MovieOpen command");

	ITemp = RunVar.VarTable2[varnum].vartype;
	switch(ITemp)
	{
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

	return(0);

}

short cmd_qtplay()
{
	short a;
	long    v;
	switch(CodeBuf[CodePos++])
	{
		case 251:
			v = get_longarg_val();
			break;
		case 250:
			v = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in MoviePlay command");

			break;
	}
	
	if( v < 0 || v > 9 )
	{
		ErrHandler( 1006, "\p in MoviePlay command");
		return(0);
	}   
	a = QT_PlayMovie( v ); 
	if(a < 0 )  ErrHandler( 1016, "\p in MoviePlay command");
	return(0);
}

short cmd_qtclose()
{
	short a;
	long    v;
	switch(CodeBuf[CodePos++])
	{
		case 251:
			v = get_longarg_val();
			break;
		case 250:
			v = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in MovieClose command");
			break;
	}
	
	if( v < 0 || v > 9 )
	{
		ErrHandler( 1016, "\p in MoviePlay command");
		return(0);
	}   
	a = QT_CloseMovie( v ); 
	return(0);
}

short cmd_qtmove()
{
	short 	a;
	long    v[4];

	for(a=0;a<3;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MovieMove command");

				break;
		}
	}

	//v[0]--;
	if( v[0] < 0 || v[0] > 9 )
	{
		ErrHandler( 1016, "\p in MovieMove command");
		return(0);
	} 
	  
	a = QT_OffsetMovie( v[0], v[1], v[2]  );
	
	return(0);

}

short cmd_seropen()
{
	short a;
	long    v[6];

	for(a=0; a<5; a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in SerOpen command");
				break;
		}
	}

	a = SER_BaudConfig( v[1], (short) v[2], (short) v[3], (short) v[4] );
	a = SER_Open( (short) v[0], a );
	return(0);

}

short cmd_serclose()
{
	short a;
	long    v;

	switch(CodeBuf[CodePos++])
	{
		case 251:
			v = get_longarg_val();
			break;
		case 250:
			v = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in SerClose command");
			break;
	}

	a = SER_Close( (short) v );

	return(0);
}



short cmd_serwrite()
{
	short a;
	short    v;
	
	switch(CodeBuf[CodePos++])
	{
		case 251:
			v = get_longarg_val();
			break;
		case 250:
			v = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in SerWrite command");
			break;
	}
	
	// Now get title string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}

	a = SER_Write( v,  ReturnStr, ReturnStrLen);
	FreeReturnStr();
	return(0);
}

short cmd_serset()
{
	short a;
	long    v[6];

	for(a=0; a<5; a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in SerSet command");
				break;
		}
	}

	a = SER_BaudConfig( v[1], (short) v[2], (short) v[3], (short) v[4] );
	if(a==-1 ) ErrHandler( 1004, "\pin SerSet command");
	a = SER_SetParm( (short) v[0], a );
	return(0);

}


// File IO

short cmd_filecreate()
{
	short 	a;
	long    v[4];
	Str255	fname;

	// Now get text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}
	RStrToStr255( ReturnStr, ReturnStrLen, fname);
	FreeReturnStr(); 

	for(a=0; a<2; a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (long) get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in Create command");
				break;
		}
	}

	a = FILE_CreateFileData( fname, v[0], v[1], 0 );

	return(0);

}

short cmd_fileopen()
{
	short 	a, varnum;
	long    v, Voffset=0;
	Str255	fname;

	// GIP is the Var num
	GIP=(short *) (CodeBuf+CodePos);
	varnum = *GIP; 
	CodePos+=2;

	if( RunVar.VarTable2[varnum].dims )	Voffset = ArrayCalc( varnum );	
	
	switch(CodeBuf[CodePos++])
	{
		case 251:
			v = (short) get_longarg_val();
			break;
		case 250:
			v = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in Open command");
			break;
	}
	// Now get text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}

	RStrToStr255( ReturnStr, ReturnStrLen, fname);
	a = FILE_OpenFileData( v,fname );
	FreeReturnStr(); 

	ITemp = RunVar.VarTable2[varnum].vartype;
	
	switch(ITemp)
	{
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
	return(0);

}


short cmd_fileclose()
{
	short a;
	long    v;

	switch(CodeBuf[CodePos++])
	{
		case 251:
			v =  get_longarg_val();
			break;
		case 250:
			v = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in Close command");
			break;
	}

	a = FILE_CloseFile( v );
	return(0);
}

short cmd_filewrite()		// Write strout
{
	short a;
	long    v;

	switch(CodeBuf[CodePos++])
	{
		case 251:
			v = get_longarg_val();
			break;
		case 250:
			v = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in Write command");
			break;
	}
	// Now get text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}

	a = FILE_Write( v );

	FreeReturnStr(); 

	return(0);
}

short cmd_filewritebuffer()		// Write strout
{
	short a;
	long    v[5];

	for(a=0; a<4; a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in WriteBuffer command");
				break;
		}
	}
	a = FILE_WriteBuffer( v[0], v[1], v[2], v[3] );

	FreeReturnStr(); 

	return(0);
}

short cmd_filesetloc()		
{
	short a;
	long    v[3];

	for(a=0; a<2; a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in Seek command");
				break;
		}
	}

	a = FILE_SetLoc( v[0], v[1] );

	return(0);
}


short cmd_setcursor()
{
	short cursorType;
	CursHandle	ch;
	Cursor tc;
	switch(CodeBuf[CodePos++])
	{
		case 251:
			cursorType = (short) get_longarg_val();
			break;
		case 250:
			cursorType = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in SetCursor command");
			break;
	}

	switch( cursorType )
	{
		case 0:
			InitCursor();
			break;
		case 1:
			SetCursor( *GetCursor( iBeamCursor));
			break;
		case 2:
			SetCursor( *GetCursor( crossCursor));
			break;
		case 3:
			SetCursor( *GetCursor( plusCursor));
			break;
		case 4:
			SetCursor( *GetCursor( watchCursor));
			break;
	}
	return(0);
}

short cmd_filedelete()
{
	short 	a;
	Str255	fname;

	// Now get text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}

	RStrToStr255( ReturnStr, ReturnStrLen, fname);
	FreeReturnStr(); 

	a = FILE_Delete( fname );
	if( a )  ErrHandler( 1013, "\p in Delete command");

	return(0);

}

short cmd_createfolder()
{
	short 	a;
	Str255	fname;

	// Now get text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}

	RStrToStr255( ReturnStr, ReturnStrLen, fname);
	FreeReturnStr(); 

	a = FILE_CreateFolder( fname );
	if( a )  ErrHandler( 1013, "\p in CreateFolder command");

	return(0);

}


short cmd_filerename()
{
	short 	a;
	Str255	fname1, fname2;

	// Now get text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}

	RStrToStr255( ReturnStr, ReturnStrLen, fname1);
	FreeReturnStr(); 

	// Now get text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}

	RStrToStr255( ReturnStr, ReturnStrLen, fname2);
	FreeReturnStr(); 
	
	a = FILE_Rename( fname1, fname2 );
	if( a )  ErrHandler( 1013, "\p in Rename command");

	return(0);

}

short cmd_filelock()
{
	short 	a;
	Str255	fname;

	// Now get text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}

	RStrToStr255( ReturnStr, ReturnStrLen, fname);
	FreeReturnStr(); 

	a = FILE_Lock( fname );
	if( a )  ErrHandler( 1013, "\p in Lock command");

	return(0);

}

short cmd_fileunlock()
{
	short 	a;
	Str255	fname;

	// Now get text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}

	RStrToStr255( ReturnStr, ReturnStrLen, fname);
	FreeReturnStr(); 

	a = FILE_Unlock( fname );
	if( a )  ErrHandler( 1013, "\p in Unlock command");

	return(0);

}

short cmd_filecopy()
{
	short 	a;
	Str255	fname1, fname2;

	// Now get text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}

	RStrToStr255( ReturnStr, ReturnStrLen, fname1);
	FreeReturnStr(); 

	// Now get text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}

	RStrToStr255( ReturnStr, ReturnStrLen, fname2);
	FreeReturnStr(); 
	
	a = FILE_Copy( fname1, fname2 );
	if( a )  ErrHandler( 1013, "\p in CopyFile command");

	return(0);

}


short cmd_showcursor()
{
	ShowCursor( );
	
	return(0);
}

short cmd_hidecursor()
{
	HideCursor( );
	
	return(0);
}

short cmd_clearerror()
{
	LastErrorNumber = 0;	
	return(0);
}

short cmd_onerror()
{

	GIP=(short *) (CodeBuf+CodePos);
	ErrorSub = *GIP; 

	return(0);

}

short cmd_events()
{
	short flag, st;;
	switch(CodeBuf[CodePos++])
	{
		case 251:
			flag = (short) get_longarg_val();
			break;
		case 250:
			flag = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in Events command");
			break;
	}
	
	if( !flag ) EventsFlag = 0;
	else
	{
		EventsFlag = 1;
		if( flag < 0 ) flag = 1;
		if( flag > 10 ) flag =10;
		EventSetting = flag;
		#if powerc
			st =  	1000;
			EventCtr = st - ((EventSetting-1)*100);
		#else
			st =  	400;
			EventCtr = st - ((EventSetting-1)*40);
		#endif
		
	}
	return(0);
}

short cmd_console()
{
	short flag;

	switch(CodeBuf[CodePos++])
	{
		case 251:
			flag = (short) get_longarg_val();
			break;
		case 250:
			flag = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in console command");
			break;
	}
	
	if( !flag ) ConsoleFlag = 0; 
	else ConsoleFlag = 1;
	return(0);
}


short cmd_printopen()		
{
	if( PrintActive ) return(0);
	//PrintHandle = (THPrint) NewHandle(sizeof(TPrint)); removed because it caused problems
	//PrintDefault(PrintHandle); removed because it caused problems
	PrintActive = true;
	PrinterPort = PrOpenDoc(PrintHandle, nil, nil);
	SetPort( &PrinterPort->gPort);
	PrOpenPage( PrinterPort, nil);
	return(0);
}

short cmd_printclose()		
{
	if( !PrintActive) return(-1);
	SetPort( &PrinterPort->gPort);
	PrClosePage(PrinterPort);
	PrCloseDoc(PrinterPort);
	PrintActive = false;
	//PrintHandle = nil; removed because it caused problems
	return(0);
}

short cmd_newpage()
{
	if( !PrintActive) return(-1);
	SetPort( &PrinterPort->gPort);
	PrClosePage( PrinterPort);
	PrOpenPage( PrinterPort, nil);
	return(0);
}

short cmd_poke()		
{
	short a;
	long    v[4];
	UCHAR	*ptr, ch;

	for(a=0; a<2;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (long) get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in Poke command");
				break;
		}
	}
	ptr = (UCHAR *) v[0];
	ch = (UCHAR) v[1];
	*ptr = ch;
	return(0);
}

short cmd_pokeint()		
{
	short a;
	long    v[4];
	short	*ptr, ch;

	for(a=0; a<2;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (long) get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in PokeInt command");
				break;
		}
	}
	ptr = (short *) v[0];
	ch = (short) v[1];
	*ptr = ch;
	return(0);
}

short cmd_pokelong()		
{
	short a;
	long    v[4];
	long	*ptr, ch;

	for(a=0; a<2;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (long) get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in PokeLong command");
				break;
		}
	}
	ptr = (long *) v[0];
	ch = (long) v[1];
	*ptr = ch;
	return(0);
}

short cmd_memalloc()
{
	short 	a, varnum;
	long    v, Voffset=0;

	// GIP is the Var num
	GIP=(short *) (CodeBuf+CodePos);
	varnum = *GIP; 
	CodePos+=2;

	if( RunVar.VarTable2[varnum].dims )	Voffset = ArrayCalc( varnum );	

	switch(CodeBuf[CodePos++])
	{
		case 251:
			v = get_longarg_val();
			break;
		case 250:
			v = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in MemAlloc command");
			break;
	}

	a = MemAlloc( v );

	ITemp = RunVar.VarTable2[varnum].vartype;
	switch(ITemp)
	{
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

	return(0);

}

short cmd_memfree()		
{
	long    v;

	switch(CodeBuf[CodePos++])
	{
		case 251:
			v = (long) get_longarg_val();
			break;
		case 250:
			v = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in MemFree command");
			break;
	}

	MemFree(v);

	return(0);
}

short cmd_memresize()		
{
	short a;
	long    v[4];

	for(a=0;a<2;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (long) get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MemResize command");
				break;
		}
	}
	MemResize( v[0], v[1]);

	return(0);
}

short cmd_memfill()		
{
	short a;
	long    v[4];

	for(a=0;a<4;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (long) get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MemFill command");
				break;
		}
	}
	MemFill( v[0], v[1], v[2], v[3]);

	return(0);
}

short cmd_memint()		
{
	short a;
	long    v[4];

	for(a=0;a<3;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (long) get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MemInt command");
				break;
		}
	}
	MemInt( v[0], v[1], v[2]);

	return(0);
}

short cmd_memlong()		
{
	short a;
	long    v[4];

	for(a=0;a<3;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (long) get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MemLong command");
				break;
		}
	}
	MemLong( v[0], v[1], v[2]);

	return(0);
}

short cmd_memsingle()		
{
	short a;
	long  b,c;
	short double    v[4];

	for(a=0;a<3;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (short double) get_longarg_val();
				break;
			case 250:
				v[a] = (short double) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MemSingle command");
				break;
		}
	}
	b = (long) v[0];
	c = (long) v[1];
	MemSingle( b, c, v[2]);

	return(0);
}

short cmd_memdouble()		
{
	short a;
	long  b,c;
	short double    v[4];

	for(a=0;a<3;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (short double) get_longarg_val();
				break;
			case 250:
				v[a] = (short double) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MemDouble command");
				break;
		}
	}
	b = (long) v[0];
	c = (long) v[1];
	MemDouble( b, c, v[2]);

	return(0);
}

short cmd_memstring()		
{
	short a;
	long    v[4];

	for(a=0;a<2;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (long) get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MemString command");
				break;
		}
	}
	// Now get text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}
	else ErrHandler( 1042, "\p in MemString command");

	MemString( v[0], v[1]);
	FreeReturnStr(); 
	
	return(0);
}

short cmd_textselect()		
{
	short a, b=0,c,d,*e ;
	long    v[4];
	
	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindCtlID( d );
		if( v[0] == -1 ) {ErrHandler( 1011, "\p in TextSelect command"); return 0 ; } 
	}

	for(a=b;a<3;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (long) get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in TextSelect command");
				break;
		}
	}
	CTL_SetTextSelect( v[0], v[1], v[2] );
	return(0);
}

short cmd_listselect()		
{
	short a, b=0,c,d,*e ;
	long    v[4];
	
	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindCtlID( d );
		if( v[0] == -1 ) {ErrHandler( 1011, "\p in ListSelect command"); return 0 ; } 
	}

	for(a=b;a<2;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (long) get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in ListSelect command");
				break;
		}
	}
	CTL_ListSelect( v[0], v[1] );
	return(0);
}

short cmd_listdeselect()		
{
	short a, b=0,c,d,*e ;
	long    v[4];
	
	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindCtlID( d );
		if( v[0] == -1 ) {ErrHandler( 1011, "\p in ListDeselect command"); return 0 ; } 
	}

	for(a=b;a<2;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (long) get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in ListDeselect command");
				break;
		}
	}
	CTL_ListSelect( v[0], v[1] );
	return(0);
}

short cmd_setvalue()		
{
	short a, b=0,c,d,*e ;
	long    v[4];
	
	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindCtlID( d );
		if( v[0] == -1 ) {ErrHandler( 1011, "\p in SetValue command"); return 0 ; } 
	}

	for(a=b;a<2;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (long) get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in SetValue command");
				break;
		}
	}
	CTL_SetValue( v[0], v[1] );
	return(0);
}

short cmd_cut()
{
	short a, b;
	
	if( ActiveTE != nil )
	{
		a = WhichControl();
		if( a >=0 )
		{
			if( !(CtlData[a].lastItem & 2) )
			{
				RGBForeColor( &CtlData[a].ForeRGB );
				RGBBackColor( &CtlData[a].BackRGB );
				TECut(ActiveTE);
				ZeroScrap();
				TEToScrap();
				if( CtlData[a].lastItem & 1 ) TextFix( a );
				b = CtlData[a].WinNum;
				RGBForeColor( &WinData[b].ForeRGB );
				RGBBackColor( &WinData[b].BackRGB );
			}
		}
	}
	
	return(0);
}

short cmd_copy()
{
	if( ActiveTE ) { TECopy( ActiveTE ); ZeroScrap(); TEToScrap();}

	
	return(0);
}

short cmd_paste()
{
	short a, b;
	if( ActiveTE != nil )
	{
		a = WhichControl();
		if( a >=0 )
		{
			if( !(CtlData[a].lastItem & 2) )
			{
				RGBForeColor( &CtlData[a].ForeRGB );
				RGBBackColor( &CtlData[a].BackRGB );
				TEFromScrap();
				TEPaste(ActiveTE);
				TESelView( CtlData[a].te );
				if( CtlData[a].status == 11 ) TextFix( a );
				b = CtlData[a].WinNum;
				RGBForeColor( &WinData[b].ForeRGB );
				RGBBackColor( &WinData[b].BackRGB );
			}
		}
 	}
 	
	return(0);
}

short cmd_clear()
{
	short a, b;
	if( ActiveTE != nil )
	{
		a = WhichControl();
		if( a >=0 )
		{
			if( !(CtlData[a].lastItem & 2) )
			{
				RGBForeColor( &CtlData[a].ForeRGB );
				RGBBackColor( &CtlData[a].BackRGB );
				TEDelete(ActiveTE);
				if( CtlData[a].lastItem & 1 ) TextFix( a );
				b = CtlData[a].WinNum;
				RGBForeColor( &WinData[b].ForeRGB );
				RGBBackColor( &WinData[b].BackRGB );
			}
		}
 	}
 	
	return(0);
}

short cmd_menublank()
{
	MENU_Blank( );
	return(0);
}

short cmd_menuclear()
{
	MENU_Clear( );
	return(0);
}

short cmd_menudelete()
{
	short v;

	switch(CodeBuf[CodePos++])
	{
		case 251:
			v = (short) get_longarg_val();
			break;
		case 250:
			v = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in MenuDelete command");
			break;
	}
	MENU_Delete( v );

	return(0);
}

short cmd_menufonts()
{
	short v;

	switch(CodeBuf[CodePos++])
	{
		case 251:
			v = (short) get_longarg_val();
			break;
		case 250:
			v = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in MenuDelete command");
			break;
	}
	MENU_AddFonts( v );

	return(0);
}


short cmd_menudeleteitem()
{
	short a;
	short    v[4];

	for(a=0;a<2;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (short) get_longarg_val();
				break;
			case 250:
				v[a] = (short) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MenuDeleteItem command");
				break;
		}
	}
	MENU_DeleteItem( v[0], v[1] );

	return(0);
}

short cmd_menudisable()
{
	short a;
	short    v[4];

	for(a=0;a<2;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (short) get_longarg_val();
				break;
			case 250:
				v[a] = (short) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MenuDisable command");
				break;
		}
	}
	MENU_Disable( v[0], v[1] );

	return(0);
}

short cmd_menuenable()
{
	short a;
	short    v[4];

	for(a=0;a<2;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (short) get_longarg_val();
				break;
			case 250:
				v[a] = (short) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MenuEnable command");
				break;
		}
	}
	MENU_Enable( v[0], v[1] );

	return(0);
}

short cmd_menuinsertitem()
{
	short a, cmd, cmdchar = 0;
	long    v[3], rsl;
	UCHAR	*rs;

	for(a=0; a<2;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MenuInsertItem command");
				break;
		}
	}
	if(v[0]<0 || v[0]> 50)
	{
		ErrHandler( 1006, "\p in MenuInsertItem command");
		return(-1);
	}
	
	// Now get attrib string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}

	rs  = ReturnStr;
	rsl = ReturnStrLen;
	ReturnStr = nil;
	ReturnStrLen = 0;
	
	// Now get cmd letter string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); 
	}
	if( ReturnStrLen && ReturnStr[0] != ' ' )  cmdchar = ReturnStr[0];
	
	FreeReturnStr(); 
	ReturnStr = rs;
	ReturnStrLen = rsl;

	GIP=(short *) (CodeBuf+CodePos);
	cmd = *GIP; 

	a = MENU_InsertItem( v[0], v[1], (char*) ReturnStr, cmd, cmdchar ); 
	FreeReturnStr(); 

	return(0);

}
short cmd_menumark()
{
	short a;
	short    v[4];

	for(a=0;a<3;a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = (short) get_longarg_val();
				break;
			case 250:
				v[a] = (short) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MenuMark command");
				break;
		}
	}
	MENU_SetMark( v[0], v[1], v[2] );

	return(0);
}

short cmd_menuprepare();

short cmd_menuredraw()
{
	MENU_Redraw( );
	return(0);
}




short cmd_menutext()
{
	short a,  v[3];

	for( a=0; a<2; a++)
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v[a] = get_longarg_val();
				break;
			case 250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MenuText command");
				break;
		}
	}
	if(v[0]<0 || v[0]> 50)
	{
		ErrHandler( 1006, "\p in MenuText command");
		return(-1);
	}
	
	// Now get attrib string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}

	a = MENU_SetText( v[0], v[1], (char*) ReturnStr ); 
	FreeReturnStr(); 

	return(0);

}

short cmd_menuprepare()
{
	return(0);
}

short cmd_formshow()
{
	short a, b=0, c, d, *e;
	long    v;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		v = *e;
		CodePos+=2;
	}
	else
	{
		switch(CodeBuf[CodePos++])
		{
			case 251:
				v = get_longarg_val();
				break;
			case 250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in Form command");
				break;
		}
	}
	a = FORM_Create( (short) v );

	return(0);
}


short cmd_conclear()
{
	GrafPtr saveport;
	EventRecord  ae;
	
	if( !PrintFlag ) return 0;

	GetPort(&saveport);
	SetPort((WindowPtr)SIOUXTextWindow);
	SelectWindow((WindowPtr)SIOUXTextWindow);

	ae.what = keyDown;
	ae.message = 'a';
	ae.when = TickCount();
	ae.modifiers = cmdKey;
	SIOUXHandleOneEvent( &ae );

	ae.what = keyDown;
	ae.message = 0x7F;
	ae.when = TickCount();
	ae.modifiers = 0;
	SIOUXHandleOneEvent( &ae );
	SetPort(saveport);
	return 0;
}

short cmd_contitle()
{
	
	short a, b=0, c,d,*e;
	long    v;
	Str255	wt;

	// Now get title string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}
	else ErrHandler( 1042, "\p in ConTitle command");

	RStrToStr255( ReturnStr, ReturnStrLen, wt);
	FreeReturnStr();
	
	if( !PrintFlag ) return 0;
	
	SIOUXSetTitle( wt);
	return 0;
}