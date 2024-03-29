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

void ErrHandler( short ErrNum, Str255 AddMsg);
short Allocate_ReturnStr( long);

void  MemInit();
short MemAlloc( long sz);
void  MemFree( long bufferID );
short MemResize( long bufferID, long newsz );
void  MemFill( long bufferID, long start, long nob, short value );
short MemInt( long bufferID, long location, short value );
short MemLong( long bufferID, long location, long value );
short MemSingle( long bufferID, long location, short double value );
short MemDouble( long bufferID, long location, short double value );
void MemString( long bufferID, long location );
short GetMemInt( long bufferID, long location );
long  GetMemLong( long bufferID, long location );
short double GetMemSingle( long bufferID, long location );
short double GetMemDouble( long bufferID, long location );
short GetMemString( long bufferID, long location, long ln);
long MemSize( long bufferID );
long MemGetHandle( short mb );

Handle  Buffers[250];
long	BufferSize[250];
extern UCHAR	*ReturnStr, ReturnStrFlag, EventsFlag;
extern long		ReturnStrLen;

void MemInit()
{
	short a;
	for(a=0;a<250;a++) Buffers[a] = nil ;

}

short MemAlloc( long sz )
{
	short a,b=-1;
	for(a=0;a<250;a++) if( Buffers[a] == nil ) { b=a; break; }
	if( b==-1) { ErrHandler( 1001, "\p"); return(-1); }
	
	Buffers[b] = NewHandleClear(  sz );

	if( Buffers[b] == nil ) { ErrHandler( 1001, "\p"); return(-1); }
	BufferSize[b] = sz;
	return( b );
}

void MemFree( long bufferID )
{
	if( Buffers[bufferID] == nil ) return;
	DisposeHandle( Buffers[bufferID] );
	return;
}

long MemSize( long bufferID )
{
	if( Buffers[bufferID] == nil ) {  ErrHandler( 1002, "\p"); return(-1); }
	return( BufferSize[bufferID] );
}

short MemResize( long bufferID, long newsz )
{
	if( Buffers[bufferID] == nil ) { ErrHandler( 1002, "\p"); return(-1); }
	if( newsz == BufferSize[bufferID] ) return(0);
	HUnlock( Buffers[bufferID] );
	SetHandleSize( Buffers[bufferID], newsz );
	if( MemError() ){ ErrHandler( 1001, "\p"); return(-1); }
	BufferSize[bufferID] = newsz;
	return(0);
}

long MemGetHandle( short mb )
{
	long result;
	
	if( Buffers[mb] == nil ) return -1;
	
	result = (long) Buffers[mb];
	
	return result;
}

void MemFill( long bufferID, long start, long nob, short value )
{
	long a, b;
	unsigned char c, *ptr;
		
	if( Buffers[bufferID] == nil ) { ErrHandler( 1002, "\pin MemFill command"); return; }
	b = start+nob;
	if( BufferSize[bufferID] < b ) b = BufferSize[bufferID]-start;
	if( b<=0 ) return;
	c = (unsigned char) value;
	HLock( Buffers[bufferID] );
	ptr = (unsigned char *) (*Buffers[bufferID])+start;
	for( a=0; a<b; a++) ptr[a] = c; 
	HUnlock( Buffers[bufferID] );
	return;
}

short MemInt( long bufferID, long location, short value )
{
	char *tbuf;
	short	*ptr;

	if( Buffers[bufferID] == nil ) { ErrHandler( 1002, "\pin MemInt command"); return(-1); }
	if( location< 0  || location>= BufferSize[bufferID] ) { ErrHandler( 1003, "\pin MemInt command"); return(-1); }
	HLock( Buffers[bufferID] );
	tbuf = (* Buffers[bufferID]) + location;
	ptr = (short*) tbuf;
	*ptr = value;
	HUnlock( Buffers[bufferID] );
	return(0);
}

short MemLong( long bufferID, long location, long value )
{
	char *tbuf;
	long	*ptr;

	if( Buffers[bufferID] == nil ) { ErrHandler( 1002, "\pin MemLong command"); return(-1); }
	if( location< 0  || location>= BufferSize[bufferID] ) { ErrHandler( 1003, "\pin MemLong command"); return(-1); }
	HLock( Buffers[bufferID] );
	tbuf = (* Buffers[bufferID]) + location;
	ptr = (long*) tbuf;
	*ptr = value;
	HUnlock( Buffers[bufferID] );
	return(0);
}

short MemSingle( long bufferID, long location, short double value )
{
	char *tbuf;
	short double	*ptr;

	if( Buffers[bufferID] == nil ) { ErrHandler( 1002, "\pin MemSingle command"); return(-1); }
	if( location< 0  || location>= BufferSize[bufferID] ) { ErrHandler( 1003, "\pin MemSingle command"); return(-1); }
	HLock( Buffers[bufferID] );
	tbuf = (* Buffers[bufferID]) + location;
	ptr = (short double*) tbuf;
	*ptr = value;
	HUnlock( Buffers[bufferID] );
	return(0);
}

short MemDouble( long bufferID, long location, short double value )
{
	char *tbuf;
	short double	*ptr;

	if( Buffers[bufferID] == nil ) { ErrHandler( 1002, "\pin MemDouble command"); return(-1); }
	if( location< 0  || location>= BufferSize[bufferID] ) { ErrHandler( 1003, "\pin MemDouble command"); return(-1); }
	HLock( Buffers[bufferID] );
	tbuf = (* Buffers[bufferID]) + location;
	ptr = (short double*) tbuf;
	*ptr = value;
	HUnlock( Buffers[bufferID] );
	return(0);
}

void MemString( long bufferID, long location )
{
	long a, b;
	unsigned char *ptr;
		
	if( Buffers[bufferID] == nil ) { ErrHandler( 1002, "\pin MemString command"); return; }
	b = location+ReturnStrLen;
	if( BufferSize[bufferID] < b ) b = BufferSize[bufferID]-location;
	if( b<=0 ) return;
	HLock( Buffers[bufferID] );
	ptr = (unsigned char *) (*Buffers[bufferID])+location;
	for( a=0; a<b; a++) ptr[a] = ReturnStr[a]; 
	HUnlock( Buffers[bufferID] );
	return;
}


short GetMemInt( long bufferID, long location)
{
	char *tbuf;
	short	*ptr, value;

	if( Buffers[bufferID] == nil ) { ErrHandler( 1002, "\pin MemInt function"); return(-1); }
	if( location< 0  || location+1>= BufferSize[bufferID] ) { ErrHandler( 1003, "\pin MemInt function"); return(-1); }
	HLock( Buffers[bufferID] );
	tbuf = (* Buffers[bufferID]) + location;
	ptr = (short*) tbuf;
	value = *ptr;
	HUnlock( Buffers[bufferID] );
	return(value);
}

long GetMemLong( long bufferID, long location)
{
	char *tbuf;
	long	*ptr, value;

	if( Buffers[bufferID] == nil ) { ErrHandler( 1002, "\pin MemLong function"); return(-1); }
	if( location< 0  || location+3>= BufferSize[bufferID] ) { ErrHandler( 1003, "\pin MemLong function"); return(-1); }
	HLock( Buffers[bufferID] );
	tbuf = (* Buffers[bufferID]) + location;
	ptr = (long*) tbuf;
	value = *ptr;
	HUnlock( Buffers[bufferID] );
	return(value);
}

short double GetMemSingle( long bufferID, long location)
{
	char *tbuf;
	short double	*ptr, value;

	if( Buffers[bufferID] == nil ) { ErrHandler( 1002, "\pin MemSingle function"); return(-1); }
	if( location< 0  || location+3>= BufferSize[bufferID] ) { ErrHandler( 1003, "\pin MemSingle function"); return(-1); }
	HLock( Buffers[bufferID] );
	tbuf = (* Buffers[bufferID]) + location;
	ptr = (short double*) tbuf;
	value = *ptr;
	HUnlock( Buffers[bufferID] );
	return(value);
}

short double GetMemDouble( long bufferID, long location)
{
	char *tbuf;
	short double *ptr, value;

	if( Buffers[bufferID] == nil ) { ErrHandler( 1002, "\pin MemDouble function"); return(-1); }
	if( location< 0  || location+7>= BufferSize[bufferID] ) { ErrHandler( 1003, "\pin MemDouble function"); return(-1); }
	HLock( Buffers[bufferID] );
	tbuf = (* Buffers[bufferID]) + location;
	ptr = (short double *) tbuf;
	value = *ptr;
	HUnlock( Buffers[bufferID] );
	return(value);
}

short GetMemString( long bufferID, long location, long ln)
{
	long a, b;
	unsigned char *ptr;
		
	if( Buffers[bufferID] == nil ) { ErrHandler( 1002, "\pin MemString function"); return(-1); }
	b = ln;
	if( BufferSize[bufferID] < location+ln ) b = BufferSize[bufferID]-location;
	if( b<=0 ) { ErrHandler( 1003, "\pin MemDouble function"); return(-1); }
	
	// Allocate ReturnStr Space
	a = Allocate_ReturnStr( b+16 );
	if(a) return(a); // Buffer allocation error
	
	HLock( Buffers[bufferID] );
	ptr = (unsigned char *) (*Buffers[bufferID])+location;
	for( a=0; a<b; a++) ReturnStr[a] = ptr[a]; 
	ReturnStrLen = b;
	HUnlock( Buffers[bufferID] );
	return(0);

}
