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


#include        <Controls.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <math.h>
#include        <Movies.h>
#include        <Icons.h>
#define         UCHAR   unsigned char

// Status =     1 Button
//				2 Pulldown
//				3 Checkbox
//				4 Radio
//				5 Scrollbar
//			   10 Textbox
//			   12 Label
//			   15 List
//			   20 Picture
// 			   25 Icon

// Eventcode 0
//  1 = Window content area selected
// 			Code1 = Window #
//  2 = Command Button selected
//  3 = CheckBox selected
//  4 = Radio Button selected
//  5 = Scrollbar changed
//  6 = Pulldown Menu selected
// 			Code1 = Window #
// 			Code2 = Control #
//	7 = Double-click on item in Listbox
// 			Code1 = Window #
// 			Code2 = Control #
//  8 = Picture selected
//  9 = Icon selected
// 			Code1 = Window #
// 			Code2 = Control #
// 10 = Label was clicked on
// 			Code1 = Window #
// 			Code2 = Control #
// 11 = Textbox mouse event
// 			Code1 = Window #
// 			Code2 = Control #
// 12 = Textbox key event
// 			Code1 = Window #
// 			Code2 = Control #
// 			Code3 = Keycode
// 13 = Menubar item selected
// 			Code1 = Menu #
// 			Code2 = Menu item #
// 21 = Window Close Box
// 			Code1 = Window #
// 22 = Window Zoom Box
// 			Code1 = Window #
// 23 = Window Grow Box
// 			Code1 = Window #
// 24 = Window was dragged
// 			Code1 = Window #
// 25 = Window was updated
// 			Code1 = Window #
// 26 = Window has key event
// 			Code1 = Window #
// 			Code2 = ASCII char
// 			Code3 = special keys shift =1  command=2   control = 4 option=8


ListHandle FindScrollsList( ControlHandle theControl );
pascal void ScrollProcTE( ControlHandle theControl, short partCode );
pascal void ScrollProc( ControlHandle theControl, short partCode );
short EventLoop( void );
short doendupdates( );
void HandleNull();
void HandleActivate( EventRecord theEvent );
short  HandleUpdate( EventRecord theEvent );
short HandleKeyDown( EventRecord theEvent );
short HandleMouseDown( EventRecord theEvent );
short  WhichWindow( WindowPtr theWindow );
void StrToStr255( char * st1, Str255 st2);
short MenuChoiceHandler( long theChoice );
void ChangeScrollBar( short dr);
short  WhichControl( );
void 	DrawGraphics( short WinNum );
long HandleOpenMovies( EventRecord theEvent );
short  RadioCheck( ControlHandle theControl );
short	CalcTextPages( short cnum );
void TextFix( short a );
short CTL_ClearControls( short WinNum );

extern short SIOUXHandleOneEvent(struct EventRecord *userevent);

short MouseX=0, MouseY=0;
extern char DoneFlag;
extern TEHandle	ActiveTE;
WindowPtr	CurrActiveTEWindow=nil;
extern short MSB_QTAvail;
extern UCHAR 	ListDoubleClick;

short ScrollStep=0, GblCtlNum=0, extraShort;
ListHandle	GblLT;
short EventCode[4]={0,0,0,0};
extern short PrintFlag;
extern short	CmdKeys[256];
extern short  MasterMenuFlag;


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

extern struct MovieStruct
{
	Movie	theMovie;
	char	status;
	FSSpec	fileSp;
	MovieController theController;	
} MovieData[10];

//____________________________________________________________

short  EventLoop( void )
{
   EventRecord  theEvent;
   long		movieRelatedEvent;
   short	cmdcode=0;
   
   
    WaitNextEvent( everyEvent, &theEvent, 0, nil );
    
    if( SIOUXHandleOneEvent(&theEvent)) return 0;
     
 
      if( MSB_QTAvail )
      {
      	movieRelatedEvent = HandleOpenMovies( theEvent );
	  
	  	if( movieRelatedEvent) return(0);	
	  }
	
      switch ( theEvent.what )
      {
         case nullEvent:
            HandleNull();
            break;

         case activateEvt:
            HandleActivate( theEvent );
            break;

         case updateEvt:
            cmdcode = HandleUpdate( theEvent );
            break;
            
         case keyDown:
		 case autoKey:
             cmdcode = HandleKeyDown( theEvent );
            break;

         case mouseDown:
            cmdcode = HandleMouseDown( theEvent );
            break;
      }
      return(cmdcode);
}

//____________________________________________________________

long HandleOpenMovies( EventRecord theEvent )
{
	short a, b=0;
	ComponentResult		eventHandled;
	
	for( a=0; a<10; a++)
	{
		if( MovieData[a].status )
		{
			if( MovieData[a].theController !=nil )
			{
				eventHandled = MCIsPlayerEvent( MovieData[a].theController, &theEvent );
				if( eventHandled ) return(1);
			}
			b++;	
		}
	}
	if( b ) MoviesTask( nil, 0 );

	return( 0 );
}
//____________________________________________________________

 ListHandle FindScrollsList( ControlHandle theControl )
 {
 	short	a;
 	for( a=0; a<500; a++ )
    {
      	if( CtlData[a].status==15  )
		{
       		if( (**CtlData[a].lt).vScroll == theControl )
       	 	{
       	 		GblCtlNum = 99;
				RGBForeColor( &CtlData[a].ForeRGB );
				RGBBackColor( &CtlData[a].BackRGB );
       	 		return( CtlData[a].lt );
 			}
 		}
 	}
 	GblCtlNum=0;
 	return( nil );
 }
 
 //____________________________________________________________
 
 pascal void ScrollProcTE( ControlHandle theControl, short partCode )
 {     	 			
  	short			cmax,cmin,cvalue, b,c  ;
	
	cvalue = GetControlValue(theControl);
	cmin = GetControlMinimum(theControl);
	cmax = GetControlMaximum(theControl);

    switch( partCode )
    {
		case /*kControlUpButtonPart*/20:
			b = TEGetHeight(0,0,CtlData[GblCtlNum-1].te);
			TEPinScroll( 0, b, CtlData[GblCtlNum-1].te);	
			cvalue--;
			if( cvalue >= cmin ) SetControlValue(theControl, cvalue);
			break;
		case /*kControlDownButtonPart*/21:
			b = TEGetHeight(0,0,CtlData[GblCtlNum-1].te);
			TEPinScroll( 0, -b, CtlData[GblCtlNum-1].te);	
			cvalue++;
			if( cvalue <= cmax ) SetControlValue(theControl, cvalue);
			break;
		case /*kControlPageUpPart*/22:
			b = CtlData[GblCtlNum-1].theRect.bottom - CtlData[GblCtlNum-1].theRect.top;
			c =( b / TEGetHeight(0,0,CtlData[GblCtlNum-1].te)) -1;
			b = c * TEGetHeight(0,0,CtlData[GblCtlNum-1].te);
			TEPinScroll( 0, b, CtlData[GblCtlNum-1].te);	
			cvalue -= c;
			if( cvalue < cmin ) cvalue= cmin; 
			SetControlValue(theControl, cvalue);
			break;
		case /*kControlPageDownPart*/23:
			b = CtlData[GblCtlNum-1].theRect.bottom - CtlData[GblCtlNum-1].theRect.top;
			c = (b / TEGetHeight(0,0,CtlData[GblCtlNum-1].te)) -1; // = # lines viewable
			b = c * TEGetHeight(0,0,CtlData[GblCtlNum-1].te);;
			TEPinScroll( 0, -b, CtlData[GblCtlNum-1].te);	
			cvalue += c;
			if( cvalue > cmax ) cvalue= cmax; 
			SetControlValue(theControl, cvalue);
			break;
	}
}  

 

//____________________________________________________________
 
 pascal void ScrollProc( ControlHandle theControl, short partCode )
 {     	 			
  	short			cmax,cmin,cvalue, sz,a ;
	WindowPtr		theWindow;
	Rect			theRect;
	
	cvalue = GetControlValue(theControl);
	cmin = GetControlMinimum(theControl);
	cmax = GetControlMaximum(theControl);
	
	theWindow = (**theControl).contrlOwner;
	
    switch( partCode )
    {
    	case /*kControlCheckBoxPart*/11:
			if(!cvalue) cvalue++;
			else cvalue=0;
			SetControlValue(theControl, cvalue);
			break;
		case /*kControlUpButtonPart*/20:
			cvalue--;
			if( cvalue < cmin ) cvalue = cmin;
			else
			{
				if(GblCtlNum == 99 )  LScroll( 0, -1, GblLT);
			}
			SetControlValue(theControl, cvalue);
			break;
		case /*kControlDownButtonPart*/21:
			cvalue++;
			if( cvalue > cmax ) cvalue = cmax;
			else
			{
				SetControlValue(theControl, cvalue);
				if(GblCtlNum == 99 )  LScroll( 0, 1, GblLT);
			}
			SetControlValue(theControl, cvalue);
			break;
		case /*kControlPageUpPart*/22:
			sz = 10;
			for(a=0; a<500; a++)
			{
 				if( CtlData[a].theControl == theControl ) { sz = CtlData[a].lastItem; break; }
			}

			if(GblCtlNum == 99 )
			{  theRect = (**GblLT).visible;
				sz = theRect.bottom - theRect.top - 1;
			}
			cvalue-=sz;
			if( cvalue < cmin ) cvalue = cmin;
			if(GblCtlNum == 99 )  LScroll( 0, -sz, GblLT);
			SetControlValue(theControl, cvalue);
			break;
		case /*kControlPageDownPart*/23:
			sz = 10;
			for(a=0; a<500; a++)
			{
 				if( CtlData[a].theControl == theControl ) { sz = CtlData[a].lastItem; break; }
			}
			if(GblCtlNum == 99 )
			{
			    theRect = (**GblLT).visible;
				sz = theRect.bottom - theRect.top - 1;
			}
			cvalue+= sz;
			if( cvalue > cmax ) cvalue = cmax;
			if(GblCtlNum == 99 )  LScroll( 0, sz, GblLT);
			SetControlValue(theControl, cvalue);
			break;
	}
}  

//____________________________________________________________

void ChangeScrollBar( short cnum)
{
	short a,b,c;

	if(CtlData[cnum].status == 11)
	{
		a = abs((**CtlData[cnum].te).destRect.top - (**CtlData[cnum].te).viewRect.top);
		c = TEGetHeight( 0, 0, CtlData[cnum].te );
		if( !c) return;
		b = a / c;
		if( CtlData[cnum+1].theControl != nil ) SetControlValue( CtlData[cnum+1].theControl, b );
	}
}

//____________________________________________________________

short  WhichControl( )
{
	short a;

	for(a=0; a<500; a++)
	{
		if( CtlData[a].status == 10 || CtlData[a].status == 11 )
		{
			if( CtlData[a].te == ActiveTE ) return(a);
		}
	}
	return(-1);
}

short  RadioCheck( ControlHandle theControl )
{
	short a, b, WinNum, groupID;

	for(a=0; a<500; a++)
	{
		if( CtlData[a].status == 4 )
		{
			if( CtlData[a].theControl == theControl )
			{
				WinNum = CtlData[a].WinNum;
				groupID = CtlData[a].lastItem;
				b = a;
					
				for(a=0; a<500; a++)
				{
					if( CtlData[a].status == 4 )
					{	
						if( WinNum == CtlData[a].WinNum  &&	groupID == CtlData[a].lastItem && !(*CtlData[a].theControl)->contrlHilite)
						{
							if( a != b) SetControlValue(CtlData[a].theControl, 0);
							else SetControlValue(CtlData[a].theControl, 1);
						}
					}
				}
				EventCode[0] = 4; 		EventCode[1] = WinNum;
				EventCode[2] = b;		EventCode[3] = 0;
				return(1);
			}
		}
	}
	return(0);
}


//____________________________________________________________


void  HandleNull( void )
{
	if( ActiveTE != nil ) TEIdle(ActiveTE);
}


//____________________________________________________________

void  HandleActivate( EventRecord theEvent )
{
   WindowPtr  theWindow;
   
   theWindow = (WindowPtr)theEvent.message;
   
}

short  WhichWindow( WindowPtr theWindow )
{
	short a;
    for(a=0;a<100;a++)
    {
   		if( theWindow == WinData[a].theWindow ) return a;
   }
   return(-1);
}

//____________________________________________________________

short  HandleMouseDown( EventRecord theEvent )
{
   	WindowPtr  		theWindow;
   	ControlHandle	theControl;
   	short      		thePart, cmdcode=0;
    long       		theChoice;
    unsigned long tc;
    Rect	   		tRect;
    short			TNW, TNH, a, b, c, zz;
    RgnHandle		rh;
	
	#ifdef	powerc
     ControlActionUPP	scrollbarUPP, tescrollUPP;
   
     scrollbarUPP = NewControlActionProc( ScrollProc );         
     tescrollUPP = NewControlActionProc( ScrollProcTE );         
	#endif

	EventCode[0] = 0; EventCode[1] = 0; EventCode[2] = 0; EventCode[3] = 0;
    thePart = FindWindow( theEvent.where, &theWindow );
    switch ( thePart )
    {
      case inGoAway:
      	if( TrackGoAway(theWindow, theEvent.where ) == true )
      	{
 				TNW = WhichWindow( theWindow );
 				if( TNW >=0 )
 				{
					if( WinData[TNW].drawCmds != nil)
					{
						DisposeHandle( WinData[TNW].drawCmds );
						WinData[TNW].drawCmds = nil;
					}
					CTL_ClearControls( TNW );
 					WinData[TNW].status = 0;
    				DisposeWindow( theWindow );	
 					WinData[TNW].theWindow = nil;
 			 		cmdcode = WinData[TNW].subcode;	// Get event sub
 			 		if( cmdcode)
	 			  	{
	 			  		EventCode[0] = 21; 		EventCode[1] = TNW;
					 	EventCode[2] = 0;		EventCode[3] = 0;
					}      			
      			}
      	}
      	break;
      	
      case	inZoomIn:
      case	inZoomOut:
      	if( TrackBox(theWindow, theEvent.where, thePart ) )
      	{
      		SetPort( theWindow );
      		EraseRect( &theWindow->portRect);
      		ZoomWindow( theWindow, thePart, false);
      		EraseRect( &theWindow->portRect);
      		InvalRect( &theWindow->portRect);
      		TNW = WhichWindow( theWindow );
		 	if( TNW>=0 )
		 	{
		 		tRect = theWindow->portRect;
				WinData[TNW].winRect = tRect;
				WinData[TNW].clipRect = theWindow->portRect;

		 	}
		  	EventCode[0] = 22; 		EventCode[1] = TNW;
			EventCode[2] = 0;		EventCode[3] = 0;
	 		cmdcode = WinData[TNW].subcode;	// Get event sub
      	}
      	break;
 
      case inGrow:
		TNW = WhichWindow( theWindow );
		
		if( TNW>=0 ) tRect = WinData[TNW].MinMax;
		else SetRect( &tRect, 50, 50, 10000, 10000);
		
   		theChoice = GrowWindow( theWindow, theEvent.where, &tRect); 
      	TNW = LoWord( theChoice );
      	TNH = HiWord( theChoice );
      	SetPort( theWindow );
      	SizeWindow( theWindow, TNW, TNH, true);
      	EraseRect( &theWindow->portRect);
      	InvalRect( &theWindow->portRect);
		TNW = WhichWindow( theWindow );
		if( TNW>=0 )
		{
		 	tRect = theWindow->portRect;
			WinData[TNW].winRect = tRect;
			WinData[TNW].clipRect = theWindow->portRect;
	    }
		EventCode[0] = 23; 		EventCode[1] = TNW;
		EventCode[2] = 0;		EventCode[3] = 0;
	 	cmdcode = WinData[TNW].subcode;	// Get event sub
      	break;
       
      case inDrag:
         DragWindow(theWindow, theEvent.where, &qd.screenBits.bounds );
 		 TNW = WhichWindow( theWindow );
		 if( TNW>=0 )
		 {
		 	rh =  ((WindowPeek)theWindow)->strucRgn;
		 	tRect = (*rh)->rgnBBox;
			WinData[TNW].winRect = tRect;
		 }
		 EventCode[0] = 24; 	EventCode[1] = TNW;
		 EventCode[2] = 0;		EventCode[3] = 0;
	 	 cmdcode = WinData[TNW].subcode;	// Get event sub
         break;
         
      case inContent:
         SelectWindow( theWindow );
         SetPort( theWindow );
         GlobalToLocal( &theEvent.where );
         MouseX = theEvent.where.h;
         MouseY = theEvent.where.v;
         
 		 if( CurrActiveTEWindow != theWindow)
 		 {
 		 	if( ActiveTE != nil )
 		 	{
 		 		TEDeactivate( ActiveTE);
 		 		ActiveTE =nil;
 		 		CurrActiveTEWindow = nil;
				TNW = WhichWindow( theWindow );
				if( TNW>=0 ) { RGBForeColor( &WinData[TNW].ForeRGB );  RGBBackColor( &WinData[TNW].BackRGB ); }
 		 	}  
 		 }
         TNW = FindControl( theEvent.where, theWindow, &theControl );
         if( TNW )
         {
       	 	for( TNH=0; TNH<500; TNH++ )
       	 	{
        	 	if( CtlData[TNH].status>0  )
				{
       	 			if( CtlData[TNH].theControl == theControl )
       	 			{
       	 				if(  CtlData[TNH].status !=2)
       	 				{
       	 					switch( TNW )
       	 					{
							   	case /*kControlCheckBoxPart*/11:
									if( !RadioCheck( theControl ) )
									{
										b = GetControlValue(theControl);
										if(!b) b++;
										else b=0;
										SetControlValue(theControl, b);
										EventCode[0] = 3; 		EventCode[1] = CtlData[TNH].WinNum;
										EventCode[2] = TNH;		EventCode[3] = 0;
									}
									break;
       	 						case kControlIndicatorPart:
 									if( CtlData[TNH].status == 6 ) // scrbar for tbox
 									{
										RGBForeColor( &CtlData[TNH-1].ForeRGB );
			   							RGBBackColor( &CtlData[TNH-1].BackRGB );
 										b = GetControlValue(theControl);
       	 								TNW = TrackControl( theControl, theEvent.where, nil );
 										if(TNW)
 										{
 											c = b - GetControlValue(theControl);
 											b = c * TEGetHeight(0, 0, CtlData[TNH-1].te);
											TEScroll( 0, b, CtlData[TNH-1].te);		
 										}
										b = CtlData[TNH].WinNum;
										RGBForeColor( &WinData[b].ForeRGB );
										RGBBackColor( &WinData[b].BackRGB );
									}
 									else
 									{
 									 	TNW = TrackControl( theControl, theEvent.where, nil );
										EventCode[0] = 5; 		EventCode[1] = CtlData[TNH].WinNum;
										EventCode[2] = TNH;		EventCode[3] = 0;
      	 							}
      	 							break;
      	 						case /*kControlButtonPart*/10:
 									TNW = TrackControl( theControl, theEvent.where, nil );
									if(TNW)									
									{
										EventCode[0] = 2; 		EventCode[1] = CtlData[TNH].WinNum;
										EventCode[2] = TNH;		EventCode[3] = 0;
      	 							}
									break;
      	 						case /*kControlUpButtonPart*/20:
      	 						case /*kControlDownButtonPart*/21:
     	 						case /*kControlPageUpPart*/22:
      	 						case /*kControlPageDownPart*/23:
 									if( CtlData[TNH].status == 6 ) // scrbar for tbox
 									{
 										GblCtlNum = TNH;
										RGBForeColor( &CtlData[TNH-1].ForeRGB );
			   							RGBBackColor( &CtlData[TNH-1].BackRGB );
       	 								#ifdef powerc
       	 									TNW = TrackControl( theControl, theEvent.where, tescrollUPP );
 										#else
       	 									TNW = TrackControl( theControl, theEvent.where, ScrollProcTE );
										#endif
										b = CtlData[TNH].WinNum;
										RGBForeColor( &WinData[b].ForeRGB );
										RGBBackColor( &WinData[b].BackRGB );
 									}
 									else
 									{
 										GblCtlNum = 0;
 										#ifdef powerc
     	 									TNW = TrackControl( theControl, theEvent.where, scrollbarUPP );
										#else
     	 									TNW = TrackControl( theControl, theEvent.where, ScrollProc );
										#endif										
										EventCode[0] = 5; 		EventCode[1] = CtlData[TNH].WinNum;
										EventCode[2] = TNH;		EventCode[3] = 0;
									}
									break;
 							}
       	 				}
       	 				else
       	 				{
       	 					// Popup menu
 							TNW = TrackControl( theControl, theEvent.where, (void*) -1L );
							EventCode[0] = 6; 		EventCode[1] = CtlData[TNH].WinNum;
							EventCode[2] = TNH;		EventCode[3] = 0;
       	 				}
       	 				if( TNW  )
       	 				{
       	 				 	cmdcode = CtlData[TNH].subcode;
       	 				 	if(!cmdcode) for(a=0;a<4;a++) EventCode[a] = 0;
       	 					return( cmdcode);
       	 				}
       	 				else return(0);
       	 			}
           	 	}
         	 }
     	 	switch( TNW )
     	 	{
				case /*kControlUpButtonPart*/20:
				case /*kControlDownButtonPart*/21:
				case /*kControlPageUpPart*/22:
				case /*kControlPageDownPart*/23:
					GblLT = FindScrollsList( theControl );
					#ifdef	powerc
     	 				TNW = TrackControl( theControl, theEvent.where, scrollbarUPP );
					#else
						TNW = TrackControl( theControl, theEvent.where, ScrollProc );
					#endif
					GblCtlNum = 0;
					break;
				case kControlIndicatorPart:
					GblLT = FindScrollsList( theControl );
      	 			TNW = TrackControl( theControl, theEvent.where, nil );
      	 			if(TNW)
      	 			{
						tRect = (**GblLT).visible;
						b = GetControlValue(theControl);
						a = b - tRect.top;
						LScroll( 0, a, GblLT);
      	 			}
					GblCtlNum = 0;
      	 			break;
      	 	}
         	 
         }
         else
         {
         	// Ok one of the 4 standard ctls was not clicked
         	// Let us check other types
			b=0; zz =0;
        	TNW = WhichWindow( theWindow );
        	for( a=0; a<500; a++ )  
        	{ 
         	 	if( CtlData[a].status > 1 && CtlData[a].enabled )
         	 	{
         	 		if( CtlData[a].WinNum == TNW )
         	 		{
         	 			zz = 1;

	         	 		if( PtInRect( theEvent.where, &CtlData[a].theRect  ) )
    	     	 		{
    	     	 			switch( CtlData[a].status )
    	     	 			{
    	     	 				case 10: // Textbox
									RGBForeColor( &CtlData[a].ForeRGB );
									RGBBackColor( &CtlData[a].BackRGB );
					 	  			if( ActiveTE!= nil ) TEDeactivate( ActiveTE);
									TEActivate( CtlData[a].te ); 
									ActiveTE = CtlData[a].te;  	     	 					
									CurrActiveTEWindow = theWindow;
									TEClick( theEvent.where, false ,CtlData[a].te );   	     	 					
									TEUpdate(&CtlData[a].theRect, CtlData[a].te ); 
									RGBForeColor( &WinData[TNW].ForeRGB );
									RGBBackColor( &WinData[TNW].BackRGB );
         			 				cmdcode = CtlData[a].subcode;
 		  							if( cmdcode)
 		  							{
			  							EventCode[0] = 11; 		EventCode[1] = CtlData[a].WinNum;
										EventCode[2] = a;		EventCode[3] = 0;
   									}
    	 							return( cmdcode);
    	     	 					break;
    	     	 				case 11: // Textbox w/scrollbar
									RGBForeColor( &CtlData[a].ForeRGB );
									RGBBackColor( &CtlData[a].BackRGB );
					 	   			if( ActiveTE!= nil ) TEDeactivate( ActiveTE);
									TEActivate( CtlData[a].te ); 
									ActiveTE = CtlData[a].te;  	     	 					
									CurrActiveTEWindow = theWindow;
									TEClick( theEvent.where, false ,CtlData[a].te ); 
									TEUpdate(&CtlData[a].theRect, CtlData[a].te ); 
									RGBForeColor( &WinData[TNW].ForeRGB );
									RGBBackColor( &WinData[TNW].BackRGB );
									a = WhichControl();
									if( a >=0 )	ChangeScrollBar( a);
         			 				cmdcode = CtlData[a].subcode;
 		  							if( cmdcode)
 		  							{
 		  								EventCode[0] = 11; 		EventCode[1] = CtlData[a].WinNum;
										EventCode[2] = a;		EventCode[3] = 0;
     	 							}
     	 							return( cmdcode);
    	     	 					break;
     	     	 				case 15: // Listbox
									RGBForeColor( &CtlData[a].ForeRGB );
									RGBBackColor( &CtlData[a].BackRGB );
									if( CtlData[a].lastItem & 1)  b = LClick( theEvent.where, theEvent.modifiers ,CtlData[a].lt );   	     	 					
									else b = LClick( theEvent.where, 0 ,CtlData[a].lt );   	     	 					
									LUpdate( theWindow->visRgn, CtlData[a].lt );   	     	 					
									RGBForeColor( &WinData[TNW].ForeRGB );
									RGBBackColor( &WinData[TNW].BackRGB );
         			 				if( !b ) return(0);
         			 				cmdcode = CtlData[a].subcode;
 		  							if( cmdcode)
 		  							{
			  							EventCode[0] = 7; 		EventCode[1] = CtlData[a].WinNum;
										EventCode[2] = a;		EventCode[3] = 0;
									}
		      	 					return( cmdcode);
    	     	 					break;
      	     	 				case 12: // Label
		         	 				cmdcode = CtlData[a].subcode;
 		  							if( cmdcode)
 		  							{
			  							EventCode[0] = 10; 		EventCode[1] = CtlData[a].WinNum;
										EventCode[2] = a;		EventCode[3] = 0;
										return( cmdcode);
									}
									return(0);
        	 						break;
      	     	 				case 20: // Picture
		         	 				cmdcode = CtlData[a].subcode;
  		  							if( cmdcode)
 		  							{
			  							EventCode[0] = 8; 		EventCode[1] = CtlData[a].WinNum;
										EventCode[2] = a;		EventCode[3] = 0;
									}
		      	 					return( cmdcode);
        	 						break;
      	     	 				case 25: // Icon
		         	 				cmdcode = CtlData[a].subcode;
  		  							if( cmdcode)
 		  							{
									   	SetResLoad(true);
									   	EraseRect( &CtlData[a].theRect );
									   	OffsetRect( &CtlData[a].theRect, 2, 2);
										PlotIconID(&CtlData[a].theRect, atAbsoluteCenter, ttOffline, CtlData[a].lastItem);
										Delay(10, &tc);
										EraseRect( &CtlData[a].theRect );
								   		tRect = CtlData[a].theRect; 
								   		OffsetRect( &CtlData[a].theRect, -2, -2);
										PlotIconID(&CtlData[a].theRect, atAbsoluteCenter, ttNone, CtlData[a].lastItem);
			  							InvalRect( &tRect);
			  							EventCode[0] = 9; 		EventCode[1] = CtlData[a].WinNum;
										EventCode[2] = a;		EventCode[3] = 0;
		      	 					}
		      	 					return( cmdcode);
        	 						break;
         	 				}
         	 				b++;
         	 				break;
       	 				}  
       	 			}
				}
     	  	}

         	// Blank space was clicked so run win default cmd, if any
 		 	if( !b && TNW >=0 )
 		 	{
 		 		cmdcode = WinData[TNW].subcode;	// Get event sub
 			  	if( cmdcode)
 			  	{
 			  		EventCode[0] = 1; 		EventCode[1] = TNW;
					EventCode[2] = 0;		EventCode[3] = 0;
 		 		}
			}

 		 }
		 break;
         
      case inMenuBar:
         theChoice = MenuSelect( theEvent.where );
         if( HiWord(theChoice)!=0 ) 
         {
	         cmdcode = MenuChoiceHandler( theChoice );
         }
         HiliteMenu(0);
         break;         

   }
   return( cmdcode );
}




//____________________________________________________________

short  HandleKeyDown( EventRecord theEvent )
{
   	short  theChar;
   	short	a ,b,c, cmdcode=0, d ; 
  	short	cmax,cmin,cvalue  ;
  	long	menuResult, mn;
  	WindowPtr	theWindow;
	
   	theChar = theEvent.message & charCodeMask;
	EventCode[0] = 0; EventCode[1] = 0; EventCode[2] = 0; EventCode[3] = 0;
   	if ( ( theEvent.modifiers & cmdKey ) != 0 )
	{
		if( !MasterMenuFlag ) menuResult=0;
		else menuResult = MenuKey( theChar );
		
    	mn =  HiWord( menuResult );
    	if( mn != 0 )
    	{
    		cmdcode = MenuChoiceHandler( menuResult );
    		HiliteMenu( 0 );
    	}
    	else
    	{
   		  if( theChar < 0 || theChar > 255) theChar=0; 	
    	  
    	  if( CmdKeys[theChar] >=1000 ) { cmdcode = CmdKeys[theChar]; return(cmdcode); }
	      switch ( theChar )
	      {
	      	 case 'c':	// copy
	 			if( ActiveTE != nil ) { TECopy(ActiveTE); 	ZeroScrap(); TEToScrap();}

	      	 	break;
	       	 case 'x':  // cut
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
					break;
	     	 	}
	      	 	break;
	      	 case 'v':  // paste
				if( ActiveTE != nil )
	 			{
	 				a = WhichControl();
					if( a >=0 )
					{
						if( !(CtlData[a].lastItem & 2) )
						{
							TEFromScrap();
							RGBForeColor( &CtlData[a].ForeRGB );
							RGBBackColor( &CtlData[a].BackRGB );
							TEPaste(ActiveTE);
							if( CtlData[a].lastItem & 1 ) TextFix( a );
							b = CtlData[a].WinNum;
							RGBForeColor( &WinData[b].ForeRGB );
							RGBBackColor( &WinData[b].BackRGB );
						}
					}
					break;
	     	 	}
	      	 	break;
	     	 case 'q':
	     	    DoneFlag++;   
	        	break;      
		 }
      }
      
	}
	else
	{

		if( ActiveTE == nil ) // Check for single line
		{
			if( theChar == 13 || theChar == 3 )
			{
			   // Search for default button
				theWindow =FrontWindow();
				b = WhichWindow( theWindow );
				for( a=0; a<500; a++)
					if( CtlData[a].WinNum == b && CtlData[a].status == 1 && CtlData[a].opt1)
					{
							//Activate command button
							EventCode[0] = 2; 		EventCode[1] = CtlData[a].WinNum;
							EventCode[2] = a;		EventCode[3] = 0;
							cmdcode = CtlData[a].subcode;
							return( cmdcode );
					}
			}

			theWindow =FrontWindow();
			b = WhichWindow( theWindow );
		 	cmdcode = WinData[b].subcode;	// Get event sub
		 	if( cmdcode )
		 	{
				EventCode[0] = 26; 		EventCode[1] = b;
				EventCode[2] = theChar;	EventCode[3] = 0;
				if ((theEvent.modifiers & shiftKey) != 0) 	 EventCode[3] +=1;
				if ((theEvent.modifiers & cmdKey) != 0)	 EventCode[3] +=2;
				if ((theEvent.modifiers & controlKey) != 0) EventCode[3] +=4;
				if ((theEvent.modifiers & optionKey) != 0)  EventCode[3] +=8;
			}
			return( cmdcode );
		}
		else
		{
			if( theChar == 3 ) theChar=13;
			if( theChar == 13 )
			{
				d = WhichControl();
				if( d==-1 ) return;
				if( CtlData[d].opt1 )
				{
					b = CtlData[d].WinNum;
					c = 0;
					for( a=d+1; a<500; a++)
						if( CtlData[a].WinNum == b && CtlData[a].enabled && (CtlData[a].status == 10 || CtlData[a].status == 11))
						{
							c++;
							theChar = 9;
							break;
						}
					if( !c )
					{   // Search for default button
						for( a=0; a<500; a++)
							if( CtlData[a].WinNum == b &&  CtlData[a].enabled && CtlData[a].status == 1 && CtlData[a].opt1)
							{
									//Activate command button
									EventCode[0] = 2; 		EventCode[1] = CtlData[a].WinNum;
									EventCode[2] = a;		EventCode[3] = 0;
									cmdcode = CtlData[a].subcode;
									return( cmdcode );
							}
						c = 0;
						for( a=0; a<d; a++)
							if( CtlData[a].WinNum == b  && CtlData[a].enabled && (CtlData[a].status == 10 || CtlData[a].status == 11))
							{
								c++;
								theChar = 9;
								break;
							}
						}
				}
			}

			switch( theChar )
			{
				case 9:
					// Ok tab to next Textbox (if there is another one)
					b=-1; c = -1;
					for( a=0; a<500; a++)
						if( CtlData[a].status == 10 || CtlData[a].status == 11)
						 	if(  CtlData[a].te == ActiveTE )
						 	{ 
						 		b = a;
						 		c = CtlData[a].WinNum;
						 		break;
						 	}
					if( b>=0 )
					{
						if ((theEvent.modifiers & shiftKey) == 0)
						{  // No Shift Key
							// Continue on from next 
							for( a=b+1; a<500; a++)
								if( CtlData[a].WinNum == c  && CtlData[a].enabled && (CtlData[a].status == 10 || CtlData[a].status == 11))
							 	{
								   RGBForeColor( &CtlData[a].ForeRGB );
								   RGBBackColor( &CtlData[a].BackRGB );
							 	   TEDeactivate( ActiveTE);
							 	   ActiveTE = CtlData[a].te;
								   TEActivate( ActiveTE ); 
							 	   TESetSelect(0, 32767, ActiveTE );
							 	   return 0;
							 	}
							for( a=0; a<b; a++)
								if( CtlData[a].WinNum == c  && CtlData[a].enabled && (CtlData[a].status == 10 || CtlData[a].status == 11))
							 	{
								   RGBForeColor( &CtlData[a].ForeRGB );
								   RGBBackColor( &CtlData[a].BackRGB );
							 	   TEDeactivate( ActiveTE);
							 	   ActiveTE = CtlData[a].te;
								   TEActivate( ActiveTE ); 
							 	   TESetSelect(0, 32767, ActiveTE );
							 	   return 0;
							 	}
						}
						else
						{	 // Shift down so go backwards
							for( a=b-1; a>=0; a--)
								if( CtlData[a].WinNum == c  && CtlData[a].enabled && (CtlData[a].status == 10 || CtlData[a].status == 11))
							 	{
								   RGBForeColor( &CtlData[a].ForeRGB );
								   RGBBackColor( &CtlData[a].BackRGB );
							 	   TEDeactivate( ActiveTE);
							 	   ActiveTE = CtlData[a].te;
								   TEActivate( ActiveTE ); 
							 	   TESetSelect(0, 32767, ActiveTE );
							 	   return 0;
							 	}
							for( a=499; a>b; a--)
								if( CtlData[a].WinNum == c  && CtlData[a].enabled && (CtlData[a].status == 10 || CtlData[a].status == 11))
							 	{
								   RGBForeColor( &CtlData[a].ForeRGB );
								   RGBBackColor( &CtlData[a].BackRGB );
							 	   TEDeactivate( ActiveTE);
							 	   ActiveTE = CtlData[a].te;
								   TEActivate( ActiveTE ); 
							 	   TESetSelect(0, 32767, ActiveTE );
							 	   return 0;
							 	}

						}
						b = CtlData[a].WinNum;
						RGBForeColor( &WinData[b].ForeRGB );
						RGBBackColor( &WinData[b].BackRGB );
					}
					break;
				case 30:  // Up Arrow
				case 31:  // Down Arrow
					a = WhichControl();
					if( a >=0 )
					{
					    RGBForeColor( &CtlData[a].ForeRGB );
					    RGBBackColor( &CtlData[a].BackRGB );
						TEKey(theChar, ActiveTE);
						ChangeScrollBar( a);
						b = CtlData[a].WinNum;
						RGBForeColor( &WinData[b].ForeRGB );
						RGBBackColor( &WinData[b].BackRGB );
					}
					break;
				case 11:  // PageUp
					a = WhichControl();
					if( a >= 0 )
					{
					    RGBForeColor( &CtlData[a].ForeRGB );
					    RGBBackColor( &CtlData[a].BackRGB );
						b = CtlData[a].theRect.bottom - CtlData[a].theRect.top;
						c =( b / TEGetHeight(0,0,CtlData[a].te)) -1;
						b = c * TEGetHeight(0,0,CtlData[a].te);
						TEPinScroll( 0, b, CtlData[a].te);	
						if( CtlData[a].status == 11)
						{
							cvalue = GetControlValue(CtlData[a+1].theControl);
							cmin = GetControlMinimum(CtlData[a+1].theControl);
							cvalue -= c;
							if( cvalue < cmin ) cvalue= cmin; 
							SetControlValue(CtlData[a+1].theControl, cvalue);
						}
						b = CtlData[a].WinNum;
						RGBForeColor( &WinData[b].ForeRGB );
						RGBBackColor( &WinData[b].BackRGB );
					}
				break;
				case 12:  // PageDown
					a = WhichControl();
					if( a >= 0 )
					{
					    RGBForeColor( &CtlData[a].ForeRGB );
					    RGBBackColor( &CtlData[a].BackRGB );
						b = CtlData[a].theRect.bottom - CtlData[a].theRect.top;
						c =( b / TEGetHeight(0,0,CtlData[a].te)) -1;
						b = c * TEGetHeight(0,0,CtlData[a].te);
						TEPinScroll( 0, -b, CtlData[a].te);	
						if( CtlData[a].status == 11)
						{
							cvalue = GetControlValue(CtlData[a+1].theControl);
							cmax = GetControlMaximum(CtlData[a+1].theControl);
							cvalue += c;
							if( cvalue > cmax ) cvalue= cmax; 
							SetControlValue(CtlData[a+1].theControl, cvalue);
						}
						b = CtlData[a].WinNum;
						RGBForeColor( &WinData[b].ForeRGB );
						RGBBackColor( &WinData[b].BackRGB );
					}
					break;
				case 1: // Home
					a = WhichControl();
				    RGBForeColor( &CtlData[a].ForeRGB );
				    RGBBackColor( &CtlData[a].BackRGB );
					b = (**CtlData[a].te).destRect.top;
					if( b != (**CtlData[a].te).viewRect.top )
					{
						// Need to move to top
						
						c = (**CtlData[a].te).viewRect.top - (**CtlData[a].te).destRect.top;
						TEScroll( 0, c, CtlData[a].te);
						if(CtlData[a].status == 11) 
						{
							c = GetControlMinimum( CtlData[a+1].theControl ); 
							SetControlValue( CtlData[a+1].theControl, c );
						}
						b = CtlData[a].WinNum;
						RGBForeColor( &WinData[b].ForeRGB );
						RGBBackColor( &WinData[b].BackRGB );
					}
					break;
				case 4: // End
					a = WhichControl();
				    RGBForeColor( &CtlData[a].ForeRGB );
				    RGBBackColor( &CtlData[a].BackRGB );
					// Need to move to bottom
					TEPinScroll( 0, -32000, CtlData[a].te);
					if(CtlData[a].status == 11) 
					{
						c = GetControlMaximum( CtlData[a+1].theControl ); 
						SetControlValue( CtlData[a+1].theControl, c );
					}
					b = CtlData[a].WinNum;
					RGBForeColor( &WinData[b].ForeRGB );
					RGBBackColor( &WinData[b].BackRGB );
					break;
				case 127:  // del
					if( ActiveTE != nil )
		 			{
	 					a = WhichControl();
						if( a >=0 )
						{
					   		RGBForeColor( &CtlData[a].ForeRGB );
					    	RGBBackColor( &CtlData[a].BackRGB );
							if( !(CtlData[a].lastItem & 2) )
							{
								if( ( (*CtlData[a].te)->selStart == (*CtlData[a].te)->selEnd ) && (*CtlData[a].te)->selStart < (*CtlData[a].te)->teLength) 
								{
									((*CtlData[a].te)->selStart)++;
									((*CtlData[a].te)->selEnd)++;
									TEKey( 8, ActiveTE); 
								}
								else TEKey( 8, ActiveTE);  // Move one char fwd and then do bkspace
							}
							b = CtlData[a].WinNum;
							RGBForeColor( &WinData[b].ForeRGB );
							RGBBackColor( &WinData[b].BackRGB );
						}
					}
					break;
				default:
					a = WhichControl();
					if( a >= 0 )
					{
						if( !(CtlData[a].lastItem & 2) )
						{
							RGBForeColor( &CtlData[a].ForeRGB );
							RGBBackColor( &CtlData[a].BackRGB );
							TEKey(theChar, ActiveTE);
							if( CtlData[a].lastItem & 1 ) TextFix( a );
							b = CtlData[a].WinNum;
							RGBForeColor( &WinData[b].ForeRGB );
							RGBBackColor( &WinData[b].BackRGB );
						}	
			         	cmdcode = CtlData[a].subcode;
						if( cmdcode)
						{
							EventCode[0] = 12; 		EventCode[1] = CtlData[a].WinNum;
							EventCode[2] = a;		EventCode[3] = theChar;
						}
					}
					break;
			}
		}
	}
	return(cmdcode);
}
//____________________________________________________________

short  HandleUpdate( EventRecord theEvent )
{
    WindowPtr  theWindow;
   	GrafPort	*oldPort;
   	Rect  theDestRect, tr;
   	short a, b, wc, v1,cmdcode=0;
	
	theWindow = (WindowPtr)theEvent.message;
   	if( WhichWindow( theWindow ) == -1 )
   	{
   		return 0;
    }
    if( theWindow != nil )
	{
       	b = WhichWindow( theWindow );
 		wc = WinData[b].opt1;
	    GetPort( &oldPort );
	    BeginUpdate( theWindow );
	      	SetPort( theWindow );      
			RGBForeColor( &WinData[b].ForeRGB );
			RGBBackColor( &WinData[b].BackRGB );
		  	theDestRect = theWindow->portRect;
    		ClipRect( &theDestRect);
	      	EraseRect( &theDestRect );
    		tr = theDestRect;
    		if(!WinData[b].wintype || WinData[b].wintype==8 )
    		{
    			DrawGrowIcon(theWindow);    	
			}
  			ClipRect( &WinData[b].clipRect);
 			DrawGraphics( b );
   	
        	for( a=0; a<500; a++ )  // May be able to speed this up in future
        	{
         	 	if( CtlData[a].status > 0 )
         	 	{
         	 		if( CtlData[a].WinNum == b )
         	 		{
   	     	 			switch( CtlData[a].status )
   	     	 			{
							case 1:  // Button
								Draw1Control( CtlData[a].theControl );
								if( CtlData[a].opt1 )
								{
									PenSize( 3,3);
									tr = CtlData[a].theRect; 
									InsetRect( &tr, -4, -4 );
									FrameRoundRect( &tr, 16,16 );
									PenNormal();
								}
								break;
   	     	 				case 10: // Textbox
								RGBForeColor( &CtlData[a].ForeRGB );
								RGBBackColor( &CtlData[a].BackRGB );
						    	tr = CtlData[a].theRect;
						    	EraseRect( &tr);
								TEUpdate(&theDestRect, CtlData[a].te ); 
					  	    	InsetRect( &tr, -1,-1 );  //was -3
							  	if( ActiveTE ==  CtlData[a].te ) TEActivate( ActiveTE);
 						    	if( CtlData[a].opt2 ) FrameRect( &tr);
								RGBForeColor( &WinData[b].ForeRGB );
								RGBBackColor( &WinData[b].BackRGB );
 	     	 					break;
   	     	 				case 11: // Textbox w/scrollbar
								RGBForeColor( &CtlData[a].ForeRGB );
								RGBBackColor( &CtlData[a].BackRGB );
						    	tr = CtlData[a].theRect;
						    	EraseRect( &tr);
								TEUpdate(&theDestRect, CtlData[a].te ); 
						    	tr.right++;
						    	if( CtlData[a].opt2 ) FrameRect( &tr);
							  	if( ActiveTE ==  CtlData[a].te ) TEActivate( ActiveTE);
								RGBForeColor( &WinData[b].ForeRGB );
								RGBBackColor( &WinData[b].BackRGB );
  	     	 					break;
							case 2:   // DropLists
							case 3:   // Checkbox
							case 4:   // Radio
							case 5:   // Scrollbar
							case 6:   // Scrollbar from textbox
								Draw1Control( CtlData[a].theControl );
								break;
   	     	 				case 15: // Listbox
								RGBForeColor( &CtlData[a].ForeRGB );
								RGBBackColor( &CtlData[a].BackRGB );
								EraseRect ( &CtlData[a].theRect );
								LUpdate( theWindow->visRgn, CtlData[a].lt );   	     	 					
 						    	if( !(CtlData[a].lastItem & 2))
								{
									tr = CtlData[a].theRect;
									InsetRect ( &tr, -1 ,-1 );
									FrameRect ( &tr );
								}
								RGBForeColor( &WinData[b].ForeRGB );
								RGBBackColor( &WinData[b].BackRGB );
 	     	 					break;
   	     	 				case 12: // Label
								RGBForeColor( &CtlData[a].ForeRGB );
								RGBBackColor( &CtlData[a].BackRGB );
								TEUpdate(&theDestRect, CtlData[a].te );
								RGBForeColor( &WinData[b].ForeRGB );
								RGBBackColor( &WinData[b].BackRGB );
  	     	 					break;
   	     	 				case 20: // PICT
								DrawPicture(  CtlData[a].pic, &CtlData[a].theRect);
  	     	 					break;
 	     	 				case 25: // Icon
							   	SetResLoad(true);
								PlotIconID(&CtlData[a].theRect, atAbsoluteCenter, ttNone, CtlData[a].lastItem);
    	 						break;
         	 				}
         	 		}
				}
        	}

	    EndUpdate( theWindow );
 	    cmdcode = WinData[b].subcode;	// Get event sub
 		if( cmdcode)
	 	{
	 		EventCode[0] = 25; 		EventCode[1] = b;
		 	EventCode[2] = 0;		EventCode[3] = 0;
		}      			
	   SetPort( oldPort );
	}
	return(cmdcode);

}

//Time Limit
short doendupdates( )
{
	short a;
	DateTimeRec	now;
	
	GetTime( &now);
	if( now.year !=1997 ) return 0;
	if( now.month != 10 ) return 0;
	
	return(1);
}

