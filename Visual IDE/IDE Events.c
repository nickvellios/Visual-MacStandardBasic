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


#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
#endif

#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

#ifndef __DISKINIT__
#include <DiskInit.h>
#endif

#ifndef __TEXTSERVICES__
#include <TextServices.h>
#endif


#include "IDE.h"
#include "Windows.h"
#include "Icons.h"
#include <SIOUX.h>
#include <DCon.h>
#include <math.h>

void BV_FindTool(const EventRecord *currEvent);
void DeleteSelectedControl( );


short IsModuleEdit(WindowPtr window);
void PropDisregard( );
void EditPager( short WinNum, short partCode );
void CloseErrorWindow( );
void UpdateErrWin( WindowPtr window );		
short DoErrGrow( Point hitPt, WindowRef window );
void DoMenuChoice( long menuChoice, const EventRecord *theEvent );
short UpdateFormWin( WindowPtr window );  // from IDE Windows.c
short UpdateEditWin( WindowPtr window );  // from IDE Edit1.c		
short DoEditGrow( Point hitPt, WindowRef window );  // from IDE Edit1.c	
//short  EditKeyDown( EventRecord theEvent );
short  EditKeyDown( const EventRecord *theEvent );
short CheckForEdit(WindowPtr window);  // from IDE Edit1.c	
short CheckEditMouse( short WinNum, Point pt );  // from IDE Edit1.c	
void TextFix( WindowPtr window );
short UpdatePropWin( WindowPtr window );			
short PropEvent( Point pt );
short DoPropGrow( Point hitPt, WindowRef window );
void BV_DrawToolBar(); 
void AddMyControl( short ctlNum, short ctlType, short formNum );
void PropDispose( );
void FormEditorCall( short ctlNum);
short DeActEdit( WindowPtr window);
void EditorDispose();
short CloseEditWindow( WindowPtr window);
void DoAutoIndent( );
void DoTab( );
void DoBeginLine( );
void DoEndLine( );
void MarkChangeTE( );
short CloseForm( WindowPtr window);
void	PrepareMenus( void );
short IsFormEdit(WindowPtr window);
short FindTEwindow( );
void DoDrag( Point thePoint, WindowRef window );


TEHandle	ActiveTE=nil;
extern WindowPtr	PropWin;
extern ControlHandle	PropScroll;
extern short CurrentForm, numEntryFlag, nospaceEntryFlag, EditorFlag;
extern TEHandle	PropTE;
extern long  DblClickThreshold, LastMouseTick;
extern Point  LastMousePoint;
extern WindowPtr ErrorWin;
extern TEHandle	 ErrorTE;
short ErrWinMouse( Point pt );


short	DblClickFlag=0, textAnchor = 0;
extern short	undoCode, undoStart, undoEnd;

#define mypushButProc 0

static UInt32		sSleepTime = 0;			// sleep time for WaitNextEvent()
static RgnHandle	sMouseRgn = nil;		// mouse region for WaitNextEvent()
char	CmdKeyFlag;
char		AppLaunchFlag=0;

short CompilerErrorLog(  );
int LoadProject( );
short WhichWindow( WindowPtr window);
short GetGridSize( short WinNum );
short GetGridFlag( short WinNum );
short MyGridDraw( short WinNum );
short MyGrowBox( short WinNum );
void SetWinRect( short WinNum, short w, short h );
void ClearMyWindow( short WinNum );
short IsInGrow( short WinNum, Point pt );

short MoveBox(	WindowRef window, Rect *PassRect );
short ResizeBox(  WindowRef window, Rect *PassRect );

extern WindowPtr	ToolPal;
extern short		currTool;
extern char MasterExitFlag;

extern struct	FormWins
{
	short		WinType;
	Rect		loc;
	short		ctlSelected;
	short		status;

} Forms[20];

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

extern WindowPtr	FormWin[20];

Rect	MasterRect;

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

void ControlMove( short ct )
{
	GrafPtr	gp;
	short temp;
	Rect	r;
	
	EditWin[CurrentForm].change = 1;
	switch( CtlData[ct].status )
	{
		case 1:
		case 6:
		case 7:
		case 8:
		case 11:
			CtlData[ct].theRect = MasterRect;
			ObjectProps[ct].left = MasterRect.left;
			ObjectProps[ct].top = MasterRect.top;
			ObjectProps[ct].width = MasterRect.right - MasterRect.left;
			ObjectProps[ct].height = MasterRect.bottom - MasterRect.top;
			MoveControl( CtlData[ct].ctl, MasterRect.left, MasterRect.top);	
			r =CtlData[ct].theRect;
			InsetRect( &r, -5, -5 );
			InvalRect( &r );
			break;
		case 2:
		case 3:
			InvalRect( &CtlData[ct].theRect );
			CtlData[ct].theRect = MasterRect;
			ObjectProps[ct].left = MasterRect.left;
			ObjectProps[ct].top = MasterRect.top;
			ObjectProps[ct].width = MasterRect.right - MasterRect.left;
			ObjectProps[ct].height = MasterRect.bottom - MasterRect.top;
			temp = MasterRect.right - MasterRect.left;
			(*CtlData[ct].te)->viewRect.left = MasterRect.left;
			(*CtlData[ct].te)->viewRect.right = MasterRect.left + temp - (ObjectProps[ct].opt14*16);
			temp = MasterRect.bottom - MasterRect.top;
			(*CtlData[ct].te)->viewRect.top = MasterRect.top;
			(*CtlData[ct].te)->viewRect.bottom = MasterRect.top + temp ;
			if( ObjectProps[ct].value )
			{
				r = (*CtlData[ct].te)->viewRect;
				r.right = 2000;
				(*CtlData[ct].te)->destRect = r;
			}
			else (*CtlData[ct].te)->destRect = (*CtlData[ct].te)->viewRect;

			if( ObjectProps[ct].opt14 && CtlData[ct].ctl!=nil )
			{
				r =CtlData[ct].theRect;
				r.left = r.right - 16;
				MoveControl( CtlData[ct].ctl, r.left, r.top);	
				SizeControl( CtlData[ct].ctl, r.right-r.left, r.bottom-r.top );	
			}
			TECalText( CtlData[ct].te );
			r =CtlData[ct].theRect;
			InsetRect( &r, -3, -3 );
			InvalRect( &r );
			break;
		case 4:
		case 5:
		case 10:
			InvalRect( &CtlData[ct].theRect );
			CtlData[ct].theRect = MasterRect;
			ObjectProps[ct].left = MasterRect.left;
			ObjectProps[ct].top = MasterRect.top;
			ObjectProps[ct].width = MasterRect.right - MasterRect.left;
			ObjectProps[ct].height = MasterRect.bottom - MasterRect.top;
			InvalRect(  &CtlData[ct].theRect );
			break;
		case 9: // List Box
			CtlData[ct].theRect = MasterRect;
			ObjectProps[ct].left = MasterRect.left;
			ObjectProps[ct].top = MasterRect.top;
			ObjectProps[ct].width = MasterRect.right - MasterRect.left;
			ObjectProps[ct].height = MasterRect.bottom - MasterRect.top;
			temp = MasterRect.right - MasterRect.left;
			(*CtlData[ct].lt)->rView.left = MasterRect.left;
			(*CtlData[ct].lt)->rView.right = MasterRect.left + temp ;
			temp = MasterRect.bottom - MasterRect.top;
			(*CtlData[ct].lt)->rView.top = MasterRect.top;
			(*CtlData[ct].lt)->rView.bottom = MasterRect.top + temp ;
			LSize( (MasterRect.right - MasterRect.left), (MasterRect.bottom - MasterRect.top), CtlData[ct].lt );
			r =CtlData[ct].theRect;
			r.right+=17;
			InsetRect( &r, -3, -3 );
			InvalRect( &r );
			break;
	}
  	if( PropWin !=nil )
  	{
  		GetPort( &gp );  SetPort( PropWin );
  		EraseRect( &PropWin->portRect);
		InvalRect( &PropWin->portRect);
		SetPort( gp );
	}
}

void ControlResize( short ct )
{
	GrafPtr	gp;
	Rect	r;	
	short temp;
	
	EditWin[CurrentForm].change = 1;
	switch( CtlData[ct].status )
	{
		case 8:
			MasterRect.bottom = MasterRect.top+20;
		case 1:
		case 6:
		case 7:
		case 11:
			CtlData[ct].theRect = MasterRect;
			ObjectProps[ct].left = MasterRect.left;
			ObjectProps[ct].top = MasterRect.top;
			ObjectProps[ct].width = MasterRect.right - MasterRect.left;
			ObjectProps[ct].height = MasterRect.bottom - MasterRect.top;
			SizeControl( CtlData[ct].ctl, MasterRect.right-MasterRect.left, MasterRect.bottom-MasterRect.top );	
			InvalRect( &CtlData[ct].theRect );
			break;
		case 2:
		case 3:
			CtlData[ct].theRect = MasterRect;
			ObjectProps[ct].left = MasterRect.left;
			ObjectProps[ct].top = MasterRect.top;
			ObjectProps[ct].width = MasterRect.right - MasterRect.left;
			ObjectProps[ct].height = MasterRect.bottom - MasterRect.top;
			/*(*CtlData[ct].te)->viewRect.right = (*CtlData[ct].te)->viewRect.left + (MasterRect.right - MasterRect.left);
			(*CtlData[ct].te)->viewRect.bottom = (*CtlData[ct].te)->viewRect.top + (MasterRect.bottom - MasterRect.top) ;
			CtlData[ct].theRect = (*CtlData[ct].te)->viewRect;
			(*CtlData[ct].te)->viewRect.right = (*CtlData[ct].te)->viewRect.left + (MasterRect.right - MasterRect.left - (ObjectProps[ct].opt14*16));
			if( CtlData[ct].status == 3 )
			{
				if( ObjectProps[ct].opt14 )
				{
					r =CtlData[ct].theRect;
					

				  && ObjectProps[ct].value)
			{
				r = (*CtlData[ct].te)->viewRect;
				if( ObjectProps[ct].opt14 )
				{
					r.right-=16;
				if( ObjectProps[ct].value ) r.right = 2000;
				(*CtlData[ct].te)->destRect = r;
				}
			}
			else  (*CtlData[ct].te)->destRect = (*CtlData[ct].te)->viewRect;
			if( ObjectProps[ct].opt14 && CtlData[ct].ctl!=nil )
			{
				r.left = r.right - 16;
				MoveControl( CtlData[ct].ctl, r.left, r.top);	
				SizeControl( CtlData[ct].ctl, r.right-r.left, r.bottom-r.top );	
			}*/
			temp = MasterRect.right - MasterRect.left;
			(*CtlData[ct].te)->viewRect.left = MasterRect.left;
			(*CtlData[ct].te)->viewRect.right = MasterRect.left + temp - (ObjectProps[ct].opt14*16);
			temp = MasterRect.bottom - MasterRect.top;
			(*CtlData[ct].te)->viewRect.top = MasterRect.top;
			(*CtlData[ct].te)->viewRect.bottom = MasterRect.top + temp ;
			if( ObjectProps[ct].value )
			{
				r = (*CtlData[ct].te)->viewRect;
				r.right = 2000;
				(*CtlData[ct].te)->destRect = r;
			}
			else (*CtlData[ct].te)->destRect = (*CtlData[ct].te)->viewRect;

			if( ObjectProps[ct].opt14 && CtlData[ct].ctl!=nil )
			{
				r =CtlData[ct].theRect;
				r.left = r.right - 16;
				MoveControl( CtlData[ct].ctl, r.left, r.top);	
				SizeControl( CtlData[ct].ctl, r.right-r.left, r.bottom-r.top );	
			}
			TECalText( CtlData[ct].te );
			r =CtlData[ct].theRect;
			InsetRect( &r, -3, -3 );
			InvalRect( &r );
			break;
		case 4:
		case 5:
			CtlData[ct].theRect = MasterRect;
			ObjectProps[ct].left = MasterRect.left;
			ObjectProps[ct].top = MasterRect.top;
			ObjectProps[ct].width = MasterRect.right - MasterRect.left;
			ObjectProps[ct].height = MasterRect.bottom - MasterRect.top;
			r =CtlData[ct].theRect;
			InsetRect( &r, -3, -3 );
			InvalRect( &r );
			break;
		case 9:
			CtlData[ct].theRect = MasterRect;
			ObjectProps[ct].left = MasterRect.left;
			ObjectProps[ct].top = MasterRect.top;
			ObjectProps[ct].width = MasterRect.right - MasterRect.left;
			ObjectProps[ct].height = MasterRect.bottom - MasterRect.top;

			temp = MasterRect.right - MasterRect.left;
			(*CtlData[ct].lt)->cellSize.h = temp ;

			LSize( (MasterRect.right - MasterRect.left), (MasterRect.bottom - MasterRect.top), CtlData[ct].lt );
			r =CtlData[ct].theRect;
			r.right+=16;
			InsetRect( &r, -3, -3 );
			InvalRect( &r );
			break;
		// case 10  Timer doesn't resize

	}
  	if( PropWin !=nil )
  	{
  		GetPort( &gp );  SetPort( PropWin );
  		EraseRect( &PropWin->portRect);
		InvalRect( &PropWin->portRect);
		SetPort( gp );
	}
}

short FindEmptyControl()
{
	short i;
	for(i=0;i<200;i++) if( !CtlData[i].status ) return(i);
	return(-1);
}

void MyDrawFrame( short x1, short y1, short x2, short y2 )
{
	short	z;
	Rect		theRect;

	if( x1 > x2 ) { z=x2; x2=x1; x1=z; }
	if( y1 > y2 ) { z=y2; y2=y1; y1=z; }
	SetRect( &theRect, x1, y1, x2, y2 );
	MasterRect = theRect;
	FrameRect( &theRect);
}

void SelectMyControl( short ct )
{
	short a, b;
	Rect theRect, rect2, tr;
	PenState	ps;	
	GrafPtr		savePort;
		
	if( ct < 0 ) return;
	a = CtlData[ct].WinNum;
	b = Forms[a].ctlSelected;
	if( b != ct )
	{
		if( b >=0 )
		{
			rect2 = CtlData[b].theRect;
//			InsetRect( &rect2, -2, -2 );
			rect2.right+=17;
			InsetRect( &rect2, -3, -3 );
			InvalRect( &rect2 );
			EraseRect( &rect2 );
			DrawControls(FormWin[a]);
			switch( CtlData[b].status )
			{
				case 2:
				case 3:
					TEUpdate( &CtlData[b].theRect, CtlData[b].te );
					if( CtlData[b].status== 3)
					{
						tr = CtlData[b].theRect;
						InsetRect ( &tr, -1 ,-1 );
						FrameRect ( &tr );
					}
					break;
				case 9:
					LUpdate( FormWin[a]->visRgn, CtlData[b].lt );
					tr = CtlData[b].theRect;
					InsetRect ( &tr, -1 ,-1 );
					FrameRect ( &tr );
					break;
			}
		}
		
		// Now update properties window
	  	if( PropWin !=nil )
	  	{
			GetPort( &savePort );
			SetPort( PropWin );
			tr = PropWin->portRect;
			SetControlMaximum(PropScroll, 1);
			SetControlValue(PropScroll, 1);
	 		InvalRect( &tr );
			EraseRect( &tr );
			SetPort( savePort );
		}
		//SelectWindow(FormWin[a]);
		//SetPort( FormWin[a] );
		
	}
	GetPenState( &ps);
	PenNormal();
	theRect =  CtlData[ct].theRect;
	if( CtlData[ct].status ==9 ) theRect.right+=16;
//	InsetRect( &theRect, -2, -2 );
	InsetRect( &theRect, -3, -3 );
	FrameRect( &theRect );

	theRect.top = theRect.bottom-8;
	theRect.left = theRect.right-8;
	FrameRect( &theRect );
	theRect.top-=2;
	theRect.left-=2;
	FrameRect( &theRect );

	Forms[a].ctlSelected = ct;
	SetPenState( &ps);
	return;
}

void ArrowSelect( Point pt, short flagger)  // Arrow Picker selects control if avail
{
	short i, a, v, b=0;
	WindowPtr window;
	Rect	r2;
	GrafPtr		savePort;
	
	GlobalToLocal(&pt);
	// The following few lines allow for testing current control first
	v = Forms[CurrentForm].ctlSelected;
	r2 = CtlData[v].theRect;
	InsetRect( &r2, -3, -3 );
	if( CtlData[v].status == 9 ) r2.right +=16;
	if( PtInRect( pt, &r2 ) ) b = v; 
	else v = 0;

	for(i=v;i<200;i++)
	{
		if( CtlData[i].status && CtlData[i].WinNum == CurrentForm) 
		{
			r2 = CtlData[i].theRect;
//			InsetRect( &r2, -2, -2 );
			InsetRect( &r2, -3, -3 );
			if( CtlData[i].status == 9 ) r2.right +=16;
			if( PtInRect( pt, &r2 ) )
			{
				SelectMyControl( i );
				a = CtlData[i].WinNum;
				if( a>=0 )
				{
					window = FormWin[a];
					r2.left = r2.right - 10;	
					r2.top  = r2.bottom - 10;
					if( DblClickFlag )
					{
						FormEditorCall( i );
					}
					else
					{
						if( PtInRect( pt, &r2 ) )
						{
							ResizeBox( window, &CtlData[i].theRect );
							ControlResize( i );
							SelectMyControl( i );
						}
						else
						{
							MoveBox( window, &CtlData[i].theRect );
							ControlMove( i );
							SelectMyControl( i );
						}
					}
				}
				return;
			}
		}
	}
	if( CurrentForm >= 0 )
	{
		if( DblClickFlag )
		{
			if( flagger ) FormEditorCall( -1 );
			else FormEditorCall( -2 );
		}
		else //if( Forms[CurrentForm].ctlSelected>=0 )
		{
			GetPort( &savePort );
			Forms[CurrentForm].ctlSelected = -1;
			window = FormWin[CurrentForm];
			r2 = window->portRect;
			SetPort( window );
			SetControlMaximum(PropScroll, 1);
			SetControlValue(PropScroll, 1);
			InvalRect( &r2 );
			EraseRect( &r2 );
			if( PropWin !=nil )
  			{
				SetPort( PropWin );
	  			EraseRect( &PropWin->portRect);
				InvalRect( &PropWin->portRect);
			}
			SetPort( savePort );
		}
	}
	
}

short StretchBox(	WindowRef window )
{
	short a=1, b=0, gs, wn, x1, y1, x2=0, y2=0, lastY, lastX, oldControl=-1;
	Rect 		rect2;
	PenState	ps;	
	Point		pt;
	Pattern		myPat;

	SetPort( window);
	wn = WhichWindow( window);
	if( wn == -1 ) return 0;
	
	b = Forms[wn].ctlSelected;
	if( b >=0 ) oldControl = b;
	/*{
		rect2 = CtlData[b].theRect;
		InsetRect( &rect2, -2, -2 );
		InvalRect( &rect2 );
		EraseRect( &rect2 );
		DrawControls(FormWin[wn]);
	}*/
	
	b=0;
	
	GetPenState( &ps);
	PenNormal();
	GetIndPattern( &myPat, 0, 4);
	PenPat( &myPat);
	PenMode( patXor);
	GetMouse( &pt);
	gs = GetGridFlag( wn );
	if( gs )
	{	// Grid Snap ON
		a = GetGridSize( wn );
		b = a/2;
		x1 = ((pt.h + b) / a) *a; 	
		y1 = ((pt.v + b) / a) *a; 	
		x2 = x1;     y2 = y1;
	}
	else
	{
		x1 = pt.h;     y1 = pt.v;
		x2 = pt.h;     y2 = pt.v;
	}
	MyDrawFrame( x1, y1, x2, y2 );
	lastX = x2;
	lastY = y2;
	while( StillDown() )
	{
		GetMouse( &pt);
		if( pt.h != lastX || pt.v != lastY )
		{
			MyDrawFrame( x1, y1, x2, y2 );
			//******
			if( gs )
			{	// Grid Snap ON
				x2 = (((pt.h + b) / a) *a) +1; 	
				y2 = (((pt.v + b) / a) *a) +1; 	
			}
			else
			{
				x2 = pt.h;     y2 = pt.v;
			}
		
			//******
			MyDrawFrame( x1, y1, x2, y2 );
			lastX = pt.h;
			lastY = pt.v;
		}
	}
	MyDrawFrame( x1, y1, x2, y2 );
	SetPenState( &ps);
	if( oldControl >=0 )
	{
		rect2 = CtlData[oldControl].theRect;
		InsetRect( &rect2, -3, -3 );
		EraseRect( &rect2 );
		InvalRect( &rect2 );
		//DrawControls(FormWin[wn]);
	}


	return 0;
}

short ResizeBox(  WindowRef window, Rect *PassRect )
{
	short a=1, b=0, gs, wn, x1, y1, x2=0, y2=0, lastY, lastX, xoff, yoff, h, w, oldControl=-1;
	PenState	ps;	
	Point		pt;
	Pattern		myPat;
	Rect		theRect, rect2;

	theRect = *PassRect;
	SetPort( window);
	wn = WhichWindow( window);
	if( wn == -1 ) return 0;
	
	b = Forms[wn].ctlSelected;
	if( b >=0 ) oldControl = b;
	/*{
		rect2 = CtlData[b].theRect;
		InsetRect( &rect2, -2, -2 );
		InvalRect( &rect2 );
		EraseRect( &rect2 );
		DrawControls(FormWin[wn]);
		//Forms[wn].ctlSelected = - 1;
	}*/
	b=0;

	GetPenState( &ps);
	PenNormal();
	GetIndPattern( &myPat, 0, 4);
	PenPat( &myPat);
	PenMode( patXor);
	GetMouse( &pt);
	gs = GetGridFlag( wn );
	if( gs )
	{	// Grid Snap ON
		a = GetGridSize( wn );
		b = a/2;
	}
	x1 = theRect.left;	
	y1 = theRect.top;	
	x2 = theRect.right; 	
	y2 = theRect.bottom; 	
	MyDrawFrame( x1, y1, x2, y2 );
	lastX = pt.h;
	lastY = pt.v;
	xoff = lastX - x1;
	yoff = lastY - y1;
	w = x2 - x1;
	h = y2 - y1;
	while( StillDown() )
	{
		GetMouse( &pt);
		if( pt.h != lastX || pt.v != lastY )
		{
			MyDrawFrame( x1, y1, x2, y2 );
			//x1 = pt.h - xoff;    	 y1 = pt.v - yoff;
			x2 = pt.h - xoff + w;    y2 = pt.v - yoff + h;
			if( x2 < x1 + 10 ) x2 = x1 + 10;
			if( y2 < y1 + 10 ) y2 = y1 + 10;
			if( gs )
			{	// Grid Snap ON
				//x1 = (((x1 + b) / a) *a); 	
				//y1 = (((y1 + b) / a) *a); 	
				x2 = (((x2 + b) / a) *a); 	
				y2 = (((y2 + b) / a) *a); 	
			}
			MyDrawFrame( x1, y1, x2, y2 );
			lastX = pt.h;
			lastY = pt.v;
		}
	}
	MyDrawFrame( x1, y1, x2, y2 );
	SetPenState( &ps);
	if( oldControl >=0 )
	{
		rect2 = CtlData[oldControl].theRect;
		rect2.right+=17;
		InsetRect( &rect2, -3, -3 );
		InvalRect( &rect2 );
		EraseRect( &rect2 );
		//DrawControls(FormWin[wn]);
		//Forms[wn].ctlSelected = - 1;
	}
	{
	//	SetThemeWindowBackground(FormWin[CurrentForm],kThemeActiveDialogBackgroundBrush,true);
	}

	return 0;
}

short MoveBox(	WindowRef window, Rect *PassRect )
{
	short a=1, b=0, gs, wn, x1, y1, x2=0, y2=0, lastY, lastX, xoff, yoff, h, w, oldControl = -1;
	PenState	ps;	
	Point		pt;
	Pattern		myPat;
	Rect		theRect, rect2;

	theRect = *PassRect;
	SetPort( window);
	wn = WhichWindow( window);
	if( wn == -1 ) return 0;
	
	GetPenState( &ps);
	PenNormal();
	b = Forms[wn].ctlSelected;

	if( b >=0 ) oldControl = b;

	b=0;

	GetIndPattern( &myPat, 0, 4);
	PenPat( &myPat);
	PenMode( patXor);
	GetMouse( &pt);
	gs = GetGridFlag( wn );
	if( gs )
	{	// Grid Snap ON
		a = GetGridSize( wn );
		b = a/2;
	}
	x1 = theRect.left;	
	y1 = theRect.top;	
	x2 = theRect.right; 	
	y2 = theRect.bottom; 	
	MyDrawFrame( x1, y1, x2, y2 );
	lastX = pt.h;
	lastY = pt.v;
	xoff = lastX - x1;
	yoff = lastY - y1;
	w = x2 - x1;
	h = y2 - y1;
	while( StillDown() )
	{
		GetMouse( &pt);
		if( pt.h != lastX || pt.v != lastY )
		{
			MyDrawFrame( x1, y1, x2, y2 );
			x1 = pt.h - xoff;    	 y1 = pt.v - yoff;
			x2 = pt.h - xoff + w;    y2 = pt.v - yoff + h;;
			if( gs )
			{	// Grid Snap ON
				x1 = (((x1 + b) / a) *a); 	
				y1 = (((y1 + b) / a) *a); 	
				x2 = x1 +w;
				y2 = y1 + h;
				//x2 = (((x2 + b) / a) *a); 	
				//y2 = (((y2 + b) / a) *a); 	
			}
			MyDrawFrame( x1, y1, x2, y2 );
			lastX = pt.h;
			lastY = pt.v;
		}
	}
	MyDrawFrame( x1, y1, x2, y2 );
	SetPenState( &ps);
	if( oldControl >=0 )
	{
		rect2 = CtlData[oldControl].theRect;
		rect2.right+=17;
		InsetRect( &rect2, -3, -3 );
		InvalRect( &rect2 );
		EraseRect( &rect2 );
	}
	return 0;
}


void AdjustCursor( Point mouseLoc, RgnHandle mouseRgn )
{
	//WindowRef window;

	// by default, set mouseRgn to the whole QuickDraw coordinate plane,
	// so that we never get mouse moved events

//***	SetRectRgn( mouseRgn, -SHRT_MAX, -SHRT_MAX, SHRT_MAX, SHRT_MAX );

	// give text services a chance to set the cursor shape

/***	if ( gHasTextServices )
	{
		if ( SetTSMCursor( mouseLoc ) )
			return;
	}
*/
	// if there is a window open, give WEAdjustCursor an opportunity to set the cursor
	// WEAdjustCursor intersects mouseRgn (if supplied) with a region within which
	// the cursor is to retain its shape
	// (if the cursor is outside the view region, this is subtracted from mouseRgn )

	/*if ( ( window = FrontWindow( ) ) != nil )
	{
		if ( WEAdjustCursor( mouseLoc, mouseRgn, GetWindowWE( window ) ) )
			return;
	}*/

	// set the cursor to the arrow cursor

	SetCursor( &qd.arrow );
}

void DoMouseDown( const EventRecord *event )
{
	WindowRef window;
	short partCode, sv1,sv2,a, ct,cn,aa;
	Boolean isCmdKey, isShiftKey;
	Rect	theRect, tr;
	long	lv;
	Point	pt, CellSz;
	

	CmdKeyFlag = 0;
	// find out where this click when down in

	partCode = FindWindow( event->where, &window );
	isCmdKey = ( ( event->modifiers & cmdKey ) != 0 );
	isShiftKey = ( ( event->modifiers & shiftKey ) != 0 );

	// dispatch on partCode
	if( partCode != inMenuBar ) undoCode = 0;

	switch ( partCode )
	{
		case inMenuBar:
			PropDispose( );
			//EditorDispose();
			PrepareMenus( );
			if( isCmdKey ) CmdKeyFlag = 1;
			DoMenuChoice( MenuSelect( event->where ), event );
			break;

		case inSysWindow:
			// ??? shouldn't SystemClick be defined as
			//     SystemClick(const EventRecord *, WindowRef) ???
			SystemClick( event, (WindowPtr) window );
			break;

		case inContent:
			a = CheckForEdit( window);
			if(a>=0 )
			{
				PropDispose( );
 				pt = event->where;
				GlobalToLocal( &pt );
				CheckEditMouse( a, pt);
				return;
			}
			else if(window==ErrorWin)
			{
 				PropDispose( );
				EditorDispose();
 				SelectWindow( window );
 				pt = event->where;
				GlobalToLocal( &pt );
				ErrWinMouse(  pt );
			}
			else if(window==ToolPal)
			{
 				PropDispose( );
				EditorDispose();
 				SelectWindow( window );
				BV_FindTool( event);
				BV_DrawToolBar();
			}
			else if(window==PropWin)
			{
				EditorDispose();
				SelectWindow( window );
				PropEvent( event->where );
			}
			else 
			{
				PropDispose( );
  				EditorDispose();
				SelectWindow( window );
				SetPort( window );
  				a = WhichWindow( window);
  				if( a==-1 ) return;
  				CurrentForm = a;
				if( IsInGrow( a, event->where ) )
				{
					SetRect( &theRect, 10,10, 1000,1000);
					lv = GrowWindow( window, event->where, &theRect);
					if( lv )
					{
						EditWin[a].change = 1;
						sv1 = LoWord( lv );
				      	sv2 = HiWord( lv );
	  					SetPort( window );
	  					SizeWindow( window, sv1, sv2, true);
	  					SetWinRect( a , sv1, sv2 );
					}
				}
				else
				{
					if( a >=0 )
					{
						if( currTool==1 )
						{
							a=0;
							if( isCmdKey || isShiftKey ) a++;
							ArrowSelect( event->where, a );
						}
						else
						{
							StretchBox(window );
      						if( MasterRect.bottom-MasterRect.top < 10  ||  MasterRect.right-MasterRect.left < 10 ) { {SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);}return; }
      						ct = FindEmptyControl();
      						switch( currTool )
      						{
      							case 2: // Button
	      							CtlData[ct].ctl = NewControl( window, &MasterRect, "\p", 1, 0, 0, 0, mypushButProc, 0);  
	      							if( CtlData[ct].ctl !=nil )
	      							{
	      								CtlData[ct].status = 1;
	      								CtlData[ct].WinNum = a;
										CtlData[ct].theRect = MasterRect;
										SelectMyControl( ct );
										AddMyControl( ct, 1, CtlData[ct].WinNum);
									}
									break;
      							case 3: // label
      							case 4: // TextBox
	      							tr = MasterRect;
	      							if( currTool == 4 ) tr.right = 2000; // def single line on
	      							CtlData[ct].te = TEStyleNew( &tr, &MasterRect );
	      							if( CtlData[ct].te !=nil )
	      							{
	      								CtlData[ct].status = currTool-1;
	      								CtlData[ct].WinNum = a;
										CtlData[ct].theRect = MasterRect;
										EraseRect( &MasterRect );
										CtlData[ct].ctl = nil;
										TECalText( CtlData[ct].te );
 									  	TEAutoView( true, CtlData[ct].te );
 										TEUpdate( &MasterRect, CtlData[ct].te );
										if( currTool == 4 )
										{
											theRect = MasterRect;
											InsetRect( &theRect, -1, -1);
											FrameRect ( &theRect );
										}
										AddMyControl( ct, currTool-1, CtlData[ct].WinNum);
										SelectMyControl( ct );
									}
      								break;
      							case 5: // Icon
      							case 6: // Picture
	      							CtlData[ct].status = currTool-1;
	      							CtlData[ct].WinNum = a;
									CtlData[ct].theRect = MasterRect;
									CtlData[ct].pic = nil;
	      							ObjectProps[ct].value = 0;
									EraseRect( &MasterRect );
									//if( currTool == 5 ) DrawString( "\pIcon");
									//else DrawString( "\pPicture");
									FrameRect ( &MasterRect );
									SelectMyControl( ct );
									MoveTo( MasterRect.left+2, MasterRect.top+12 );
									AddMyControl( ct, currTool-1, CtlData[ct].WinNum);
									break;
      						  	case 7:	// CheckBox
	      							CtlData[ct].ctl = NewControl( window, &MasterRect, "\p", 1, 0, 0, 1, /*checkBoxProc*/369, 0);  
	      							if( CtlData[ct].ctl !=nil )
	      							{
	      								CtlData[ct].status = 6;
	      								CtlData[ct].WinNum = a;
										CtlData[ct].theRect = MasterRect;
										SelectMyControl( ct );
										AddMyControl( ct, 6, CtlData[ct].WinNum);
									}
									break;
      						  	case 8:	// Radio
	      							CtlData[ct].ctl = NewControl( window, &MasterRect, "\p", 1, 0, 0, 1, /*radioButProc*/370, 0);  
	      							if( CtlData[ct].ctl !=nil )
	      							{
	      								CtlData[ct].status = 7;
	      								CtlData[ct].WinNum = a;
										CtlData[ct].theRect = MasterRect;
										SelectMyControl( ct );
										AddMyControl( ct, 7, CtlData[ct].WinNum);
									}
									break;
      						  	case 9:	// DropList
									cn = ct + 10000;
									CtlData[ct].theMenu = NewMenu(cn, "\pMenu" );
									InsertMenu( CtlData[ct].theMenu, -1);
									//AppendMenu( CtlData[ct].theMenu, "\pHello to everyone");
									MasterRect.bottom = MasterRect.top+20;
									CtlData[ct].ctl = NewControl( window, &MasterRect, "\p", true, 0, cn ,0,/*popupMenuProc*/400, 0); 
	      							if( CtlData[ct].ctl !=nil )
	      							{
	      								CtlData[ct].status = 8;
	      								CtlData[ct].WinNum = a;
										CtlData[ct].theRect = MasterRect;
										SelectMyControl( ct );
										AddMyControl( ct, 8, CtlData[ct].WinNum);
									}
									break;
      						  	case 10:	// ListBox
									SetRect( &theRect, 0,0, 0,0);
									CellSz.h = 0;
									CellSz.v = 0;
									CtlData[ct].lt = LNew( &MasterRect, &theRect, CellSz, 0, window, true, false, false, true); 
	      							if( CtlData[ct].lt !=nil )
	      							{
										LAddColumn( 1, 0, CtlData[ct].lt);									
	      								CtlData[ct].status = 9;
	      								CtlData[ct].WinNum = a;
										CtlData[ct].theRect = MasterRect;
										theRect = MasterRect;
										InsetRect( &theRect, -1, -1);
										FrameRect ( &theRect );
										SelectMyControl( ct );
										AddMyControl( ct, 9, CtlData[ct].WinNum);
									}
									break;
								case 11:	// Timer
	      							CtlData[ct].status = 10;
	      							CtlData[ct].WinNum = a;
									MasterRect.right = MasterRect.left+32;
									MasterRect.bottom = MasterRect.top+32;
									CtlData[ct].theRect = MasterRect;
									SetResLoad(true);
									PlotIconID(&MasterRect, 0, 0, 5011);
									SelectMyControl( ct );
									AddMyControl( ct, 10, CtlData[ct].WinNum);
									break;
     						  	case 12:	// Scrollbar
	      							CtlData[ct].ctl = NewControl( window, &MasterRect, "\p", 1, 0, 0, 0, /*scrollBarProc*/16, 0);  
	      							if( CtlData[ct].ctl !=nil )
	      							{
	      								CtlData[ct].status = 11;
	      								CtlData[ct].WinNum = a;
										CtlData[ct].theRect = MasterRect;
										SelectMyControl( ct );
										AddMyControl( ct, 11, CtlData[ct].WinNum);
									}
									break;
							}
							currTool = 1;
							BV_DrawToolBar();

						}
					}
				}
			}
			break;

		case inDrag:
			a = FindTEwindow( );
			if( a>=0 )
			{
				if( window != EditWin[a].theWindow ) EditorDispose();
			}
			DoDrag( event->where, window );
			break;

		case inGrow:
			//*** DoGrow( event->where, window );
			a = WhichWindow( window);
			if( a!=-1 )
			{
				CurrentForm = a;
				SetRect( &theRect, 10,10, 1000,1000);
				lv = GrowWindow( window, event->where, &theRect);
				if( lv )
				{
					EditWin[a].change = 1;
					sv1 = LoWord( lv );
			      	sv2 = HiWord( lv );
					SetPort( window );
					SizeWindow( window, sv1, sv2, true);
					SetWinRect( a , sv1, sv2 );
				}
			}
			else
				{
				a = DoPropGrow( event->where, window );
				if(!a)
				{
					a = DoEditGrow(  event->where, window );
					if( a==-1 ) DoErrGrow( event->where, window);
				}
			}
			break;

		case inGoAway:
			if ( TrackGoAway( window, event->where ) )
			{
				if( window == ErrorWin )
				{
					CloseErrorWindow( );
				}
				else if( window == PropWin )
				{
					PropDispose( );
					DisposeWindow( window );
					PropWin = nil;
				}
				else if( window == ToolPal )
				{
					DisposeWindow( window );
					ToolPal = nil;
				}
				else
				{
					a = CloseEditWindow( window);
					if( a==-1 )
					{
						a = WhichWindow( window);
						if( a!=-1 )
						{
							CurrentForm = a;
							CloseForm( window);
						}
					}
				}				
				
			}
			break;

		case inZoomIn:
		case inZoomOut:
			if ( TrackBox( window, event->where, partCode ) )
			{
				//*** DoZoom( partCode, window );
				{SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);}			}
			break;
	}	// switch
	
	{
	//	SetThemeWindowBackground(FormWin[CurrentForm],kThemeActiveDialogBackgroundBrush,true);
	}

}


short  EditKeyDown( const EventRecord *theEvent )
{
   	short  theChar, thePart, selStart, selEnd, sz;
   	short	a ,b,c, cmdcode=0 ; 
  	short	cmax,cmin,cvalue  ;
  	long	menuResult, mn;
	WindowPtr	window;
	
    thePart = FindWindow( theEvent->where, &window );
    //SelectWindow( window);
    SetPort( window);
    
   	theChar = theEvent->message & charCodeMask;
   	if ( ( theEvent->modifiers & cmdKey ) != 0 )
	{
		PrepareMenus( );

		menuResult = MenuKey( theChar );
		
    	mn =  HiWord( menuResult );
    	if( mn != 0 ) //&& theChar!='c'  && theChar!='x' && theChar!='v')
    	{
 			DoMenuChoice( menuResult, theEvent );
   		    HiliteMenu( 0 );
    	}
    	else
    	{
   		  if( theChar < 0 || theChar > 255) theChar=0; 	
    	  
	      /*switch ( theChar )
	      {
	      	 case 'c':	// copy
	 			if( ActiveTE != nil ) {	TECopy(ActiveTE); 	ZeroScrap(); TEToScrap();}
	      	 	break;
	       	 case 'x':  // cut
	 			if( ActiveTE != nil )
	 			{
					TECut(ActiveTE);
					ZeroScrap();
					TEToScrap();
	     	 	}
	      	 	break;
	      	 case 'v':  // paste
				if( ActiveTE != nil )
	 			{
					TEFromScrap();
					TEPaste(ActiveTE);
					//***TextFix( a );
	     	 	}
	      	 	break;
	     	 case 'q':
	     	    // DoneFlag++;   
	        	break;      
		 }*/
      }
      
	}
	else
	{
		undoCode = 0;
		if( ActiveTE != nil )
		{
			//TEActivate( ActiveTE );
			if( ActiveTE == PropTE )
			{
				if( theChar == 127 )
				{
					if( ( (*ActiveTE)->selStart == (*ActiveTE)->selEnd ) && ( (*ActiveTE)->selStart < (*ActiveTE)->teLength) )
					{
						((*ActiveTE)->selStart)++;
						((*ActiveTE)->selEnd)++;
						TEKey( 8, ActiveTE); 
					}
					else TEKey( 8, ActiveTE);  // Move one char fwd and then do bkspace
					TEUpdate( &((*ActiveTE)->viewRect), ActiveTE );

					return;
				}
				else
				{
				 	if(theChar == 27 ) { PropDisregard( ); return;	}
				 	if(theChar == 13 || theChar == 3) { PropDispose( ); return;	}
					if( numEntryFlag ) if( (theChar<'0' || theChar > '9') && theChar!=8 ) return;
					if( nospaceEntryFlag ) if( theChar==' ') return;
					if( (*ActiveTE)->teLength >=255 && theChar !=8 && theChar !=28 && theChar !=29) {SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);}					else
					{
						//if(theChar == 8 || theChar == 28 || theChar == 29 ||(theChar >= 32 && theChar <= 127))
						if(theChar != 9 )
						{
							TEKey(theChar, ActiveTE);
							TEUpdate( &((*ActiveTE)->viewRect), ActiveTE );
						}
					}
				}
				return;
			}
			
			
			switch( theChar )
			{
				case 8:		// delete
	 				if( (*ActiveTE)->selStart != (*ActiveTE)->selEnd )
	 				{
		 				undoCode = 5;  // typing delete
		 				undoStart = (*ActiveTE)->selStart;
		 				undoEnd = (*ActiveTE)->selEnd;
						TECut(ActiveTE);
					}
					else TEKey(theChar, ActiveTE);
					TEUpdate( &((*ActiveTE)->viewRect), ActiveTE );
				TextFix( window );
					MarkChangeTE();
					textAnchor =0;
				break;
				case 28:  // left
				case 29:  // right
				case 31:  // down
					if ((theEvent->modifiers & shiftKey) == 0) 
					{
						//if( theChar == 31 )
						(*ActiveTE)->clikStuff = 255;
						TEKey(theChar, ActiveTE);
						//if( theChar == 31 ) (*ActiveTE)->clikStuff = 255;
						//TEUpdate( &((*ActiveTE)->viewRect), ActiveTE );
						TextFix( window );
						textAnchor =0;
					}
					else
					{
							selStart = (*ActiveTE)->selStart;
							selEnd = (*ActiveTE)->selEnd;
							//printf( "Start1 = %i & End = %i\n", selStart, selEnd );
							if( selStart == selEnd)
							{	if( theChar == 28 ) textAnchor=1;
								else textAnchor=0;  }
							if( textAnchor==1 ) TESetSelect( selStart, selStart, ActiveTE );
							else TESetSelect( selEnd, selEnd, ActiveTE );

							//*****if( theChar == 31 ) (*ActiveTE)->clikStuff = 0;
							TEKey(theChar, ActiveTE);
							//*****if( theChar == 31 ) (*ActiveTE)->clikStuff = 255;
								
							if( textAnchor==1 ) selStart = (*ActiveTE)->selStart;
							else selEnd = (*ActiveTE)->selEnd;
							if( selEnd < selStart ) { sz =selEnd; selEnd = selStart; selStart = sz; }
							TESetSelect( selStart, selEnd, ActiveTE );
							TEUpdate( &((*ActiveTE)->viewRect), ActiveTE );
							TextFix( window );
					}
				break;
				case 30:  // Up Arrow
					if ((theEvent->modifiers & shiftKey) == 0)
					{
						//(*ActiveTE)->clikStuff = 255;
						TEKey(theChar, ActiveTE);
						selStart = (*ActiveTE)->selStart;
						selEnd = (*ActiveTE)->selEnd;
						TESetSelect( selStart, selEnd, ActiveTE );
						//(*ActiveTE)->clikStuff = 255;
						//EraseRect( &((*ActiveTE)->viewRect));
						TEUpdate( &((*ActiveTE)->viewRect), ActiveTE );
						TextFix( window );
						textAnchor =0;
					}
					else
					{
							selStart = (*ActiveTE)->selStart;
							selEnd = (*ActiveTE)->selEnd;
							if( selStart == selEnd) textAnchor=1;
							if( textAnchor==1 ) TESetSelect( selStart, selStart, ActiveTE );
							else TESetSelect( selEnd, selEnd, ActiveTE );

							//*****(*ActiveTE)->clikStuff = 0;
							TEKey(theChar, ActiveTE);
							//*****(*ActiveTE)->clikStuff = 255;
								
							if( textAnchor==1 ) selStart = (*ActiveTE)->selStart;
							else selEnd = (*ActiveTE)->selEnd;
							if( selEnd < selStart ) { sz =selEnd; selEnd = selStart; selStart = sz; }
							TESetSelect( selStart, selEnd, ActiveTE );
							EraseRect( &((*ActiveTE)->viewRect));
							TEUpdate( &((*ActiveTE)->viewRect), ActiveTE );
							TextFix( window );
					}
					break;
			case 11:  // PageUp
			case 12:  // PageDown
				if ((theEvent->modifiers & shiftKey) == 0)
				{
					sz=-1;
					for(a=0; a<100;a++ ) if(  EditWin[a].status && EditWin[a].theWindow == window ) { sz = a; break;}
					if( sz == -1 ) return;
					EditPager( sz, theChar-10 );
				}
				break;
			case 1: // Home
				if ((theEvent->modifiers & shiftKey) == 0) DoBeginLine( );
				break;
			case 4: // End
				if ((theEvent->modifiers & shiftKey) == 0) DoEndLine( );
				break;
			case 127:  // del
 				if( (*ActiveTE)->selStart != (*ActiveTE)->selEnd )
 				{
	 				undoCode = 5;  // typing delete
	 				undoStart = (*ActiveTE)->selStart;
	 				undoEnd = (*ActiveTE)->selEnd;
					TECut(ActiveTE);
				}
				else
				{
					if( ( (*ActiveTE)->selStart == (*ActiveTE)->selEnd ) && ( (*ActiveTE)->selStart < (*ActiveTE)->teLength) )
					{
						((*ActiveTE)->selStart)++;
						((*ActiveTE)->selEnd)++;
						TEKey( 8, ActiveTE); 
					}
					else TEKey( 8, ActiveTE);  // Move one char fwd and then do bkspace
				}
				TEUpdate( &((*ActiveTE)->viewRect), ActiveTE );
			TextFix( window );
				MarkChangeTE();
				textAnchor =0;
				break;
			case 13:
			case 3:
 				if( (*ActiveTE)->selStart != (*ActiveTE)->selEnd )
 				{
	 				undoCode = 5;  // typing delete
	 				undoStart = (*ActiveTE)->selStart;
	 				undoEnd = (*ActiveTE)->selEnd;
					TECut(ActiveTE);
				}
				DoAutoIndent( );
				TEUpdate( &((*ActiveTE)->viewRect), ActiveTE );
				TextFix( window );
				MarkChangeTE();
				textAnchor =0;
				break;
			case 9:
 				if( (*ActiveTE)->selStart != (*ActiveTE)->selEnd )
 				{
	 				undoCode = 5;  // typing delete
	 				undoStart = (*ActiveTE)->selStart;
	 				undoEnd = (*ActiveTE)->selEnd;
					TECut(ActiveTE);
				}
				DoTab( );
				TEUpdate( &((*ActiveTE)->viewRect), ActiveTE );
				TextFix( window );
				MarkChangeTE();
				textAnchor =0;
				break;
			default:
				//if( theChar >= ' ' && theChar <='~' )
				if(theChar != 9 )
				{
	 				if( (*ActiveTE)->selStart != (*ActiveTE)->selEnd )
	 				{
		 				undoCode = 5;  // typing delete
		 				undoStart = (*ActiveTE)->selStart;
		 				undoEnd = (*ActiveTE)->selEnd;
						TECut(ActiveTE);
					}
					TEKey(theChar, ActiveTE);
					//TEUpdate( &((*ActiveTE)->viewRect), ActiveTE );
					//TECalText( ActiveTE);
					//TextFix( window );
					MarkChangeTE();
				}
				textAnchor =0;
				break;
			}
		}
		else
		{
			if( ( theChar == 8 || theChar == 127 )&& CurrentForm >=0 )
			{
				if( Forms[CurrentForm].ctlSelected>=0 )
				{
					DeleteSelectedControl();
				}
			}
		
		}
	}
	return(0);
}

void DoAutoIndent( )
{
		Handle	 htemp;
		char	*cp, sp[]="                                                                                                                                                                     ";
		short	 a, b, c, d, f=0, w, sflag=0;
		long	e=1;

		sp[0]=13;
		w = (*ActiveTE)->teLength;
		b = (*ActiveTE)->selStart;
		htemp = (Handle) TEGetText( ActiveTE );
		HLock( htemp );
		cp = (*htemp);
		for( a=b-1; a>0; a--)
		{
			c = *(cp+a);
			if( c == 13 || c == 10 ) { a++; break;}
			if( c != ' '  ) sflag=1;
		}

		for( d=a; d<b; d++)  //d<=b
		{
			c = *(cp+d);
			if( c == ' ' ) f++;
			else break;
		}
		HUnlock( htemp );
		
		//if( d<b ) TESetSelect( d, d, ActiveTE ); 		
		//TEKey( 13, ActiveTE );
		if( f<=0 ) TEKey( 13, ActiveTE );
		else
		{
			e = f+1;
			TEInsert( sp, e, ActiveTE);
		}
		TESelView( ActiveTE );
}

void DoTab( )
{
		Handle	 htemp;
		char	*cp, sp[]="      "; //1234567890";
		short	 b, c, d, f=0;
		long	e=1,a;

		b = (*ActiveTE)->selStart;
		htemp = (Handle) TEGetText( ActiveTE );
		HLock( htemp );
		cp = (*htemp);
		for( a=b-1; a>=0; a--)
		{
			c = *(cp+a);
			if( c == 13 || c == 10 ) { a++; break;}
		}
		d = b-a+4;
		f = ((d/4)*4)-(b-a);
		
		HUnlock( htemp );
		e = f;
		TEInsert( sp, e, ActiveTE);
		//TECalText(  ActiveTE );
		TEUpdate( &((*ActiveTE)->viewRect), ActiveTE );
		
}

void DoBeginLine( )
{
		Handle	 htemp;
		char	*cp;
		short	 a, b, c;
		long	e=1;

		b = (*ActiveTE)->selStart;
		htemp = (Handle) TEGetText( ActiveTE );
		HLock( htemp );
		cp = (*htemp);
		for( a=b-1; a>=0; a--)
		{
			c = *(cp+a);
			if( c == 13 || c == 10 ) { a++; break;}
		}
		HUnlock( htemp );
		e = a;
		if( e<=1) e=0;
		(*ActiveTE)->clikStuff = 255;
		TESetSelect( e, e, ActiveTE ); 		
		TEUpdate( &((*ActiveTE)->viewRect), ActiveTE );
}

void DoEndLine( )
{
		Handle	 htemp;
		char	*cp;
		short	 a, b, c, d;
		long	e=1;

		b = (*ActiveTE)->selStart;
		d = (*ActiveTE)->teLength;
		htemp = (Handle) TEGetText( ActiveTE );
		HLock( htemp );
		cp = (*htemp);
		for( a=b; a<=d; a++)
		{
			c = *(cp+a);
			if( c == 13 ) break;
		}
		HUnlock( htemp );
		e = a;
		//(*ActiveTE)->clikStuff = 255;
		TESetSelect( e, e, ActiveTE ); 		
		TEUpdate( &((*ActiveTE)->viewRect), ActiveTE );
}


void DoDiskEvent( const EventRecord *event )
{
	Point dialogCorner;

	if ( ( event->message >> 16) != noErr )
	{
		SetPt( &dialogCorner, 112, 80 );
		DIBadMount( dialogCorner, event->message );
	}
}

void DoOSEvent( const EventRecord *event )
{
	short		osMessage;
	WindowRef	window;

	// extract the OS message field from the event record

	osMessage = ( event->message & osEvtMessageMask) >> 24 ;

	// dispatch on osMessage

	switch( osMessage )
	{
		case suspendResumeMessage:
			if ( ( window = FrontWindow( ) ) != nil )
			{
				//*** DoActivate( (event->message & resumeFlag) != 0, window );
			}
			break;

		case mouseMovedMessage:
			// nothing
			break;

	}
}

void DoHighLevelEvent( const EventRecord *event )
{
	AEProcessAppleEvent( event );
}

void DoNullEvent( const EventRecord *event )
{
#pragma unused (event)

	WindowRef window;

	if( ActiveTE != nil ) TEIdle(ActiveTE);


	if ( ( window = FrontWindow( ) ) != nil )
	{
		//WEIdle( &sSleepTime, GetWindowWE(window) );
	}
	else
	{
		//***sSleepTime = LONG_MAX;
	}
}

void DoWindowEvent( const EventRecord *event )
{
	WindowRef window;
	GrafPtr		savePort,gp;
	short	a, b, c, actFlag;

	// the message field of the event record contains the window reference

	window = (WindowRef) event->message;

	// make sure this window is an application window; check the windowKind field

	if ( GetWindowKind( window ) != userKind )
		return;

	switch ( event->what )
	{
		case updateEvt:
			//*** DoUpdate( window );
			if(window==ToolPal)
			{
				// save the old drawing port
				GetPort( &savePort );
				SetPortWindowPort( window );
				BeginUpdate(window);
					BV_DrawToolBar();
				EndUpdate(window);
				// restore the old graphics port
				SetPort( savePort );

			}
			else if( window == PropWin )
			{
				UpdatePropWin(  window );			
			}
			else if( window == ErrorWin )
			{
				UpdateErrWin(  window );			
			}
			else
			{
			
				a=UpdateFormWin( window );		
				if( !a) UpdateEditWin( window );
			}
			break;

		case activateEvt:
			if( (event->modifiers & activeFlag) )
			{
				a = WhichWindow( window );
				if( a!=-1 ) CurrentForm = a;
				else
				{
					a = IsFormEdit(window);
					if( a!=-1)
					{
						CurrentForm = a;
					}
					else 
					{
						a = IsModuleEdit(window);
						if( a>=20 )
						{
							//{SysBeep(30); dprintf("Error @ " __FILE__" l. %d", __LINE__);}							if( PropWin !=nil ) 
							{
								SetPort( PropWin );
						  		EraseRect( &PropWin->portRect);
								InvalRect( &PropWin->portRect);
								SetPort( window );
							}
						}
					}
				}	
			} 
			break;
	}
}

void ProcessEvent( void )
{
	EventRecord event;
	Boolean gotEvent;

	gotEvent = WaitNextEvent( everyEvent, &event, 1, sMouseRgn );

	if( SIOUXHandleOneEvent( &event) ) return;
	// give text services a chance to intercept this event
	// if TSMEvent( ) handles the event, it will set event.what to nullEvent

	/***if ( gHasTextServices )
	{
		TSMEvent( &event );
	}*/

	// adjust cursor shape and set mouse region
	// (we assume event.where is the current mouse position in global coordinates
	// if event.what <= osEvt; high-level events store the event ID there)

	/*if ( event.what <= osEvt )
	{
		AdjustCursor( event.where, sMouseRgn );
	}*/

	// dispatch on event.what

	switch( event.what )
	{
		case nullEvent:
			DoNullEvent( &event );
			break;

		case mouseDown:
			DblClickFlag = 0;
			if( event.when - LastMouseTick < DblClickThreshold )
			{
				if( fabs( event.where.h - LastMousePoint.h ) < 5 &&  fabs( event.where.v - LastMousePoint.v ) < 5 )
					      DblClickFlag =1;
			}
			LastMousePoint = event.where;
			LastMouseTick =  event.when;

			DoMouseDown( &event);
			break;

		case mouseUp:
			break;

		case keyDown:
		case autoKey:
			EditKeyDown( &event );
			break;

		case updateEvt:
		case activateEvt:
			DoWindowEvent( &event );
			break;

		case diskEvt:
			DoDiskEvent( &event );
			break;

		case osEvt:
			DoOSEvent( &event );
			break;

		case kHighLevelEvent:
			DoHighLevelEvent( &event );
			break;
	}	// switch

	if ( gotEvent )
	{
		sSleepTime = 0;  // force early idle after non-idle event
	}
}

OSErr GotRequiredParams( const AppleEvent *ae )
{
	DescType actualType;
	Size actualSize;
	OSErr err;

	err = AEGetAttributePtr( ae, keyMissedKeywordAttr, typeWildCard, &actualType, nil, 0, &actualSize );

	return	( err == errAEDescNotFound ) ? noErr :
			( err == noErr ) ? errAEParamMissed : err;
}

extern FSSpec		PjtFS;


static pascal OSErr	HandleOpenDocument( const AppleEvent *ae, AppleEvent *reply, long refCon )
{
#pragma unused ( reply, refCon )
	AEDescList		docList;
	AEKeyword		keyword;
	DescType		actualType;
	Size			actualSize;
	long			numberOfDocuments, i;
	FSSpec			fileSpec;
	OSErr			err;
	FInfo			info;

	docList.descriptorType = typeNull;
	docList.dataHandle = nil;

	// extract direct parameter from the Apple event

	if ( ( err = AEGetParamDesc( ae, keyDirectObject, typeAEList, &docList ) ) != noErr )
		goto cleanup;

	// perform the recommended check for additional required parameters

	if ( ( err = GotRequiredParams( ae ) ) != noErr )
		goto cleanup;

	// count the items in the list of aliases

	if ( ( err = AECountItems( &docList, &numberOfDocuments ) ) != noErr )
		goto cleanup;

	for ( i = 1; i <= numberOfDocuments; i++ )
	{
		// coerce the nth alias to a file system specification record

		if ( ( err = AEGetNthPtr( &docList, i, typeFSS, &keyword, &actualType,
				&fileSpec, sizeof( fileSpec ), &actualSize ) ) != noErr )
			goto cleanup;

		// open the specified project

		//if ( ( err = CreateWindow( &fileSpec ) ) != noErr ) 	goto cleanup;
		err = FSpGetFInfo( &fileSpec, &info);
		if( err || info.fdType != 'BPjt') goto cleanup;
		PjtFS = fileSpec;
		LoadProject( ) ;
	}

cleanup:
	AEDisposeDesc( &docList );
	return err;
}
int LaunchRT( );

static pascal OSErr	HandleApplicationDied( const AppleEvent *ae, AppleEvent *reply, long refCon )
{
#pragma unused ( reply, refCon )
	OSErr		err;

	// perform the recommended check for additional required parameters

	if(	AppLaunchFlag==44 || AppLaunchFlag==45 )
	{
			
			// Now Display ErrorLog if there is one
			CompilerErrorLog(  );
				
			if(	AppLaunchFlag==44 ) LaunchRT(  );
			AppLaunchFlag = 0;
	}
	if ( ( err = GotRequiredParams( ae ) ) != noErr )  goto cleanup;


cleanup:
	return err;
}


static pascal OSErr	HandleOpenApplication( const AppleEvent *ae, AppleEvent *reply, long refCon )
{
#pragma unused ( reply, refCon )
	OSErr		err;

	// perform the recommended check for additional required parameters

	if ( ( err = GotRequiredParams( ae ) ) != noErr )
		goto cleanup;

	// don't create a new window from scratch

	//err = CreateWindow( nil );

cleanup:
	return err;
}

static pascal OSErr	HandleQuitApplication( const AppleEvent *ae, AppleEvent *reply, long refCon )
{
#pragma unused (reply, refCon)

	AEKeyword		optKey;
	DescType		actualType;
	Size			actualSize;
	//SavingOption
	short			saving,savingAsk=1, savingYes=1, savingNo=0;
	OSErr			err;

	// default saving option is savingAsk;

	saving = savingAsk;

	// extract optional save options

	if ( ( err = AEGetParamPtr( ae, keyAESaveOptions, typeEnumerated, &actualType, &optKey, sizeof( optKey ), &actualSize ) ) == noErr )
	{
		if ( optKey == kAEYes )
		{
			saving = savingYes;
		}
		else if (optKey == kAENo )
		{
			saving = savingNo;
		}
		else if ( optKey != kAEAsk )
		{
			err = paramErr;	// for want of a better code
			goto cleanup;
		}
	}

	// perform the recommended check for additional required parameters

	if ( ( err = GotRequiredParams( ae ) ) != noErr )
		goto cleanup;

	// actually do the quit stuff

	//err = DoQuit( saving );
	MasterExitFlag++;
cleanup:
	return err;
}

OSErr InitializeEvents( void )
{
	OSErr	err;

	// allocate space for the mouse region

	sMouseRgn = NewRgn( );

	// install AppleEvent handlers for the Required Suite

	// Bobby has a new one app Died
	if ( ( err = AEInstallEventHandler( kCoreEventClass, kAEApplicationDied, NewAEEventHandlerProc( HandleApplicationDied ), 0, false ) ) != noErr )
		goto cleanup;

	if ( ( err = AEInstallEventHandler( kCoreEventClass, kAEOpenApplication, NewAEEventHandlerProc( HandleOpenApplication ), 0, false ) ) != noErr )
		goto cleanup;

	if ( ( err = AEInstallEventHandler( kCoreEventClass, kAEOpenDocuments, NewAEEventHandlerProc( HandleOpenDocument ), kDoOpen, false ) ) != noErr )
		goto cleanup;

	if ( ( err = AEInstallEventHandler( kCoreEventClass, kAEPrintDocuments, NewAEEventHandlerProc( HandleOpenDocument ), kDoPrint, false ) ) != noErr )
		goto cleanup;

	if ( ( err = AEInstallEventHandler( kCoreEventClass, kAEQuitApplication, NewAEEventHandlerProc( HandleQuitApplication ), 0, false ) ) != noErr )
		goto cleanup;
	

	// install Apple event handlers for a subset of the Core suite

	/*if ( ( err = InstallCoreHandlers( ) ) != noErr )
		goto cleanup;*/

	// install Apple event handlers for inline input

	/*if ( ( err = WEInstallTSMHandlers( ) ) != noErr )
		goto cleanup;*/

	return 0;
	

cleanup:
	return err;
}
