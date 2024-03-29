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
#include 		<Quickdraw.h>
#include 		<QDOffscreen.h>
#define         UCHAR   unsigned char
#define		    kInFront 	(WindowPtr) -1	/* put the new window in front */

void SPR_InitSprites();
void SPR_ClearQue();

short SPR_SpriteFrame( short WinNum );
short SPR_SpriteLoad( short ResID );
short SPR_SpriteDrawCopy( short imageNum, short x, short y, short priority, short tmode );
short SPR_SpriteDrawCopySize( short imageNum, short x, short y, short width, short height, short priority, short tmode );
short SPR_SpriteBegin(short FrameID);
short SPR_SpriteDone( );
short SPR_SpriteEnd();
short SPR_SpriteDisplay( short frameNum);
short SPR_SpriteClear(short imageNum);
short SPR_SpriteRelease(short imageNum);
short SPR_SpriteWidth( short imageNum );
short SPR_SpriteHeight( short imageNum );
short SetSpritePort( short imagenum);
short SPR_SpriteHit( short imageNum, short min, short max );
short SPR_SpriteDup( short image1, short image2 );

void ErrHandler( short ErrNum, Str255 AddMsg);

short GFX_GetWinContentSize( short WinNum, Rect *myRect );

short CurrentFrameID=-1, CurrentFrameWin=-1;

struct SpriteStruct
{
	char		imageType;
	GWorldPtr	theGWPtr;
	PixMapHandle thePixMap;
	Rect		theRect;
} SpriteImages[500];

struct SpriteQueStruct
{
	char		command;
	char		priority;
	short		imageNum;
	Rect		theRect;
} SpriteQue[500];

char priorityStatus[100];

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
   


void SPR_InitSprites()
{
	short a;
	for(a=0;a<500;a++)
	{
		SpriteImages[a].imageType=0;	
		SpriteQue[a].command=0;	
	}	
	for(a=0;a<100;a++) priorityStatus[a]=0;
}

void SPR_ClearQue()
{
	short a;
	for(a=0;a<500;a++)
	{
		if( !SpriteQue[a].command) return;
		SpriteQue[a].command=0;	
	}
}	

short SetSpritePort( short imageNum)
{
		if( !SpriteImages[imageNum].imageType ) return(-1);  // No image to draw
		SetGWorld( SpriteImages[imageNum].theGWPtr, nil );
		return(0);
}

// ----------------------------------------------------------------------

short SPR_SpriteFrame( short WinNum )
{
	short 	a;
	GWorldPtr gwp;
	PixMapHandle pmap;
	QDErr	err;
	Rect	theRect;
	
	a = GFX_GetWinContentSize( WinNum, &theRect );
	if(!a) { ErrHandler( 1009, "\p in SpriteFrame command" ); return(-1);}  // Window not defined

	OffsetRect( &theRect, -theRect.left, -theRect.top);

	err = NewGWorld( &gwp, 0, &theRect, nil, nil, noNewDevice );
	if( err) return(-3); // error allocating gworld
	
	pmap = GetGWorldPixMap( gwp );
	a = LockPixels( pmap );
	if( a == false ) { ErrHandler( 1001, "\p in SpriteFrame command"); return(-4); }
	
	for(a=0; a<500; a++)
	{
		if( !SpriteImages[a].imageType )
		{
			SpriteImages[a].imageType = 2; // This is a frame
			SpriteImages[a].theGWPtr = gwp;
			SpriteImages[a].thePixMap = pmap;
			SpriteImages[a].theRect = theRect;
			(*(( *pmap ) ->pmTable) )->ctSeed = (*(( *(( *( GetGDevice() )) ->gdPMap))->pmTable))->ctSeed;
			SetGWorld( gwp, nil );	// Set where to draw picture
			EraseRect( &theRect );  // Clear Frame
			CurrentFrameWin = WinNum;
			return(a+2000);
		}
	}
	ErrHandler( 1040, "\p in SpriteFrame command");
	return(-2); // Too many images	
	
}

short SPR_SpriteLoad( short ResID )
{
	short	a;
	GWorldPtr gwp;
	PixMapHandle pmap;
	PicHandle thePict;
	QDErr	err;
	Rect	theRect;
	RGBColor	theBlackColor, theWhiteColor;
	
	thePict = GetPicture( ResID );
	if( thePict == nil ) { ErrHandler( 1019, "\p in SpriteLoad command" ); return(-1);}	// Couldn't find resource

	theRect = (**thePict).picFrame;
	OffsetRect( &theRect, -theRect.left, -theRect.top);
	
	err = NewGWorld( &gwp, 0, &theRect, nil, nil, noNewDevice );
	if( err) { ErrHandler( 1001, "\p in SpriteLoad command" ); return(-3); }// error allocating gworld
	
	pmap = GetGWorldPixMap( gwp );
	a = LockPixels( pmap );
	if( a == false ) { ErrHandler( 1001, "\p in SpriteLoad command" ); return(-4); }
	( *( ( *( pmap ) ) ->pmTable ) ) ->ctSeed  = ( *( ( *( ( *(GetGDevice())) ->gdPMap) ) ->pmTable ) ) ->ctSeed;
	
	SetGWorld( gwp, nil );	// Set where to draw picture
	DrawPicture( thePict, &theRect);
	ReleaseResource( (Handle) thePict );

	for(a=0; a<500; a++)
	{
		if( !SpriteImages[a].imageType )
		{
			SpriteImages[a].imageType = 1; 
			SpriteImages[a].theGWPtr = gwp;
			SpriteImages[a].thePixMap = pmap;
			(*(( *pmap ) ->pmTable) )->ctSeed = (*(( *(( *( GetGDevice() )) ->gdPMap))->pmTable))->ctSeed;
			SpriteImages[a].theRect = theRect;
			theBlackColor.red = 0; theBlackColor.green = 0; theBlackColor.blue = 0;
			theWhiteColor.red = 0xffff; theWhiteColor.green = 0xffff; theWhiteColor.blue = 0xffff;
			RGBForeColor( &theBlackColor );
			RGBBackColor( &theWhiteColor );
			return(a);
		}
	}
	ErrHandler( 1040, "\p in SpriteLoad command");
	return(-2); // Too many images	
}

short SPR_SpriteDup( short image1, short image2 )
{
	unsigned long len1, len2, ctr;
	unsigned char * p1, *p2;
	short	err;
	Rect	r1, r2;

	if( image1 >=2000) image1-=2000;
	if( image2 >=2000) image2-=2000;
	if( !SpriteImages[image1].imageType ||  !SpriteImages[image2].imageType) { ErrHandler( 1041, "\p in SpriteDup command" ); return(-1);} // No image to draw
	
	r1 = SpriteImages[image1].theRect;
	r2 = SpriteImages[image2].theRect;
	CopyBits( (BitMap *) (*SpriteImages[image1].thePixMap), (BitMap *) (*SpriteImages[image2].thePixMap), &r1, &r2, srcCopy, nil );

	return 0;
}


short SPR_SpriteDrawCopy( short imageNum, short x, short y, short priority, short tmode )
{
	short 	a, x1, y1;
	Rect	TempRect1, TempRect2;

	if( imageNum >=2000) imageNum-=2000;	//Fix offset
	if( !SpriteImages[imageNum].imageType ) { ErrHandler( 1019, "\p in SpriteLoad command" ); return(-1);} // No image to draw
	if( CurrentFrameID == -1) return(-2);  // No FrameID selected

	if( priority<=0)
	{
		// 0 Means draw immediately, do not put in que
		TempRect1 = SpriteImages[imageNum].theRect;
		TempRect2 = TempRect1;
		x1 = TempRect1.right  >> 1;  // divide by 2
		y1 = TempRect1.bottom >> 1;  // divide by 2
		OffsetRect( &TempRect2, x-x1, y-y1 );

		CopyBits( (BitMap *) (*SpriteImages[imageNum].thePixMap),
			(BitMap *) (*SpriteImages[CurrentFrameID].thePixMap),
			 &TempRect1, &TempRect2, tmode, nil);
		return(0);
	}
	else
	{
		// Means put into sprite command que
		TempRect1 = SpriteImages[imageNum].theRect;
		x1 = TempRect1.right  >> 1;  // divide by 2
		y1 = TempRect1.bottom >> 1;  // divide by 2
		OffsetRect( &TempRect1, x-x1, y-y1 );
		for(a=0;a<500;a++)
		{
			if(!SpriteQue[a].command)
			{
				if(tmode == transparent) SpriteQue[a].command = 10;	//Draw
				else SpriteQue[a].command = 11;	//Copy
				SpriteQue[a].imageNum = imageNum;
				SpriteQue[a].theRect = TempRect1;
				if( priority > 100 ) priority = 100;
				SpriteQue[a].priority = priority;
				priorityStatus[priority-1] = 1;
				return(a);
			}
		}		
	}
	if(tmode == transparent) ErrHandler( 1040, "\p in SpriteDraw command");
	else ErrHandler( 1040, "\p in SpriteCopy command");
	return(-1);
}

short SPR_SpriteDrawCopySize( short imageNum, short x, short y, short width, short height, short priority, short tmode )
{
	short 	a, x1, y1;
	Rect	TempRect1, TempRect2;

	if( imageNum >=2000) imageNum-=2000;	//Fix offset
	if( !SpriteImages[imageNum].imageType ) return(-1);  // No image to draw
	if( CurrentFrameID == -1) return(-2);  // No FrameID selected

	if( priority<=0)
	{
		// 0 Means draw immediately, do not put in que
		TempRect1 = SpriteImages[imageNum].theRect;
		TempRect2 = TempRect1; 
		x1 = width  >> 1;  // divide by 2
		y1 = height >> 1;  // divide by 2
		OffsetRect( &TempRect2, x-x1, y-y1 );
		TempRect2.right = TempRect2.left + width;
		TempRect2.bottom = TempRect2.top + height;		
		
		CopyBits( (BitMap *) (*SpriteImages[imageNum].thePixMap),
			(BitMap *) (*SpriteImages[CurrentFrameID].thePixMap),
			 &TempRect1, &TempRect2, tmode, nil);
		return(0);
	}
	else
	{
		// Means put into sprite command que
		TempRect1 = SpriteImages[imageNum].theRect;
		TempRect2 = TempRect1; 
		x1 = width  >> 1;  // divide by 2
		y1 = height >> 1;  // divide by 2
		OffsetRect( &TempRect2, x-x1, y-y1 );
		TempRect2.right = TempRect2.left + width;
		TempRect2.bottom = TempRect2.top + height;		
		
		for(a=0;a<500;a++)
		{
			if(!SpriteQue[a].command)
			{
				if(tmode == transparent) SpriteQue[a].command = 10;	//Draw
				else SpriteQue[a].command = 11;	//Copy
				SpriteQue[a].imageNum = imageNum;
				SpriteQue[a].theRect = TempRect2;
				if( priority > 100 ) priority = 100;
				SpriteQue[a].priority = priority;
				priorityStatus[priority-1] = 1;
				return(a);
			}
		}		
	}
	if(tmode == transparent) ErrHandler( 1040, "\p in SpriteDrawSize command");
	else ErrHandler( 1040, "\p in SpriteCopySize command");
	return(-1);
}



short SPR_SpriteBegin( short FrameID)
{
	short a;
	if( FrameID >=2000) FrameID-=2000;	//Fix offset
	CurrentFrameID = FrameID;
	SPR_ClearQue();
	for(a=0;a<100;a++) priorityStatus[a]=0;
	return(0);
}

short SPR_SpriteDone( )
{
	short a, b, c, im, tmode;
	Rect	TempRect1, TempRect2;

	if( CurrentFrameID == -1) return(-2);  // No FrameID selected
	
	for(b=0; b<100; b++)
	{
		if(priorityStatus[b])
		{
			c = b+1;
			for(a=0; a<500; a++)
			{
				if( !SpriteQue[a].command) break;
				if( SpriteQue[a].priority == c)
				{
					if( SpriteQue[a].command == 10) tmode = transparent;
					else tmode = srcCopy;
					im = SpriteQue[a].imageNum;
					if( SpriteImages[im].imageType )
					{
						TempRect1 = SpriteImages[im].theRect;
						TempRect2 = SpriteQue[a].theRect;
						CopyBits( (BitMap *) (*SpriteImages[im].thePixMap),
							(BitMap *) (*SpriteImages[CurrentFrameID].thePixMap),
						 	&TempRect1, &TempRect2, tmode, nil);
					}
				}
			}
		}
	}
	return(0);
}

short SPR_SpriteHit( short imageNum, short min, short max )
{
	short a, b, result=-1;
	Rect	TempRect1, TempRect2, TempRect3;

	if( min>max ) { a=min; min=max; max = a;}  // Switch if user screwed up.	
	min--; max--;
	if( min <  0 ) min =0;
	if( max > 99 ) max =99;
	for(b=min; b<max; b++) if( !priorityStatus[b] ) return(0);

	if( imageNum >=2000) imageNum-=2000;	//Fix offset
	if( !SpriteImages[imageNum].imageType ) return(0);  // No image 
	TempRect1 = SpriteImages[imageNum].theRect;

	for(a=0; a<500; a++)
	{
	
		if( !SpriteQue[a].command ) return(result);
		b = SpriteQue[a].priority;
		if( b>=min && b<=max && b>result)
		{
			// Ok it falls it req. layer bandwidth, check for crossing
			
			TempRect1 = SpriteImages[imageNum].theRect;
			TempRect2 = SpriteQue[a].theRect;
			
			if( SectRect( &TempRect1, &TempRect2, &TempRect3 ) ) result = b;
		}
	}				 
	return(result);
}


short SPR_SpriteEnd()
{
	short a;
	
	CurrentFrameID = -1;
	for(a=0;a<500;a++)
	{
		SPR_SpriteRelease(a);
		SpriteQue[a].command=0;	
	}	
	return(0);
}

short SPR_SpriteDisplay( short frameNum)
{
	Rect	TempRect1;
	CGrafPtr	oldPort;
	GDHandle	oldDevice;
	
	
	if( frameNum >=2000) frameNum-=2000;
	if ( frameNum < 0 || frameNum > 499 ) { ErrHandler( 1041, "\p in SpriteDisplay command"); return(-1); }
	if( !SpriteImages[frameNum].imageType ) { ErrHandler( 1041, "\p in SpriteDisplay command"); return(-1); }  // No image to draw
	
	GetGWorld( &oldPort, &oldDevice);
	SetGWorld( (CWindowPtr) WinData[CurrentFrameWin].theWindow, GetMainDevice());
	
	// 0 Means draw immediately, do not put in que
	TempRect1 = SpriteImages[frameNum].theRect;
		
	CopyBits( (BitMap *) (*SpriteImages[frameNum].thePixMap),
			(BitMap *) &(WinData[CurrentFrameWin].theWindow->portBits),
		 	&TempRect1, &TempRect1, srcCopy, nil);
	SetGWorld( oldPort, oldDevice);

	return(0);
}

short SPR_SpriteClear(short imageNum)
{
	
	if( imageNum >=2000) imageNum-=2000;	//Fix offset
	if( !SpriteImages[imageNum].imageType ) { ErrHandler( 1041, "\p in SpriteClear command"); return(-1); } // No image to erase

	SetGWorld( SpriteImages[imageNum].theGWPtr, nil );	// Set where to erase
	EraseRect( &SpriteImages[imageNum].theRect );  // Clear Frame
	return(0);
}

short SPR_SpriteRelease(short imageNum)
{
	
	if( imageNum >=2000) imageNum-=2000;	//Fix offset
	if( !SpriteImages[imageNum].imageType ) return(0);  // Nothing to release

	DisposeGWorld( SpriteImages[imageNum].theGWPtr);
	SpriteImages[imageNum].imageType = 0;
	return(0);
}

// Functions

short SPR_SpriteWidth( short imageNum )
{
	if( imageNum >=2000) imageNum-=2000;	//Fix offset
	if( !SpriteImages[imageNum].imageType ) { ErrHandler( 1041, "\p in SpriteWidth function"); return(-1); } // No image to erase
	
	return( SpriteImages[imageNum].theRect.right ); 
}

short SPR_SpriteHeight( short imageNum )
{
	if( imageNum >=2000) imageNum-=2000;	//Fix offset
	if( !SpriteImages[imageNum].imageType ) { ErrHandler( 1041, "\p in SpriteHeight function"); return(-1); } // No image to erase

	return( SpriteImages[imageNum].theRect.bottom ); 
}
