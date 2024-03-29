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


// FUNCS1.C
// Contains small general use functions 

#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <math.h>
#include 		<ctype.h>
#define			UCHAR	unsigned char

void StrToStr255( char * st1, Str255 st2);
void Str255ToStr( Str255 st1, char * st2);
void Str63Copy ( Str255 st1, Str255 st2);
void Str255Copy ( Str255 st1, Str255 st2);
void Str255Cat( Str255 st1, Str255 st2);
short StrPartCmp( char * st1, char * st2);

short Str255Cpy(	UCHAR *st1, UCHAR *st2);
short isvalidvar(short ch);
short isnumber(short ch);
short OneOfThem( short ch, char *src );

short Str255Cpy(	UCHAR *st1, UCHAR *st2)
{
	// Copy st2 to  st1 string, both are Str255, returns len

	short a;
	for(a=0; a <= st2[0]; a++)	st1[a] = st2[a]; 
	return( st2[0] );
}  

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

short OneOfThem( short ch, char *src )
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

void Str255ToStr( Str255 st1, char * st2)
{
	short a, b=1, c=0;

	a = st1[0];
	while( b <= a ) st2[c++] = st1[b++];
	st2[c++] = 0;
}

void Str255Copy( Str255 st1, Str255 st2)
{
	short a=0;
	while( a<=st1[0] ) {st2[a] = st1[a]; a++;}
}

void Str255Cat( Str255 st1, Str255 st2)
{
	short a=1, b,c;
	
	b = st2[0]+1;
	while( a<=st1[0] ) st2[b++] = st1[a++];
	st2[0] = b-1;
}

void Str63Copy( Str255 st1, Str255 st2)
{
	short a=0;
	
	while( a<=st1[0] && a<=63 ) { st2[a] = st1[a]; a++;}
}

short StrPartCmp( char * st1, char * st2)
{
	short a=0;
	
	//while( st1[a] && st2[a] )
	while( st2[a] )
	{
		if( toupper(st1[a]) != toupper(st2[a]) ) return 0;
		a++;
	}
	return 1;
}
