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

// This is for the error display window

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
#include <math.h>
#include "IDE.h"

aDocumentRecord appr2;

void PropDispose( );
void IDEmemErr();

WindowPtr	ErrorWin = nil;
TEHandle	ErrorTE= nil;
short		ErrorTEht = 0;
ControlHandle	ErrorScrollBar;
extern FSSpec	IDESpec;


void CloseErrorWindow( );
short CreateErrorWindow( );
short DoErrGrow( Point hitPt, WindowRef window );
void ErrTextFix( );
void ErrChangeScrollBar();
short	ErrCalcTextPages( short b );
short	ErrCalcTextView( short b );   // Returns lines viewable
pascal void ScrollProcTE( ControlHandle theControl, short partCode );
short ErrWinMouse( Point pt );
short ErrGWHt;


void CloseErrorWindow( )
{

	if( ErrorTE !=nil ) { TEDispose( ErrorTE ); ErrorTE=nil; }
	if( ErrorWin !=nil ) { DisposeWindow( ErrorWin ); ErrorWin=nil; }
}


short CreateErrorWindow( )
{
	short			a,b=-1,err, g,fref, f;
	Rect 			tempRect, tr, theRect, theRect2;
	RGBColor		black1, white1;
	FSSpec			rt;
	long			fl;
 	Handle			hbuf;
 	char			*ebuffer;
 	
	if( ErrorWin != nil )  CloseErrorWindow( );
	
	// Now let's create the window itself
	f= b*10;

	SetRect(&tempRect,40,100,540,360);
	ErrorWin = NewCWindow( nil, &tempRect, "\pVisual MSB Compiler Error Report" , true, documentProc, (WindowPtr) (-1), true, 0); 
	if( ErrorWin == nil ) return -1;

	// Now let's fill it's busty innards
	SelectWindow( ErrorWin );
	SetPort( ErrorWin );

	black1.red = 0;			black1.green = 0;		black1.blue = 0;
	white1.red = 0xffff;	white1.green = 0xffff;	white1.blue = 0xffff;
		
	RGBForeColor( &black1 );
	RGBBackColor( &white1 );
	
	//if(appr2.gHaveAppearanceMgr)
	{
		SetThemeWindowBackground(ErrorWin,kThemeActiveDialogBackgroundBrush,true);
	}
	
   	tr = ErrorWin->portRect;
   	tr.right = tempRect.right - tempRect.left-15;
   	tr.bottom = tempRect.bottom-tempRect.top-15;
	
	TextFont( 22 );
	ErrorTE = TEStyleNew( &tr, &tr); 
	if( ErrorTE==nil ) { IDEmemErr(); return -1; }

	// Adjust so no partial  lines
	ErrorTEht = TEGetHeight( 0, 0, ErrorTE );
	a = (tr.bottom-tr.top ) / ErrorTEht;
	tr.bottom =  (a * ErrorTEht);
	(*ErrorTE)->viewRect = tr;
	(*ErrorTE)->destRect = tr;

	//ActiveTE = ErrorTE;
	TEActivate( ErrorTE );
    TEAutoView( true, ErrorTE );

	tr = ErrorWin->portRect;
	tr.left = tr.right-15;
	tr.bottom -=15;
	ErrorScrollBar = NewControl( ErrorWin, &tr, "\p", TRUE,0,0,1, 16,0); 

	theRect2 = ErrorWin->portRect;
	OffsetRect( &theRect2, -theRect2.left, -theRect2.top);
	theRect2.bottom-=15;
	theRect2.left = theRect2.right-15;
	MoveControl( ErrorScrollBar, theRect2.left, theRect2.top);
	SizeControl( ErrorScrollBar, 16, theRect2.bottom - theRect2.top);
	

	//KillErrorWindow();

	err = FSMakeFSSpec(IDESpec.vRefNum,IDESpec.parID, "\p:Tools:ERRORLOG", &rt);
	FSpOpenDF( &rt, fsRdPerm, &fref );

	GetEOF( fref, &fl );

	hbuf = NewHandle( fl );
	if( hbuf == nil )
	{
		FSClose( fref );	
		return -1;
	}
	HLock( hbuf );
	ebuffer = (*hbuf );
	FSRead( fref, &fl, ebuffer );	   // Write header
	FSClose( fref );
	if( fl > 30000 ) fl = 30000;
	TESetText( ebuffer, fl, ErrorTE );
	HUnlock( hbuf );
	DisposeHandle( hbuf );
	TESetSelect( 0, 0, ErrorTE );
	TESelView( ErrorTE );
	
	g = ErrCalcTextPages( tr.bottom - tr.top);
	SetControlMaximum( ErrorScrollBar, (**ErrorTE).nLines - ErrCalcTextView(  tr.bottom - tr.top ) );  //-1
	if(!g)	HiliteControl( ErrorScrollBar, 255);  // Disable scrollbar

	DrawControls(ErrorWin);
	ErrTextFix( );

	return b;

}

short DoErrGrow( Point hitPt, WindowRef window )
{
	short	a, b;
	Rect sizeRect,theRect, tr;
	long newSize;
	GrafPtr		savePort;

	if( window != ErrorWin ) return -1;
	
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
	theRect.right-=15;
	theRect.bottom-=15;

	tr = theRect;
	a = (tr.bottom-tr.top ) / ErrorTEht;
	tr.bottom =  (a * ErrorTEht);
	(*ErrorTE)->viewRect = tr;
	(*ErrorTE)->destRect = tr;

	//(*ErrorTE)->viewRect = theRect;
	//(*ErrorTE)->destRect = theRect;
	TEUpdate(&theRect, ErrorTE);
	TECalText(ErrorTE );
	TESelView( ErrorTE );
	
	theRect = window->portRect;
	OffsetRect( &theRect, -theRect.left, -theRect.top);
	theRect.bottom-=15;
	theRect.left = theRect.right-15;
	MoveControl( ErrorScrollBar, theRect.left, theRect.top);
	SizeControl( ErrorScrollBar, 16, theRect.bottom - theRect.top);

	SetControlMaximum(ErrorScrollBar, 1);
	SetControlValue(ErrorScrollBar, 1);
  	EraseRect( &window->portRect);
	InvalRect( &window->portRect);
	//DrawGrowIcon( window );

	SetPort( savePort );
	return 1;
}

void UpdateErrWin( WindowPtr window )			
{
	short 		a,b,c;
	GrafPtr		savePort;
	Rect		theRect, tempRect;
	
	if( window != ErrorWin ) return ;
	
	GetPort( &savePort );
	SetPortWindowPort( window );
	BeginUpdate(window);
		theRect = window->portRect;
		OffsetRect( &theRect, -theRect.left, -theRect.top);
		EraseRect( &theRect );

		TEUpdate(&theRect, ErrorTE ); 
		ErrTextFix( /*window*/ );
		DrawGrowIcon( window );

		DrawControls(window);

	EndUpdate(window);
	// restore the old graphics port
	SetPort( savePort );
			
}

void ErrTextFix( )
{
	short a, b, c, d, cnum;
	Rect	theRect;
	
	theRect = ErrorWin->portRect;
	d = theRect.bottom - theRect.top - 16;
	b = ErrCalcTextPages(  d );
	
	c = (**ErrorTE).nLines - ErrCalcTextView( d ); // -1 ;
	SetControlMaximum( ErrorScrollBar, c);
	if(!b) HiliteControl( ErrorScrollBar, 255);  // Disable scrollbar
	else HiliteControl( ErrorScrollBar, 0);  // Enable scrollbar

	ErrChangeScrollBar( );

}

void ErrChangeScrollBar()
{
	short	a,b,c;
	
	a = fabs((**ErrorTE).destRect.top - (**ErrorTE).viewRect.top);
	b = ErrorTEht;
	c = a / b;
	SetControlValue( ErrorScrollBar, c );
}

short	ErrCalcTextPages( short b )
{
		short a, c;
		
		a = ErrorTEht;
		//b = CtlData[cnum].theRect.bottom - CtlData[cnum].theRect.top;
		c = b / a;   	// c now equals # of line viewable
		a = (**ErrorTE).nLines;
		if( !c || c >= a ) return(0);
		b = (a/c);
		return(b);		
}

short	ErrCalcTextView( short b )   // Returns lines viewable
{
		short c;
		
		c = b / ErrorTEht;   	// c now equals # of line viewable
		return(c);		
}

pascal void ErrScrollProcTE( ControlHandle theControl, short partCode )
 {     	 			
  	short			cmax,cmin,cvalue, b,c  ;
	//Rect			theRect;
	
	cvalue = GetControlValue(theControl);
	cmin = GetControlMinimum(theControl);
	cmax = GetControlMaximum(theControl);

    switch( partCode )
    {
		case 20: // inUpButton:
			b = TEGetHeight(0, 0,ErrorTE);
			TEPinScroll( 0, b, ErrorTE);	
			cvalue--;
			if( cvalue >= cmin ) SetControlValue(theControl, cvalue);
			break;
		case 21: //inDownButton:
			b = TEGetHeight(0, 0,ErrorTE);
			TEPinScroll( 0, -b, ErrorTE);	
			cvalue++;
			if( cvalue <= cmax ) SetControlValue(theControl, cvalue);
			break;
		case 22: //inPageUp:
			b = ErrGWHt;
			c =( b / TEGetHeight(0,0,ErrorTE)) -1;
			b = c * TEGetHeight(0,0,ErrorTE);
			TEPinScroll( 0, b, ErrorTE);	
			cvalue -= c;
			if( cvalue < cmin ) cvalue= cmin; 
			SetControlValue(theControl, cvalue);
			break;
		case 23: //inPageDown:
			b = ErrGWHt;
			c = (b / TEGetHeight(0,0,ErrorTE)) -1; // = # lines viewable
			b = c * TEGetHeight(0,0,ErrorTE);;
			TEPinScroll( 0, -b, ErrorTE);	
			cvalue += c;
			if( cvalue > cmax ) cvalue= cmax; 
			SetControlValue(theControl, cvalue);
			break;
	}
}  

short ErrWinMouse( Point pt )
{
	short	b, c, ct, dt;
	Rect	theRect, tr;
	WindowPtr window;
	ControlHandle	theControl;
	Str255	text1;

	
	#ifdef	powerc
     //ControlActionUPP	scrollbarUPP, tescrollUPP;
     ControlActionUPP	tescrollUPP;
   
     //scrollbarUPP = NewControlActionProc( ScrollProc );         
     tescrollUPP = NewControlActionProc( ErrScrollProcTE );         
	#endif
	
	window = ErrorWin;
	
	theRect = window->portRect;
	//theRect = EditWin[WinNum].theRect;
	tr.right = theRect.right - theRect.left-15;
	tr.bottom = theRect.bottom-theRect.top - 16;
	
	if( PtInRect( pt, &tr  ) )
 	{
		TEClick( pt, false ,ErrorTE );
		ErrChangeScrollBar( );
		return 1;
 	}  	     	 					
    ct = FindControl( pt, window, &theControl );
    if( ct )
	{
		switch( ct )
		{
			case 129: //inThumb:
				ct = GetControlValue(theControl);
				dt = TrackControl( theControl, pt, nil );
				c = ct - GetControlValue(theControl);
				b = c * ErrorTEht;
				TEPinScroll( 0, b, ErrorTE);		
				break;
			case 20: //inUpButton:
			case 21: //inDownButton:
			case 22: //inPageUp:
			case 23: //inPageDown:
				tr = ((*ErrorTE)->viewRect);
				ErrGWHt = tr.bottom - tr.top;
				#ifdef powerc
					dt = TrackControl( theControl, pt, tescrollUPP );
				#else
					dt = TrackControl( theControl, pt, ErrScrollProcTE );
				#endif
				break;
		}
	}
	return 0;
	
}
