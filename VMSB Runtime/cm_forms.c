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
#include        <Palettes.h>
#include		<QDOffscreen.h>
#define         UCHAR   unsigned char
#define		    kInFront 	(WindowPtr) -1	/* put the new window in front */

#include	"Structs1.h"

short	ControlVisibleFlag = 1;

extern struct RunVarStruct // This Data Structure holds runtime info 3.0
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

extern struct RunHdrStruct // This Data Structure holds runtime info for 3.0
{       
	long    NumOfVars;
	long    NumOfCalls;
	long    NumOfLabels;
	long    StrLen;
	long    CodeLen;
	long    CodeStart;		//Where to start
	long	GlbVarSz;
	long	StartVarSz;
	long	CallTableOffset;	
	long	LabelTableOffset;
	long	StrTableOffset;
	long	SelectTableOffset;
	long	NumOfSelects;
	long	NumOfProps;   		// New for 3.0
	long	PropTableOffset;    // New for 3.0
} RunHdr;	

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

short GFX_CreateWin(short x1, short y1, short x2, short y2, short WinType, Str255, short);
short	CTL_DropList(short WinNum, short x1, short y1, short width, short cmd );
short	CTL_Button(short WinNum, short x1, short y1, short x2, short y2, char *, short );
short	CTL_CheckBox(short WinNum, short x1, short y1, short x2, short y2, char *, short );
short	CTL_Radio(short WinNum, short x1, short y1, short wide, short ht, short group, char* btext, short cmd   );
short	CTL_ScrollBar(short WinNum, short x1, short y1, short x2, short y2, short );
short	CTL_Textbox(short WinNum, short x1, short y1, short wide, short ht, short opts, short opts2, char* btext, short cmd );
short	CTL_List(short WinNum, short x1, short y1, short x2, short y2, short option, short cmd );
short	CTL_Label( short WinNum, short x1, short y1, short width, short height, char* btext, short cmd );
short	CTL_Picture( short WinNum, short ResID, short x1, short y1, short x2, short y2, short cmd  );
short 	CTL_LgIcon( short WinNum, short ResID, short x1, short y1, short width, short height, short cmd );
short	CTL_Timer( short WinNum, long interval, short enabled, short cmd );
short CTL_Disable( short a );

void Str255ToStr( Str255 st1, char * st2);
RGBColor 	GlobalBackRGB, GlobalForeRGB;	


short FORM_Create( short FormID );


short FORM_Create( short FormID )
{
	short 		a, b=-1, c[5], d, WinNum, winType, v1,m1,m2,m3,m4, button=-1, seeit =1;
	short		wtype=0, w, wt[]={ 4, 12, 0, 8, 4, 12, 0, 8, 1, 16, 2, 3 };
	Rect		r;
	char 		text1[260];
	TextStyle	tes;
	short		ts1, ts2;
	Point		theCell;
	Str255		text2;
	
	for( a=0; a<100;a++ ) if ( WinData[a].status && WinData[a].FormID == FormID ) return 0;  // check if already open


	for(a=0; a<RunHdr.NumOfProps; a++)
		if( RunVar.PropTable[a].FormID == FormID && RunVar.PropTable[a].CtlID==-1 ) { b=a; break; }

	if( b==-1) { return 0; } 

	c[0] = RunVar.PropTable[b].left;
	c[1] = RunVar.PropTable[b].top;
	c[2] = RunVar.PropTable[b].width;
	c[3] = RunVar.PropTable[b].height;
	w = RunVar.PropTable[b].value;
	wtype = wt[w-1];
	if( !RunVar.PropTable[b].visible ) seeit =0;
	wtype+=2000;  // Invisible window
	if( RunVar.PropTable[b].opt21 ) wtype+=4000;  // Center window
	if( (w>= 5 && w<=8 ) || w ==10 ) wtype+=1000;

	GlobalForeRGB = RunVar.PropTable[b].ForeRGB;
	GlobalBackRGB = RunVar.PropTable[b].BackRGB;

	WinNum = GFX_CreateWin( c[0], c[1], c[2], c[3], wtype, RunVar.PropTable[b].text, RunVar.PropTable[b].procID );
	if( WinNum ==-1) return 0; 
	
	WinData[WinNum].FormID = FormID;
	SelectWindow( WinData[WinNum].theWindow );
	SetPort( WinData[WinNum].theWindow );

	WinData[WinNum].MinMax.left = RunVar.PropTable[b].opt17;
	WinData[WinNum].MinMax.right = RunVar.PropTable[b].opt18;
	WinData[WinNum].MinMax.top = RunVar.PropTable[b].opt19;
	WinData[WinNum].MinMax.bottom = RunVar.PropTable[b].opt20;
	b++;
	
	button=-1;
	//Ok now create def button first
	for(a=b; a<RunHdr.NumOfProps; a++)
	{
		if( RunVar.PropTable[a].FormID == FormID && RunVar.PropTable[a].CtlID>=1000 )
		{
			if( RunVar.PropTable[a].CtlType==1 && RunVar.PropTable[a].value )  // def button
			{
				c[0] = RunVar.PropTable[a].left; 	c[1] = RunVar.PropTable[a].top;
				c[2] = RunVar.PropTable[a].width;	c[3] = RunVar.PropTable[a].height;
				Str255ToStr( RunVar.PropTable[a].text, text1);
				ControlVisibleFlag = RunVar.PropTable[a].visible;
				d = CTL_Button( WinNum, c[0], c[1], c[2], c[3], text1, RunVar.PropTable[a].procID );
				if( d>=0 )
				{
					CtlData[d].opt1 = RunVar.PropTable[a].value;
					CtlData[d].CtlID =  RunVar.PropTable[a].CtlID;
					if( !RunVar.PropTable[a].enabled ) CTL_Disable( d );

				}
				ControlVisibleFlag = 1;  // being sure to reset
				button = a;
				break;
			}
		}
		else break;
	}
	
	//Ok now create any controls that are defined
	for(a=b; a<RunHdr.NumOfProps; a++)
	{
		if( RunVar.PropTable[a].FormID == FormID && RunVar.PropTable[a].CtlID>=1000 )
		{
			c[0] = RunVar.PropTable[a].left;
			c[1] = RunVar.PropTable[a].top;
			c[2] = RunVar.PropTable[a].width; 
			c[3] = RunVar.PropTable[a].height;
			d = -1;
			switch( RunVar.PropTable[a].CtlType )
			{
				case 1: // Button
					if( button != a )
					{
						Str255ToStr( RunVar.PropTable[a].text, text1);
						ControlVisibleFlag = RunVar.PropTable[a].visible;
						d = CTL_Button( WinNum, c[0], c[1], c[2], c[3], text1, RunVar.PropTable[a].procID );
						if( d>=0 ) CtlData[d].opt1 = RunVar.PropTable[a].value;
					}
					break;
				case 2: // Label
					Str255ToStr( RunVar.PropTable[a].text, text1);
					ControlVisibleFlag = RunVar.PropTable[a].visible;
					RGBForeColor( &RunVar.PropTable[a].ForeRGB);
					RGBBackColor( &RunVar.PropTable[a].BackRGB);
					d = CTL_Label( WinNum, c[0], c[1], c[2], c[3], text1, RunVar.PropTable[a].procID );
					if( d>=0 )
					{
						CtlData[d].ForeRGB = RunVar.PropTable[a].ForeRGB;
						CtlData[d].BackRGB = RunVar.PropTable[a].BackRGB;
						TEGetStyle( 0, &tes, &ts1, &ts2, CtlData[d].te);
						GetFNum( RunVar.PropTable[a].fontname, &v1 );
						tes.tsFont = v1;
						tes.tsFace = RunVar.PropTable[a].fontstyle;
						tes.tsSize = RunVar.PropTable[a].fontsize;
						tes.tsColor = RunVar.PropTable[a].ForeRGB;
						TESetSelect( 0, 32767, CtlData[d].te );
						TESetStyle( 15, &tes, true, CtlData[d].te);
						TECalText( CtlData[d].te );
					  	TEAutoView( true, CtlData[d].te );
						TEUpdate( &CtlData[d].theRect, CtlData[d].te );
					}
					RGBForeColor( &GlobalForeRGB);
					RGBBackColor( &GlobalBackRGB);
					break;
				case 3: // Textbox
					Str255ToStr( RunVar.PropTable[a].text, text1);
					ControlVisibleFlag = RunVar.PropTable[a].visible;
					m1 = 0;
					if( RunVar.PropTable[a].opt14 ) m1=1;
					if( RunVar.PropTable[a].opt15 ) m1+=2;
					if( !RunVar.PropTable[a].opt21 ) m1+=4;
					RGBForeColor( &RunVar.PropTable[a].ForeRGB);
					RGBBackColor( &RunVar.PropTable[a].BackRGB);
					d = CTL_Textbox( WinNum, c[0], c[1], c[2], c[3], m1, RunVar.PropTable[a].value, text1, RunVar.PropTable[a].procID );
					if( d>=0 )
					{
						CtlData[d].opt1 = RunVar.PropTable[a].value; // single line
						CtlData[d].opt2 = RunVar.PropTable[a].opt21; // frame
						CtlData[d].ForeRGB = RunVar.PropTable[a].ForeRGB;
						CtlData[d].BackRGB = RunVar.PropTable[a].BackRGB;
						GetFNum( RunVar.PropTable[a].fontname, &v1 );
						tes.tsFont = v1;
						tes.tsFace = RunVar.PropTable[a].fontstyle;
						tes.tsSize = RunVar.PropTable[a].fontsize;
						tes.tsColor = RunVar.PropTable[a].ForeRGB;
						if( !text1[0] )	TESetText( " ", 1, CtlData[d].te );
						TESetSelect( 0, 32767, CtlData[d].te );
						TESetStyle( 15, &tes, false, CtlData[d].te);
						if( !text1[0] )	{ TEDelete( CtlData[d].te );}
						TECalText( CtlData[d].te );
					  	TEAutoView( true, CtlData[d].te );
						TESetSelect( 0, 0, CtlData[d].te );
						TEUpdate( &CtlData[d].theRect, CtlData[d].te );
					}
					RGBForeColor( &GlobalForeRGB);
					RGBBackColor( &GlobalBackRGB);
					break;
				case 4: // Icon
					ControlVisibleFlag = RunVar.PropTable[a].visible;
					d = CTL_LgIcon( WinNum,  c[0], c[1], RunVar.PropTable[a].width, RunVar.PropTable[a].height, RunVar.PropTable[a].value, RunVar.PropTable[a].procID );
					break;
				case 5: // Picture
					ControlVisibleFlag = RunVar.PropTable[a].visible;
					d = CTL_Picture( WinNum, c[0], c[1], c[2], c[3], RunVar.PropTable[a].value, RunVar.PropTable[a].procID );
					break;
				case 6: // CheckBox
					Str255ToStr( RunVar.PropTable[a].text, text1);
					ControlVisibleFlag = RunVar.PropTable[a].visible;
					d = CTL_CheckBox( WinNum, c[0], c[1], c[2], c[3],text1, RunVar.PropTable[a].procID );
					if( d>=0 )
					{
						if( RunVar.PropTable[a].value ) SetControlValue( CtlData[d].theControl, 1 );
					}
					break;
				case 7: // Radio
					Str255ToStr( RunVar.PropTable[a].text, text1);
					ControlVisibleFlag = RunVar.PropTable[a].visible;
					
					d = RunVar.PropTable[a].opt19;
					d = CTL_Radio( WinNum, c[0], c[1], c[2], c[3], RunVar.PropTable[a].opt19, text1, RunVar.PropTable[a].procID );
					if( d>=0 )
					{
						if( RunVar.PropTable[a].value ) SetControlValue( CtlData[d].theControl, 1 );
					}
					break;
				case 8: // DropList
					ControlVisibleFlag = RunVar.PropTable[a].visible;
					d  = CTL_DropList( WinNum, c[0], c[1], c[2], RunVar.PropTable[a].procID );
					if( d>=0 )
					{
						m1=1;   m2=1;  m3=1;  m4=0; 
						v1 = (long)  RunVar.PropTable[a].text[0];
						while(m1 <= v1)
						{
							if( RunVar.PropTable[a].text[m1] == '|' || m1 == v1)
							{
								if( RunVar.PropTable[a].text[m1] != '|' ) text2[m3++] =  RunVar.PropTable[a].text[m1++];
								text2[0] = m3-1;
								AppendMenu( CtlData[d].theMenu, text2 );
								m4++;
								m1++;
								m3=1;
							}
							else text2[m3++] =  RunVar.PropTable[a].text[m1++];
						}
						SetControlMinimum( CtlData[d].theControl, 0 );
						SetControlMaximum( CtlData[d].theControl, m4 );
						if( RunVar.PropTable[a].value ) SetControlValue( CtlData[d].theControl, RunVar.PropTable[a].value );
					}
					break;
				case 9: // ListBox
					ControlVisibleFlag = RunVar.PropTable[a].visible;
					RGBForeColor( &RunVar.PropTable[a].ForeRGB);
					RGBBackColor( &RunVar.PropTable[a].BackRGB);
					m1=0;
					if ( RunVar.PropTable[a].opt14 ) m1=1;
					if( !RunVar.PropTable[a].opt21) m1+=2;
					d = CTL_List( WinNum, c[0], c[1], c[2], c[3], m1, RunVar.PropTable[a].procID );
					if( d>=0 )
					{
						m1=1;   m2=1;  
						v1 = (long)  RunVar.PropTable[a].text[0];
						while(m1 <= v1)
						{
							if( RunVar.PropTable[a].text[m1] == '|' || m1 == v1)
							{
								m3 = LAddRow( 1, 30000, CtlData[d].lt);
								SetPt( &theCell, 0, m3);
								if( m1 == v1) m1++;
								LSetCell( RunVar.PropTable[a].text+m2, m1-m2, theCell, CtlData[d].lt);
								LDraw( theCell, CtlData[d].lt );
								m1++;
								m2 = m1;
							}
							else m1++;
						}
					}
					if( RunVar.PropTable[a].value >=1 )
					{
						SetPt( &theCell, 0, RunVar.PropTable[a].value-1);
						LSetSelect( true, theCell, CtlData[d].lt);
						LDraw( theCell, CtlData[d].lt );
					}
					RGBForeColor( &GlobalForeRGB);
					RGBBackColor( &GlobalBackRGB);
					break;
				case 10: // Timer new 3.0
					d = CTL_Timer( WinNum, RunVar.PropTable[a].value, RunVar.PropTable[a].enabled, RunVar.PropTable[a].procID );
					break;
				case 11: // ScrollBar
					ControlVisibleFlag = RunVar.PropTable[a].visible;
					d = CTL_ScrollBar( WinNum, c[0], c[1], c[2], c[3], RunVar.PropTable[a].procID );
					if( d>=0 )
					{
						SetControlMinimum( CtlData[d].theControl, RunVar.PropTable[a].opt17);
						SetControlMaximum( CtlData[d].theControl, RunVar.PropTable[a].opt18);
						SetControlValue( CtlData[d].theControl, RunVar.PropTable[a].value);
						CtlData[d].lastItem = RunVar.PropTable[a].opt19;

					}
					break;
			}
			if( d>=0 )
			{
				CtlData[d].CtlID =  RunVar.PropTable[a].CtlID;
				CtlData[d].BackRGB =  RunVar.PropTable[a].BackRGB;
				CtlData[d].ForeRGB =  RunVar.PropTable[a].ForeRGB;
				if( !RunVar.PropTable[a].enabled ) CTL_Disable( d );
			}
			ControlVisibleFlag = 1;  // being sure to reset

		}
		else break;
		
	}
	if( seeit ) ShowWindow( WinData[WinNum].theWindow );


}