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

#include <ctype.h>

void StrToStr255( char * st1, Str255 st2);
void Str255ToStr( Str255 st1, char * st2);
void Str63Copy ( Str255 st1, Str255 st2);
void Str255Copy ( Str255 st1, Str255 st2);

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

short Str255Cmp(  Str255 st1, Str255 st2)
{
	short a=1;
	
	if( st1[0] != st2[0] ) return 0;  // Quick len compare
	while( a <= st1[0] )
	{
		if( toupper(st1[a]) != toupper(st2[a]) ) return 0;
		//if( st1[a] != st2[a] ) return 0;
		a++;
	}
	return 1;
}