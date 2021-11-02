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
#include		<SIOUX.h>
#define         UCHAR   unsigned char
#include	"Structs1.h"


unsigned char * FormPropsDesc[]={"\pName", "\pTitle","\pLeft", "\pTop", "\pWidth", "\pHeight",  "\pSub Proc.",
						 "\pVisible", "\pStyle", "\pEnabled", "\pFont", "\pFontSize", "\pFontStyle",
						 "\pShow Grid", "\pGrid Snap", "\pGrid Size",
						 "\pMin. Width", "\pMax. Width", "\pMin. Height", "\pMax. Height","\pAutoCenter",
						 "\pForeColor", "\pBackColor", };

unsigned char * CtlPropsDesc[]={"\pName", "\pText","\pLeft (x)", "\pTop (y)", "\pWidth", "\pHeight",  "\pSub Proc.",
						 "\pVisible", "\pValue", "\pEnabled", "\pFont", "\pFontSize", "\pFontStyle",
						 "\pScrollBar", "\pRead Only", "\pTab Index",
						 "\pMin. Value", "\pMax. Value", "\pPgUpDn", "\pOpt20","\pFrame",
						 "\pForeColor", "\pBackColor", };

unsigned char * objectNames[]={ "\pForm", "\pButton", "\pLabel", "\pTextBox", "\pIcon", "\pPicture",
								"\pCheckBox", "\pRadio", "\pDropList", "\pListBox", "\pTimer", "\pScrollBar"}; 

short	FormID = 1000, AControlID = 1000;

long extra_buf_pos, extra_line_ctr;
extern long source_len, buf_pos, line_ctr;
extern Str255	zparm1, zparm2;
extern struct ObjectPropsStruct1  *PropPtr1;    //global now
extern struct ObjectPropsStruct2  *PropPtr2;	 //global now
extern short pjt_ctr;

extern struct PjtFilesStruct
{
	Str63	fname;
	short	tp;  	// 1= form  2= module 3= resource
	long    codeStart;
	short	lineCtr;
	short	FormID;
} PjtFileList[200];

extern short nextCapFlag;
short next_line();
//short getspot_InPropTable();
short AddToPropTable( );
void zsplit2( );
short LoadFormContents( );
void StoreFormValue( short dd, short objecttype );
void Str255Copy ( Str255 st1, Str255 st2);
void Str63Copy( Str255 st1, Str255 st2);

short LoadFormContents( )
{

	short a, b, c, objnum, g=-1;
	char cbuf[512],t1[256];
	long bp=0;
	
	nextCapFlag = 1;
	a = next_line();
	zsplit2();
	//printf(">>[%s] [%s]\n", zparm1+1, zparm2+1 );
	if( !EqualString( zparm1, "\pFormBegin", false, false )) { 	nextCapFlag = 0; return(-1);  }
	// Get Prop spot
	objnum = AddToPropTable();
	if( objnum == -1 ) { nextCapFlag = 0; return(-1); }
	PjtFileList[pjt_ctr-1].FormID = FormID;  // for pass2 
	PropPtr1[objnum].filenum = pjt_ctr-1;
	PropPtr2[objnum].FormID = FormID++;
	PropPtr2[objnum].CtlID = -1;
	PropPtr2[objnum].CtlType = 0;
	//ControlID = 1000;
	
	while( !a )
	{
	    bp = buf_pos;
		a = next_line();
		zsplit2();
		//printf(">>[%s] [%s]\n", zparm1+1, zparm2+1 );
		if( EqualString( zparm1, "\pFormEnd", false, false )) a=1;
		else
		{
			StoreFormValue( objnum, 0 );  // Store stuff that is in zparm1, zparm2
			//getchar();
		}
	}
	//printf("Ok %i\n", objnum);
	//getchar();
	if( a==999 )
	{
			nextCapFlag = 0;
			return(0); 
	}

	
	extra_buf_pos = buf_pos;
	extra_line_ctr = line_ctr;
	bp = buf_pos;
	a = next_line();
	zsplit2();
	while( !a )
	{
		//printf(">>%s %s\n", zparm1+1, zparm2+1 );
		if( EqualString( zparm1, "\pControlBegin", false, false ))
		{
			g = -1;
			for( c=1; c<=11; c++ ) if( EqualString( zparm2, objectNames[c] , false, false)) { g = c; break; }
			if( g==-1) {nextCapFlag = 0; return -2; }

			// Get Prop spot
			objnum = AddToPropTable();
			if( objnum == -1 ) { nextCapFlag = 0; return -1;}
			PropPtr2[objnum].FormID = FormID-1;
			PropPtr2[objnum].CtlID = AControlID++;
			PropPtr2[objnum].CtlType = g;
			
			while( !a )
			{
				bp = buf_pos;
				a = next_line();
				zsplit2();
				//printf(">>%s %s\n", zparm1+1, zparm2+1 );
				if( EqualString( zparm1, "\pControlEnd", false, false )) a=6;
				else StoreFormValue( objnum, g);  // Store stuff that is in zparm1, zparm2
			}
			if( a==6 ) a=0; 
			if( a==999 ) { nextCapFlag = 0; return 0;}
			if( !a)
			{
				extra_buf_pos = buf_pos;
				extra_line_ctr = line_ctr;
				bp = buf_pos;
				a = next_line();
				zsplit2();
			}
			//printf(">>%s %s\n", zparm1+1, zparm2+1 );
		}
		else
		{
			//buf_pos = bp;
			//line_ctr--;
			a=1;
		}
	}
	
	//getchar();
	nextCapFlag = 0;
	return(0);
}

void StoreFormValue( short objnum, short objecttype )
{
	short propnum=-1,  v1, a, b, c,d, v2;
	Str255	text1;
	long		t;
	RGBColor	blue1;


	if( !objecttype )
	{
		for( v1=0; v1<=22; v1++ ) if( EqualString( zparm1, FormPropsDesc[v1] , false, false)) { propnum = v1; break; }
	}	
	else
	{
		for( v1=0; v1<=22; v1++ ) if( EqualString( zparm1, CtlPropsDesc[v1] , false, false)) { propnum = v1; break; }
	}	
		
	if( propnum == -1 ) return; 
		
	switch(propnum)
	{
		case 0:
			for( a=1; a<=zparm2[0]; a++ ) if( zparm2[a] >='a' && zparm2[a] <='z' ) zparm2[a] -=32;
			Str63Copy( zparm2, PropPtr1[objnum].name );
			break;
		case 1:
			Str255Copy( zparm2, PropPtr2[objnum].text );
			break;
		case 2:
			StringToNum( zparm2, &t);
			PropPtr2[objnum].left = (short) t;
			break;
		case 3:
			StringToNum( zparm2, &t);
			PropPtr2[objnum].top = (short) t;
			break;
		case 4:
			StringToNum( zparm2, &t);
			PropPtr2[objnum].width = (short) t;
			break;
		case 5:
			StringToNum( zparm2, &t);
			PropPtr2[objnum].height = (short) t;
			break;
		case 6:
			for( a=1; a<=zparm2[0]; a++ ) if( zparm2[a] >='a' && zparm2[a] <='z' ) zparm2[a] -=32;
			Str63Copy( zparm2, PropPtr1[objnum].proc );
			PropPtr2[objnum].procID = 0;
			break;
		case 7:
			StringToNum( zparm2, &t);
			PropPtr2[objnum].visible = (short) t;
			break;
		case 8:
			StringToNum( zparm2, &t);
			PropPtr2[objnum].value = (short) t;
			break;
		case 9:
			StringToNum( zparm2, &t);
			PropPtr2[objnum].enabled = (short) t;
			break;
		case 10:
			for( a=1; a<=zparm2[0]; a++ ) if( zparm2[a] >='a' && zparm2[a] <='z' ) zparm2[a] -=32;
			Str63Copy( zparm2, PropPtr2[objnum].fontname );
			break;
		case 11:
			StringToNum( zparm2, &t);
			PropPtr2[objnum].fontsize = (short) t;
			break;
		case 12:
			StringToNum( zparm2, &t);
			PropPtr2[objnum].fontstyle = (short) t;
			break;
		case 13:
			StringToNum( zparm2, &t);
			PropPtr2[objnum].opt14 = (short) t;
			break;
		case 14:
			StringToNum( zparm2, &t);
			PropPtr2[objnum].opt15 = (short) t;
			break;
		case 15:
			StringToNum( zparm2, &t);
			PropPtr2[objnum].opt16 = (short) t;
			break;
		case 16:
			StringToNum( zparm2, &t);
			PropPtr2[objnum].opt17 = (short) t;
			break;
		case 17:
			StringToNum( zparm2, &t);
			PropPtr2[objnum].opt18 = (short) t;
			break;
		case 18:
			StringToNum( zparm2, &t);
			PropPtr2[objnum].opt19 = (short) t;
			break;
		case 19:
			StringToNum( zparm2, &t);
			PropPtr2[objnum].opt20 = (short) t;
			break;
		case 20:
			StringToNum( zparm2, &t);
			PropPtr2[objnum].opt21 = (short) t;
			break;
		case 21:
			v1 = 1; v2=1;
			while( v1<= zparm2[0] && zparm2[v1] !=' ' ) text1[v2++] = zparm2[v1++];
			text1[0] = v2-1;
			StringToNum( text1, &t); blue1.red = (unsigned short) t;

			v1++; v2=1;
			while( v1<= zparm2[0] && zparm2[v1] !=' ' ) text1[v2++] = zparm2[v1++];
			text1[0] = v2-1;
			StringToNum( text1, &t); blue1.green = (unsigned short) t;

			v1++; v2=1;
			while( v1<= zparm2[0] && zparm2[v1] !=' ' ) text1[v2++] = zparm2[v1++];
			text1[0] = v2-1;
			StringToNum( text1, &t); blue1.blue = (unsigned short) t;

			PropPtr2[objnum].ForeRGB = blue1;
			break;
		case 22:
			v1 = 1; v2=1;
			while( v1<= zparm2[0] && zparm2[v1] !=' ' ) text1[v2++] = zparm2[v1++];
			text1[0] = v2-1;
			StringToNum( text1, &t); blue1.red = (unsigned short) t;

			v1++; v2=1;
			while( v1<= zparm2[0] && zparm2[v1] !=' ' ) text1[v2++] = zparm2[v1++];
			text1[0] = v2-1;
			StringToNum( text1, &t); blue1.green = (unsigned short) t;

			v1++; v2=1;
			while( v1<= zparm2[0] && zparm2[v1] !=' ' ) text1[v2++] = zparm2[v1++];
			text1[0] = v2-1;
			StringToNum( text1, &t); blue1.blue = (unsigned short) t;

			PropPtr2[objnum].BackRGB = blue1;
			break;

	}
}

