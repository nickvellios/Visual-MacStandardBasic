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


// Contains small general use functions 

#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <math.h>
#include        <QuickDraw.h>
#define			UCHAR	unsigned char

short isvalidvar(short ch);
short isnumber(short ch);
short OneOfThem( short ch, unsigned char *src );
void StrToStr255( char * st1, Str255 st2);
long RandomRange( long v1, long v2 );
void RStrToStr255( UCHAR * st1, long slen, Str255 st2);
void Str255ToStr( Str255 st1, char * st2);
void Str63Copy( Str255 st1, Str255 st2);
void RStrToStr( UCHAR * st1, long slen, unsigned char* st2, long max);


short isnumber(short ch)
{
	if(ch>='0' && ch<='9') return(1);
	if(ch=='.') return(1);
	return(0);
}

short isvalidvar(short ch)
{
		
	if( ch>='A' && ch<='Z' ) return(1);
	if( ch>='0' && ch<='9' ) return(1);
	if( ch == '_' ) return(1);

	return(0);	
}

short OneOfThem( short ch, unsigned char *src )
{
	short a=0;
	while( src[a] )
	{
		if(ch == src[a]) return(a+1);
		a++;
	}
	return(0);
}

void StrToStr255( char * st1, Str255 st2)
{
	short a=0, b=1;
	
	while( st1[a] && a<255 ) st2[b++] = st1[a++];
	st2[0] = a;
}

void RStrToStr255( UCHAR * st1, long slen, Str255 st2)
{
	short a=0, b=1;
	long v1;
	
	v1 = slen;
	
	while( v1 && a<255 ) { st2[b++] = st1[a++]; v1--;}
	st2[0] = a;
}

void RStrToStr( UCHAR * st1, long slen, unsigned char* st2, long max)
{
	long a=0;	

	while( a<=slen && a<max ) { st2[a] = st1[a]; a++;}
}


long RandomRange( long v1, long v2 )
{
	unsigned short	rdm;
	long	range, t, m1, m2;
	
	//qd.randSeed = TickCount();
	//GetDateTime( (unsigned long*) (&qd.randSeed) );
	rdm = Random();
	if( v2 > v1 ) { m1 = v1; m2 = v2;}
	else  { m1 = v2; m2 = v1;}
	range = m2 - m1;
	t = (rdm * range) / 65536;
	return( t + m1 );
}

void Str255ToStr( Str255 st1, char * st2)
{
	short a, b=1, c=0;

	a = st1[0];
	while( b <= a ) st2[c++] = st1[b++];
	st2[c] = 0;
}

void Str63Copy( Str255 st1, Str255 st2)
{
	short a=0;
	
	while( a<=st1[0] && a<=63 ) { st2[a] = st1[a]; a++;}
}

