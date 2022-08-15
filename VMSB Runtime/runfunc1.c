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
#include        <Palettes.h>
#include        <Resources.h>
#include 		<ctype.h>
#define         UCHAR   unsigned char

void RStrToStr255( UCHAR * st1, long slen, Str255 st2);
void Str255ToStr( Str255 st1, char * st2);
void RStrToStr( UCHAR * st1, long slen, unsigned char* st2, long max);
short FreeReturnStr();
short Allocate_ReturnStr( long);
long GetLabelLoc( short labelnum );
long get_longarg_val();
float get_singlearg_val();
short double get_doublearg_val();
long grab_longvar(short varnum);
unsigned char *get_stringarg_val();
long RandomRange( long v1, long v2 );
long LaunchAnApplication (UCHAR *appfile);
long grab_varaddress(short varnum);
short FindCtlID( short id );
short FindFormID( short id );
long MemGetHandle( short mb );
long CTL_GetHandle( short a );
short GetCtlStatus( short a );
short GetWinStatus( short a );
short GFX_GetWinTitle( short a );
short GFX_GetFrontWindow( );

short CTL_GetText( short a );
short CTL_TextSelect( short ctl );
short CTL_GetTextStart( short ctl);
short CTL_GetTextEnd( short ctl);
short CTL_TextLen( short ctl );
short CTL_IsListSelected( short ListNum, short item );
short CTL_SelectCount( short ListNum);
short CTL_ListItem( short ListNum, short item );
long CTL_GetValue( short a );

long SER_Read( short portNum, unsigned char *buf, long numToRead );
long SER_CharAvail(short portNum);
short SER_Check( short portNum );


short FILE_Read( short filenum, long ReadLen  );
short FILE_Exist( Str255 filename );
long FILE_LenOpen( short filenum );
long FILE_GetLoc(short filenum);
long FILE_IfEOF(short filenum);
short FILE_OpenDialog( Str255 filetypes );
short FILE_SaveDialog( Str255 filename, Str255 prompt);
long  FILE_Size( Str255 filename );
short FILE_LineInput( short filenum  );
short FILE_AppPath( );
short FILE_Dir( Str255 filename );


short SPR_SpriteWidth( short imageNum );
short SPR_SpriteHeight( short imageNum );
short SPR_SpriteHit( short imageNum, short min, short max );

short GFX_Windowinfo(short WinNum, short parm);
short GFX_Screeninfo( short parm);
short GFX_MsgBox(UCHAR * message, short buttonTypes );
long GFX_WinHandle( short v );

short CTL_ListCount( short ListNum );
short CTL_Border( short ctl, short dm );

short MENU_GetText( short menu, short where );
short MENU_Count( short menu );
short MENU_GetMark( short menu, short where );

long CRES_CodeRes( short ResID, long v1, long v2, long v3, long v4 );

short GetMemInt( long bufferID, long location );
long  GetMemLong( long bufferID, long location );
short double GetMemSingle( long bufferID, long location );
short double GetMemDouble( long bufferID, long location );
short GetMemString( long bufferID, long location, long ln);
long MemSize( long bufferID );
long FILE_LoadPictFile( Str255 filename ); 
long  FILE_ReadBuffer( short filenum, short bufferID, long Start, long ReadLen  );
short double GFX_GetPixel(short WinNum, short x1, short y1);
short double GFX_RGB( long red, long green, long blue );

short  SND_IsPlaying( short sndNum );
short QT_Ver();
short SND_Ver();
short SPK_Ver();
short SPK_Busy();

void ErrHandler( short ErrNum, Str255 AddMsg);
unsigned char * GetErrMsg( );
extern short	LastErrorNumber, ErrorSub;

long 			Long_Function_Result;
float			Single_Function_Result;
short double	Double_Function_Result;

extern UCHAR    *CodeBuf;
extern long     CodePos, CodeLen, NLP;
extern short 		current_module_num, CallStackPtr, RunModCtr, LoopStackPtr, NextFlag;
extern UCHAR	*ReturnStr, ReturnStrFlag;
extern long		ReturnStrLen;

extern THPrint	PrintHandle;
extern TPPrPort	PrinterPort;
extern char 	PrintActive;
extern short MouseX, MouseY;
extern short EventCode[4];

short	func_abs(short rtype);
short	func_atan(short rtype);
short	func_acos(short rtype);
short	func_asin(short rtype);
short	func_cos(short rtype);
short	func_log(short rtype);
short	func_sin(short rtype);
short	func_sqr(short rtype);
short	func_tan(short rtype);
short	func_len(short rtype);
short	func_val(short rtype);
short	func_chr(short rtype);
short	func_date(short rtype);
short	func_lcase(short rtype);
short	func_ucase(short rtype);
short	func_trim(short rtype);
short	func_ltrim(short rtype);
short	func_rtrim(short rtype);
short	func_space(short rtype);
short	func_str(short rtype);
short	func_string(short rtype);
short	func_rnd(short rtype);  // Random #
short	func_ctext(short rtype);
short	func_serread(short rtype);
short	func_fileread(short rtype);
short	func_fileexist(short rtype);
short	func_filelenopen(short rtype);
short	func_fileloc(short rtype);
short 	func_fileeof(short rtype );
short	func_spritewidth(short rtype);
short	func_spriteheight(short rtype);
short	func_seravail(short rtype);
short	func_sercheck(short rtype);
short	func_time(short rtype);
short	func_day(short rtype);
short	func_wborder(short rtype);
short	func_sborder(short rtype);
short	func_timer(short rtype);
short	func_listcnt(short rtype);
short	func_errornum(short rtype);
short	func_errormsg(short rtype);
short	func_pagesetup(short rtype);
short	func_printsetup(short rtype);
short   func_launch(short rtype);
short	func_peek(short rtype);
short	func_peekint(short rtype);
short	func_peeklong(short rtype);
short	func_varaddr(short rtype);
short	func_getmemint(short rtype);
short	func_getmemlong(short rtype);
short	func_getmemsingle(short rtype);
short	func_getmemdouble(short rtype);
short	func_getmemstring(short rtype);
short	func_memsize(short rtype);
short	func_gettextlen(short rtype);
short	func_gettextstart(short rtype);
short	func_gettextend(short rtype);
short	func_gettextselect(short rtype);
short	func_listselcount(short rtype);
short	func_listselect(short rtype);
short	func_listitem(short rtype);
short	func_getctlvalue(short rtype);
short	func_mousex(short rtype);
short	func_mousey(short rtype);
short	func_getmousex(short rtype);
short	func_getmousey(short rtype);
short	func_mousedown(short rtype);
short	func_keysdown(short rtype);
short	func_ifkey(short rtype);
short	func_getkeys(short rtype);
short	func_fontnum(short rtype);
short	func_menucount(short rtype);
short	func_getmenumark(short rtype);
short	func_menutext(short rtype);
short	func_msgbox(short rtype);
short	func_eventcode(short rtype);
short	func_sndplaying(short rtype);
short	func_qtver(short rtype);
short	func_sndver(short rtype);
short	func_spkver(short rtype);
short	func_spkbusy(short rtype);
short	func_getctlborder(short rtype);
short	func_opendialog(short rtype);
short	func_savedialog(short rtype);
short	func_clipavail(short rtype);
short	func_strres(short rtype);
short	func_strreslen(short rtype);
short	func_filename(short rtype);
short	func_powerpc(short rtype);
short	func_mid(short rtype);
short	func_left(short rtype);
short	func_right(short rtype);
short	func_spritehit(short rtype);
short	func_pictfile(short rtype);
short	func_and(short rtype);
short	func_or(short rtype);
short	func_xor(short rtype);
short	func_exp(short rtype);
short	func_pow(short rtype);
short	func_filesize(short rtype);
short	func_typecvt(short rtype);
short	func_mod(short rtype);
short	func_asc(short rtype);
short	func_filereadbuffer(short rtype);
short	func_pi(short rtype);
short	func_rad2deg(short rtype);
short	func_deg2rad(short rtype);
short	func_getdepth(short rtype);
short	func_hasdepth(short rtype);
short	func_lineinput(short rtype);
short 	func_call(short rtype);
short	func_winhandle(short rtype);
short	func_ctlhandle(short rtype);
short	func_memhandle(short rtype);
short	func_winexist(short rtype);
short	func_ctlexist(short rtype);
short	func_not(short rtype);
short	func_winnum(short rtype);
short	func_ctlnum(short rtype);
short	func_frontwindow(short rtype);
short	func_wintitle(short rtype);
short	func_getpixel(short rtype);
short	func_rgb(short rtype);
short	func_apppath(short rtype);
short	func_hex(short rtype);
short	func_int(short rtype);
short	func_dir(short rtype);
short	func_numfiles(short rtype);
short	func_getfilename(short rtype);
short	func_instr(short rtype);



#include        "Structs1.h"

#include "FuncList.h"

short	func_and(short rtype)
{
	short c;
	long v[3];

	for(c=0; c<2; c++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[c] =  get_longarg_val();
				break;
			case	250:
				v[c] =  (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in AND function");
				return(-1);
				break;
		}
	}

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = v[0] & v[1];
			break;
		case 3:
		case 4:
			Double_Function_Result = (long) v[0] & v[1];
			break;
	}

	return(0);
}

short	func_or(short rtype)
{
	short c;
	long v[3];

	for(c=0; c<2; c++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[c] =  get_longarg_val();
				break;
			case	250:
				v[c] =  (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in OR function");
				return(-1);
				break;
		}
	}

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = v[0] | v[1];
			break;
		case 3:
		case 4:
			Double_Function_Result = (long) v[0] | v[1];
			break;
	}

	return(0);
}

short	func_xor(short rtype)
{
	short c;
	long v[3];

	for(c=0; c<2; c++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[c] =  get_longarg_val();
				break;
			case	250:
				v[c] =  (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in XOR function");
				return(-1);
				break;
		}
	}

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = v[0] ^ v[1];
			break;
		case 3:
		case 4:
			Double_Function_Result = (long) v[0] ^ v[1];
			break;
	}

	return(0);
}

short	func_mod(short rtype)
{
	short c;
	short double v[3];

	for(c=0; c<2; c++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[c] =  (short double) get_longarg_val();
				break;
			case	250:
				v[c] =   get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MOD function");
				return(-1);
				break;
		}
	}

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) fmod( v[0], v[1] );
			break;
		case 3:
		case 4:
			Double_Function_Result =  fmod( v[0], v[1] );
			break;
	}

	return(0);
}

short	func_asc(short rtype)
{
	short	cd=0;
	if(CodeBuf[CodePos++]!=249)
		{ ErrHandler( 1042, "\p in Asc function"); return(-1); }
	get_stringarg_val();
	if( ReturnStrLen ) cd = ReturnStr[0];
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = cd;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) cd;
			break;
	}
	FreeReturnStr();
	return(0);
}


short	func_exp(short rtype)
{
	short double v;

	switch( CodeBuf[CodePos++] )
	{
		case	251:
			v =  (short double) get_longarg_val();
			break;
		case	250:
			v =  get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in EXP function");
			return(-1);
			break;
	}

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) exp( v);
			break;
		case 3:
		case 4:
			Double_Function_Result = exp( v);
			break;
	}

	return(0);
}

short	func_pow(short rtype)
{
	short c;
	short double v[3];

	for(c=0; c<2;c++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[c] =  (short double) get_longarg_val();
				break;
			case	250:
				v[c] =  get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in POWER function");
				return(-1);
				break;
		}
	}
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) pow( v[0], v[1]);
			break;
		case 3:
		case 4:
			Double_Function_Result = pow( v[0], v[1]);
			break;
	}

	return(0);
}

short	func_not(short rtype)
{
	short a;
	long result;
	
	a = CodeBuf[CodePos++];

	if( a==251) result = get_longarg_val();
	else result = (long) get_doublearg_val();
	
	if( !result) result =1;
	else result = 0;
		
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = result;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) result;
			break;
	}
	return(0);
}


short	func_abs(short rtype)
{
	short a;
	
	a = CodeBuf[CodePos++];
	
	switch(rtype)
	{
		case 1:
		case 2:
			if( a==251) Long_Function_Result = labs(get_longarg_val());
			else Long_Function_Result = (long) fabs(get_doublearg_val());
			break;
		case 3:
		case 4:
			Double_Function_Result = (long) fabs(get_doublearg_val());
			break;
	}
	return(0);
}

short	func_int(short rtype)
{
	short a;
	
	a = CodeBuf[CodePos++];
	
	switch(rtype)
	{
		case 1:
		case 2:
			if( a==251) Long_Function_Result = (long) get_longarg_val();
			else Long_Function_Result = (long) get_doublearg_val();
			break;
		case 3:
		case 4:
			Double_Function_Result = (long) get_doublearg_val();
			break;
	}
	return(0);
}

short	func_acos(short rtype)
{
	CodePos++;
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) acos(get_doublearg_val());
			break;
		case 3:
		case 4:
			Double_Function_Result = acos(get_doublearg_val());
			break;
	}
	return(0);
}

short	func_cos(short rtype)
{
	CodePos++;
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) cos(get_doublearg_val());
			break;
		case 3:
		case 4:
			Double_Function_Result = cos(get_doublearg_val());
			break;
	}
	return(0);
}

short	func_pi(short rtype)
{
	CodePos++;
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) 3;
			break;
		case 3:
		case 4:
			Double_Function_Result = 3.1415926535898;
			break;
	}
	return(0);
}

short	func_rad2deg(short rtype)
{
	short double  dv;
	CodePos++;

	dv = get_doublearg_val()*57.29577951;
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) dv;
			break;
		case 3:
		case 4:
			Double_Function_Result = dv;
			break;
	}
	return(0);
}

short	func_deg2rad(short rtype)
{
	short double  dv;
	CodePos++;
	
	dv = get_doublearg_val() / 57.29577951;
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) dv;
			break;
		case 3:
		case 4:
			Double_Function_Result = dv;
			break;
	}
	return(0);
}

short	func_asin(short rtype)
{
	CodePos++;
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) asin(get_doublearg_val());
			break;
		case 3:
		case 4:
			Double_Function_Result = asin(get_doublearg_val());
			break;
	}
	return(0);
}


short	func_atan(short rtype)
{
	CodePos++;
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) atan(get_doublearg_val());
			break;
		case 3:
		case 4:
			Double_Function_Result = atan(get_doublearg_val());
			break;
	}
	return(0);
}

short	func_log(short rtype)
{
	CodePos++;
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) log(get_doublearg_val());
			break;
		case 3:
		case 4:
			Double_Function_Result = log(get_doublearg_val());
			break;
	}
	return(0);
}

short	func_rnd(short rtype)
{
	long v1, v2, v3;
	
	CodePos++;
	v1 = get_longarg_val();
	CodePos++;
	v2 = get_longarg_val();
	v3 = RandomRange( v1, v2+1 );
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = v3; 
			break;
		case 3:
		case 4:
			Double_Function_Result = (double) v3;
			break;
	}
	return(0);
}

short	func_sin(short rtype)
{
	CodePos++;
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) sin(get_doublearg_val());
			break;
		case 3:
		case 4:
			Double_Function_Result = sin(get_doublearg_val());
			break;
	}
	return(0);
}

short	func_sqr(short rtype)
{
	CodePos++;
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) sqrt(get_longarg_val());
			break;
		case 3:
		case 4:
			Double_Function_Result = sqrt(get_doublearg_val());
			break;
	}
	return(0);
}

short	func_tan(short rtype)
{
	CodePos++;
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) tan(get_doublearg_val());
			break;
		case 3:
		case 4:
			Double_Function_Result = tan(get_doublearg_val());
			break;
	}
	return(0);
}


short	func_len(short rtype)
{
	if(CodeBuf[CodePos++]!=249)
		{ ErrHandler( 1042, "\p in Len function"); return(-1); }
	get_stringarg_val();
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = ReturnStrLen;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) ReturnStrLen;
			break;
	}
	FreeReturnStr();
	return(0);
}

short	func_val(short rtype)
{
	if(CodeBuf[CodePos++]!=249) { ErrHandler( 1042, "\p in Val function"); return(-1); }
	get_stringarg_val();
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = atol( (char *) (ReturnStr));
			break;
		case 3:
		case 4:
			Double_Function_Result = atof( (char *) (ReturnStr));
			break;
	}
	FreeReturnStr();
	return(0);
}


short	func_chr(short rtype)
{
	short a;
	CodePos++;
	Long_Function_Result = get_longarg_val();

	// Allocate ReturnStr Space
	a = Allocate_ReturnStr( 4L );
	if(a) return(a); // Buffer allocation error

	ReturnStr[0] = (UCHAR) Long_Function_Result;
	ReturnStr[1] = 0;
	ReturnStrLen = 1;

	return(0);
}

short	func_hex(short rtype)
{
	short a;
	CodePos++;
	Long_Function_Result = get_longarg_val();

	// Allocate ReturnStr Space
	a = Allocate_ReturnStr( 100L );
	if(a) return(a); // Buffer allocation error

	sprintf( (char*)ReturnStr, "%x", Long_Function_Result);
	a=0;
	while( a< 99 )
	{
		if(!ReturnStr[a])
		{	
			ReturnStrLen = a;
			return;
		}
		a++;
	}
	ReturnStrLen = a;

	return(0);
}

short	func_date(short rtype)
{
	short a;
	DateTimeRec	now;
	
	// Allocate ReturnStr Space
	a = Allocate_ReturnStr( 16L );
	if(a) return(a); // Buffer allocation error
	GetTime( &now);
	
	ReturnStr[0] = '0' + (now.month/10); 
	ReturnStr[1] = '0' + (now.month % 10);
	ReturnStr[2] = '/';
	ReturnStr[3] = '0' + (now.day/10);  
	ReturnStr[4] = '0' + (now.day % 10 );
	ReturnStr[5] = '/';
	ReturnStr[6] = '0' + (now.year/1000); now.year-= (now.year/1000) *1000; 
	ReturnStr[7] = '0' + (now.year/100); now.year-= (now.year/100) *100; 
	ReturnStr[8] = '0' + (now.year/10); now.year-= (now.year/10) *10; 
	ReturnStr[9] = '0' + now.year;
	ReturnStr[10] = 0;
	ReturnStrLen = 10;

	return(0);
}

short	func_time(short rtype)
{
	short a;
	DateTimeRec	now;
	
	// Allocate ReturnStr Space
	a = Allocate_ReturnStr( 16L );
	if(a) return(a); // Buffer allocation error
	GetTime( &now);
	
	ReturnStr[0] = '0' + (now.hour/10);
	ReturnStr[1] = '0' + (now.hour % 10);
	ReturnStr[2] = ':';
	ReturnStr[3] = '0' + (now.minute/10);  
	ReturnStr[4] = '0' + (now.minute % 10);
	ReturnStr[5] = ':';
	ReturnStr[6] = '0' + (now.second/10);  
	ReturnStr[7] = '0' + (now.second % 10);
	ReturnStr[8] = 0;
	ReturnStrLen = 8;

	return(0);
}

short	func_day(short rtype)
{
	short a,b,c;
	DateTimeRec	now;
	char *dy, *days[] = {  "Saturday", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
	short dn;
	short ds[] = {8,6,6,7,9,8,6};
	short total, year, month, day;
	if(CodeBuf[CodePos++]!=249) { ErrHandler( 1042, "\p in Day function"); return(-1); }
	
	get_stringarg_val();
	if( ReturnStrLen !=10 ) { FreeReturnStr(); ErrHandler( 1050, "\p in Day function"); return(-1); }
	if( !isdigit(ReturnStr[0]) || !isdigit(ReturnStr[1]) ) { FreeReturnStr(); ErrHandler( 1050, "\p in Day function"); return(-1); }
	if( !isdigit(ReturnStr[3]) || !isdigit(ReturnStr[4]) ) { FreeReturnStr(); ErrHandler( 1050, "\p in Day function"); return(-1); }
	for( a=6;a<10;a++ ) if( !isdigit(ReturnStr[a]) ) { FreeReturnStr(); ErrHandler( 1050, "\p in Day function"); return(-1); }
	if( ReturnStr[2]!='\/' && ReturnStr[2]!='\\'  && ReturnStr[2]!='-') { FreeReturnStr(); ErrHandler( 1050, "\p in Day function"); return(-1); }
	if( ReturnStr[5]!='\/' && ReturnStr[5]!='\\'  && ReturnStr[2]!='-') { FreeReturnStr(); ErrHandler( 1050, "\p in Day function"); return(-1); }
	month = ((ReturnStr[0]-'0')*10) + ReturnStr[1]-'0';
	day = ((ReturnStr[3]-'0')*10) + ReturnStr[4]-'0';
	year = ((ReturnStr[6]-'0')*1000) + ((ReturnStr[7]-'0')*100) + ((ReturnStr[8]-'0')*10) + ReturnStr[9]-'0';
	FreeReturnStr(); 
	

	// Allocate ReturnStr Space
	a = Allocate_ReturnStr( 16L );
	if(a) return(a); // Buffer allocation error

	// follow the 10 steps of the given algorithm in order
	total = (year % 100) / 4;
	total = total + day + (year % 100);
	if ((month == 1) || (month == 10)) total = total + 1;
	if (month == 5) total = total + 2;
	if (month == 8) total = total + 3;
	if ((month == 2) || (month == 3) || (month == 11)) total = total + 4;
	if (month == 6) total = total + 5;
	if ((month == 9) || (month == 12)) total = total + 6;
	if (((year % 4) == 0) && ((month == 1) || (month == 2))) total = total - 1;
	total = total % 7;

	dn = ds[total];
	dy = days[total];
	for(a=0; a< dn; a++ ) ReturnStr[a] = dy[a];
	ReturnStr[a] = 0;
	ReturnStrLen = dn;

	return(0);

}

short	func_typecvt(short rtype)
{
	short a;
	long tp=0, ctr=1, b;
	
	if(CodeBuf[CodePos++]!=249)
		{ ErrHandler( 1042, "\p in TypeCvt function"); return(-1); }
	get_stringarg_val();
	
	for(a=3;a>=0;a--)
	{
		if( ReturnStrLen > a ) b = ReturnStr[a];
		else b=' ';
		{
			tp += b*ctr;
		}
		ctr = ctr*256;
	}
	FreeReturnStr();

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = tp;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) tp;
			break;
	}
	return(0);
}

short	func_lcase(short rtype)
{
	long a;
	if(CodeBuf[CodePos++]!=249) 
		{ ErrHandler( 1042, "\p in Lcase function"); return(-1); }
	
	get_stringarg_val();

	for(a=0; a<ReturnStrLen; a++) 
		if(ReturnStr[a] >='A' && ReturnStr[a] <='Z') ReturnStr[a]+=32;

	return(0);
}

short	func_ucase(short rtype)
{
	long a;
	if(CodeBuf[CodePos++]!=249) 
		{ ErrHandler( 1042, "\p in Ucase function"); return(-1); }
	
	get_stringarg_val();

	for(a=0; a<ReturnStrLen; a++) 
		if(ReturnStr[a] >='a' && ReturnStr[a] <='z') ReturnStr[a]-=32;

	return(0);
}

short	func_ltrim(short rtype)
{
	long a, b=0;
	if(CodeBuf[CodePos++]!=249) 
		{ ErrHandler( 1042, "\p in Ltrim function"); return(-1); }
	
	get_stringarg_val();
	if( !ReturnStrLen ) return(0);

	// Ok, first find 1st non-space
	for(a=0; a<ReturnStrLen; a++) if(ReturnStr[a] !=' ' ) { b = a; break; }
	a=0;	
	if( b>0 )
	{
		for(b=b; b<ReturnStrLen; b++) ReturnStr[a++] = ReturnStr[b];
		ReturnStr[a]=0;
		ReturnStrLen = a;
	}
	// Adjust Len
	return(0);
}

short	func_rtrim(short rtype)
{
	long a;
	if(CodeBuf[CodePos++]!=249) 
		{ ErrHandler( 1042, "\p in Rtrim function"); return(-1); }
	
	get_stringarg_val();
	if( !ReturnStrLen ) return(0);

	for(a=ReturnStrLen-1; a>=0; a--)
	{
 		if(ReturnStr[a] ==' ') ReturnStrLen--;
		else break;
	}
	// Adjust Len
	ReturnStr[ReturnStrLen] = 0;
	return(0);
}

short	func_trim(short rtype)
{
	long a,b=0;
	if(CodeBuf[CodePos++]!=249) 
		{ ErrHandler( 1042, "\p in Trim function"); return(-1); }
	
	get_stringarg_val();
	if( !ReturnStrLen ) return(0);

	for(a=ReturnStrLen-1; a>=0; a--)
	{
 		if(ReturnStr[a] ==' ') ReturnStrLen--;
		else break;
	}

	ReturnStr[ReturnStrLen] = 0;
	if( !ReturnStrLen ) return(0);

	// Ok, first find 1st non-space
	for(a=0; a<ReturnStrLen; a++) if(ReturnStr[a] !=' ' ) { b = a; break; }
		
	if( b>0 )
	{
		for(a=0; b<ReturnStrLen; a++, b++) ReturnStr[a] = ReturnStr[b];
		// Adjust Len
		ReturnStrLen=a;
		ReturnStr[ReturnStrLen] = 0;
	}

	return(0);
}

short	func_space(short rtype)
{
	long a;
	short b;
	CodePos++;

	Long_Function_Result = get_longarg_val();

	// Allocate ReturnStr Space
	b = Allocate_ReturnStr( Long_Function_Result );
	if(b) return(b); // Buffer allocation error

	ReturnStrLen = Long_Function_Result;
	for(a=0; a<ReturnStrLen; a++) ReturnStr[a] = ' ';
	ReturnStr[ReturnStrLen] = 0;

	return(0);
}

short	func_string(short rtype)
{
	short a;
	long v[3];
	
	for(a=0;a<2;a++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[a] = (long) get_longarg_val();
				break;
			case	250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in String function");
				return(-1);
				break;
		}
	}

	// Allocate ReturnStr Space
	a = Allocate_ReturnStr( v[0] );
	if(a) return(a); // Buffer allocation error

	ReturnStrLen = v[0];
	for(a=0; a<ReturnStrLen; a++) ReturnStr[a] = (UCHAR) v[1];
	ReturnStr[ReturnStrLen] = 0;

	return(0);
}

short	func_str(short rtype)
{
	long a;
	char temp[64];
	short b, y, z;
	short double d;

	// Allocate ReturnStr Space
	b = Allocate_ReturnStr( 64L );
	if(b) return(b); // Buffer allocation error

	switch(CodeBuf[CodePos])
	{
		case 251:
			CodePos++;
			a = get_longarg_val();
			if( CodeBuf[CodePos]==250 || CodeBuf[CodePos]==251 )
			{
				CodePos++;
				b = get_longarg_val();
				if( b<0 ) b=0;
				if( b>13 ) b=13;
				d = (short double) a;
				switch( b)
				{
					case 0: sprintf( temp, "%13.0f", d); break;
					case 1: sprintf( temp, "%13.1f", d); break;
					case 2: sprintf( temp, "%13.2f", d); break;
					case 3: sprintf( temp, "%13.3f", d); break;
					case 4: sprintf( temp, "%13.4f", d); break;
					case 5: sprintf( temp, "%13.5f", d); break;
					case 6: sprintf( temp, "%13.6f", d); break;
					case 7: sprintf( temp, "%13.7f", d); break;
					case 8: sprintf( temp, "%13.8f", d); break;
					case 9: sprintf( temp, "%13.9f", d); break;
					case 10: sprintf( temp, "%13.10f", d); break;
					case 11: sprintf( temp, "%13.11f", d); break;
					case 12: sprintf( temp, "%13.12f", d); break;
					case 13: sprintf( temp, "%13.13f", d); break;
				}
			}
			else
			{
				// No second param
				sprintf(temp,"%li", a);
			}
			break;
		case 250:
			CodePos++;
			d = get_doublearg_val();
			if( CodeBuf[CodePos]==250 || CodeBuf[CodePos]==251 )
			{
				CodePos++;
				b = get_longarg_val();
				if( b<0 ) b=0;
				if( b>13 ) b=13;
				switch( b)
				{
					case 0: sprintf( temp, "%13.0f", d); break;
					case 1: sprintf( temp, "%13.1f", d); break;
					case 2: sprintf( temp, "%13.2f", d); break;
					case 3: sprintf( temp, "%13.3f", d); break;
					case 4: sprintf( temp, "%13.4f", d); break;
					case 5: sprintf( temp, "%13.5f", d); break;
					case 6: sprintf( temp, "%13.6f", d); break;
					case 7: sprintf( temp, "%13.7f", d); break;
					case 8: sprintf( temp, "%13.8f", d); break;
					case 9: sprintf( temp, "%13.9f", d); break;
					case 10: sprintf( temp, "%13.10f", d); break;
					case 11: sprintf( temp, "%13.11f", d); break;
					case 12: sprintf( temp, "%13.12f", d); break;
					case 13: sprintf( temp, "%13.13f", d); break;
				}
			}
			else
			{
				// No second param
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
			}
			break;
		default:
			ErrHandler( 1042, "\p in Str function");
			return(-1);
			break;
	}

	// Copy results
	a=0;
	b=0;
	while(temp[a])
	{
	 	if( temp[a]!=' ' ) ReturnStr[b++] = temp[a];
	 	a++;
	}
	ReturnStr[a] = 0;
	ReturnStrLen = b;
	return(0);
}

short	func_left(short rtype)
{
	long v;
	UCHAR	*tReturnStr;
	long	tReturnStrLen;

	if(CodeBuf[CodePos++]!=249) 
		{ 	ErrHandler( 1042, "\p in Left function"); return(-1); }
	
	get_stringarg_val();
	if( !ReturnStrLen ) return(0);

	tReturnStr = ReturnStr;
	tReturnStrLen = ReturnStrLen;
	
	switch( CodeBuf[CodePos++] )
	{
		case	251:
			v = (long) get_longarg_val();
			break;
		case	250:
			v = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in Left function");
			return(-1);
			break;
	}
	ReturnStr = tReturnStr;
	ReturnStrLen = tReturnStrLen;

	if( v<=0 || v > ReturnStrLen) return(0);
	ReturnStrLen = v;
	ReturnStr[ReturnStrLen] = 0;

	return(0);
}

short	func_right(short rtype)
{
	long a, b, v;
	UCHAR	*tReturnStr;
	long	tReturnStrLen;
	
	if(CodeBuf[CodePos++]!=249) 
		{ ErrHandler( 1042, "\p in Right function"); return(-1); }
	get_stringarg_val();
	if( !ReturnStrLen ) return(0);

	tReturnStr = ReturnStr;
	tReturnStrLen = ReturnStrLen;

	switch( CodeBuf[CodePos++] )
	{
		case	251:
			v = (long) get_longarg_val();
			break;
		case	250:
			v = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in Right function");
			return(-1);
			break;
	}

	ReturnStr = tReturnStr;
	ReturnStrLen = tReturnStrLen;

	if( v<=0 || v>ReturnStrLen) return(0);
	
	b = ReturnStrLen - v;
	for(a=0; b<ReturnStrLen; a++, b++) ReturnStr[a] = ReturnStr[b];
	
	ReturnStrLen = a;
	ReturnStr[ReturnStrLen] = 0;

	return(0);
}

short	func_mid(short rtype)
{
	long a, b, c;
	long v[3];
	UCHAR	*tReturnStr;
	long	tReturnStrLen;

	if(CodeBuf[CodePos++]!=249) 
		{ ErrHandler( 1042, "\p in Mid function"); return(-1); }
	get_stringarg_val();
	if( !ReturnStrLen ) return(0);

	tReturnStr = ReturnStr;
	tReturnStrLen = ReturnStrLen;

	for(a=0;a<2;a++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[a] = (long) get_longarg_val();
				break;
			case	250:
				v[a] = (long) get_doublearg_val();
				if( v[a]< 0 ) v[a] = 0;
				break;
			default:
				ErrHandler( 1042, "\p in Lcase function");
				return(-1);
				break;
		}
	}
	
	ReturnStr = tReturnStr;
	ReturnStrLen = tReturnStrLen;

	if( v[0] < 1 ) v[0] = 1;
	if( v[0] > ReturnStrLen ) return(0);
	v[0]--;
	b = v[0];
	c = b + v[1];
	if( c > ReturnStrLen ) c = ReturnStrLen;

	for(a=0; b<c; a++, b++) ReturnStr[a] = ReturnStr[b];
	
	ReturnStrLen = a;
	ReturnStr[ReturnStrLen] = 0;

	return(0);
}


short	func_ctext(short rtype)
{
	short v;
	short b=0, c,d,*e;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindCtlID( d );
		if( v == -1 ) {ErrHandler( 1011, "\p in CtlText function"); return 0 ; } 
	}
	else
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v = (short) get_longarg_val();
				break;
			case	250:
				v = (short) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in CtlText function");
				return(-1);
				break;
		}
	}
	b = CTL_GetText( v );
	
	return(0);
}

short	func_serread(short rtype)
{
	long a, b[3], d;
	short c;
	

	for(c=0; c<2; c++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				b[c] =  get_longarg_val();
				break;
			case	250:
				b[c] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in SerRead function");
				return(-1);
				break;
		}
	}

	d = SER_CharAvail((short) b[0]); 
	if( d<1 ) { ReturnStr=nil; ReturnStrLen=0; return(0); }
	
	a = b[1];
	if(a<1 || d<a) a = d; 
	
	// Allocate ReturnStr Space
	c = Allocate_ReturnStr( a+16 );
	if(c) { ReturnStr=nil; ReturnStrLen=0; return(-1); } // Buffer allocation error

	d = SER_Read( (short) b[0], ReturnStr, a );
	if(d>=0)
	{
		ReturnStrLen=d;
		ReturnStr[d] = 0;
	}

	return(0);
}

// File IO

short	func_lineinput(short rtype)
{
	short b;
	short c;

	switch( CodeBuf[CodePos++] )
	{
		case	251:
			b =  get_longarg_val();
			break;
		case	250:
			b = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in LineInput function");
			return(-1);
			break;
	}

	c = Allocate_ReturnStr( 305 );
	if(c) { ReturnStr=nil; ReturnStrLen=0; return(-1); } // Buffer allocation error

	c = FILE_LineInput( b );
	
	return(0);
}

short	func_fileread(short rtype)
{
	long b[3];
	short c;

	for(c=0; c<2; c++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				b[c] =  get_longarg_val();
				break;
			case	250:
				b[c] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in Read function");
				return(-1);
				break;
		}
	}

	c = Allocate_ReturnStr( b[1]+16 );
	if(c) { ReturnStr=nil; ReturnStrLen=0; return(-1); } // Buffer allocation error

	c = FILE_Read( b[0], b[1] );
	
	return(0);
}

short	func_filereadbuffer(short rtype)
{
	long b[6];
	long c;

	for(c=0; c<4; c++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				b[c] =  get_longarg_val();
				break;
			case	250:
				b[c] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in ReadBuffer function");
				return(-1);
				break;
		}
	}

	c = FILE_ReadBuffer( b[0], b[1], b[2], b[3] );
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = c;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) c;
			break;
	}
	return(0);
}

short	func_filelenopen(short rtype)
{
	long b;
	long c;
	
	switch( CodeBuf[CodePos++] )
	{
		case	251:
			b = get_longarg_val();
			break;
		case	250:
			b = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in LOF function");
			return(-1);
			break;
	}

	c = FILE_LenOpen( b );

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = c;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) c;
			break;
	}
	
	return(0);
}

short	func_fileloc(short rtype)
{
	long b;
	long c;
	
	switch( CodeBuf[CodePos++] )
	{
		case	251:
			b = get_longarg_val();
			break;
		case	250:
			b = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in LOF function");
			return(-1);
			break;
	}

	c = FILE_GetLoc( b );

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = c;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) c;
			break;
	}
	
	return(0);
}

short	func_fileeof(short rtype)
{
	short b;
	long c;
	
	switch( CodeBuf[CodePos++] )
	{
		case	251:
			b = (short) get_longarg_val();
			break;
		case	250:
			b = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in EOF function");
			return(-1);
			break;
	}

	c = FILE_IfEOF(b);


	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = c;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) c;
			break;
	}
	
	return(0);
}

short	func_fileexist(short rtype)
{
	short c;
	Str255	fname;
	
	if(CodeBuf[CodePos++]!=249) 
		{ ErrHandler( 1042, "\p in FileExist function"); return(-1); }
	get_stringarg_val();

	RStrToStr255(  ReturnStr,  ReturnStrLen, fname);
	FreeReturnStr();

	c = FILE_Exist( fname );

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) c;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) c;
			break;
	}
	return(0);
}

short	func_filesize(short rtype)
{
	long c;
	Str255	fname;
	
	if(CodeBuf[CodePos++]!=249) 
		{ ErrHandler( 1042, "\p in FileSize function"); return(-1); }
	get_stringarg_val();

	RStrToStr255(  ReturnStr,  ReturnStrLen, fname);
	FreeReturnStr();

	c = FILE_Size( fname );

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result =  c;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) c;
			break;
	}
	return(0);
}

short	func_dir(short rtype)
{
	long c;
	Str255	fname;
	
	if(CodeBuf[CodePos++]!=249) 
		{ ErrHandler( 1042, "\p in Dir function"); return(-1); }
	get_stringarg_val();

	RStrToStr255(  ReturnStr,  ReturnStrLen, fname);
	FreeReturnStr();

	FILE_Dir( fname );

	return(0);
}


short	func_pictfile(short rtype)  //LoadPict
{
	short c;
	Str255	fname;
	
	if(CodeBuf[CodePos++]!=249) 
		{ ErrHandler( 1042, "\p in PictFile function"); return(-1); }
	get_stringarg_val();

	RStrToStr255(  ReturnStr,  ReturnStrLen, fname);
	FreeReturnStr();

	c = FILE_LoadPictFile( fname );

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) c;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) c;
			break;
	}
	return(0);
}


short	func_spritewidth(short rtype)
{
	short b;
	long c;
	
	switch( CodeBuf[CodePos++] )
	{
		case	251:
			b = (short) get_longarg_val();
			break;
		case	250:
			b = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in SpriteWidth function");
			return(-1);
			break;
	}

	c = SPR_SpriteWidth( b );

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = c;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) c;
			break;
	}
	
	return(0);
}

short	func_spriteheight(short rtype)
{
	short b;
	long c;
	
	switch( CodeBuf[CodePos++] )
	{
		case	251:
			b = (short) get_longarg_val();
			break;
		case	250:
			b = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in SpriteHeight function");
			return(-1);
			break;
	}

	c = SPR_SpriteHeight( b );

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = c;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) c;
			break;
	}
	
	return(0);
}

short	func_seravail(short rtype)
{
	short b;
	long c;
	
	switch( CodeBuf[CodePos++] )
	{
		case	251:
			b = (short) get_longarg_val();
			break;
		case	250:
			b = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in SerAvail function");
			return(-1);
			break;
	}

    c = SER_CharAvail( b );

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = c;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) c;
			break;
	}
	
	return(0);
}

short	func_sercheck(short rtype)
{
	short b;
	long c;
	
	switch( CodeBuf[CodePos++] )
	{
		case	251:
			b = (short) get_longarg_val();
			break;
		case	250:
			b = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in SerCheck function");
			return(-1);
			break;
	}

    c = SER_Check( b );

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = c;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) c;
			break;
	}
	
	return(0);
}


short	func_wborder(short rtype)
{
	long v[4],a;
	short b=0, c,d,*e;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindFormID( d );
		if( v[0] == -1 ) {ErrHandler( 1009, "\p in WinBorder function"); return 0 ; } 
	}

	for(c=b; c<2; c++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[c] =  get_longarg_val();
				break;
			case	250:
				v[c] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in WinBorder function");
				return(-1);
				break;
		}
	}

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) GFX_Windowinfo(v[0], v[1]);
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) GFX_Windowinfo(v[0], v[1]);
			break;
	}

	return(0);
}

short	func_sborder(short rtype)
{
	long a;

	switch( CodeBuf[CodePos++] )
	{
		case	251:
			a = (long) get_longarg_val();
			break;
		case	250:
			a = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in ScreenBorder function");
			return(-1);
			break;
	}

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) GFX_Screeninfo(a);
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) GFX_Screeninfo(a);
			break;
	}


	return(0);
}

short	func_getdepth(short rtype)
{
	short a;	
	PixMapHandle	pmh;

	pmh = ((**GetMainDevice()).gdPMap); //->pixelSize;
	a = (*pmh)->pixelSize;

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) a;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) a;
			break;
	}


	return(0);
}

short	func_hasdepth(short rtype)
{
	short	a, c, b=0;

	switch( CodeBuf[CodePos++] )
	{
		case	251:
			a = (long) get_longarg_val();
			break;
		case	250:
			a = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in HasDepth function");
			return(-1);
			break;
	}
	
	c = HasDepth(GetMainDevice(), a, 1, 1);
	if( c ) b= 1;

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) b;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) b;
			break;
	}


	return(0);
}


short	func_timer(short rtype)
{

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) TickCount();
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) TickCount();
			break;
	}


	return(0);
}

short	func_listcnt(short rtype)
{
	short v;
	short b=0, c,d,*e;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindCtlID( d );
		if( v == -1 ) {ErrHandler( 1011, "\p in ListCnt function"); return 0 ; } 
	}
	else
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v = (long) get_longarg_val();
				break;
			case	250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in ListCnt function");
				return(-1);
				break;
		}
	}
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long)  CTL_ListCount( v );
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) CTL_ListCount( v );
			break;
	}


	return(0);
}

short	func_errornum(short rtype)
{

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = LastErrorNumber;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) LastErrorNumber;
			break;
	}


	return(0);
}

short	func_errormsg(short rtype)
{
	short a;
	unsigned char *ptr;
		
	// Allocate ReturnStr Space
	a = Allocate_ReturnStr( 120L );
	if(a) return(a); // Buffer allocation error

	ptr = GetErrMsg( );
	ReturnStrLen = (long) ptr[0];
	ptr++;
	for( a=0;a<1;a++) ReturnStr[a] = ptr[a];
	ReturnStr[ReturnStrLen+1]=0;

	return(0);
}

short	func_pagesetup(short rtype)
{
	
	switch(rtype)
	{
		case 1:
		case 2:
			if( PrintActive) Long_Function_Result=-1;
			else Long_Function_Result = PrStlDialog(PrintHandle);
			break;
		case 3:
		case 4:
			if( PrintActive) Double_Function_Result=-1;
			else Double_Function_Result = PrStlDialog(PrintHandle);
			break;
	}
	return(0);
}

short	func_printsetup(short rtype)
{
	
	switch(rtype)
	{
		case 1:
		case 2:
			if( PrintActive) Long_Function_Result=-1;
			else  Long_Function_Result = PrJobDialog(PrintHandle);
			break;
		case 3:
		case 4:
			if( PrintActive) Double_Function_Result=-1;
			else  Double_Function_Result = PrJobDialog(PrintHandle);
			break;
	}
	return(0);
}

short func_launch(short rtype)
{
	UCHAR	app[256];
	if(CodeBuf[CodePos++]!=249) 
		{ ErrHandler( 1042, "\p in Launch function"); return(-1); }
	get_stringarg_val();

	RStrToStr255( ReturnStr, ReturnStrLen, app);
	FreeReturnStr();

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = LaunchAnApplication (app);
			break;
		case 3:
		case 4:
			Double_Function_Result = ( short double) LaunchAnApplication (app);
			break;
	}
	return(0);
}
	LaunchParamBlockRec		myLaunchParams;

long LaunchAnApplication (UCHAR *appfile)
{
	OSErr		launchErr, error;
	FSSpec		fspec;
	
	error = FSMakeFSSpec(0, 0, appfile, &fspec);
	if(error)
	{
	 	return(-1); 
	}

	myLaunchParams.launchBlockID = extendedBlock;
	myLaunchParams.launchEPBLength = extendedBlockLen;
	myLaunchParams.launchFileFlags = 0;
	myLaunchParams.launchControlFlags = launchContinue; 
	myLaunchParams.launchAppSpec = &fspec;
	myLaunchParams.launchAppParameters = nil;

	launchErr = LaunchApplication(&myLaunchParams);

	return(0);
}

short	func_peek(short rtype)
{
	long b;
	UCHAR *ptr, ch;
	
	switch( CodeBuf[CodePos++] )
	{
		case	251:
			b = (long) get_longarg_val();
			break;
		case	250:
			b = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in Peek function");
			return(-1);
			break;
	}

	ptr = (UCHAR*) b;
    ch = *ptr;

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) ch;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) ch;
			break;
	}
	
	return(0);
}

short	func_peekint(short rtype)
{
	long b;
	short *ptr, ch;
	
	switch( CodeBuf[CodePos++] )
	{
		case	251:
			b = (long) get_longarg_val();
			break;
		case	250:
			b = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in PeekInt function");
			return(-1);
			break;
	}

	ptr = (short*) b;
    ch = *ptr;

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) ch;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) ch;
			break;
	}
	
	return(0);
}

short	func_peeklong(short rtype)
{
	long b;
	long *ptr, ch;
	
	switch( CodeBuf[CodePos++] )
	{
		case	251:
			b = (long) get_longarg_val();
			break;
		case	250:
			b = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in PeekLong function");
			return(-1);
			break;
	}

	ptr = (long*) b;
    ch = *ptr;

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) ch;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) ch;
			break;
	}
	
	return(0);
}

short	func_varaddr(short rtype)
{
	short  *ptr, a;
	long	b;

	// GIP is the Var num
	ptr=(short *) (CodeBuf+CodePos);
	a = *ptr; 
	CodePos+=2;
	b = grab_varaddress(a);
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = b;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) b;
			break;
	}
	
	return(0);
}

short	func_getmemint(short rtype)
{
	short a;
	long v[3];
	
	for(a=0;a<2;a++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[a] = (long) get_longarg_val();
				break;
			case	250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MemInt function");
				return(-1);
				break;
		}
	}

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) GetMemInt( v[0], v[1]);
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) GetMemInt( v[0], v[1]);
			break;
	}
	
	return(0);
}

short	func_getmemlong(short rtype)
{
	short a;
	long v[3];
	
	for(a=0;a<2;a++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[a] = (long) get_longarg_val();
				break;
			case	250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MemLong function");
				return(-1);
				break;
		}
	}

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) GetMemLong( v[0], v[1]);
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) GetMemLong( v[0], v[1]);
			break;
	}
	
	return(0);
}

short	func_getmemsingle(short rtype)
{
	short a;
	long v[3];
	
	for(a=0;a<2;a++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[a] = (long) get_longarg_val();
				break;
			case	250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MemSingle function");
				return(-1);
				break;
		}
	}

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) GetMemSingle( v[0], v[1]);
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) GetMemSingle( v[0], v[1]);
			break;
	}
	
	return(0);
}

short	func_getmemdouble(short rtype)
{
	short a;
	long v[3];
	
	for(a=0;a<2;a++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[a] = (long) get_longarg_val();
				break;
			case	250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MemDouble function");
				return(-1);
				break;
		}
	}

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) GetMemDouble( v[0], v[1]);
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) GetMemDouble( v[0], v[1]);
			break;
	}
	
	return(0);
}



short	func_getmemstring(short rtype)
{
	short a;
	long v[3];
	
	for(a=0;a<3;a++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[a] = (long) get_longarg_val();
				break;
			case	250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MemString function");
				return(-1);
				break;
		}
	}

	a = GetMemString( v[0], v[1], v[2]);

	return(a);
}

short	func_memsize(short rtype)
{
	long v;
	
	switch( CodeBuf[CodePos++] )
	{
		case	251:
			v = (long) get_longarg_val();
			break;
		case	250:
			v = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in MemSize function");
			return(-1);
			break;
	}

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) MemSize( v);
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) MemSize( v);
			break;
	}
	
	return(0);
}

short	func_gettextlen(short rtype)
{
	long v;
	short b=0, c,d,*e;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindCtlID( d );
		if( v == -1 ) {ErrHandler( 1011, "\p in TextLen function"); return 0 ; } 
	}
	else
	{
		
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v = (long) get_longarg_val();
				break;
			case	250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in TextLen function");
				return(-1);
				break;
		}
	}
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) CTL_TextLen( v);
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) CTL_TextLen( v);
			break;
	}
	
	return(0);
}

short	func_gettextstart(short rtype)
{
	long v;
	short b=0, c,d,*e;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindCtlID( d );
		if( v == -1 ) {ErrHandler( 1011, "\p in TextStart function"); return 0 ; } 
	}
	else
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v = (long) get_longarg_val();
				break;
			case	250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in TextStart function");
				return(-1);
				break;
		}
	}
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) CTL_GetTextStart( v);
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) CTL_GetTextStart( v);
			break;
	}
	
	return(0);
}

short	func_gettextend(short rtype)
{
	long v;
	short b=0, c,d,*e;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindCtlID( d );
		if( v == -1 ) {ErrHandler( 1011, "\p in TextEnd function"); return 0 ; } 
	}
	else
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v = (long) get_longarg_val();
				break;
			case	250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in TextEnd function");
				return(-1);
				break;
		}
	}

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) CTL_GetTextEnd( v);
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) CTL_GetTextEnd( v);
			break;
	}
	
	return(0);
}

short	func_gettextselect(short rtype)
{
	short a;
	long v;
	short b=0, c,d,*e;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindCtlID( d );
		if( v == -1 ) {ErrHandler( 1011, "\p in TextSelect function"); return 0 ; } 
	}
	else
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v = (long) get_longarg_val();
				break;
			case	250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in TextSelect function");
				return(-1);
				break;
		}
	}
	a = CTL_TextSelect( v );

	return(a);
}

short	func_listselcount(short rtype)
{
	short v;
	short b=0, c,d,*e;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindCtlID( d );
		if( v == -1 ) {ErrHandler( 1011, "\p in ListNumSel function"); return 0 ; } 
	}
	else
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v = (long) get_longarg_val();
				break;
			case	250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in ListNumSel function");
				return(-1);
				break;
		}
	}
	b = CTL_SelectCount( v );

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) b;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) b;
			break;
	}
	return(b);
}

short	func_listselect(short rtype)
{
	short a;
	long v[3];
	short b=0, c,d,*e;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindCtlID( d );
		if( v[0] == -1 ) {ErrHandler( 1011, "\p in ListSelect function"); return 0 ; } 
	}

	for(a=b;a<2;a++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[a] = (long) get_longarg_val();
				break;
			case	250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in ListSelect function");
				return(-1);
				break;
		}
	}

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) CTL_IsListSelected( v[0], v[1] );
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) CTL_IsListSelected( v[0], v[1] );
			break;
	}

	return(a);
}

short	func_listitem(short rtype)
{
	short a;
	long v[3];
	short b=0, c,d,*e;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindCtlID( d );
		if( v[0] == -1 ) {ErrHandler( 1011, "\p in ListItem function"); return 0 ; } 
	}
	
	for(a=b;a<2;a++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[a] = (long) get_longarg_val();
				break;
			case	250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in ListItem function");
				return(-1);
				break;
		}
	}
	a = CTL_ListItem( v[0], v[1] );  // Returning a string

	return(0);
}

short	func_getctlvalue(short rtype)
{
	long v;
	short b=0, c,d,*e;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindCtlID( d );
		if( v == -1 ) {ErrHandler( 1011, "\p in CtlValue function"); return 0 ; } 
	}
	else
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v = (long) get_longarg_val();
				break;
			case	250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in CtlValue function");
				return(-1);
				break;
		}
	}
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) CTL_GetValue( v);
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) CTL_GetValue( v);
			break;
	}

	return(0);
}

short CTL_Border( short ctl, short dm );
short	func_getctlborder(short rtype)
{
	short a, v[2];
	short b=0, c,d,*e;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindCtlID( d );
		if( v[0] == -1 ) {ErrHandler( 1011, "\p in CtlText function"); return 0 ; } 
	}

	for(a=b; a<2;a++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[a] = (short) get_longarg_val();
				break;
			case	250:
				v[a] = (short) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in CtlBorder function");
				return(-1);
				break;
		}
	}
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) CTL_Border( v[0], v[1]);
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) CTL_Border( v[0], v[1]);
			break;
	}

	return(0);
}


short	func_mousex(short rtype)
{
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result=(long) MouseX;
			break;
		case 3:
		case 4:
			Double_Function_Result=(short double) MouseX;
			break;
	}
	return(0);
}

short	func_mousey(short rtype)
{
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result=(long) MouseY;
			break;
		case 3:
		case 4:
			Double_Function_Result=(short double) MouseY;
			break;
	}
	return(0);
}

short	func_getmousex(short rtype)
{
	Point pt;
	
	GetMouse( &pt );
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result=pt.h;
			break;
		case 3:
		case 4:
			Double_Function_Result=(short double) pt.h;
			break;
	}
	return(0);
}

short	func_getmousey(short rtype)
{
	Point pt;
	
	GetMouse( &pt );
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result=(long) pt.v;
			break;
		case 3:
		case 4:
			Double_Function_Result=(short double) pt.v;
			break;
	}
	return(0);
}

short	func_mousedown(short rtype)
{
	short a;
	a = StillDown();
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result=(long) a;
			break;
		case 3:
		case 4:
			Double_Function_Result=(short double)  a;
			break;
	}
	return(0);
}


short	func_keysdown(short rtype)
{
	unsigned char	km[16], a, flag =0;
	
	GetKeys( (unsigned long*) km );
	for( a=0; a<16;a++) if( km[a] ) { flag++; break; }

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result=(long) flag;
			break;
		case 3:
		case 4:
			Double_Function_Result=(short double)  flag;
			break;
	}
	return(0);
}

short	func_ifkey(short rtype)
{
	unsigned char	km[16];
	short v, a;
	
	switch( CodeBuf[CodePos++] )
	{
		case	251:
			v = (short) get_longarg_val();
			break;
		case	250:
			v = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in IfKey function");
			return(-1);
			break;
	}
	
	GetKeys( (unsigned long*) km );
	a = (km[v >> 3] >> (v & 7)) & 1;

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result=(long) a;
			break;
		case 3:
		case 4:
			Double_Function_Result=(short double)  a;
			break;
	}
	return(0);
}

short	func_getkeys(short rtype)
{
	unsigned char	km[16], values[128], a, b, c;
	long ctr=0;
	
	GetKeys( (unsigned long*) km );
	for( a=0; a<16;a++)
	{
		if( km[a] )
	 	{
	 		b = 1;
	 		for(c=0; c<8; c++)
	 		{
	 			if( km[a] & b ) 
	 			{
	 				values[ctr++] = (a<<3) + c;
	 				b <<=1;
	 			}
	 		}
	 	}
	}
	
	if( ctr)
	{
		// Allocate ReturnStr Space
		b = Allocate_ReturnStr(ctr+4 );
		if(b) return(b); // Buffer allocation error
	
		ReturnStrLen = ctr;
		for(a=0; a<ReturnStrLen; a++) ReturnStr[a] = values[a];
		ReturnStr[ReturnStrLen] = 0;
	}
	
	return(0);
}

short	func_fontnum(short rtype)
{
	short c;
	Str255	fnt, sysfnt;
	
	if(CodeBuf[CodePos++]!=249) 
		{ ErrHandler( 1042, "\p in FontNum function"); return(-1); }
	get_stringarg_val();

	RStrToStr255(  ReturnStr,  ReturnStrLen, fnt);
	FreeReturnStr();

	GetFNum( fnt, &c );
	if( !c )
	{
		// Special if 0
		GetFontName( 0, sysfnt );
		if( !EqualString( fnt, sysfnt, false, false) ) c = -1;
	}

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) c;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) c;
			break;
	}
	return(0);
}


short	func_menucount(short rtype)
{
	long v;
	
	switch( CodeBuf[CodePos++] )
	{
		case	251:
			v = (long) get_longarg_val();
			break;
		case	250:
			v = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in MenuCount function");
			return(-1);
			break;
	}

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) MENU_Count( v);
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) MENU_Count( v);
			break;
	}
	
	return(0);
}

short	func_getmenumark(short rtype)
{
	short a;
	long v[2];
	
	for(a=0; a<2; a++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[a] = (long) get_longarg_val();
				break;
			case	250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MenuMark function");
				return(-1);
				break;
		}
	}
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) MENU_GetMark( v[0], v[1]);
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) MENU_GetMark( v[0], v[1]);
			break;
	}
	
	return(0);
}

short	func_menutext(short rtype)
{
	short a;
	long v[2];
	
	for(a=0; a<2; a++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[a] = (long) get_longarg_val();
				break;
			case	250:
				v[a] = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in MenuText function");
				return(-1);
				break;
		}
	}
	a = MENU_GetText( v[0], v[1]);
	
	return(0);
}

short	func_msgbox(short rtype)
{
	short a;
	long v;
	Str255	txt;
	
	
	if(CodeBuf[CodePos++]!=249) 
		{ ErrHandler( 1042, "\p in Msgbox function"); return(-1); }
	get_stringarg_val();

	RStrToStr255(  ReturnStr,  ReturnStrLen, txt);
	FreeReturnStr();
	
	switch( CodeBuf[CodePos++] )
	{
		case	251:
			v = (long) get_longarg_val();
			break;
		case	250:
			v = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in MsgBox function");
			return(-1);
			break;
	}
	
	a = GFX_MsgBox(txt, v );

	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = a;
			break;
		case 3:
		case 4:
			Double_Function_Result = a;
			break;
	}
	
	return(0);
}

short	func_eventcode(short rtype)
{
	short v, a=0;
	
	switch( CodeBuf[CodePos++] )
	{
		case	251:
			v = (short) get_longarg_val();
			break;
		case	250:
			v = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in EventCode function");
			return(-1);
			break;
	}
	
	if( v>=0 && v<=3) a = EventCode[v];

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result=(long) a;
			break;
		case 3:
		case 4:
			Double_Function_Result=(short double)  a;
			break;
	}
	return(0);
}

short	func_sndplaying(short rtype)
{
	short v, a=0;
	
	switch( CodeBuf[CodePos++] )
	{
		case	251:
			v = (short) get_longarg_val();
			break;
		case	250:
			v = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in SoundPlaying function");
			return(-1);
			break;
	}
	
	a = SND_IsPlaying( v );

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result=(long) a;
			break;
		case 3:
		case 4:
			Double_Function_Result=(short double)  a;
			break;
	}
	return(0);
}

short	func_qtver(short rtype)
{
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result=(long) QT_Ver();
			break;
		case 3:
		case 4:
			Double_Function_Result=(short double) QT_Ver();
			break;
	}
	return(0);
}

short	func_sndver(short rtype)
{
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result=(long) SND_Ver();
			break;
		case 3:
		case 4:
			Double_Function_Result=(short double) SND_Ver();
			break;
	}
	return(0);
}

short	func_spkver(short rtype)
{
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result=(long) SPK_Ver();
			break;
		case 3:
		case 4:
			Double_Function_Result=(short double) SPK_Ver();
			break;
	}
	return(0);
}

short	func_spkbusy(short rtype)
{
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result=(long) SPK_Busy();
			break;
		case 3:
		case 4:
			Double_Function_Result=(short double) SPK_Busy();
			break;
	}
	return(0);
}

short	func_apppath(short rtype)
{
	short a;
	
	a = FILE_AppPath( );

	return(0);
}

short	func_opendialog(short rtype)
{
	short a;
	Str255	ft;
	
	// Now get text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}
	else { ErrHandler( 1042, "\p in OpenDialog function"); return(-1); }

	RStrToStr255( ReturnStr, ReturnStrLen, ft);
	FreeReturnStr(); 
	a = FILE_OpenDialog( ft );

	return(0);
}

short	func_savedialog(short rtype)
{
	short a;
	Str255	st1, st2;

	// Now get filename text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}
	else { ErrHandler( 1042, "\p in SaveDialog function"); return(-1); }

	RStrToStr255( ReturnStr, ReturnStrLen, st1);
	FreeReturnStr(); 

	// Now get prompt text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}
	else { ErrHandler( 1042, "\p in SaveDialog function"); return(-1); }

	RStrToStr255( ReturnStr, ReturnStrLen, st2);
	FreeReturnStr(); 

	a = FILE_SaveDialog( st1, st2 );

	return(0);
}

short	func_clipavail(short rtype)
{
	long	sc;
	
	sc = TEGetScrapLength();
	if( sc ) sc = 1;
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result=(long) sc;
			break;
		case 3:
		case 4:
			Double_Function_Result=(short double) sc;
			break;
	}
	return(0);
}

short	func_strres(short rtype)
{
	short 	v, a;
	long	ctr, slen;
	UCHAR   *buf;
	Handle 	shandle;
	
	switch( CodeBuf[CodePos++] )
	{
		case	251:
			v = (short) get_longarg_val();
			break;
		case	250:
			v = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in StrRes function");
			return(-1);
			break;
	}
	shandle  = GetResource( 'STR ', v);
	if( shandle !=nil )
	{
		slen = GetResourceSizeOnDisk( shandle );
		
		// Allocate ReturnStr Space
		a = Allocate_ReturnStr( slen + 16L );
		buf = (UCHAR*) *shandle;
	    HLock( shandle );
		for( ctr=0; ctr<slen; ctr++) ReturnStr[ctr] = buf[ctr];
		ReturnStrLen = slen;
	    HUnlock( shandle );
	    ReleaseResource( shandle );

	}
	else
	{
		ErrHandler( 1048, "\p in StrRes function");
		return(-1);

	}

	return(0);
}

short	func_strreslen(short rtype)
{
	long	slen=0;
	short v, a=0;
	Handle 	shandle;
	
	switch( CodeBuf[CodePos++] )
	{
		case	251:
			v = (short) get_longarg_val();
			break;
		case	250:
			v = (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in StrResLen function");
			return(-1);
			break;
	}

	shandle  = GetResource( 'STR ', v);
	if( shandle !=nil ) slen = GetResourceSizeOnDisk( shandle );
	else
	{
		ErrHandler( 1048, "\p in StrResLen function");
		return(-1);

	}
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result=(long) slen;
			break;
		case 3:
		case 4:
			Double_Function_Result=(short double) slen;
			break;
	}
	return(0);
}

short	func_filename(short rtype)
{
	long a, b=0;
	if(CodeBuf[CodePos++]!=249) 
		{ ErrHandler( 1042, "\p in FileName function"); return(-1); }
	
	get_stringarg_val();
	if( !ReturnStrLen ) return(0);

	for(a=ReturnStrLen-1; a>=0; a--)
	{
 		if(ReturnStr[a] ==':') break;
	}

 	if(ReturnStr[a] ==':')
 	{
 		a++;
 		while( a < ReturnStrLen && b < 32 )
 		{
 			ReturnStr[b++] = ReturnStr[a++];	
		}
		ReturnStr[b] = 0;
		ReturnStrLen = b;
	}

	return(0);
}

short	func_powerpc(short rtype)
{
	short a=0;
	
	#ifdef powerc
		a = 1;
	#endif
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) a;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) a;
			break;
	}


	return(0);
}

short func_spritehit(short rtype)
{
	short a;
	short v[3];
	
	for(a=0;a<3;a++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[a] = (short) get_longarg_val();
				break;
			case	250:
				v[a] = (short) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in SpriteHit function");
				return(-1);
				break;
		}
	}

	a = SPR_SpriteHit( v[0], v[1], v[2] );
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result=(long) a;
			break;
		case 3:
		case 4:
			Double_Function_Result=(short double) a;
			break;
	}
	return(0);
}

short func_call(short rtype)
{
	short a;
	long result, v[7];
	
	for(a=0;a<5;a++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[a] = get_longarg_val();
				break;
			case	250:
				v[a] = (long)  get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in Call function");
				return(-1);
				break;
		}
	}

	result = CRES_CodeRes( v[0], v[1], v[2], v[3], v[4]);
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result= result;
			break;
		case 3:
		case 4:
			Double_Function_Result=(short double) result;
			break;
	}
	return(0);
}

short	func_winhandle(short rtype)
{
	short v;
	short b=0, c,d,*e;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindFormID( d );
		if( v == -1 ) {ErrHandler( 1009, "\p in WinHandle function"); return 0 ; } 
	}
	else
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v =  get_longarg_val();
				break;
			case	250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in WinHandle function");
				return(-1);
				break;
		}
	}
	
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = GFX_WinHandle( v );
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) GFX_WinHandle( v );
			break;
	}

	return(0);
}

short	func_ctlhandle(short rtype)
{
	short v;
	short b=0, c,d,*e;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindCtlID( d );
		if( v == -1 ) {ErrHandler( 1011, "\p in CtlHandle function"); return 0 ; } 
	}
	else
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v =  get_longarg_val();
				break;
			case	250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in CtlHandle function");
				return(-1);
				break;
		}
	}
	
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = CTL_GetHandle( v );
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) CTL_GetHandle( v );
			break;
	}

	return(0);
}

short	func_memhandle(short rtype)
{
	short v;
	short b=0, c,d,*e;

	switch( CodeBuf[CodePos++] )
	{
		case	251:
			v =  get_longarg_val();
			break;
		case	250:
			v = (long) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in MemHandle function");
			return(-1);
			break;
	}
	
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result =  MemGetHandle( v );
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) MemGetHandle( v );
			break;
	}

	return(0);
}

short	func_ctlexist(short rtype)
{
	short v;
	short b=0, c,d,*e, res=0;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindCtlID( d );
		if( v >=0 ) res=1;
	}
	else
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v =  get_longarg_val();
				break;
			case	250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in CtlExist function");
				return(-1);
				break;
		}
		if( GetCtlStatus( v ) ) res = 1;
	}
	
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) res;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) res;
			break;
	}

	return(0);
}

short	func_winexist(short rtype)
{
	short v;
	short b=0, c,d,*e, res = 0;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindFormID( d );
		if( v >=0 ) res =1;
	}
	else
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v =  get_longarg_val();
				break;
			case	250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in WinExist function");
				return(-1);
				break;
		}
		if( GetWinStatus( v ) ) res = 1;
	}
	
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) res;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) res;
			break;
	}

	return(0);
}

short	func_ctlnum(short rtype)
{
	short v;
	short b=0, c,d,*e;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindCtlID( d );
		if( v == -1 ) {ErrHandler( 1011, "\p in CtlNum function"); return 0 ; } 
	}
	else
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v =  get_longarg_val();
				break;
			case	250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in CtlNum function");
				return(-1);
				break;
		}
	}
	
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = v;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) v;
			break;
	}

	return(0);
}

short	func_winnum(short rtype)
{
	short v;
	short b=0, c,d,*e;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindFormID( d );
		if( v == -1 ) {ErrHandler( 1009, "\p in WinNum function"); return 0 ; } 
	}
	else
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v =  get_longarg_val();
				break;
			case	250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in WinNum function");
				return(-1);
				break;
		}
	}
	
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = v;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) v;
			break;
	}

	return(0);
}

short	func_wintitle(short rtype)
{
	short a,v;
	short b=0, c,d,*e;
	Str255	st1, st2;

	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v = FindFormID( d );
		if( v == -1 ) {ErrHandler( 1009, "\p in WinTitle function"); return 0 ; } 
	}
	else
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v =  get_longarg_val();
				break;
			case	250:
				v = (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in WinTitle function");
				return(-1);
				break;
		}
	}
	
	a = GFX_GetWinTitle( v );

	return(0);
}

short	func_frontwindow(short rtype)
{
	short a;
	
	a = GFX_GetFrontWindow( );    // returns title in a string
    
	return(0);
}

short	func_rgb(short rtype)
{
	short c;
	long v[5];
	short double d;

	for(c=0; c<3; c++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[c] =  get_longarg_val();
				break;
			case	250:
				v[c] =  (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in RGB function");
				return(-1);
				break;
		}
	}

	d = GFX_RGB(  v[0], v[1], v[2]  );

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long)d;
			break;
		case 3:
		case 4:
			Double_Function_Result = d;
			break;
	}

	return(0);
}


short	func_getpixel(short rtype)
{
	short v[5];
	short double d1;

	short b=0, c,d,*e;

	
	c = CodeBuf[CodePos++];
	if( c==2 )
	{
		b=1; // skip first param & find #
		e = (short *) (CodeBuf+CodePos);
		d = *e;
		CodePos+=2;
		v[0] = FindFormID( d );
		if( v[0] == -1 ) {ErrHandler( 1009, "\p in GetPixel function"); return 0 ; } 
	}
	
	for(c=b; c<3; c++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[c] =  (short) get_longarg_val();
				break;
			case	250:
				v[c] =  (short) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in GetPixel function");
				return(-1);
				break;
		}
	}

	d1 = GFX_GetPixel( v[0], v[1], v[2] );

	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = (long) d1;
			break;
		case 3:
		case 4:
			Double_Function_Result = d1;
			break;
	}

	return(0);
}

short	func_numfiles(short rtype)
{
	long a=0, b=0;
	if(CodeBuf[CodePos++]!=249)
		{ ErrHandler( 1042, "\p in NumOfFiles function"); return(-1); }
	
	get_stringarg_val();
	
	while( a < ReturnStrLen )
	{
		if( ReturnStr[a++] == ':' )
		{
			b++;
			if( ReturnStr[a] == ':' ) a++;  //skip dir marker w/o cnting it
		}
	}
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = b;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) b;
			break;
	}
	FreeReturnStr();
	return(0);
}

short	func_getfilename(short rtype)
{
	long v, a=0,b=1,c;
	Str255	st1;

	// Now get filename text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}
	else { ErrHandler( 1042, "\p in GetFilename function"); return(-1); }

	switch( CodeBuf[CodePos++] )
	{
		case	251:
			v =  (short) get_longarg_val();
			break;
		case	250:
			v =  (short) get_doublearg_val();
			break;
		default:
			ErrHandler( 1042, "\p in GetFilename function");
			return(-1);
			break;
	}
	
	if( v<=0 ) { ReturnStrLen = 0; return(0);}
	
	while( a < ReturnStrLen )
	{
		if( ReturnStr[a++] == ':' )
		{
			if( b == v ) 
			{
				c=1;
				if( ReturnStr[a] == ':' ) st1[c++]=ReturnStr[a++];
				while( a < ReturnStrLen && ReturnStr[a] != ':' && c < 70)
				{
					st1[c++]=ReturnStr[a++];
				}
				st1[0] = c-1;
				Str255ToStr( st1, (char*)ReturnStr );
				ReturnStrLen = c-1;
				return(0);
			}
			else if( ReturnStr[a] == ':' ) a++;  //skip dir marker w/o cnting it
			b++;
		}
	}
	ReturnStrLen = 0;

	return(0);
}

short	func_instr(short rtype)
{
	long a, r=0, st1_len=0, b, g, startp=0;
	UCHAR	st1[3200];
	short ch, c, flag;
	long v[3];


	// Now get filename text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}
	else { ErrHandler( 1042, "\p in InStr function"); return(-1); }

	RStrToStr( ReturnStr, ReturnStrLen, st1, 3200);
	st1_len = ReturnStrLen;
	FreeReturnStr(); 

	// Now get prompt text string
	if( CodeBuf[CodePos] == 249)
	{
		CodePos++;
		get_stringarg_val(); // ignore using it for now
	}
	else { ErrHandler( 1042, "\p in InStr function"); return(-1); }

	for(c=0; c<2; c++)
	{
		switch( CodeBuf[CodePos++] )
		{
			case	251:
				v[c] =  get_longarg_val();
				break;
			case	250:
				v[c] =  (long) get_doublearg_val();
				break;
			default:
				ErrHandler( 1042, "\p in InStr function");
				return(-1);
				break;
		}
	}
	
	if( v[0] > 1 ) startp = v[0]-1;  // else it =0
	r=0; flag=1;
	if( st1_len > 0 && ReturnStrLen > 0 && st1_len <= ReturnStrLen && startp < ReturnStrLen)
	{
		ch = st1[0];
		if( !v[1] ) ch = toupper(ch);
		// Find first occ.
		for( a=startp;  a < ReturnStrLen; a++)
		{
			if( v[1] )
			{  // case sensitive
				if( ReturnStr[a] == ch ) // first char match
				{
					g = a; flag=0; b=0;
					while( b < st1_len && g < ReturnStrLen )
						if( ReturnStr[g++] != st1[b++] ) {flag=1; break;}
				}
				if( !flag ) break; 	
			}
			else
			{  // not case sensitive
				if( toupper(ReturnStr[a]) == ch ) // first char match
				{
					g = a; flag=0; b=0;
					while( b < st1_len && g < ReturnStrLen )
						if( toupper(ReturnStr[g++]) != toupper(st1[b++]) ) {flag=1; break;}
				}	
				if( !flag ) break; 	
			}
		}
	}
	FreeReturnStr();
	
	if( !flag ) r = a+1;
	
	switch(rtype)
	{
		case 1:
		case 2:
			Long_Function_Result = r;
			break;
		case 3:
		case 4:
			Double_Function_Result = (short double) r;
			break;
	}

	return(0);
}


