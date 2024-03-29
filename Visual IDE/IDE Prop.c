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

#include "IDE.h"
#include "Icons.h"
#include "ColorPicker.h"
#include <stdio.h>
#include <DCon.h>

WindowPtr		PropWin=nil;
ControlHandle	PropScroll=nil;
short 			lastProp =-1, numEntryFlag=0, nospaceEntryFlag=0;
short 			lastObjectType, lastObjNum;
MenuHandle		fontsMenu=nil;
extern short VDE_ResFileNum;
extern MenuHandle FormsSubMenu;
extern MenuHandle ModulesSubMenu;
extern MenuHandle ResourcesSubMenu;
extern FSSpec		PjtFS;

#define myscrollBarProc 16

extern short CurrentForm;
extern WindowPtr	FormWin[20];
extern struct	FormWins
{
	short		WinType;
	Rect		loc;
	short		ctlSelected;
	short		status;

} Forms[20];

void FormEditorCall( short ctlNum);
pascal void ScrollProp( ControlHandle theControl, short partCode );
void PropDisregard( );
short RecreateFormWindow( short c);
void ZapForm( short c );
void DrawPropWin();
short GetControlSelected( );
short GetControlNumSelected( );
void DisplayPropValue( short objecttype, short pnum);
void Str255ToStr( Str255 st1, char * st2);
void StrToStr255( char * st1, Str255 st2);
void EditPropValue( Point pt );
void Str63Copy ( Str255 st1, Str255 st2);
void Str255Copy ( Str255 st1, Str255 st2);
void PropDispose( );
void RedrawFormWin();
void ControlMove( short ct );
void ControlResize( short ct );
void SaveFormContents( short fnum, short formNum );
void WriteProps( short fnum, short objecttype, short objnum );
void GetPropStr( short pnum, short objecttype, short objnum );
short LoadFormContents( short formNum );
void ControlResize( short ct );
void StoreFormValue( short dd, short objecttype );

void zsplit();
short znext_line();
char *zbuffer = nil;
char  zline[512];
short zline_pos,zline_len;
long  zbuf_pos, zsource_len, ztstart;
Str255	zparm1, zparm2;
 
Str255	retPropStr;
extern Rect	MasterRect;

TEHandle	PropTE=nil;
extern TEHandle	ActiveTE;
Rect	PropTERect;

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
								"\pCheckBox", "\pRadio", "\pDropList", "\pListBox", "\pTimer", "\pScrollBar",}; 

// 1 = Textbox, 2 = True/False, 3 = On/Off, 4 = Color Picker, 5 = Sub Proc, 6=Font, 7 rotor
char	*objectdef[]={	"11111152106173311111144",   //Form
						"11111152120000000000000",	 //Command
						"11111152026110000000044",	 //Label
						"11111152126111100000344",	 //Textbox
						"10111152120000000000300",	 //Icon
						"10111152120000000000300",	 //Picture
						"11111152120000000000000",	 //Checkbox
						"11111152120000000010000",	 //Radio
						"11111152120000000000000",	 //DropList
						"11111152120001000000344",	 //ListBox
						"10111150120000000000000",	 //Timer
						"10111152120000001110000",	 //ScrollBar
						
};

struct ObjectPropsStruct
{
	Str63	name;
	Str255	text;
	short	left, top, width, height;
	Str63	proc;
	short	visible, value, enabled;
	Str63	fontname;
	short	fontsize, fontstyle;
	short	opt14, opt15, opt16, opt17, opt18, opt19, opt20, opt21;
	RGBColor BackRGB, ForeRGB;	
} ObjectProps[200], FormProps[20];

extern struct IntControlStruct
{
   	ControlHandle	ctl;
   	TEHandle		te;
   	ListHandle		lt;			// Thats a lowercase L in lt
	PicHandle		pic;
	Rect			theRect;
   	short			WinNum;
	short			status;
	short			subcode;
	MenuHandle		theMenu;
	short			lastItem;
} CtlData[200];   

extern struct	EditWinStruct  // 0-19 for forms - 20-99 reserved for modules
{
	Str63			FileName;
	WindowPtr		theWindow;
   	TEHandle		te;
   	ControlHandle	theControl;		//scrollbar
   	ControlHandle	dropList;		//dropList
	MenuHandle		theMenu;
	short			status;
	Rect			theRect;
	short			ht;
	short			FormNum;
	short			change;
	short			editFlag;   // 1=New Object // 2=New Ind. Proc  // 3 existing proc
	short			currentProc;
	FSSpec			fs;
	Handle			GlobalVar;
	long			GlobalVarSize;
}
EditWin[100];


// Create a properties window
short CreatePropWindow()
{
	short			a,b=-1,err, g;
	Rect 			tempRect, tr, theRect;
	RGBColor		rgb;
	Pattern		thePat;
	WindowPtr		window;
	
	if( PropWin != nil )
	{
		SelectWindow( PropWin );
		SetPort( PropWin );
		EraseRect( &PropWin->portRect);
		InvalRect( &PropWin->portRect);
	 	return;
	}

	// Now let's create the window itself
	SetRect(&tempRect,460,40,620,400);
	
	PropWin = NewCWindow( nil, &tempRect, "\pProperties", true,/*documentProc*/floatGrowProc, (WindowPtr) (-1), true, 0); 
	if( PropWin == nil ) return;
	
	// Now let's fill it's busty innards
	SelectWindow( PropWin );
	SetPort( PropWin );

	OffsetRect( &tempRect, -tempRect.left, -tempRect.top);
	tempRect.left = tempRect.right-15;
	tempRect.bottom -=15;
	PropScroll = NewControl( PropWin, &tempRect, "\p", true,0,0,1, 16,0); 

	theRect = PropWin->portRect; 
	OffsetRect( &theRect, -theRect.left, -theRect.top);
	theRect.top =0; theRect.bottom-=13;
	theRect.left = theRect.right-14;
	MoveControl( PropScroll, theRect.left, theRect.top);
	SizeControl( PropScroll, 16, theRect.bottom - theRect.top);


	SetControlMinimum(PropScroll, 1);
	SetControlMaximum(PropScroll, 1);
	SetControlValue(PropScroll, 1);
	return;

}

short UpdatePropWin( WindowPtr window )			
{
	short 		a,b,c;
	GrafPtr		savePort;
	Rect		theRect, tempRect;
	Pattern		thePat;
	RGBColor	rgb, oldrgb;
	//WindowPtr	oldWindow;
	
	if( PropWin == window )
	{
	
		//oldWindow = FrontWindow();
		GetPort( &savePort );
		
		//SetPortWindowPort( window );
		SetPort (window );
		BeginUpdate(window);
			theRect = window->portRect;
			OffsetRect( &theRect, -theRect.left, -theRect.top);
			EraseRect( &theRect );
			
			if( CurrentForm >= 0 ) DrawPropWin();

			DrawGrowIcon( window );
			DrawControls(window);
		EndUpdate(window);
		// restore the old graphics port
		SetPort( savePort );
		//if( oldWindow!=nil ) SelectWindow( oldWindow);
		return 1;
	}
	return 0; // No, a form window didn't request an update
}

short PropEvent( Point pt )
{
	short	b, c, ct, dt, et;
	Rect	theRect, tr;
	WindowPtr window;
	ControlHandle	theControl;

	#ifdef	powerc
     ControlActionUPP	scrollPropUPP;
     scrollPropUPP = NewControlActionProc( ScrollProp );         
	#endif
	if( PropWin==nil || CurrentForm==-1) return;
	window = PropWin;
	SelectWindow( PropWin );
	SetPort( PropWin );
	theRect = window->portRect;  //EditWin[a].theRect;
	OffsetRect( &theRect, -theRect.left, -theRect.top );
	theRect.right -=15;
	theRect.bottom -=15;
	GlobalToLocal( &pt);
	if( PtInRect( pt, &theRect ) )
	{
		EditPropValue( pt );
		//SetPort( PropWin );
  		//EraseRect( &PropWin->portRect);
		//InvalRect( &PropWin->portRect);
		return(0);
	}
	PropDispose( );

    ct = FindControl( pt, window, &theControl );
    if( ct )
	{
		et = GetControlValue(theControl);
		switch( ct )
		{
			case 129: //inThumb:
				dt = TrackControl( theControl, pt, nil );
				break;
			/*case 20: //inUpButton:
				dt = TrackControl( theControl, pt, nil );
				SetControlValue(theControl, et-1);
				break;
			case 21: //inDownButton:
				dt = TrackControl( theControl, pt, nil );
				SetControlValue(theControl, et+1);
				break;
			case 22: //inPageUp:
				dt = TrackControl( theControl, pt, nil );
				SetControlValue(theControl, et-5);
				break;*/
			case 20:
			case 21:
			case 22:
			case 23: //inPageDown:
				#ifdef powerc
					dt = TrackControl( theControl, pt, scrollPropUPP );
				#else
					dt = TrackControl( theControl, pt, ScrollProp );
				#endif
				//dt = TrackControl( theControl, pt, nil );
				//SetControlValue(theControl, et+5);
				break;
		}
		if ( et != GetControlValue(theControl))
		{
			if( PropWin !=nil )
			{
				SetPort( PropWin );
				EraseRect( &PropWin->portRect);
				InvalRect( &PropWin->portRect);
			}
		}
	}
	return 0;
	
}

void DrawPropWin()
{
	short 		a, x,y,ww,wht, cc, numOfProps=1, numOfLines, diff, v1;
	PenState	ps;
	Pattern		thePat;
	Rect		theRect, tr,er;
	RGBColor	rgb;
	
	if( PropWin==nil ) return;
	//SelectWindow( PropWin );
	//SetPort( PropWin );
	theRect = PropWin->portRect;
	OffsetRect( &theRect, -theRect.left, -theRect.top );
	a = theRect.right - 82;
	SetRect( &er, a,1, a+32, 32);

	cc =  GetControlSelected( );  // cc = current select control if one
	if( cc == -1 ) cc = 0; // form excep

	theRect.right-=15;
	theRect.bottom-=15;
	a = GetControlMaximum(PropScroll);
	if( a == 1 )
	{

		// Setting up scroll bar
		for( a=0; a<=22; a++ ) if( objectdef[cc][a] > '0' ) numOfProps++;

		numOfLines = (theRect.bottom-18) / 18;  //  equals whole lines that can be seen

		diff = numOfProps - numOfLines;
		if( diff <= 0 )
		{ diff = 0;}
		else
		{
			SetControlMaximum(PropScroll, diff); //+1);  rmh
			SetControlValue(PropScroll, 1);
		}
	}	
	tr = theRect;
	a = tr.right/2;
	if( a > 90 ) a=90;
	tr.right = a;
	ww = theRect.right;
	wht = theRect.bottom;
	ClipRect( &theRect);
	
	GetPenState( &ps );
	TextFont(3);
	TextSize(12);
	GetIndPattern( &thePat, 0, 4);
	
	v1 = GetControlValue(PropScroll);
	y=18;
	if( v1 ==1 ) 
	{
		PenNormal();
		TextFace(1);
		rgb.red = 0;  rgb.green = 0; rgb.blue =0xd400;
		RGBForeColor( &rgb );
		MoveTo( 4, y-4);
		DrawString( objectNames[cc]);
		rgb.red = 0;  rgb.green = 0; rgb.blue =0;
		RGBForeColor( &rgb );
	   	if( !cc )
	   	{
	   		SetResLoad(true);
			PlotIconID(&er, atTop, ttNone, 133);
			OffsetRect( &er, 32, 0);
			PlotIconID(&er, atTop, ttNone, 134);
		}
		TextFace(0);
		PenPat( & thePat );
		MoveTo( 0, y);
		LineTo( ww, y);
		y+=18;
	}

	for( a=v1-1; a<=22; a++ )
	{
		if( objectdef[cc][a] >= '1' )
		{
			PenNormal();
			MoveTo( 4, y-4);
			TextFace(1);
			ClipRect( &tr);
			if( !cc ) DrawString( FormPropsDesc[a]);
			else
			{
				if( a == 18 && cc == 7 ) 	DrawString( "\pGroupID");
				else if( a == 13 && cc == 9 ) 	DrawString( "\pMultiSel.");
				else if( a == 8 )
				{
					switch( cc )
					{
					  case 1:
					  	DrawString( "\pDefault");
					  	break;
					  case 3:
					  	DrawString( "\pSingle Line");
					  	break;
					  case 10: // Timer
					  	DrawString( "\pInterval");
					  	break;
					  default:
					  	DrawString( CtlPropsDesc[a]);
					  	break;
					}				  
				}
				else DrawString( CtlPropsDesc[a]);
			}	
			
			ClipRect( &theRect);
			TextFace(0);
			MoveTo( tr.right+4, y-4);
			DisplayPropValue( cc, a);
			
			PenPat( & thePat );
			MoveTo( 0, y);
			LineTo( ww, y);
			y+=18;
		}
	}
	if( v1==1) MoveTo( tr.right, 18);
	else MoveTo( tr.right, 0);
	LineTo( tr.right, wht);

	theRect = PropWin->portRect;
	OffsetRect( &theRect, -theRect.left, -theRect.top );
	ClipRect( &theRect);
	
	
	SetPenState( &ps );
	
}

short DoPropGrow( Point hitPt, WindowRef window )
{
	short	a, b;
	Rect sizeRect,theRect, tr;
	long newSize;
	GrafPtr		savePort;

	if( window != PropWin ) return 0;
	
	PropDispose( );
	GetPort( &savePort );
	SetPortWindowPort( window );

	SetRect( &sizeRect, 50, 50, 2000, 2000 ); //kMinWindowWidth, kMinWindowHeight, SHRT_MAX, SHRT_MAX );
	newSize = GrowWindow( window, hitPt, &sizeRect ) ;	
	if( !newSize ) { SetPort( savePort ); return; }

	//	for some reason, GrowWindow( ) returns a long value,
	theRect = window->portRect;  //EditWin[a].theRect;
	theRect.right = theRect.left +  LoWord( newSize );
	theRect.bottom = theRect.top +  HiWord( newSize );

	SizeWindow( window, LoWord(newSize), HiWord( newSize ), true );
   	// Now adjust textbox
	theRect = window->portRect;  //EditWin[a].theRect;

	
	OffsetRect( &theRect, -theRect.left, -theRect.top);
	theRect.top =0; theRect.bottom-=15;
	theRect.left = theRect.right-15;
	MoveControl( PropScroll, theRect.left, theRect.top);
	SizeControl( PropScroll, 16, theRect.bottom - theRect.top);

	SetControlMaximum(PropScroll, 1);
	SetControlValue(PropScroll, 1);
  	EraseRect( &window->portRect);
	InvalRect( &window->portRect);
	//DrawGrowIcon( window );

	SetPort( savePort );
	return 1;
}

void DisplayPropValue( short objecttype, short pnum)
{
	short	objnum, v1;
	Str255	text1;
	char	text2[256];
	RGBColor	black1, red1, blue1;
	Rect		tr;
	Point		pt;
	Pattern		pat;
	
	black1.red = 0;			black1.green = 0;		black1.blue = 0;
	red1.red = 0xDD6B;		red1.green = 0X08C2;	red1.blue = 0X06A2;
	blue1.red = 0;			blue1.green = 0;		blue1.blue = 0xD400;
	
	objnum = GetControlNumSelected( );
	switch(pnum)
	{
		case 0:
			if( !objecttype ) DrawString( FormProps[CurrentForm].name ); 
			else DrawString( ObjectProps[objnum].name ); 
			break;
		case 1:
			if( !objecttype ) DrawString( FormProps[CurrentForm].text ); 
			else DrawString( ObjectProps[objnum].text ); 
			break;
		case 2:
			if( !objecttype ) NumToString( FormProps[CurrentForm].left, text1); 
			else NumToString( ObjectProps[objnum].left, text1); 
			DrawString( text1 ); 
			break;
		case 3:
			if( !objecttype ) NumToString( FormProps[CurrentForm].top, text1); 
			else NumToString( ObjectProps[objnum].top, text1); 
			DrawString( text1 ); 
			break;
		case 4:
			if( !objecttype ) NumToString( FormProps[CurrentForm].width, text1); 
			else NumToString( ObjectProps[objnum].width, text1); 
			DrawString( text1 ); 
			break;
		case 5:
			if( !objecttype ) NumToString( FormProps[CurrentForm].height, text1); 
			else NumToString( ObjectProps[objnum].height, text1); 
			DrawString( text1 ); 
			break;
		case 6:
			if( !objecttype ) DrawString( FormProps[CurrentForm].proc ); 
			else DrawString( ObjectProps[objnum].proc ); 
			break;
		case 7:
			if( !objecttype ) v1 =  FormProps[CurrentForm].visible; 
			else v1 = ObjectProps[objnum].visible; 
			if( !v1 ) { RGBForeColor( &red1); DrawString( "\pFalse" ); }
			else { RGBForeColor( &blue1); DrawString( "\pTrue" ); }
			RGBForeColor( &black1);
			break;
		case 8:
			if( !objecttype )
			{
				switch( FormProps[CurrentForm].value)
				{
					case 1:
						Str255Copy( "\pDocument Window", text1);
						break;
					case 2: 
						Str255Copy( "\pDocument Window with Zoombox", text1);
						break;
					case 3:
						Str255Copy( "\pDocument Window with Sizebox", text1);
						break;
					case 4:
						Str255Copy( "\pDocument Window with Zoombox & Sizebox", text1);
						break;
					case 5:
						Str255Copy( "\pDocument Window with Closebox", text1);
						break;
					case 6:
						Str255Copy( "\pDocument Window with Closebox & ZoomBox", text1);
						break;
					case 7:
						Str255Copy( "\pDocument Window with Closebox & SizeBox", text1);
						break;
					case 8:
						Str255Copy( "\pDocument Window with Closebox, Zoombox & SizeBox", text1);
						break;
					case 9:
						Str255Copy( "\pDialog Box Window", text1);
						break;
					case 10:
						Str255Copy( "\pAccessory Window", text1);
						break;
					case 11:
						Str255Copy( "\pPlain Box Window", text1);
						break;
					case 12:
						Str255Copy( "\pPlain Box Window with shadow", text1);
						break;
				}
				DrawString( text1 ); 
			}
			else
			{
				if( objecttype ==1 || objecttype == 3)
				{
					v1 = ObjectProps[objnum].value; 
					if( !v1 ) { RGBForeColor( &red1); DrawString( "\pFalse" ); }
					else { RGBForeColor( &blue1); DrawString( "\pTrue" ); }
					RGBForeColor( &black1);
				}
				else if( objecttype ==6 || objecttype == 7)
				{
					v1 = ObjectProps[objnum].value; 
					if( !v1 ) { RGBForeColor( &red1); DrawString( "\pOff" ); }
					else { RGBForeColor( &blue1); DrawString( "\pOn" ); }
					RGBForeColor( &black1);
				}
				else
				{
			 		NumToString( ObjectProps[objnum].value, text1); 
					DrawString( text1 ); 
				}
			}
			break;
		case 9:
			if( !objecttype ) v1 =  FormProps[CurrentForm].enabled; 
			else v1 = ObjectProps[objnum].enabled; 
			if( !v1 ) { RGBForeColor( &red1); DrawString( "\pFalse" ); }
			else { RGBForeColor( &blue1); DrawString( "\pTrue" ); }
			RGBForeColor( &black1);
			break;
		case 10:
			if( !objecttype ) DrawString( FormProps[CurrentForm].fontname ); 
			else DrawString( ObjectProps[objnum].fontname ); 
			break;
		case 11:
			if( !objecttype ) NumToString( FormProps[CurrentForm].fontsize, text1); 
			else NumToString( ObjectProps[objnum].fontsize, text1); 
			DrawString( text1 ); 
			break;
		case 12:
			if( !objecttype ) v1= FormProps[CurrentForm].fontstyle; 
			else v1 = ObjectProps[objnum].fontstyle; 
			switch( v1 )
			{
				case 0:
					DrawString( "\pNormal" ); 
					break;
				case 1:
					DrawString( "\pBold" ); 
					break;
				case 2:
					DrawString( "\pItalic" ); 
					break;
				case 3:
					DrawString( "\pBold Italic" ); 
					break;
				case 4:
					DrawString( "\pUnderlined" ); 
					break;
				case 5:
					DrawString( "\pBold Underlined" ); 
					break;
				case 6:
					DrawString( "\pItalic Underlined" ); 
					break;
				case 7:
					DrawString( "\pBold Italic Underlined" ); 
					break;
			}
			break;
		case 13:
			if( !objecttype ) v1 =  FormProps[CurrentForm].opt14; 
			else v1 = ObjectProps[objnum].opt14; 
			if( !v1 ) { RGBForeColor( &red1); DrawString( "\pFalse" ); }
			else { RGBForeColor( &blue1); DrawString( "\pTrue" ); }
			RGBForeColor( &black1);
			break;
		case 14:
			if( !objecttype ) v1 =  FormProps[CurrentForm].opt15; 
			else v1 = ObjectProps[objnum].opt15; 
			if( !v1 ) { RGBForeColor( &red1); DrawString( "\pFalse" ); }
			else { RGBForeColor( &blue1); DrawString( "\pTrue" ); }
			RGBForeColor( &black1);
			break;
		case 15:
			if( !objecttype ) NumToString( FormProps[CurrentForm].opt16, text1); 
			else NumToString( ObjectProps[objnum].opt16, text1); 
			DrawString( text1 ); 
			break;
		case 16:
			if( !objecttype ) NumToString( FormProps[CurrentForm].opt17, text1); 
			else NumToString( ObjectProps[objnum].opt17, text1); 
			DrawString( text1 ); 
			break;
		case 17:
			if( !objecttype ) NumToString( FormProps[CurrentForm].opt18, text1); 
			else NumToString( ObjectProps[objnum].opt18, text1); 
			DrawString( text1 ); 
			break;
		case 18:
			if( !objecttype ) NumToString( FormProps[CurrentForm].opt19, text1); 
			else NumToString( ObjectProps[objnum].opt19, text1); 
			DrawString( text1 ); 
			break;
		case 19:
			if( !objecttype ) NumToString( FormProps[CurrentForm].opt20, text1); 
			else NumToString( ObjectProps[objnum].opt20, text1); 
			DrawString( text1 ); 
			break;
		case 20:
			if( !objecttype ) v1 =  FormProps[CurrentForm].opt21; 
			else v1 = ObjectProps[objnum].opt21; 
			if( !v1 ) { RGBForeColor( &red1); DrawString( "\pFalse" ); }
			else { RGBForeColor( &blue1); DrawString( "\pTrue" ); }
			RGBForeColor( &black1);
			break;
		case 21:
			if( !objecttype ) blue1 =  FormProps[CurrentForm].ForeRGB;
			else blue1 =  ObjectProps[objnum].ForeRGB;
			GetPen ( &pt );
			tr.left = pt.h; 			tr.right = pt.h+100;
			tr.top = pt.v-10; 			tr.bottom = pt.v;
			GetIndPattern( &pat, 0, 1);
			RGBForeColor( &blue1);
			FillRect( &tr , &pat);
			RGBForeColor( &black1);
			break;
		case 22:
			if( !objecttype ) blue1 =  FormProps[CurrentForm].BackRGB;
			else blue1 =  ObjectProps[objnum].BackRGB;
			GetPen ( &pt );
			tr.left = pt.h; 			tr.right = pt.h+100;
			tr.top = pt.v-10; 			tr.bottom = pt.v;
			GetIndPattern( &pat, 0, 1);
			RGBForeColor( &blue1);
			FillRect( &tr, &pat );
			RGBForeColor( &black1);
			break;


	}
}

void AddMyControl( short ctlNum, short ctlType, short formNum )
{
	short a, c, max=0, b;
	Str255	text1;
	Rect	tempRect;
	char	text2[256], text3[256];
	long	lv;
	Point	theCell;
	RGBColor	rgb;
	
	// Find last control #
	for( a=0; a<200; a++)
	{
		if( a !=ctlNum && CtlData[a].status == ctlType )
		{ 
			 if( CtlData[a].WinNum == formNum )
			{
				if( max <= ObjectProps[a].opt20 ) max = ObjectProps[a].opt20;
			}
		}
	}
	
	EditWin[formNum].change = 1;
	max++;
	Str255ToStr( objectNames[ctlType], text3);
	sprintf( text2, "%s%i", text3, max);
	StrToStr255( text2, text1);

	
	c = ctlNum;
	tempRect = CtlData[c].theRect;
	
	Str255Copy ( text1, ObjectProps[c].name);
	Str255Copy ( text1, ObjectProps[c].text);
	ObjectProps[c].left = tempRect.left;
	ObjectProps[c].top = tempRect.top;
	ObjectProps[c].width = tempRect.right - tempRect.left;
	ObjectProps[c].height = tempRect.bottom - tempRect.top;
	Str255Copy ( "\pNone", ObjectProps[c].proc);
	ObjectProps[c].visible = 1;
	ObjectProps[c].value = 0;
	ObjectProps[c].enabled = 1;
	ObjectProps[c].visible = 1;
	Str255Copy ( "\pGeneva", ObjectProps[c].fontname);
	ObjectProps[c].fontsize = 12;
	ObjectProps[c].fontstyle = 0;
	ObjectProps[c].opt14=0;
	ObjectProps[c].opt15=0;
	ObjectProps[c].opt16= max;		
	ObjectProps[c].opt17=1;
	ObjectProps[c].opt18=10;
	ObjectProps[c].opt19 =0;
	ObjectProps[c].opt20 = max;
	ObjectProps[c].opt21 = 0;
	rgb.red = 0xffff;	rgb.green = 0xffff;		rgb.blue = 0xffff;  ///white
	ObjectProps[c].BackRGB = rgb;
	rgb.red = 0;	rgb.green = 0;		rgb.blue = 0;  ///black
	ObjectProps[c].ForeRGB = rgb;
	
	switch( ctlType )
	{

		case 11:
			SetControlMinimum( CtlData[c].ctl, 1 );
			SetControlMaximum( CtlData[c].ctl, 10 );
			SetControlValue( CtlData[c].ctl, 1 );
			ObjectProps[c].value = 1;
			ObjectProps[c].opt19 = 3;
			break;
		case 1:
		case 6:
		case 7:
			SetControlTitle( CtlData[c].ctl, text1);
			ObjectProps[c].ForeRGB = FormProps[formNum].ForeRGB;  //Use same fg as form
			ObjectProps[c].BackRGB = FormProps[formNum].BackRGB;  //Use same bg as form
			if( ctlType ==7 ) ObjectProps[c].opt19 = 1;
			break;
		case 8:
			SetControlTitle( CtlData[c].ctl, text1);
			AppendMenu( CtlData[c].theMenu, text1);
			SetControlMinimum( CtlData[c].ctl, 1 );
			SetControlMaximum( CtlData[c].ctl, 2);
			SetControlValue( CtlData[c].ctl, 1 );
			InvalRect( &CtlData[c].theRect );
			break;
		case 2:
			//ObjectProps[c].BackRGB = FormProps[formNum].BackRGB;  //Use same bg as form
			lv = (long) text1[0];
			TESetText( text1+1, lv, CtlData[c].te);
			//EraseRect ( &CtlData[c].theRect );
			TEUpdate( &CtlData[c].theRect, CtlData[c].te );
			InvalRect ( &CtlData[c].theRect );
			ObjectProps[c].ForeRGB = FormProps[formNum].ForeRGB;  //Use same fg as form
			ObjectProps[c].BackRGB = FormProps[formNum].BackRGB;
			break;
		case 3:
			lv = (long) text1[0];
			TESetText( text1+1, lv, CtlData[c].te);
			EraseRect ( &CtlData[c].theRect );
			TEUpdate( &CtlData[c].theRect, CtlData[c].te );
			InvalRect ( &CtlData[c].theRect );
			ObjectProps[c].opt21 = 1;
			ObjectProps[c].value = 1; //Single line flag set on default
			break;
		case 4:
		case 5:
			CtlData[c].pic = nil;
			DrawString(text1);
			break;
		case 10:
			ObjectProps[c].value = 3600;
			break;
		case 9:
			ObjectProps[c].opt21 = 1;
			lv = (long) text1[0];
			b = LAddRow( 1, 30000, CtlData[c].lt);
			SetPt( &theCell, 0, b);
			LSetCell( text1+1, lv, theCell, CtlData[c].lt);
			LSetSelect( true, theCell, CtlData[c].lt);
			LDraw( theCell, CtlData[c].lt );
			//InvalRect( &CtlData[c].theRect );
			/*b = LAddRow( 1, 30000, CtlData[c].lt);*/
			ObjectProps[c].value = 1;
			//SetPt( &theCell, 0, 1);
			//LSetCell( text1+1, lv, theCell, CtlData[c].lt);
			//LDraw( theCell, CtlData[c].lt );
			InvalRect ( &CtlData[c].theRect );
			break;
	}
	
	
}

void EditPropValue( Point pt )  // Point is already local
{
	short	a, objnum, v1, SCRoffset=0, propSel, pnum =0, objecttype=0, ctr=0, cc=0;
	long	tl;
	Str255	text1;
	unsigned char	text2[256], *cptr;
	RGBColor	black1, red1, blue1;
	Rect		tr, theRect, tr2, tr3;
	Point		pt1, pt2 = {50,30};
	Pattern		pat;
	TextStyle	tes;
	short		ts1, ts2, xx1,xx2;
		
	if( PropWin ==nil ) return;
	if( CurrentForm == -1 ) return;
	
	objecttype =  GetControlSelected( );  // cc = current select control if one

	if( objecttype <= 0 )
	{	// Click in view code button?
		theRect = PropWin->portRect;
		OffsetRect( &theRect, -theRect.left, -theRect.top );
		a = theRect.right - 82;
		SetRect( &tr, a,1, a+64, 20);
		if( PtInRect( pt, &tr ) )
		{
			SetRect( &tr, a,1, a+32, 32);
	   		SetResLoad(true);
			PlotIconID(&tr, atTop, ttNone, 135);
			OffsetRect( &tr, 32, 0);
			PlotIconID(&tr, atTop, ttNone, 136);
			while( StillDown() );
			OffsetRect( &tr, -32, 0);
			PlotIconID(&tr, atTop, ttNone, 133);
			OffsetRect( &tr, 32, 0);
			PlotIconID(&tr, atTop, ttNone, 134);
			FormEditorCall( -2 );
			return;
		}
	}
	
	a = GetControlMaximum(PropScroll);
	if( a > 1 )
	{
			a = GetControlValue(PropScroll);
			if( a > 1 )	SCRoffset = 18 + ((a-1) *18);
	}
	
	if( objecttype == -1 ) objecttype = 0; // form excep
	for( a=0; a<=22; a++ ) if( objectdef[objecttype][a] >= '1' ) ctr++;

	propSel = ((SCRoffset+pt.v) / 18);
	if( !propSel || propSel >ctr ) { PropDispose( ); return; }
	propSel--;

	theRect = PropWin->portRect;
	OffsetRect( &theRect, -theRect.left, -theRect.top );
	theRect.right-=15; 	theRect.bottom-=15;
	a = theRect.right/2;
	if( a > 90 ) a=90;
	a+=3;
	tr = theRect;  tr.left = a;
	tr.top = 20 + (propSel*18) - SCRoffset; 
	tr.bottom = tr.top + 16; 
	tr.right -=1;
	tr2 = tr;  tr2.right = 2000;
	//FrameRect( &tr );

	black1.red = 0;			black1.green = 0;		black1.blue = 0;
	red1.red = 0xDD6B;		red1.green = 0X08C2;	red1.blue = 0X06A2;
	blue1.red = 0;			blue1.green = 0;		blue1.blue = 0xD400;
	
	objnum = GetControlNumSelected( );
	
	if( PropTE != nil )
	{
		if( PtInRect( pt, &PropTERect ) )
		{
			TEClick( pt, false, PropTE);
			return; 
		}
		
		else PropDispose( );
	}
	PropTERect = tr;
	
	lastObjectType = objecttype;
	lastObjNum = objnum;
	numEntryFlag=0;	 nospaceEntryFlag=0;
	
	ctr=0;
	for( a=0; a<=22; a++ )
	{
		if( objectdef[objecttype][a] >= '1' ) 
		{
			if( ctr == propSel) { propSel=a; break; }
			else ctr++;
		}
	}
	if( CurrentForm <0 ) {SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);}	EditWin[CurrentForm].change = 1;
	lastProp = propSel;

	switch(propSel)
	{
		case 0:
			PropTE = TENew( &tr2, &tr );
			if( !objecttype ) cptr =  FormProps[CurrentForm].name; 
			else cptr = ObjectProps[objnum].name;
			nospaceEntryFlag++;
			break;
		case 1:
			PropTE = TENew( &tr2, &tr );
			if( !objecttype ) cptr =  FormProps[CurrentForm].text; 
			else cptr = ObjectProps[objnum].text; 
			break;
		case 2:
			PropTE = TENew( &tr2, &tr );
			if( !objecttype ) NumToString( FormProps[CurrentForm].left, text1 ); 
			else NumToString( ObjectProps[objnum].left, text1 );
			cptr = text1;  
			numEntryFlag++;
			break;
		case 3:
			PropTE = TENew( &tr2, &tr );
			if( !objecttype ) NumToString( FormProps[CurrentForm].top, text1 ); 
			else NumToString( ObjectProps[objnum].top, text1 );
			cptr = text1;  
			numEntryFlag++;
			break;
		case 4:
			PropTE = TENew( &tr2, &tr );
			if( !objecttype ) NumToString( FormProps[CurrentForm].width, text1 ); 
			else NumToString( ObjectProps[objnum].width, text1 );
			cptr = text1;  
			numEntryFlag++;
			break;
		case 5:
			PropTE = TENew( &tr2, &tr );
			if( !objecttype ) NumToString( FormProps[CurrentForm].height, text1 ); 
			else NumToString( ObjectProps[objnum].height, text1 );
			cptr = text1;  
			numEntryFlag++;
			break;
		case 6: // Proc
			PropTE = TENew( &tr2, &tr );
			if( !objecttype ) cptr =  FormProps[CurrentForm].proc; 
			else cptr = ObjectProps[objnum].proc;
			nospaceEntryFlag++;
			break;
		case 7:
			if( !objecttype )
			{
			    if( FormProps[CurrentForm].visible ) FormProps[CurrentForm].visible = 0;
			    else  FormProps[CurrentForm].visible = 1;
			 }
			else
			{
				if( ObjectProps[objnum].visible )  ObjectProps[objnum].visible = 0;
				else  ObjectProps[objnum].visible = 1;
			}
		 	EraseRect( &tr );
			InvalRect( &tr );
			break;
		case 8: // WindowType & value
			if( !objecttype ) 
			{
				ctr=FormProps[CurrentForm].value;
				fontsMenu = NewMenu( 22222, "\pWindowTypes");
				InsertMenu( fontsMenu, -1);
				AppendMenu( fontsMenu, "\pDocument Window" );  //4
				AppendMenu( fontsMenu, "\pDocument Window with Zoombox" ); // 12
				AppendMenu( fontsMenu, "\pDocument Window with Sizebox" ); // 0
				AppendMenu( fontsMenu, "\pDocument Window with Zoombox & Sizebox" ); // 8
				AppendMenu( fontsMenu, "\pDocument Window with Closebox" ); // 1004
				AppendMenu( fontsMenu, "\pDocument Window with Closebox & ZoomBox" ); // 1012
				AppendMenu( fontsMenu, "\pDocument Window with Closebox & SizeBox" ); // 1000
				AppendMenu( fontsMenu, "\pDocument Window with Closebox, Zoombox & SizeBox" ); // 1008
				AppendMenu( fontsMenu, "\pDialog Box Window" ); // 1
				AppendMenu( fontsMenu, "\pAccessory Window" ); // 16 or 1016?
				AppendMenu( fontsMenu, "\pPlain Box Window" ); // 2
				AppendMenu( fontsMenu, "\pPlain Box Window with shadow" ); //3
				
				LocalToGlobal( &pt);
				SetItemMark( fontsMenu, ctr, 0x12 );
				tl = PopUpMenuSelect( fontsMenu, pt.v, pt.h, ctr); 
				if( HiWord( tl) )
				{
					v1 = LoWord( tl );
					FormProps[CurrentForm].value = v1;
				}
				DeleteMenu( 22222 );
				DisposeMenu( fontsMenu );
				xx1 = EditWin[CurrentForm].status;
				ZapForm( CurrentForm );
			 	RecreateFormWindow( CurrentForm );
				EditWin[CurrentForm].change = 1;
				EditWin[CurrentForm].status = xx1;
				SetPort( PropWin );
			 	EraseRect( &tr );
				InvalRect( &tr );
			}
			else	
			{
				if( objecttype==1 || objecttype == 3 || objecttype == 6 || objecttype == 7)
				{
				 	SetPort( FormWin[CurrentForm]);
					if( ObjectProps[objnum].value )
					{
						ObjectProps[objnum].value = 0;
						if( objecttype == 3 )
						{
							(**CtlData[objnum].te).destRect = (**CtlData[objnum].te).viewRect; //tr3;
							TECalText( CtlData[objnum].te );
							TEUpdate( &(FormWin[CurrentForm]->portRect), CtlData[objnum].te );
						}
					}
					else
					{
				 		if( objecttype==1 || objecttype == 7 )
				 		{
				 			for( ctr=0; ctr<200; ctr++)
				 			{
				 				if( CtlData[ctr].status && CtlData[ctr].WinNum == CurrentForm )
				 				{
				 			 		if( objecttype==1 ) ObjectProps[ctr].value = 0;
				 			 		else if( ObjectProps[objnum].opt19 == ObjectProps[ctr].opt19 )
				 			 		{
				 			 			ObjectProps[ctr].value = 0;
				 			 			SetControlValue( CtlData[ctr].ctl, ObjectProps[ctr].value);
				 			 		}
								}
							}
				 		}
						ObjectProps[objnum].value = 1;
						if( objecttype == 3 )
						{
							tr3 = (**CtlData[objnum].te).viewRect;
							tr3.right = 2000;
							(**CtlData[objnum].te).destRect = tr3;
							TECalText( CtlData[objnum].te );
							TEUpdate( &(FormWin[CurrentForm]->portRect), CtlData[objnum].te );
						}
				 	}
				 	if( objecttype==6 || objecttype == 7 ) SetControlValue( CtlData[objnum].ctl, ObjectProps[objnum].value);
	 			 	EraseRect(  &FormWin[CurrentForm]->portRect );
	 				InvalRect( &FormWin[CurrentForm]->portRect );
					SetPort( PropWin);
					EraseRect( &tr );
					InvalRect( &tr );
				}
				else
				{	
					PropTE = TENew( &tr2, &tr );
					NumToString( ObjectProps[objnum].value, text1 );
					cptr = text1;  
					numEntryFlag++;
				}
			}
			break;
		case 9:
			if( !objecttype )
			{
			  if( FormProps[CurrentForm].enabled ) FormProps[CurrentForm].enabled = 0;
			  else  FormProps[CurrentForm].enabled = 1;
			 }
		
			else
			{
				if( ObjectProps[objnum].enabled )  ObjectProps[objnum].enabled = 0;
				else  ObjectProps[objnum].enabled = 1;
			}
		 	EraseRect( &tr );
			InvalRect( &tr );
			break;
		case 10: // Fontname
			ctr=-1;
			fontsMenu = NewMenu( 22222, "\pFonts");
			AppendResMenu( fontsMenu , kTypeFont );
			InsertMenu( fontsMenu, -1);
			cc = CountMItems( fontsMenu );			
			for( v1=1; v1<=cc; v1++)
			{
				GetMenuItemText( fontsMenu, v1, text1 );
				if( !objecttype ) if( EqualString( FormProps[CurrentForm].fontname, text1, false, false )) { ctr=v1; break;}
				else  if( EqualString( ObjectProps[objnum].fontname, text1, false, false )) {ctr=v1; break;}
			}
			if( ctr==-1) ctr = 1;
			else SetItemMark( fontsMenu, ctr, 0x12 );
			LocalToGlobal( &pt);
			tl = PopUpMenuSelect( fontsMenu, pt.v, pt.h, ctr); 
			if( HiWord( tl) )
			{
				v1 = LoWord( tl );
				GetMenuItemText( fontsMenu, v1, text1 );
				if( !objecttype ) Str255Copy( text1, FormProps[CurrentForm].fontname );
				else  Str255Copy( text1, ObjectProps[objnum].fontname);
			}
			DeleteMenu( 22222 );
			DisposeMenu( fontsMenu );
			if( objecttype==2 || objecttype==3 )
			{
				SetPort( FormWin[CurrentForm]);
				RGBForeColor( &ObjectProps[objnum].ForeRGB );
				RGBBackColor( &ObjectProps[objnum].BackRGB );
				//SetThemeWindowBackground(PropWin,kThemeActiveDialogBackgroundBrush,true);
				TESetSelect( 0, 32767, CtlData[objnum].te );
				TEGetStyle( 0, &tes, &ts1, &ts2, CtlData[objnum].te);
				GetFNum( ObjectProps[objnum].fontname, &v1 );
				if( v1 )
				{
					tes.tsFont = v1;
					TESetStyle( 1, &tes, true, CtlData[objnum].te);
				}
				RGBForeColor( &FormProps[CurrentForm].ForeRGB );
				RGBBackColor( &FormProps[CurrentForm].BackRGB );
				//SetThemeWindowBackground(PropWin,kThemeActiveDialogBackgroundBrush,true);
				SetPort( PropWin );
			}	
		 	EraseRect( &tr );
			InvalRect( &tr );
			break;
		case 11:
			PropTE = TENew( &tr2, &tr );
			if( !objecttype ) NumToString( FormProps[CurrentForm].fontsize, text1 ); 
			else NumToString( ObjectProps[objnum].fontsize, text1 );
			cptr = text1;  
			numEntryFlag++;
			break;
		case 12:
			ctr=FormProps[CurrentForm].value;
			if( !objecttype ) ctr = FormProps[CurrentForm].fontstyle+1;
			else ctr = ObjectProps[objnum].fontstyle+1;
			fontsMenu = NewMenu( 22222, "\pStyles");
			InsertMenu( fontsMenu, -1);
			AppendMenu( fontsMenu, "\pNormal" );
			AppendMenu( fontsMenu, "\pBold" );
			AppendMenu( fontsMenu, "\pItalic" );
			AppendMenu( fontsMenu, "\pBold Italic" );
			AppendMenu( fontsMenu, "\pUnderlined" );
			AppendMenu( fontsMenu, "\pBold Underlined" );
			AppendMenu( fontsMenu, "\pItalic Underlined" );
			AppendMenu( fontsMenu, "\pBold Italic Underlined" );
			LocalToGlobal( &pt);
			SetItemMark( fontsMenu, ctr, 0x12 );
			tl = PopUpMenuSelect( fontsMenu, pt.v, pt.h, ctr); 
			if( HiWord( tl) !=0  )
			{
				v1 = LoWord( tl );
				if( !objecttype ) FormProps[CurrentForm].fontstyle = v1-1;
				else ObjectProps[objnum].fontstyle = v1-1;
				if( objecttype==2 || objecttype==3 )
				{
					SetPort( FormWin[CurrentForm]);
					RGBForeColor( &ObjectProps[objnum].ForeRGB );
					RGBBackColor( &ObjectProps[objnum].BackRGB );
					TESetSelect( 0, 32767, CtlData[objnum].te );
					TEGetStyle( 15, &tes, &ts1, &ts2, CtlData[objnum].te);
					tes.tsFace = 0;
					TESetStyle( 2, &tes, false, CtlData[objnum].te);
					if( v1-1 ) 
					{
						tes.tsFace = v1-1;
						TESetStyle( 2, &tes, true, CtlData[objnum].te);
					}
					TEUpdate( &CtlData[objnum].theRect, CtlData[objnum].te );
					RGBForeColor( &FormProps[CurrentForm].ForeRGB );
					RGBBackColor( &FormProps[CurrentForm].BackRGB );
					SetPort( PropWin );
				}	
			}
			DeleteMenu( 22222 );
			DisposeMenu( fontsMenu );
		 	EraseRect( &tr );
			InvalRect( &tr );
			break;
		case 13:
			if( !objecttype )
			{
				  if( FormProps[CurrentForm].opt14 ) FormProps[CurrentForm].opt14 = 0;
				  else  FormProps[CurrentForm].opt14 = 1;
				  SetPort( FormWin[CurrentForm]);
			 	  EraseRect(  &FormWin[CurrentForm]->portRect );
				  InvalRect( &FormWin[CurrentForm]->portRect );
				  SetPort( PropWin);
			 }
			else
			{
				if( ObjectProps[objnum].opt14 )  ObjectProps[objnum].opt14 = 0;
				else  ObjectProps[objnum].opt14 = 1;
				
				if( objecttype == 3 ) // Textbox scrollbar
				{				
				    SetPort( FormWin[CurrentForm]);
					MasterRect = CtlData[objnum].theRect;
					ControlResize( objnum );
					if( ObjectProps[objnum].opt14 )  // Need to add it?
					{   // Yes add it 
						tr3 = CtlData[objnum].theRect;
						tr3.left = tr3.right-16;
						CtlData[objnum].ctl = NewControl( FormWin[CurrentForm], &tr3, "\p", 1, 0, 0, 0, myscrollBarProc, 0);  
						SetControlMinimum( CtlData[objnum].ctl, 1 );
						SetControlMaximum( CtlData[objnum].ctl, 10 );
						SetControlValue( CtlData[objnum].ctl, 1 );
						ObjectProps[objnum].value = 0;   // Turn single line off
						tr3 = CtlData[objnum].theRect;  tr3.right==16;
						(**CtlData[objnum].te).viewRect = tr3;
						(**CtlData[objnum].te).destRect = tr3;
						TECalText( CtlData[objnum].te );
						TEUpdate( &(FormWin[CurrentForm]->portRect), CtlData[objnum].te );
					}
					else
					{	// No remove it
						if( CtlData[objnum].ctl != nil )
						{
							DisposeControl( CtlData[objnum].ctl );
							CtlData[objnum].ctl = nil;
							tr3 = CtlData[objnum].theRect;
							(**CtlData[objnum].te).viewRect = tr3;
							(**CtlData[objnum].te).destRect = tr3;
							TECalText( CtlData[objnum].te );
							TEUpdate( &(FormWin[CurrentForm]->portRect), CtlData[objnum].te );
						}
					}
			 	  	EraseRect(  &FormWin[CurrentForm]->portRect );
				  	InvalRect( &FormWin[CurrentForm]->portRect );
				    SetPort( PropWin);
				    tr = PropWin->portRect;
				}
			}
		 	EraseRect( &tr );
			InvalRect( &tr );
			break;
		case 14:
			if( !objecttype )
			{
				  if( FormProps[CurrentForm].opt15 ) FormProps[CurrentForm].opt15 = 0;
				  else  FormProps[CurrentForm].opt15 = 1;
				  SetPort( FormWin[CurrentForm]);
			 	  EraseRect(  &FormWin[CurrentForm]->portRect );
				  InvalRect( &FormWin[CurrentForm]->portRect );
				  SetPort( PropWin);
			 }
			else
			{
				if( ObjectProps[objnum].opt15 )  ObjectProps[objnum].opt15 = 0;
				else  ObjectProps[objnum].opt15 = 1;
			}
		 	EraseRect( &tr );
			InvalRect( &tr );
			break;
		case 15:
			PropTE = TENew( &tr2, &tr );
			if( !objecttype ) NumToString( FormProps[CurrentForm].opt16, text1 ); 
			else NumToString( ObjectProps[objnum].opt16, text1 );
			cptr = text1;  
			numEntryFlag++;
			break;
		case 16:
			PropTE = TENew( &tr2, &tr );
			if( !objecttype ) NumToString( FormProps[CurrentForm].opt17, text1 ); 
			else NumToString( ObjectProps[objnum].opt17, text1 );
			cptr = text1;  
			numEntryFlag++;
			break;
		case 17:
			PropTE = TENew( &tr2, &tr );
			if( !objecttype ) NumToString( FormProps[CurrentForm].opt18, text1 ); 
			else NumToString( ObjectProps[objnum].opt18, text1 );
			cptr = text1;  
			numEntryFlag++;
			break;
		case 18:
			PropTE = TENew( &tr2, &tr );
			if( !objecttype ) NumToString( FormProps[CurrentForm].opt19, text1 ); 
			else NumToString( ObjectProps[objnum].opt19, text1 );
			cptr = text1;  
			numEntryFlag++;
			break;
		case 19:
			PropTE = TENew( &tr2, &tr );
			if( !objecttype ) NumToString( FormProps[CurrentForm].opt20, text1 ); 
			else NumToString( ObjectProps[objnum].opt20, text1 );
			cptr = text1;  
			numEntryFlag++;
			break;
		case 20:  // Frame Controls only
			if( !objecttype )
			{
 				if( FormProps[CurrentForm].opt21 )  FormProps[CurrentForm].opt21 = 0;
				else  FormProps[CurrentForm].opt21 = 1;
			}
			else
  			{
	  			if( ObjectProps[objnum].opt21 )  ObjectProps[objnum].opt21 = 0;
				else  ObjectProps[objnum].opt21 = 1;
		 		SetPort( FormWin[CurrentForm]);
	  	 	    EraseRect(  &FormWin[CurrentForm]->portRect );
			    InvalRect( &FormWin[CurrentForm]->portRect );
				SetPort( PropWin);
			}
		 	EraseRect( &tr );
			InvalRect( &tr );
			break;
		case 21: // ForeColor
			if( !objecttype ) 
			{
				v1 = GetColor( pt2, "\pForm ForeColor", &FormProps[CurrentForm].ForeRGB, &red1);
				if( v1 )
				{
					FormProps[CurrentForm].ForeRGB = red1;
	 				SetPort( FormWin[CurrentForm]);
	 				RGBForeColor( &FormProps[CurrentForm].ForeRGB);
	 			 	EraseRect(  &FormWin[CurrentForm]->portRect );
	 				InvalRect( &FormWin[CurrentForm]->portRect );
					SetPort( PropWin);
				}
			}
			else
			{
				v1 = GetColor( pt2, "\pControl ForeColor", &ObjectProps[objnum].ForeRGB, &red1);
				if( v1 )
				{
					ObjectProps[objnum].ForeRGB = red1;
					SetPort( FormWin[CurrentForm]);
					if( objecttype==2 || objecttype==3 )
					{
						TESetSelect( 0, 32767, CtlData[objnum].te );
						TEGetStyle( 0, &tes, &ts1, &ts2, CtlData[objnum].te);
						tes.tsColor = red1;
						TESetStyle( 8, &tes, false, CtlData[objnum].te);
						SetPort( PropWin );
					}	
	 			 	EraseRect(  &FormWin[CurrentForm]->portRect );
	 				InvalRect( &FormWin[CurrentForm]->portRect );
					SetPort( PropWin);
				}
			
			}
		 	EraseRect( &tr );
			InvalRect( &tr );
			break;
		case 22: // BackColor
			if( !objecttype ) 
			{
				v1 = GetColor( pt2, "\pForm BackColor", &FormProps[CurrentForm].BackRGB, &red1);
				if( v1 )
				{
					FormProps[CurrentForm].BackRGB = red1;
	 				SetPort( FormWin[CurrentForm]);
	 			 	RGBBackColor( &FormProps[CurrentForm].BackRGB);
	 			 	EraseRect(  &FormWin[CurrentForm]->portRect );
	 				InvalRect( &FormWin[CurrentForm]->portRect );
					SetPort( PropWin);
				}
			}
			else
			{
				v1 = GetColor( pt2, "\pControl BackColor", &ObjectProps[objnum].BackRGB, &red1);
				if( v1 )
				{
					ObjectProps[objnum].BackRGB = red1;
	 				SetPort( FormWin[CurrentForm]);
	 			 	EraseRect(  &FormWin[CurrentForm]->portRect );
	 				InvalRect( &FormWin[CurrentForm]->portRect );
					SetPort( PropWin);
				}
			
			}
		 	EraseRect( &tr );
			InvalRect( &tr );
			break;
		

	}

	switch(propSel)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 11:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
			tl = (long) cptr[0];
			cptr++;
			TESetText( cptr, tl, PropTE );
			ActiveTE = PropTE;
			TEActivate( ActiveTE );
			TEAutoView( true, PropTE );
			TESetSelect( 0, 32767,ActiveTE );
			TEUpdate( &tr, PropTE );
			TEClick( pt, false, PropTE);
			break;
		case 8:
			if( objecttype>1 && objecttype !=3 && objecttype !=6 && objecttype !=7) 
			{
				tl = (long) cptr[0];
				cptr++;
				TESetText( cptr, tl, PropTE );
				ActiveTE = PropTE;
				TEActivate( ActiveTE );
				TEAutoView( true, PropTE );
				TESetSelect( 0, 32767,ActiveTE );
				TEUpdate( &tr, PropTE );
				TEClick( pt, false, PropTE);
			}
			break;


	}
	EditWin[CurrentForm].change = 1;
}

void PropDispose( )
{
	short objecttype, objnum, SCRoffset=0, a, b, c,d, mm, z1 ,z2, rf,v2;
	Str255	text1;
	long v1;
	Rect	tr, tr2;
	GrafPtr		gp;
	Point	theCell;
	TextStyle	tes;
	short		ts1, ts2;
	Handle		hdl;
	
	if( PropTE==nil ) return;
	GetPort( &gp );
	SetPort( PropWin );
	objecttype= lastObjectType;
	objnum = lastObjNum;

	a = GetControlMaximum(PropScroll);
	if( a > 1 )
	{
			a = GetControlValue(PropScroll);
			if( a > 1 )	SCRoffset = 18 + ((a-1) *18);
	}
	tr.left = 0; tr.right = PropWin->portRect.right-15;
	tr.top = 18 + (lastProp*18) - SCRoffset; 
	tr.bottom = tr.top + 20; 

	GetDialogItemText( (*PropTE)->hText, text1 );

	switch(lastProp)
	{
		case 0:
			if( !objecttype ) Str63Copy( text1, FormProps[CurrentForm].name );
			else Str63Copy( text1, ObjectProps[objnum].name );
			ActiveTE=nil;
			TEDispose( PropTE );
			PropTE=nil;
			break;
		case 1:
			if( !objecttype )
			{
				Str255Copy( text1, FormProps[CurrentForm].text );
				SetWTitle( FormWin[CurrentForm], text1 );	
			}
			else
			{
				Str255Copy( text1, ObjectProps[objnum].text );
				switch( objecttype )
				{
					case 1:
					case 6:
					case 7:
						SetControlTitle( CtlData[objnum].ctl, text1 );
						break;
					case 2:
					case 3:
	 					SetPort( FormWin[CurrentForm]);
						RGBForeColor( &ObjectProps[objnum].ForeRGB );
						RGBBackColor( &ObjectProps[objnum].BackRGB );
						//TESetSelect( 0, 32767, CtlData[objnum].te );
						//TEGetStyle( 15, &tes, &ts1, &ts2, CtlData[objnum].te);
						v1 = (long) text1[0];
						TESetText( text1+1, v1, CtlData[objnum].te);
						TESetSelect( 0, 32767, CtlData[objnum].te );
						GetFNum( ObjectProps[objnum].fontname, &v2 );
						tes.tsFont = v2;
						tes.tsFace = ObjectProps[objnum].fontstyle;
						tes.tsSize = ObjectProps[objnum].fontsize;
						tes.tsColor = ObjectProps[objnum].ForeRGB;
						TESetStyle( 15, &tes, true, CtlData[objnum].te);
						EraseRect ( &CtlData[objnum].theRect );
						InvalRect ( &CtlData[objnum].theRect );
						TECalText( CtlData[objnum].te );
						TEUpdate( &CtlData[objnum].theRect, CtlData[objnum].te );
						RGBForeColor( &FormProps[CurrentForm].ForeRGB );
						RGBBackColor( &FormProps[CurrentForm].BackRGB );
						break;
					case 9:
	 					SetPort( FormWin[CurrentForm]);
						LDelRow( 0, 1, CtlData[objnum].lt);
						c=1;   d=1;
						v1 = (long) text1[0];
						while(c <= v1)
						{
							if( text1[c] == '|' || c == v1)
							{
								b = LAddRow( 1, 30000, CtlData[objnum].lt);
								SetPt( &theCell, 0, b);
								if( c == v1) c++;
								LSetCell( text1+d, c-d, theCell, CtlData[objnum].lt);
								LDraw( theCell, CtlData[objnum].lt );
								c++;
								d = c;
							}
							else c++;
						}
	 					ObjectProps[objnum].value = 1;
						SetPt( &theCell, 0, 0);
						LSetSelect( true, theCell, CtlData[objnum].lt);
						EraseRect ( &CtlData[objnum].theRect );
						InvalRect ( &CtlData[objnum].theRect );
						break;
				}
			}
			TEDispose( PropTE );
			ActiveTE=nil;
			PropTE=nil;
			break;
		case 2:
			StringToNum( text1, &v1 );
			if(  v1 >= -32767 && v1 <= 32767 )
			{
				if( !objecttype )
				{
					FormProps[CurrentForm].left = (short) v1;
					MoveWindow( FormWin[CurrentForm], FormProps[CurrentForm].left, FormProps[CurrentForm].top, false );
				}
				else
				{
					ObjectProps[objnum].left = (short) v1;
					CtlData[objnum].theRect.left = (short) v1;
					CtlData[objnum].theRect.right = ObjectProps[objnum].left + ObjectProps[objnum].width;
					MasterRect = CtlData[objnum].theRect;
					ControlMove( objnum );
					RedrawFormWin();
				}
			}
			ActiveTE=nil;
			TEDispose( PropTE );
			PropTE=nil;
			break;
		case 3:
			StringToNum( text1, &v1 );
			if(  v1 >= -32767 && v1 <= 32767 )
			{
				if( !objecttype )
				{
					FormProps[CurrentForm].top = (short) v1;
					MoveWindow( FormWin[CurrentForm], FormProps[CurrentForm].left, FormProps[CurrentForm].top, true );
				}
				else
				{
					ObjectProps[objnum].top = (short) v1;
					CtlData[objnum].theRect.top = (short) v1;
					CtlData[objnum].theRect.bottom = ObjectProps[objnum].top + ObjectProps[objnum].height;
					MasterRect = CtlData[objnum].theRect;
					ControlMove( objnum );
				}
				RedrawFormWin();
			}
			ActiveTE=nil;
			TEDispose( PropTE );
			PropTE=nil;
			break;
		case 4:
			StringToNum( text1, &v1 );
			if(  v1 >= -32767 && v1 <= 32767 )
			{
				if( !objecttype )
				{
					FormProps[CurrentForm].width = (short) v1;
					SizeWindow( FormWin[CurrentForm], FormProps[CurrentForm].width, FormProps[CurrentForm].height, true );
				}
				else
				{
					ObjectProps[objnum].width = (short) v1;
					CtlData[objnum].theRect.right = ObjectProps[objnum].left + ObjectProps[objnum].width;
					MasterRect = CtlData[objnum].theRect;
					ControlResize( objnum );
				}
				RedrawFormWin();
			}
			ActiveTE=nil;
			TEDispose( PropTE );
			PropTE=nil;
			break;
		case 5:
			StringToNum( text1, &v1 );
			if(  v1 >= -32767 && v1 <= 32767 )
			{
				if( !objecttype )
				{
					FormProps[CurrentForm].height = (short) v1;
					SizeWindow( FormWin[CurrentForm], FormProps[CurrentForm].width, FormProps[CurrentForm].height, false );
				}
				else
				{
					ObjectProps[objnum].height = (short) v1;
					CtlData[objnum].theRect.bottom = ObjectProps[objnum].top + ObjectProps[objnum].height;
					MasterRect = CtlData[objnum].theRect;
					ControlResize( objnum );
				}
				RedrawFormWin();
			}
			ActiveTE=nil;
			TEDispose( PropTE );
			PropTE=nil;
			break;
		case 6:  // Proc
			if( !text1[0] || EqualString( text1, "\pNONE" , false, false ) ) Str255Copy( "\pNone", text1);
			if( !objecttype ) Str63Copy( text1, FormProps[CurrentForm].proc );
			else Str63Copy( text1, ObjectProps[objnum].proc );
			ActiveTE=nil;
			TEDispose( PropTE );
			PropTE=nil;
			break;
		//case 7: ignore
		case 8: // Value
			if( objecttype>1 && objecttype !=3 && objecttype !=6 && objecttype !=7) 
			{
				mm = ObjectProps[objnum].value;
				StringToNum( text1, &v1 );
				if(  v1 >= -32767 && v1 <= 32767 )
				{
					ObjectProps[objnum].value = (short) v1;
				}
				ActiveTE=nil;
				TEDispose( PropTE );
				PropTE=nil;
				switch( objecttype)
				{
					case 4:  // Icon
					case 5:  // Picture
						if( ObjectProps[objnum].value > 0 )
						{
							if( CtlData[objnum].pic !=nil )
							{
								ReleaseResource((Handle) CtlData[objnum].pic);
								CtlData[objnum].pic = nil;
							}
							SetResLoad(true);
							z1 = CountMenuItems( ResourcesSubMenu );
							for( z2 = 1; z2<=z1; z2++ )
							{
								GetMenuItemText( ResourcesSubMenu, z2, text1 );
								rf = HOpenResFile( PjtFS.vRefNum,PjtFS.parID, text1, 4 );
								if( rf !=-1  )
								{
									UseResFile(rf);	
									if( objecttype == 4 )
									{   //icon
										d = PlotIconID(&CtlData[objnum].theRect, atAbsoluteCenter, ttNone, ObjectProps[objnum].value);
										if( !d )
										{
											SetRect( &tr2, 0,0,31,31 );
											CtlData[objnum].pic = OpenPicture( &tr2 );
											d = PlotIconID(&tr2, atAbsoluteCenter, ttNone, ObjectProps[objnum].value);
											ClosePicture();
										}
									}
									else CtlData[objnum].pic = (PicHandle) Get1Resource( 'PICT', ObjectProps[objnum].value);
									
									if( CtlData[objnum].pic != nil )
									{
										HNoPurge( (Handle)  CtlData[objnum].pic );
										DetachResource(  (Handle) CtlData[objnum].pic );
										CloseResFile( rf );
										break;
									}
									else CloseResFile( rf );
								}
							}
							UseResFile(VDE_ResFileNum);
						}
						SetPort( FormWin[CurrentForm]);
						InvalRect ( &CtlData[objnum].theRect );
						break;
					case 9:  // ListBox
						SetPort( FormWin[CurrentForm]);
						SetPt( &theCell, 0, mm-1);
						if( mm-1  >= 0 ) LSetSelect( false, theCell, CtlData[objnum].lt);
						if( ObjectProps[objnum].value>0 )
						{
							SetPt( &theCell, 0, ObjectProps[objnum].value-1);
							LSetSelect( true, theCell, CtlData[objnum].lt);
						}
						//LDraw( theCell, CtlData[c].lt );
						InvalRect ( &CtlData[objnum].theRect );
						break;
					case 11:  // ScrollBar
						SetPort( FormWin[CurrentForm]);
						SetControlValue( CtlData[objnum].ctl, ObjectProps[objnum].value );
						break;
				}
			}
			break;
		//case 9: ignore
		//case 10: ignore
		case 11: // fontsize 
			StringToNum( text1, &v1 );
			if(  v1 >= -32767 && v1 <= 32767 )
			{
				if( !objecttype ) FormProps[CurrentForm].fontsize = (short) v1;
				else ObjectProps[objnum].fontsize = (short) v1;
			}
			ActiveTE=nil;
			TEDispose( PropTE );
			PropTE=nil;
			if( objecttype==2 || objecttype==3 )
			{
				SetPort( FormWin[CurrentForm]);
				RGBForeColor( &ObjectProps[objnum].ForeRGB );
				RGBBackColor( &ObjectProps[objnum].BackRGB );
				TESetSelect( 0, 32767, CtlData[objnum].te );
				TEGetStyle( 0, &tes, &ts1, &ts2, CtlData[objnum].te);
				tes.tsSize = v1;
				TESetStyle( 4, &tes, true, CtlData[objnum].te);
				RGBForeColor( &ObjectProps[CurrentForm].ForeRGB );
				RGBBackColor( &ObjectProps[CurrentForm].BackRGB );
				SetPort( PropWin );
			}
			break;
		//case 12: ignore style
		//case 13: ignore 14
		//case 14: ignore 15
		case 15: // 16 
			StringToNum( text1, &v1 );
			if(  v1 >= -32767 && v1 <= 32767 )
			{
				if( !objecttype )
				{
					FormProps[CurrentForm].opt16 = (short) v1;
	 				SetPort( FormWin[CurrentForm]);
	 			 	EraseRect(  &FormWin[CurrentForm]->portRect );
 					InvalRect( &FormWin[CurrentForm]->portRect );
					SetPort( PropWin);
				}
				else ObjectProps[objnum].opt16 = (short) v1;
			}
			ActiveTE=nil;
			TEDispose( PropTE );
			PropTE=nil;
			break;
		case 16: // 17 
			StringToNum( text1, &v1 );
			if(  v1 >= -32767 && v1 <= 32767 )
			{
				if( !objecttype ) FormProps[CurrentForm].opt17 = (short) v1;
				else ObjectProps[objnum].opt17 = (short) v1;
			}
			ActiveTE=nil;
			TEDispose( PropTE );
			PropTE=nil;
			if( objecttype == 11 )
			{
				SetPort( FormWin[CurrentForm]);
				SetControlMinimum( CtlData[objnum].ctl, ObjectProps[objnum].opt17 );
			}
			break;
		case 17: // 18 
			StringToNum( text1, &v1 );
			if(  v1 >= -32767 && v1 <= 32767 )
			{
				if( !objecttype ) FormProps[CurrentForm].opt18 = (short) v1;
				else ObjectProps[objnum].opt18 = (short) v1;
			}
			ActiveTE=nil;
			TEDispose( PropTE );
			PropTE=nil;
			if( objecttype == 11 )
			{
				SetPort( FormWin[CurrentForm]);
				SetControlMaximum( CtlData[objnum].ctl, ObjectProps[objnum].opt18 );
			}
			break;
		case 18: // 19 
			StringToNum( text1, &v1 );
			if(  v1 >= -32767 && v1 <= 32767 )
			{
				if( !objecttype ) FormProps[CurrentForm].opt19 = (short) v1;
				else
				{
					mm = ObjectProps[objnum].opt19;
					ObjectProps[objnum].opt19 = (short) v1;
				}					
			}
			ActiveTE=nil;
			TEDispose( PropTE );
			PropTE=nil;
			if( objecttype == 7 && 	mm != ObjectProps[objnum].opt19)
			{
			 	SetPort( FormWin[CurrentForm]);
			 	ObjectProps[objnum].value = 0;
				SetControlValue( CtlData[objnum].ctl, 0);
	 			EraseRect(  &FormWin[CurrentForm]->portRect );
	 			InvalRect( &FormWin[CurrentForm]->portRect );
			}
			break;
		case 19: // 20 
			StringToNum( text1, &v1 );
			if(  v1 >= -32767 && v1 <= 32767 )
			{
				if( !objecttype ) FormProps[CurrentForm].opt20 = (short) v1;
				else ObjectProps[objnum].opt20 = (short) v1;
			}
			ActiveTE=nil;
			TEDispose( PropTE );
			PropTE=nil;
			break;
		//case 20: ignore frame


	}
	SetPort( PropWin );
 	tr = PropWin->portRect;
 	tr.right-=15;
 	EraseRect( &tr );
	InvalRect( &tr );
	UpdatePropWin( PropWin );
	SetPort( gp );
	return;

}

void PropDisregard( )
{
	Rect	tr;
	GrafPtr		gp;
	
	if( PropTE==nil ) return;
	GetPort( &gp );
	SetPort( PropWin );

	ActiveTE=nil;
	TEDispose( PropTE );
	PropTE=nil;

	SetPort( PropWin );
 	tr = PropWin->portRect;
 	EraseRect( &tr );
	InvalRect( &tr );
	UpdatePropWin( PropWin );
	SetPort( gp );
	return;
}

void SaveFormContents( short fnum, short formNum )
{

	short a, b;
	long c;
	char cbuf[512],t1[256];
	
	c = sprintf( cbuf, "    FormBegin\x0d");
	FSWrite( fnum, &c, cbuf );	
	WriteProps( fnum, 0, formNum );
	c = sprintf( cbuf, "    FormEnd\x0d\x0d", t1);
	FSWrite( fnum, &c, cbuf );	

	for( a=0; a<200; a++)
	{
		if( CtlData[a].status && CtlData[a].WinNum == formNum )
		{
			b = CtlData[a].status;
			Str255ToStr( objectNames[b], t1 );
			c = sprintf( cbuf, "    ControlBegin=%s\x0d", t1);
			FSWrite( fnum, &c, cbuf );	
			WriteProps( fnum, b, a );
			c = sprintf(  cbuf,"    ControlEnd\x0d\x0d" );
			FSWrite( fnum, &c, cbuf );	
		}
	}


}

void WriteProps( short fnum, short objecttype, short objnum )
{
	short 		a;
	Str255		text1;
	char		cbuf[512], t1[256], t2[256];
	long	    c;
	
	for( a=0; a<=22; a++ )
	{
		if( objectdef[objecttype][a] >= '1' || a==19 )  // plus opt20
		{
			if( !objecttype ) Str255Copy( FormPropsDesc[a], text1);
			else Str255Copy( CtlPropsDesc[a], text1 );
			GetPropStr( a, objecttype, objnum );
			Str255ToStr( text1, t1 );
			Str255ToStr( retPropStr, t2 );
			c = sprintf( cbuf, "      %s=%s\x0d", t1, t2 );
			FSWrite( fnum, &c, cbuf );	
		}
	}
	
}


void GetPropStr( short pnum, short objecttype, short objnum )
{
	short v1;
	RGBColor	blue1;
	
	switch(pnum)
	{
		case 0:
			if( !objecttype ) Str255Copy( FormProps[CurrentForm].name, retPropStr ); 
			else Str255Copy( ObjectProps[objnum].name, retPropStr ); 
			break;
		case 1:
			if( !objecttype ) Str255Copy( FormProps[CurrentForm].text, retPropStr ); 
			else Str255Copy( ObjectProps[objnum].text, retPropStr ); 
			break;
		case 2:
			if( !objecttype ) NumToString( FormProps[CurrentForm].left, retPropStr); 
			else NumToString( ObjectProps[objnum].left, retPropStr); 
			break;
		case 3:
			if( !objecttype ) NumToString( FormProps[CurrentForm].top, retPropStr); 
			else NumToString( ObjectProps[objnum].top, retPropStr); 
			break;
		case 4:
			if( !objecttype ) NumToString( FormProps[CurrentForm].width, retPropStr); 
			else NumToString( ObjectProps[objnum].width, retPropStr); 
			break;
		case 5:
			if( !objecttype ) NumToString( FormProps[CurrentForm].height, retPropStr); 
			else NumToString( ObjectProps[objnum].height, retPropStr); 
			break;
		case 6:
			if( !objecttype ) Str255Copy( FormProps[CurrentForm].proc, retPropStr ); 
			else Str255Copy( ObjectProps[objnum].proc, retPropStr ); 
			break;
		case 7:
			if( !objecttype ) NumToString( FormProps[CurrentForm].visible, retPropStr); 
			else NumToString( ObjectProps[objnum].visible, retPropStr); 
			/*if( !objecttype ) v1 =  FormProps[CurrentForm].visible; 
			else v1 = ObjectProps[objnum].visible; 
			if( !v1 ) Str255Copy( "\pFalse", retPropStr );
			else Str255Copy( "\pTrue", retPropStr );*/
			break;
		case 8:
			if( !objecttype ) NumToString( FormProps[CurrentForm].value, retPropStr); 
			else NumToString( ObjectProps[objnum].value, retPropStr); 
			break;
		case 9:
			if( !objecttype ) NumToString( FormProps[CurrentForm].enabled, retPropStr); 
			else NumToString( ObjectProps[objnum].enabled, retPropStr); 
/*			if( !objecttype ) v1 =  FormProps[CurrentForm].enabled; 
			else v1 = ObjectProps[objnum].enabled; 
			if( !v1 ) Str255Copy( "\pFalse", retPropStr );
			else Str255Copy( "\pTrue", retPropStr );*/
			break;
		case 10:
			if( !objecttype ) Str255Copy( FormProps[CurrentForm].fontname, retPropStr ); 
			else Str255Copy( ObjectProps[objnum].fontname, retPropStr ); 
			break;
		case 11:
			if( !objecttype ) NumToString( FormProps[CurrentForm].fontsize, retPropStr); 
			else NumToString( ObjectProps[objnum].fontsize, retPropStr); 
			break;
		case 12:
			if( !objecttype ) v1= FormProps[CurrentForm].fontstyle; 
			else v1 = ObjectProps[objnum].fontstyle; 
			NumToString(v1, retPropStr); 
			break;
		case 13:
			if( !objecttype ) NumToString( FormProps[CurrentForm].opt14, retPropStr); 
			else NumToString( ObjectProps[objnum].opt14, retPropStr); 
/*			if( !objecttype ) v1 =  FormProps[CurrentForm].opt14; 
			else v1 = ObjectProps[objnum].opt14; 
			if( !v1 ) Str255Copy( "\pFalse", retPropStr );
			else Str255Copy( "\pTrue", retPropStr );*/
			break;
		case 14:
			if( !objecttype ) NumToString( FormProps[CurrentForm].opt15, retPropStr); 
			else NumToString( ObjectProps[objnum].opt15, retPropStr); 
/*			if( !objecttype ) v1 =  FormProps[CurrentForm].opt15; 
			else v1 = ObjectProps[objnum].opt15; 
			if( !v1 ) Str255Copy( "\pFalse", retPropStr );
			else Str255Copy( "\pTrue", retPropStr );*/
			break;
		case 15:
			if( !objecttype ) NumToString( FormProps[CurrentForm].opt16, retPropStr); 
			else NumToString( ObjectProps[objnum].opt16, retPropStr); 
			break;
		case 16:
			if( !objecttype ) NumToString( FormProps[CurrentForm].opt17, retPropStr); 
			else NumToString( ObjectProps[objnum].opt17, retPropStr); 
			break;
		case 17:
			if( !objecttype ) NumToString( FormProps[CurrentForm].opt18, retPropStr); 
			else NumToString( ObjectProps[objnum].opt18, retPropStr); 
			break;
		case 18:
			if( !objecttype ) NumToString( FormProps[CurrentForm].opt19, retPropStr); 
			else NumToString( ObjectProps[objnum].opt19, retPropStr); 
			break;
		case 19:
			if( !objecttype ) NumToString( FormProps[CurrentForm].opt20, retPropStr); 
			else NumToString( ObjectProps[objnum].opt20, retPropStr); 
			break;
		case 20:
			if( !objecttype ) NumToString( FormProps[CurrentForm].opt21, retPropStr); 
			else NumToString( ObjectProps[objnum].opt21, retPropStr); 
/*			if( !objecttype ) v1 =  FormProps[CurrentForm].opt21; 
			else v1 = ObjectProps[objnum].opt21; 
			if( !v1 ) Str255Copy( "\pFalse", retPropStr );
			else Str255Copy( "\pTrue", retPropStr );*/ 
			break;
		case 21:
			if( !objecttype ) blue1 =  FormProps[CurrentForm].ForeRGB;
			else blue1 =  ObjectProps[objnum].ForeRGB;
			v1 = sprintf((char*)retPropStr+1, "%i %i %i", blue1.red, blue1.green, blue1.blue );
			retPropStr[0] = v1;
			break;
		case 22:
			if( !objecttype ) blue1 =  FormProps[CurrentForm].BackRGB;
			else blue1 =  ObjectProps[objnum].BackRGB;
			v1 = sprintf( (char*)retPropStr+1, "%i %i %i", blue1.red, blue1.green, blue1.blue );
			retPropStr[0] = v1;
			break;


	}
}

short LoadFormContents( short formNum )
{

	short a, b, c, f, g=-1;
	char cbuf[512],t1[256];
	
	a = znext_line();
	zsplit();
	//printf(">>%s %s\n", zparm1+1, zparm2+1 );
	if( !EqualString( zparm1, "\pFormBegin", false, false )) return(-1);

	while( !a )
	{
		a = znext_line();
		zsplit();
		if( EqualString( zparm1, "\pFormEnd", false, false )) a=1;
		//printf(">>  %s %s\n", zparm1+1, zparm2+1 );
		else StoreFormValue( formNum, 0 );  // Store stuff that is in zparm1, zparm2
	}
	if( a==999 ) return 0;
	
	a = znext_line();
	zsplit();
	//printf(">>%s %s\n", zparm1+1, zparm2+1 );
	while( !a )
	{
		if( EqualString( zparm1, "\pControlBegin", false, false ))
		{
			for( c=1; c<=11; c++ ) if( EqualString( zparm2, objectNames[c] , false, false)) { g = c; break; }
			if( f==-1) return -2;

			/// Create new control in memory
			f = -1;
			for( c=0; c<200; c++) if( CtlData[c].status == 0 ) { f=c; break; }
			if( f==-1) return -3;
			CtlData[f].ctl=nil;		CtlData[f].te=nil;		CtlData[f].lt=nil;		CtlData[f].pic=nil;
			CtlData[f].status = g;	CtlData[f].WinNum = formNum;  					CtlData[f].lastItem =0;
			
			while( !a )
			{
				a = znext_line();
				zsplit();
				//printf(">>%s %s\n", zparm1+1, zparm2+1 );
				if( EqualString( zparm1, "\pControlEnd", false, false )) a=6;
				else StoreFormValue( f, g);  // Store stuff that is in zparm1, zparm2
			}
			if( a == 6 ) a = 0;
			if( a==999 ) return 0;
			if( !a)
			{
				a = znext_line();
				zsplit();
			}
			//printf(">>%s %s\n", zparm1+1, zparm2+1 );
		}
		else a=1;
	}
	
	RecreateFormWindow( formNum );

	return(0);
}

void StoreFormValue( short dd, short objecttype )
{
	short propnum=-1,  objnum, v1, b, c,d, v2;
	Str255	text1;
	char	text2[256];
	long		t;
	RGBColor	blue1;
	Rect		r;
	Point		theCell;


	if( !objecttype )
	{
		for( v1=0; v1<=22; v1++ ) if( EqualString( zparm1, FormPropsDesc[v1] , false, false)) { propnum = v1; break; }
	}	
	else
	{
		for( v1=0; v1<=22; v1++ ) if( EqualString( zparm1, CtlPropsDesc[v1] , false, false)) { propnum = v1; break; }
	}	
		
	
	if( propnum == -1 ) return;
	
	objnum = dd;
	switch(propnum)
	{
		case 0:
			if( !objecttype ) Str255Copy( zparm2, FormProps[dd].name);
			else Str255Copy( zparm2, ObjectProps[objnum].name ); 
			break;
		case 1:
			if( !objecttype ) Str255Copy( zparm2, FormProps[dd].text);
			else Str255Copy( zparm2, ObjectProps[objnum].text ); 
			break;
		case 2:
			StringToNum( zparm2, &t); v1 = (short) t;
			if( !objecttype ) FormProps[dd].left = v1; 
			else
			{
				ObjectProps[objnum].left = v1; 
				SetRect( &CtlData[objnum].theRect, ObjectProps[objnum].left, ObjectProps[objnum].top, ObjectProps[objnum].left+ObjectProps[objnum].width, ObjectProps[objnum].top+ObjectProps[objnum].height);
			}
			break;
		case 3:
			StringToNum( zparm2, &t); v1 = (short) t;
			if( !objecttype ) FormProps[dd].top = v1; 
			else
			{
				ObjectProps[objnum].top = v1; 
				SetRect( &CtlData[objnum].theRect, ObjectProps[objnum].left, ObjectProps[objnum].top, ObjectProps[objnum].left+ObjectProps[objnum].width, ObjectProps[objnum].top+ObjectProps[objnum].height);
			}
			break;
		case 4:
			StringToNum( zparm2, &t); v1 = (short) t;
			if( !objecttype ) FormProps[dd].width = v1; 
			else
			{
				ObjectProps[objnum].width = v1; 
				SetRect( &CtlData[objnum].theRect, ObjectProps[objnum].left, ObjectProps[objnum].top, ObjectProps[objnum].left+ObjectProps[objnum].width, ObjectProps[objnum].top+ObjectProps[objnum].height);
			}
			break;
		case 5:
			StringToNum( zparm2, &t); v1 = (short) t;
			if( !objecttype ) FormProps[dd].height = v1; 
			else
			{
				ObjectProps[objnum].height = v1; 
				SetRect( &CtlData[objnum].theRect, ObjectProps[objnum].left, ObjectProps[objnum].top, ObjectProps[objnum].left+ObjectProps[objnum].width, ObjectProps[objnum].top+ObjectProps[objnum].height);
			}
			break;
		case 6:
			if( !objecttype ) Str255Copy( zparm2, FormProps[dd].proc ); 
			else Str255Copy( zparm2, ObjectProps[objnum].proc ); 
			break;
		case 7:
			StringToNum( zparm2, &t); v1 = (short) t;
			if( !objecttype ) FormProps[dd].visible = v1; 
			else ObjectProps[objnum].visible = v1; 
			break;
		case 8:
			StringToNum( zparm2, &t); v1 = (short) t;
			if( !objecttype ) FormProps[dd].value = v1; 
			else ObjectProps[objnum].value = v1; 
			break;
		case 9:
			StringToNum( zparm2, &t); v1 = (short) t;
			if( !objecttype ) FormProps[dd].enabled = v1; 
			else ObjectProps[objnum].enabled = v1; 
			break;
		case 10:
			if( !objecttype ) Str255Copy( zparm2, FormProps[dd].fontname);
			else Str255Copy( zparm2, ObjectProps[objnum].fontname ); 
			break;
		case 11:
			StringToNum( zparm2, &t); v1 = (short) t;
			if( !objecttype ) FormProps[dd].fontsize = v1; 
			else ObjectProps[objnum].fontsize = v1; 
			break;
		case 12:
			StringToNum( zparm2, &t); v1 = (short) t;
			if( !objecttype ) FormProps[dd].fontstyle = v1; 
			else ObjectProps[objnum].fontstyle = v1; 
			break;
		case 13:
			StringToNum( zparm2, &t); v1 = (short) t;
			if( !objecttype ) FormProps[dd].opt14 = v1; 
			else ObjectProps[objnum].opt14 = v1; 
			break;
		case 14:
			StringToNum( zparm2, &t); v1 = (short) t;
			if( !objecttype ) FormProps[dd].opt15 = v1; 
			else ObjectProps[objnum].opt15 = v1; 
			break;
		case 15:
			StringToNum( zparm2, &t); v1 = (short) t;
			if( !objecttype ) FormProps[dd].opt16 = v1; 
			else ObjectProps[objnum].opt16 = v1; 
			break;
		case 16:
			StringToNum( zparm2, &t); v1 = (short) t;
			if( !objecttype ) FormProps[dd].opt17 = v1; 
			else ObjectProps[objnum].opt17 = v1; 
			break;
		case 17:
			StringToNum( zparm2, &t); v1 = (short) t;
			if( !objecttype ) FormProps[dd].opt18 = v1; 
			else ObjectProps[objnum].opt18 = v1; 
			break;
		case 18:
			StringToNum( zparm2, &t); v1 = (short) t;
			if( !objecttype ) FormProps[dd].opt19 = v1; 
			else ObjectProps[objnum].opt19 = v1; 
			break;
		case 19:
			StringToNum( zparm2, &t); v1 = (short) t;
			if( !objecttype ) FormProps[dd].opt20 = v1; 
			else ObjectProps[objnum].opt20 = v1; 
			break;
		case 20:
			StringToNum( zparm2, &t); v1 = (short) t;
			if( !objecttype ) FormProps[dd].opt21 = v1; 
			else ObjectProps[objnum].opt21 = v1; 
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

			if( !objecttype ) FormProps[dd].ForeRGB = blue1;
			else ObjectProps[objnum].ForeRGB = blue1;
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

			if( !objecttype ) FormProps[dd].BackRGB = blue1;
			else ObjectProps[objnum].BackRGB = blue1;
			break;

	}
}


short AddLoadCtls( short formNum, WindowPtr window )
{
	short  c, cn, v1, e,d,b,zz=0, z1,z2,rf;
	Rect	theRect, tr1, tr2;
	Point	CellSz;
	long	lv;
	TextStyle	tes;
	short		ts1, ts2;
	Point	theCell;
	Str255	text1;
	Handle hdl;
	
	for(c=0; c<200; c++)
	{
		if( CtlData[c].status && CtlData[c].WinNum == formNum )
		{

			switch( CtlData[c].status )
			{
				case 1: // Button
					CtlData[c].ctl = NewControl( window, &CtlData[c].theRect, ObjectProps[c].text, 1, 0, 0, 0, /*pushButProc*/0, 0);  
					break;
				case 2: // label
				case 3: // TextBox
					tr1 = CtlData[c].theRect;
					if( ObjectProps[c].opt14 ) tr1.right -=16;
					tr2 = tr1;
					if( ObjectProps[c].opt14 ) tr2.right = 2000;
					CtlData[c].te = TEStyleNew( &tr2, &tr1 );
					if( CtlData[c].te !=nil )
					{
						RGBForeColor( &ObjectProps[c].ForeRGB );
						RGBBackColor( &ObjectProps[c].BackRGB );
						lv = (long) ObjectProps[c].text[0];
						TESetText( ObjectProps[c].text+1, lv, CtlData[c].te);
						TESetSelect( 0, 32767, CtlData[c].te );
						TEGetStyle( 0, &tes, &ts1, &ts2, CtlData[c].te);
						GetFNum( ObjectProps[c].fontname, &v1 );
						tes.tsFont = v1;
						tes.tsFace = ObjectProps[c].fontstyle;
						tes.tsSize = ObjectProps[c].fontsize;
						tes.tsColor = ObjectProps[c].ForeRGB;
						TESetStyle( 15, &tes, false, CtlData[c].te);

						//if( CtlData[c].status == 4 ) EraseRect( &CtlData[c].theRect );
						if( ObjectProps[c].opt14 )
						{   // Scrollbar
							tr1 = CtlData[c].theRect;
							tr1.left = tr1.right-16;
							CtlData[c].ctl = NewControl( FormWin[formNum], &tr1, "\p", 1, 0, 0, 0, myscrollBarProc, 0);  
							SetControlMinimum( CtlData[c].ctl, 1 );
							SetControlMaximum( CtlData[c].ctl, 10 );
							SetControlValue( CtlData[c].ctl, 1 );
						}
						else CtlData[c].ctl = nil;
						TECalText( CtlData[c].te );
					  	TEAutoView( true, CtlData[c].te );
						TEUpdate( &CtlData[c].theRect, CtlData[c].te );
						if( CtlData[c].status == 4 )
						{
							theRect = CtlData[c].theRect;
							InsetRect( &theRect, -1, -1);
							FrameRect ( &theRect );
						}
					}
					RGBForeColor( &FormProps[formNum].ForeRGB );
					RGBBackColor( &FormProps[formNum].BackRGB );
					break;
				case 4: // Icon
				case 5: // Picture
					if( ObjectProps[c].value > 0 )
					{
						SetResLoad(true);
						z1 = CountMenuItems( ResourcesSubMenu );
						for( z2 = 1; z2<=z1; z2++ )
						{
							GetMenuItemText( ResourcesSubMenu, z2, text1 );
							rf = HOpenResFile( PjtFS.vRefNum,PjtFS.parID, text1, 4 );
							if( rf !=-1  )
							{
								UseResFile(rf);	
								if( CtlData[c].status == 4 )
								{   //icon
									d = PlotIconID(&CtlData[c].theRect, atAbsoluteCenter, ttNone, ObjectProps[c].value);
									if( !d )
									{
										SetRect( &tr2, 0,0,31,31 );
										CtlData[c].pic = OpenPicture( &tr2 );
										d = PlotIconID(&tr2, atAbsoluteCenter, ttNone, ObjectProps[c].value);
										ClosePicture();
									}
								}
								else CtlData[c].pic = (PicHandle) Get1Resource( 'PICT', ObjectProps[c].value);
								
								if( CtlData[c].pic != nil )
								{
									HNoPurge( (Handle)  CtlData[c].pic );
									DetachResource(  (Handle) CtlData[c].pic );
									CloseResFile( rf );
									break;
								}
								else CloseResFile( rf );
							}
						}
						UseResFile(VDE_ResFileNum);
					}
					break;
			  	case 6:	// CheckBox
					CtlData[c].ctl = NewControl( window, &CtlData[c].theRect, ObjectProps[c].text, 1, ObjectProps[c].value, 0, 1, /*checkBoxProc*/369, 0);  
					break;
			  	case 7:	// Radio
					CtlData[c].ctl = NewControl( window, &CtlData[c].theRect, ObjectProps[c].text, 1, ObjectProps[c].value, 0, 1, /*radioButProc*/370, 0);  
					break;
			  	case 8:	// DropList
					cn = c + 10000;
					CtlData[c].theMenu = NewMenu(cn, "\pMenu" );
					InsertMenu( CtlData[c].theMenu, -1);
					AppendMenu( CtlData[c].theMenu, ObjectProps[c].name);
					CtlData[c].ctl = NewControl( window, &CtlData[c].theRect, ObjectProps[c].name, true, 0, cn ,0,/*popupMenuProc*/400, 0); 
					SetControlMinimum( CtlData[c].ctl, 1 );
					SetControlMaximum( CtlData[c].ctl, 2);
					SetControlValue( CtlData[c].ctl, 1 );
					break;
			  	case 9:	// ListBox
					SetRect( &theRect, 0,0, 0,0);
					CellSz.h = 0;
					CellSz.v = 0;
					CtlData[c].lt = LNew( &CtlData[c].theRect, &theRect, CellSz, 0, window, true, false, false, true); 
					if( CtlData[c].lt !=nil )
					{
						RGBForeColor( &ObjectProps[c].ForeRGB );
						RGBBackColor( &ObjectProps[c].BackRGB );
						LAddColumn( 1, 0, CtlData[c].lt);									
						e=1;   d=1; zz= 0;
						v1 = (long)  ObjectProps[c].text[0];
						while(e <= v1)
						{
							if( ObjectProps[c].text[e] == '|' || e == v1)
							{
								b = LAddRow( 1, 30000, CtlData[c].lt);
								SetPt( &theCell, 0, b);
								if( e == v1) e++;
								LSetCell( ObjectProps[c].text+d, e-d, theCell, CtlData[c].lt);
								//if( !zz ) { zz++; LSetSelect( true, theCell, CtlData[c].lt); }
								LDraw( theCell, CtlData[c].lt );
								e++;
								d = e;
							}
							else e++;
						}
						if( ObjectProps[c].value>0 )
						{
							SetPt( &theCell, 0, ObjectProps[c].value-1);
							LSetSelect( true, theCell, CtlData[c].lt); 
							LDraw( theCell, CtlData[c].lt );
						}
					}
					RGBForeColor( &FormProps[formNum].ForeRGB );
					RGBBackColor( &FormProps[formNum].BackRGB );
					break;
				case 10:	// Timer
					SetResLoad(true);
					PlotIconID(&CtlData[c].theRect, 0, 0, 5011);
					break;
			  	case 11:	// Scrollbar
					CtlData[c].ctl = NewControl( window, &CtlData[c].theRect, "\p", 1, ObjectProps[c].value,  ObjectProps[c].opt17, ObjectProps[c].opt18, myscrollBarProc, 0);  
					break;
			}
		}
	}
}

void DeleteSelectedControl( )
{
	short a;
	
	a = Alert( 137, nil );
	if( a != 2 ) return;
	
	a = Forms[CurrentForm].ctlSelected;
	
	switch( CtlData[a].status )
	{
		case 1:
		case 6:
		case 7:
		case 11:
			DisposeControl( CtlData[a].ctl );
			CtlData[a].ctl=nil;
			break;
		case 2:  // Label
		case 3:  // TextBox
			if( CtlData[a].te!= nil ) TEDispose( CtlData[a].te );
			CtlData[a].te = nil;
			if( CtlData[a].ctl != nil )	{ DisposeControl( CtlData[a].ctl ); CtlData[a].ctl=nil; }
			break;
		case 8: // DropList
			DeleteMenu( a+10000 );
			if( CtlData[a].theMenu!= nil ) DisposeMenu( CtlData[a].theMenu );
			break;
		case 4:
		case 5:
			//if( CtlData[a].pic != nil ) ReleaseResource( CtlData[a].pic );
			break;
		case 9:
			if( CtlData[a].lt !=nil ) { LDispose( CtlData[a].lt );  CtlData[a].lt=nil ; }
	}
	CtlData[a].status = 0;
	Forms[CurrentForm].ctlSelected = -1;
	if( PropWin!=nil )
	{
		SetPort( PropWin );
  		EraseRect( &PropWin->portRect);
		InvalRect( &PropWin->portRect);
		SetPort( FormWin[CurrentForm] );
  	}
  	EraseRect( &FormWin[CurrentForm]->portRect);
	InvalRect( &FormWin[CurrentForm]->portRect);
					
}


/*	Str63	name;
	Str255	text;
	short	left, top, width, height;
	Str63	proc;
	short	visible, value, enabled;
	Str63	fontname;
	short	fontsize, fontstyle;
	short	opt14, opt15, opt16, opt17, opt18, opt19, opt20;
	RGBColor BackRGB, ForeRGB;	
} ObjectProps[200], FormProps[20];
extern struct IntControlStruct
{
   	ControlHandle	ctl;
   	TEHandle		te;
   	ListHandle		lt;			// Thats a lowercase L in lt
	PicHandle		pic;
	Rect			theRect;
   	short			WinNum;
	short			status;
	short			subcode;
	MenuHandle		theMenu;
	short			lastItem;
} CtlData[200];   


*/

pascal void ScrollProp( ControlHandle theControl, short partCode )
 {     	 			
  	short			cmax,cmin,cvalue, b,c  ;
	Rect			theRect;
	
	if( PropWin ==nil ) return;

	cvalue = GetControlValue(theControl);
	cmin = GetControlMinimum(theControl);
	cmax = GetControlMaximum(theControl);

    switch( partCode )
    {
		case 20: // inUpButton:
			if( cvalue == cmin ) return;
			cvalue--;
			if( cvalue >= cmin ) SetControlValue(theControl, cvalue);
			break;
		case 21: //inDownButton:
			if( cvalue == cmax ) return;
			cvalue++;
			if( cvalue <= cmax ) SetControlValue(theControl, cvalue);
			break;
		case 22: //inPageUp:
			if( cvalue == cmin ) return;
			cvalue -= 5;
			if( cvalue < cmin ) cvalue= cmin; 
			SetControlValue(theControl, cvalue);
			break;
		case 23: //inPageDown:
			if( cvalue == cmax ) return;
			cvalue += 5;
			if( cvalue > cmax ) cvalue= cmax; 
			SetControlValue(theControl, cvalue);
			break;
	}
	theRect = PropWin->portRect;
	theRect.right-=15;
	EraseRect( &theRect );
	DrawPropWin();

}  
