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

#include "Windows.h"
#include "Icons.h"
#include <Types.h>
#include <Memory.h>
#include <Quickdraw.h>
#include <Fonts.h>
#include <Events.h>
#include <Menus.h>
#include <Windows.h>
#include <TextEdit.h>
#include <Dialogs.h>
#include <OSUtils.h>
#include <ToolUtils.h>
#include <SegLoad.h>
#include <Sound.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "IDE.h"

short CloseEditWindow( WindowPtr window);
short SaveSourceFile( WindowPtr window );
void Str255Cat( Str255 st1, Str255 st2);

short SafeForm( WindowPtr window);
void SelectMyControl( short ct ); // from IDE Events.c
short CheckForEdit(WindowPtr window);  // from IDE Edit1.c
void StrToStr255( char * st1, Str255 st2);
void Str255Copy ( Str255 st1, Str255 st2);
short WhichWindow( WindowPtr window);
short AddLoadCtls( short formNum, WindowPtr window );
extern TEHandle	ActiveTE;
extern WindowPtr PropWin;
void ZapForm( short c );
extern short VDE_ResFileNum;
extern MenuHandle FormsSubMenu;
extern MenuHandle ModulesSubMenu;
extern MenuHandle ResourcesSubMenu;
extern FSSpec		PjtFS;

aDocumentRecord appr;

struct	FormWins
{
	short		WinType;
	Rect		loc;
	short		ctlSelected;
	short		status;

} Forms[20];

extern  struct EditProcStruct
{
	Handle	ProcHandle;
	short	EditWinNum;
	long	size;
}	
EditProc[1000];

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
} EditWin[100];

struct IntControlStruct
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

extern struct ObjectPropsStruct
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

short CurrentForm = -1;
extern short FormCtr, ModuleCtr;

WindowPtr	FormWin[20];

void WindowInit()
{
	short	a;
	for(a=0;a<20;a++) FormWin[a]=nil;
}

short GetControlSelected( )
{
	short 	a;
	if( CurrentForm == -1 ) return -1;
	a = Forms[CurrentForm].ctlSelected;
	return CtlData[a].status;
	
}

short GetControlNumSelected( )
{
	short 	a;
	if( CurrentForm == -1 ) return -1;
	a = Forms[CurrentForm].ctlSelected;
	return a;
	
}

short WhichWindow( WindowPtr window)
{
	short	a;
	for(a=0;a<20;a++) if( FormWin[a]==window ) return a;
	return -1;
}

short GetGridSize( short WinNum )
{
	if( WinNum < 0 ) return 10;
	if( FormWin[WinNum]==nil) return 10;
	return FormProps[WinNum].opt16;
}

short GetGridFlag( short WinNum ) // Snap
{
	if( WinNum < 0 ) return 0;
	if( FormWin[WinNum]==nil) return 0;
	return FormProps[WinNum].opt15;
}

short GetGridShow( short WinNum )
{
	if( WinNum < 0 ) return 0;
	if( FormWin[WinNum]==nil) return 0;
	return FormProps[WinNum].opt14;
}

void ClearMyWindow( short WinNum )
{
	Rect theRect;
	if( WinNum < 0 ) return ;
	if( FormWin[WinNum]==nil) return ;
	//theRect = Forms[WinNum].loc;
	//OffsetRect( &theRect, -theRect.left, -theRect.top);
	theRect = FormWin[WinNum]->portRect;
	EraseRect( &theRect );
}

void SetWinRect( short WinNum, short w, short h )
{
	Rect	theRect, tRect;
    Point pt;

	if( WinNum < 0 ) return ;
	if( FormWin[WinNum]==nil) return ;
	SelectWindow( FormWin[WinNum] );
	SetPort( FormWin[WinNum] );
	
	tRect = FormWin[WinNum]->portRect;
 	pt.h = tRect.left; pt.v = tRect.top;
 	LocalToGlobal( &pt );
	tRect.left = pt.h ; tRect.top = pt.v;
 
	Forms[WinNum].loc = tRect;
	
	theRect = Forms[WinNum].loc;
	theRect.right = theRect.left + w;
	theRect.bottom = theRect.top + h;
	Forms[WinNum].loc = theRect;
	tRect = theRect;
	FormProps[WinNum].left = tRect.left;
	FormProps[WinNum].top = tRect.top;
	FormProps[WinNum].width = tRect.right-tRect.left;
	FormProps[WinNum].height = tRect.bottom - tRect.top;
	if( PropWin != nil ) 
	{
		SetPort( PropWin );
		EraseRect( &PropWin->portRect);
		InvalRect( &PropWin->portRect);
		SetPort( FormWin[WinNum] );
	}


	OffsetRect( &theRect, -theRect.left, -theRect.top);
	InvalRect( &theRect );
}

short IsInGrow( short WinNum, Point pt )
{
	short		a=0, h, w;
	GrafPtr 	oldPort;
	Rect		theRect;
	
	if( WinNum < 0 || WinNum >=20) return 0;
	if( FormWin[WinNum]==nil) return 0;
	
	//theRect = Forms[WinNum].loc;
	theRect = FormWin[WinNum]->portRect;
	h = theRect.bottom - theRect.top;
	w = theRect.right - theRect.left;

	GetPort(&oldPort);
	SetPortWindowPort( FormWin[WinNum] );
	
	GlobalToLocal( &pt );
	theRect.top = h-10; theRect.bottom = h+1;
	theRect.left = w-10; theRect.right = w+1;
	if( PtInRect( pt, &theRect) ) a=1;
	SetPort(oldPort);
	return(a);
}

short MyGridDraw( short WinNum )
{
	short	x, y, gridsize, w, h;
	PenState	ps;	
	//Point		pt;
	GrafPtr 	oldPort;
	Rect		theRect;

	
	if( WinNum < 0 ) return 0;
	if( FormWin[WinNum]==nil) return 0;

	//theRect = Forms[WinNum].loc;
	theRect = FormWin[WinNum]->portRect;
	h = theRect.bottom - theRect.top;
	w = theRect.right - theRect.left;
 	x = FormProps[WinNum].value;
	if( x==3 || x==4 || x==7 || x==8 ) { w-=15; h -=15; }

	if( GetGridShow( WinNum ) )
	{
		gridsize = GetGridSize( WinNum );
		GetPort(&oldPort);
		SetPort( FormWin[WinNum] );
		GetPenState( &ps);
		PenNormal();
		for( y=0; y<h; y+=gridsize)
		{
			for( x=0; x<w; x+=gridsize) { MoveTo( x, y); Line (0,0); }
		}	
		SetPort(oldPort);
	}
	return 0;
}

short MyGrowBox( short WinNum )
{
	short	 h, w;
	PenState	ps;	
	GrafPtr 	oldPort;
	Rect		theRect;
	Rect 		tRect;
    RgnHandle		rh;
	
	if( WinNum < 0 ) return 0;
	if( FormWin[WinNum]==nil) return 0;
	
	//theRect = Forms[WinNum].loc;
	theRect = FormWin[WinNum]->portRect;
	h = theRect.bottom - theRect.top;
	w = theRect.right - theRect.left;

	GetPort(&oldPort);
	SetPortWindowPort( FormWin[WinNum] );
	GetPenState( &ps);
	PenNormal();
	theRect.top = h-10; theRect.bottom = h+1;
	theRect.left = w-10; theRect.right = w+1;
	FrameRect( &theRect);
	theRect.top +=3;; theRect.left += 3;
	//theRect.left +=2; theRect.right -= 2;
	FrameRect( &theRect);


	SetPort(oldPort);
	return 0;
}


void DoDrag( Point thePoint, WindowRef window )
{
	short a, b=-1;
	Rect limitR, tRect;
    RgnHandle		rh;
    Point	pt;

	//if ( gHasColorQD )
		limitR = ( **GetGrayRgn()).rgnBBox;
	//else
	//	limitR = qd.screenBits.bounds;

	SelectWindow( window );
	SetPort( window );
	a = CheckForEdit( window);
	if( a >=0 && a<20 )	CurrentForm = a;
	else
	{
		a = WhichWindow( window);
		if( a >=0 )	{ CurrentForm = a; b=a; }
	}
	DragWindow( window, thePoint, &limitR );
	if( b>= 0 )
	{
	 	tRect = window ->portRect;
	 	pt.h = tRect.left; pt.v = tRect.top;
	 	LocalToGlobal( &pt );
	 	//  (*((WindowPeek)window)->strucRgn)->rgnBBox;
	 	//tRect =  (((WindowPeek)window)->strucRgn)->rgnBBox;
	 	//tRect = (*rh)->rgnBBox;
	 	if( tRect.bottom - tRect.top > 40)
	 	{
			Forms[b].loc = tRect;
			//FormProps[b].left = tRect.left;
			//FormProps[b].top = tRect.top;
			FormProps[b].left = pt.h;
			FormProps[b].top = pt.v;
			FormProps[b].width = tRect.right-tRect.left;
			FormProps[b].height = tRect.bottom - tRect.top;
			if( PropWin != nil ) 
			{
				SetPort( PropWin );
	  			EraseRect( &PropWin->portRect);
				InvalRect( &PropWin->portRect);
				SetPort( window );
	 			EraseRect( &window->portRect);
				InvalRect( &window->portRect);
			}
		}
	}
}

void SetFormStatus( short fn, short st )
{
	Forms[fn].status = st;
}

short CreateFormWindow()
{
	OSErr		error	= noErr;
	Rect 	tempRect;
	short	a,c=-1,f;
	char	text1[256];
	Str255	text2;
	RGBColor	rgb;

	rgb.red = 0xcccc;	rgb.green = 0xcccc;		rgb.blue = 0xcccc;
	
	for(a=0;a<20;a++) if(FormWin[a]==nil ) {c=a; break;}
	
	if(c==-1) return(-1);
	
	FormCtr++;
	sprintf( text1, "Form%i", FormCtr);
	StrToStr255( text1, text2);

	f= c*10;
	SetRect(&tempRect,100+f,50+f,100+340+f,50+300+f);

	FormWin[c] = NewCWindow( nil, &tempRect, text2, true, 4, (WindowPtr) (-1), true, 0); 
	RGBBackColor(&rgb);
	EraseRect(&FormWin[c]->portRect);
	SelectWindow( FormWin[c] );
	SetPort(FormWin[c]);
	CurrentForm = c;
	Forms[c].status = 1;
	Forms[c].loc = tempRect;
	Forms[c].ctlSelected = -1;
	EditWin[c].theWindow = nil;
	EditWin[c].change = 1;
	EditWin[c].GlobalVar = nil;
	EditWin[c].GlobalVarSize = 0;
	EditWin[c].status = 1;

	
	Str255Copy ( text2, FormProps[c].name);
	Str255Copy ( text2, FormProps[c].text);
	Str255Copy ( text2, EditWin[c].FileName);
	FormProps[c].left = tempRect.left;
	FormProps[c].top = tempRect.top;
	FormProps[c].width = tempRect.right - tempRect.left;
	FormProps[c].height = tempRect.bottom - tempRect.top;
	Str255Copy ( "\pNone", FormProps[c].proc);
	FormProps[c].visible = 1;
	FormProps[c].value = 5;
	FormProps[c].enabled = 1;
	FormProps[c].visible = 1;
	Str255Copy ( "\pGeneva", FormProps[c].fontname);
	FormProps[c].fontsize = 12;
	FormProps[c].fontstyle = 0;
	FormProps[c].opt14=1;		FormProps[c].opt15=1;
	FormProps[c].opt16=8;		
	FormProps[c].opt17=100;		FormProps[c].opt18=2000;
	FormProps[c].opt19=100;		FormProps[c].opt20=2000;
	FormProps[c].opt21 = 0;
//	rgb.red = 0xffff;	rgb.green = 0xffff;		rgb.blue = 0xffff;  ///white
	rgb.red = 0xcccc;	rgb.green = 0xcccc;		rgb.blue = 0xcccc;
	FormProps[c].BackRGB = rgb;
	rgb.red = 0;	rgb.green = 0;		rgb.blue = 0;  ///black
	FormProps[c].ForeRGB = rgb;

	MyGridDraw ( c );
	MyGrowBox ( c );

	//if(appr1.gHaveAppearanceMgr)
	{
	//	SetThemeWindowBackground(FormWin[c],kThemeActiveDialogBackgroundBrush,true);
	}
		
	return error;

}

short RecreateFormWindow( short c)
{
	OSErr		error	= noErr;
	Rect 		tempRect;
	short		w, wt[]={ 4, 12, 0, 8, 4, 12, 0, 8, 1, 16, 2, 3 };
	RGBColor	rgb;

	// This sub creates window when loaded
	if(c==-1) return(-1);
	
	SetRect(&tempRect,FormProps[c].left,FormProps[c].top,FormProps[c].left+FormProps[c].width, FormProps[c].top+FormProps[c].height);
	
	w = FormProps[c].value;
	if( w == 9 || w >=11 ) FormWin[c] = NewCWindow( nil, &tempRect, FormProps[c].text, false, wt[w-1], (WindowPtr) (-1), false, 0); 
	else  FormWin[c] = NewCWindow( nil, &tempRect, FormProps[c].text, false, wt[w-1], (WindowPtr) (-1), true, 0); 
	
	SelectWindow( FormWin[c] );
	SetPort(FormWin[c]);
	CurrentForm = c;
	Forms[c].status = 1;
	Forms[c].loc = tempRect;
	Forms[c].ctlSelected = -1;
	EditWin[c].change = 0;
	EditWin[c].status = 2;
	
	RGBForeColor( &FormProps[c].ForeRGB );
	RGBBackColor( &FormProps[c].BackRGB );

	MyGridDraw ( c );
	MyGrowBox ( c );
	
	// Add controls ***********
	AddLoadCtls( c , FormWin[c] );
	ShowWindow( FormWin[c] );
    //EraseRect(  &FormWin[c]->portRect );
    //InvalRect( &FormWin[c]->portRect );

	{
	//	SetThemeWindowBackground(FormWin[c],kThemeActiveDialogBackgroundBrush,true);
	}
	
	return error;

}

short SafeForm( WindowPtr window)
{
	OSErr		error	= noErr;
	Rect 	tempRect;
	short	a,c=-1,f, bb=-1, b;
	char	text1[256];
	Str255	text2;
	RGBColor	rgb;

	
	for(a=0;a<20;a++) if(FormWin[a]==window ) {c=a; break;}
	if(c==-1) return(-1);

	if( ActiveTE !=nil )
	{
		TEDeactivate( ActiveTE );
		ActiveTE = nil;
	}
	
	
	if( EditWin[c].theWindow !=nil )
	{
		bb=CloseEditWindow( EditWin[c].theWindow);
		if( bb==-2 ) return -2;
	}
	
	if( EditWin[c].change )
	{
		
		ParamText( EditWin[c].FileName, "\pclosing", nil, nil );
		//SetCursor( &qd.arrow );

		b = Alert( 131, nil );

			// exit if the user canceled the alert box
		if ( b == 2 ) return -2;
		if ( b == 1 ) SaveSourceFile( window );
			// if b==3 then don't save
	}
	return 0;
}

short CloseForm( WindowPtr window)
{
	OSErr		error	= noErr;
	Rect 	tempRect;
	short	a,c=-1,f, bb=-1, b;
	char	text1[256];
	Str255	text2;
	RGBColor	rgb;

	
	for(a=0;a<20;a++) if(FormWin[a]==window ) {c=a; break;}
	if(c==-1) return(-1);

	if( ActiveTE !=nil )
	{
		TEDeactivate( ActiveTE );
		ActiveTE = nil;
	}
	
	
	if( EditWin[c].theWindow !=nil )
	{
		bb=CloseEditWindow( EditWin[c].theWindow);
		if( bb==-2 ) return -2;
	}

	if( EditWin[c].change )
	{
		
		ParamText( EditWin[c].FileName, "\pclosing", nil, nil );
		//SetCursor( &qd.arrow );

		b = Alert( 131, nil );

			// exit if the user canceled the alert box
		if ( b == 2 ) return -2;
		if ( b == 1 ) SaveSourceFile( window );
			// if b==3 then don't save
	}

	
	//SaveSourceFile(window );
	
	// Now let's clean up form stuff
	
	// Remove controls
	// Find last control #
	for( a=0; a<200; a++)
	{
		if( CtlData[a].status && CtlData[a].WinNum == c )
		{
			switch( CtlData[a].status )
			{
				case 2:  // Label
				case 3:  // TextBox
					if( CtlData[a].te!= nil ) TEDispose( CtlData[a].te );
					CtlData[a].te = nil;
					break;
				case 8: // DropList
					DeleteMenu( a+10000 );
					if( CtlData[a].theMenu!= nil ) DisposeMenu( CtlData[a].theMenu );
					break;
				case 5:
					//if( CtlData[a].pic != nil ) { ReleaseResource( (Handle) CtlData[a].pic ); CtlData[a].pic=nil; }
					if( CtlData[a].pic != nil ) { KillPicture(  CtlData[a].pic ); CtlData[a].pic=nil; }
					break;
				case 10:
					if( CtlData[a].lt !=nil ) LDispose( CtlData[a].lt );
			}
			CtlData[a].status = 0;						
		}
	}

	DisposeWindow( FormWin[c] );
	FormWin[c] = nil;
	// Remove edit proc from memory
   	for( b=0; b<1000; b++)
	{
		if( EditProc[b].ProcHandle != nil)
		{
			if( EditProc[b].EditWinNum == c)
			{
				DisposeHandle( EditProc[b].ProcHandle);
				EditProc[b].ProcHandle = nil;
				EditProc[b].size = 0;
				EditProc[b].EditWinNum = -1;
			}
		}
	}	
	
	CurrentForm = -1;
	Forms[c].status = 0;
	Forms[c].ctlSelected = -1;
	EditWin[c].change = 0;
	EditWin[c].status = 0;
	EditWin[c].GlobalVar = nil;
	EditWin[c].GlobalVarSize = 0;
	// Now check if there are no forms open, if none close propwin
	if( PropWin!=nil )
	{
		for(a=0;a<20;a++)
		{
			if(FormWin[a]!=nil )
			{
				SelectWindow( FormWin[a] );
				SetPort( FormWin[a] );
				CurrentForm = a;
				SetPort( PropWin );
				tempRect = PropWin->portRect;
 				tempRect.right-=15;
			 	EraseRect( &tempRect );
				InvalRect( &tempRect );
				SetPort( FormWin[a] );
				return;
			}
		}
		//Ok none open close PropWin
		DisposeWindow( PropWin );
		PropWin = nil;
	}

}

void ZapForm( short c )
{
	OSErr		error	= noErr;
	Rect 	tempRect;
	short	a,f, bb=-1, b;
	char	text1[256];
	Str255	text2;
	RGBColor	rgb;

	if(c==-1) return;
	if(FormWin[c]==nil ) return;

	if( ActiveTE !=nil )
	{
		TEDeactivate( ActiveTE );
		ActiveTE = nil;
	}
	
	// Remove controls
	// Find last control #
	for( a=0; a<200; a++)
	{
		if( CtlData[a].status && CtlData[a].WinNum == c )
		{
			switch( CtlData[a].status )
			{
				case 2:  // Label
				case 3:  // TextBox
					if( CtlData[a].te!= nil ) TEDispose( CtlData[a].te );
					CtlData[a].te = nil;
					break;
				case 8: // DropList
					DeleteMenu( a+10000 );
					if( CtlData[a].theMenu!= nil ) DisposeMenu( CtlData[a].theMenu );
					break;
				case 4:
				case 5:
					//if( CtlData[a].pic != nil ) ReleaseResource( CtlData[a].pic );
					if( CtlData[a].pic != nil ) { KillPicture(  CtlData[a].pic ); CtlData[a].pic=nil; }
					break;
				case 10:
					if( CtlData[a].lt !=nil ) LDispose( CtlData[a].lt );
			}
		}
	}
	DisposeWindow( FormWin[c] );
	FormWin[c] = nil;

}


short UpdateFormWin( WindowPtr window )			
{
	short a,b,c, d, flag1, fg=0, bg=0, z1, z2, rf;
	GrafPtr		savePort;
	Rect	tr;
	Str255	text1;
	for(a=0;a<100;a++)
	{
		if( FormWin[a] == window )
		{
			GetPort( &savePort );
			SetPortWindowPort( window );
			b = WhichWindow( window );
			BeginUpdate(window);
				ClearMyWindow( b );
				PenNormal();
				MyGridDraw ( b );
				//DrawControls( window );
				for(d=0;d<200;d++)		// # of controls
				{
					if( CtlData[d].WinNum == a )
					{
						switch( CtlData[d].status )
						{
							case 6:	  // CheckBox
							case 7:   // Radio
								RGBForeColor( &FormProps[a].ForeRGB );
								RGBBackColor( &FormProps[a].BackRGB );
								Draw1Control( CtlData[d].ctl );
								break;
							case 1:   // Button
								RGBForeColor( &FormProps[a].ForeRGB );
								RGBBackColor( &FormProps[a].BackRGB );
								if( ObjectProps[d].value )
								{
									PenSize( 3,3);
									tr = CtlData[d].theRect; 
									InsetRect( &tr, -4, -4 );
									FrameRoundRect( &tr, 16,16 );
									PenNormal();
								}
								Draw1Control( CtlData[d].ctl );
								break;
							case 8:   // DropLists
							case 11:  // Scrollbar
								RGBForeColor( &FormProps[a].ForeRGB );
								RGBBackColor( &FormProps[a].BackRGB );
								Draw1Control( CtlData[d].ctl );
								break;
							case 2:	  // Label
							case 3:	  // TextBox
								RGBForeColor( &ObjectProps[d].ForeRGB );
								RGBBackColor( &ObjectProps[d].BackRGB );
								if( CtlData[d].status==3) EraseRect ( &CtlData[d].theRect );
								TEUpdate( &CtlData[d].theRect, CtlData[d].te );
								if( ObjectProps[d].opt21 )
								{
									tr = CtlData[d].theRect;
									InsetRect ( &tr, -1 ,-1 );
									FrameRect ( &tr );
								}
								if( CtlData[d].ctl !=nil ) Draw1Control( CtlData[d].ctl ); // TE scrollbar
								RGBForeColor( &FormProps[a].ForeRGB );
								RGBBackColor( &FormProps[a].BackRGB );
								break;
							/*case 4: //Icon
								flag1=0;
								RGBForeColor( &FormProps[a].ForeRGB );
								RGBBackColor( &FormProps[a].BackRGB );
								EraseRect ( &CtlData[d].theRect );
								if( ObjectProps[d].value && CtlData[d].pic!=nil)
								{
									z2 = PlotIconSuite(&CtlData[d].theRect, atAbsoluteCenter, ttNone, (Handle ) CtlData[d].pic);
									flag1++;
								}
								else
								{	// empty
									MoveTo( CtlData[d].theRect.left+2, CtlData[d].theRect.top+12 );
									DrawString( ObjectProps[d].name );
								}
								if( !flag1 || ObjectProps[d].opt21 )
								{
									FrameRect ( &CtlData[d].theRect );
								}
								break;*/
							/*case 4: //Icon
								flag1=0;
								RGBForeColor( &FormProps[a].ForeRGB );
								RGBBackColor( &FormProps[a].BackRGB );
								EraseRect ( &CtlData[d].theRect );
								if( ObjectProps[d].value )
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
											z2 = PlotIconID(&CtlData[d].theRect, atAbsoluteCenter, ttNone, ObjectProps[d].value);
											CloseResFile( rf );
											if( !z2 ) {flag1++; break; }
										}
										UseResFile(VDE_ResFileNum);
									}
								}
								else
								{	// empty
									MoveTo( CtlData[d].theRect.left+2, CtlData[d].theRect.top+12 );
									DrawString( ObjectProps[d].name );
								}
								if( !flag1 || ObjectProps[d].opt21 )
								{
									FrameRect ( &CtlData[d].theRect );
								}
								break;*/
							case 4: // Icon
							case 5: // Picture
								flag1=0;
								RGBForeColor( &FormProps[a].ForeRGB );
								RGBBackColor( &FormProps[a].BackRGB );
								EraseRect ( &CtlData[d].theRect );
								if( ObjectProps[d].value )
								{
									if(CtlData[d].pic !=nil )
									{
										DrawPicture(  CtlData[d].pic, &CtlData[d].theRect);
										flag1++;
									}
								}
								if(  !flag1 || ObjectProps[d].opt21 )
								{
									FrameRect ( &CtlData[d].theRect );
									MoveTo( CtlData[d].theRect.left+2, CtlData[d].theRect.top+12 );
									DrawString( ObjectProps[d].name );
								}
								break;
							case 9:
								RGBForeColor( &ObjectProps[d].ForeRGB );
								RGBBackColor( &ObjectProps[d].BackRGB );
								EraseRect ( &CtlData[d].theRect );
								LUpdate( window->visRgn, CtlData[d].lt );
								if( ObjectProps[d].opt21 )
								{
									tr = CtlData[d].theRect;
									InsetRect ( &tr, -1 ,-1 );
									FrameRect ( &tr );
								}
								RGBForeColor( &FormProps[a].ForeRGB );
								RGBBackColor( &FormProps[a].BackRGB );
								break;
							case 10:
								SetResLoad(true);
								PlotIconID(&CtlData[d].theRect, 0, 0, 5011);
								break;

						}			
					}
				}
				c = Forms[b].ctlSelected;
				RGBForeColor( &FormProps[a].ForeRGB);
				RGBBackColor( &FormProps[a].BackRGB);
				
				//if(appr.gHaveAppearanceMgr)
				{
					SetThemeWindowBackground(PropWin,kThemeActiveDialogBackgroundBrush,true);
				}
				
				SelectMyControl( c );
				z2 = FormProps[a].value;
				if( z2 == 3 || z2 == 4 || z2 == 7 || z2 == 8 ) DrawGrowIcon( window );
				else MyGrowBox ( b );
			EndUpdate(window);
			// restore the old graphics port
			SetPort( savePort );
			
			return 1;
		}
	}
	
	{
	//	SetThemeWindowBackground(FormWin[c],kThemeActiveDialogBackgroundBrush,true);
	}

	return 0; // No, a form window didn't request an update
}

void RedrawFormWin()
{
	GrafPtr	gp;
	Rect	r2;
	
	GetPort( &gp );
	SetPort(FormWin[CurrentForm]);
	r2 = FormWin[CurrentForm]->portRect;
	InvalRect( &r2 );
	EraseRect( &r2 );

	{
	//	SetThemeWindowBackground(FormWin[CurrentForm],kThemeActiveDialogBackgroundBrush,true);
	}

	SetPort( gp );
}

void AddStationary()
{
	Str255	text1;
	long	bl;

	Str255Copy( "\pStart\x0d", text1 );
	Str255Cat(  "\p    Dim A\x0d\x0d", text1 );
	Str255Cat(  "\p    CreateTheMenus\x0d", text1 );
	Str255Cat(  "\p    Form Form1\x0d\x0d", text1 );
	Str255Cat(  "\p    A=0\x0d", text1 );
	Str255Cat(  "\p    Do While A=0\x0d", text1 );
	Str255Cat(  "\p    Loop\x0d", text1 );
	Str255Cat(  "\pFinish\x0d", text1 );
	bl=text1[0];
	PtrToHand( text1+1, &EditProc[0].ProcHandle, bl);
	EditProc[0].size = bl;
	EditProc[0].EditWinNum = 0;
		
	Str255Copy( "\pSub CreateTheMenus( )\x0d", text1 );
	Str255Cat(  "\p    MenuApple\x0d", text1 );
	Str255Cat(  "\p    MenuAddItem 1, \"About Application\", \"\", AboutTheApp\x0d", text1 );
	Str255Cat(  "\p    MenuDesk\x0d", text1 );
	Str255Cat(  "\p    MenuAdd 2, \"File\", None\x0d", text1 );
	Str255Cat(  "\p    MenuAddItem 2, \"Quit\", \"Q\", QuitTheApp\x0d\x0d", text1 );
	Str255Cat(  "\pEndSub\x0d", text1 );
	bl=text1[0];
	PtrToHand( text1+1, &EditProc[1].ProcHandle, bl);
	EditProc[1].size = bl;
	EditProc[1].EditWinNum = 0;

 	Str255Copy( "\pSub QuitTheApp( )\x0d", text1 );
	Str255Cat(  "\p    End\x0d", text1 );
	Str255Cat(  "\pEndSub\x0d", text1 );
	bl=text1[0];
	PtrToHand( text1+1, &EditProc[2].ProcHandle, bl);
	EditProc[2].size = bl;
	EditProc[2].EditWinNum = 0;
 
 	Str255Copy( "\pSub AboutTheApp( )\x0d", text1 );
	Str255Cat(  "\p    Dim a\x0d", text1 );
	Str255Cat(  "\p    a = MsgBox (\"Application Info\", 0)\x0d", text1 );
	Str255Cat(  "\pEndSub\x0d", text1 );
	bl=text1[0];
	PtrToHand( text1+1, &EditProc[3].ProcHandle, bl);
	EditProc[3].size = bl;
	EditProc[3].EditWinNum = 0;

}


	//MenuApple
	//MenuAddItem 1, "About DrawPad Demo", "", AboutMyProgram
	//MenuDesk

	//MenuAdd 2, "File", None
	//MenuAddItem 2, "Quit", "Q", QuitMyApp	
 	//Sub AboutTheApp()
	//	Dim a	
	//	a = MsgBox ("Application Info"+chr(13), 0)
	//EndSub


/*void DoUpdate( WindowRef window )
{
	GrafPtr		savePort;
	RgnHandle	updateRgn;
	PenState	ps;
	Rect		r;
	WEReference		we;
	long a;
	Str255		txt;


	// if we have no windows, there's nothing to update!
	if ( window == nil )
		return;

	// save the old drawing port
	GetPort( &savePort );
	SetPortWindowPort( window );

	// notify everything that we're doing an update.
	BeginUpdate( window );

	// BeginUpdate sets the window port visRgn to the region to update
	updateRgn = GetWindowPort(window)->visRgn;

	if ( !EmptyRgn( updateRgn ) )	// if it's not an empty region, let's update it!
	{
		// erase the update region
		EraseRgn( updateRgn );

		//	draw scroll bars
		UpdateControls( window, updateRgn );
		
	}
	// tell everything we're done updating
	EndUpdate( window );

	// restore the old graphics port
	SetPort( savePort );
}*/

/*void Resize( Point newSize, WindowRef window )
{
	DocumentHandle	hDocument;
	GrafPtr			savePort;
	Rect			r;
	RgnHandle		tempRgn, dirtyRgn;

	GetPort( &savePort );
	SetPortWindowPort( window );

	hDocument = GetWindowDocument( window );

	//	create temporarty regions for calculations
	tempRgn = NewRgn();
	dirtyRgn = NewRgn();

	//	save old text region

	CalcTextRect( window, &r );
	RectRgn( tempRgn, &r );

	//	erase the old grow icon rect
	CalcGrowIconRect( window, &r );
	EraseRect( &r );

	//	hide the scroll bars

	HideControl( (*hDocument)->scrollBars[ v ] );
	//RMH5 HideControl( (*hDocument)->scrollBars[ h ] );

	//	perform the actual resizing of the window, redraw scroll bars and grow icon
	SizeWindow( window, newSize.h, newSize.v, false );
	ViewChanged( window );
	MyDrawGrowIcon( window, true );

	//	calculate the dirty region (to be updated)
	CalcTextRect( window, &r );
	RectRgn( dirtyRgn, &r );
	XorRgn( dirtyRgn, tempRgn, dirtyRgn );
	InsetRect( &r, -kTextMargin, -kTextMargin );
	RectRgn( tempRgn, &r );
	SectRgn( dirtyRgn, tempRgn, dirtyRgn );

	//	mark the dirty region as invalid
	InvalRgn( dirtyRgn );

	//	throw away temporary regions
	DisposeRgn( tempRgn );
	DisposeRgn( dirtyRgn );

	SetPort( savePort );
}

void DoGrow( Point hitPt, WindowRef window )
{
	Rect sizeRect;
	long newSize;

	SetRect( &sizeRect, kMinWindowWidth, kMinWindowHeight, SHRT_MAX, SHRT_MAX );
	if ( ( newSize = GrowWindow( window, hitPt, &sizeRect ) ) != 0L )
	{
		//	for some reason, GrowWindow( ) returns a long value,
		//	but it's really a Point

		Resize( * (Point *) &newSize, window );
	}
}

void DoZoom( short partCode, WindowRef window )
{
	DocumentHandle	hDocument;
	GrafPtr			savePort;
	Rect			r;

	GetPort( &savePort );
	SetPortWindowPort( window );

	hDocument = GetWindowDocument(window);

	r = GetWindowPort( window )->portRect;
	EraseRect( &r );
	HideControl( (*hDocument)->scrollBars[ v ] );
	//RMH5 HideControl( (*hDocument)->scrollBars[ h ] );

	ZoomWindow( window, partCode, false );

	ViewChanged( window );
	CalcTextRect( window, &r );
	InvalRect( &r );

	SetPort( savePort );
}
*/