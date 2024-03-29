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
#define         UCHAR   unsigned char
#include		"d4all.h"

void DB_Init();
short DB_Open( Str63 fname );
void DB_Close( short db );
void DB_Goto( short db, long recNo );
void DB_Bottom( short db );
void DB_AppendBlank( short db );
void DB_Top( short db );
void DB_Delete( short db );
void DB_Undelete( short db );
void DB_Skip( short db, long NoRecs );

long DB_Deleted( short db );
long DB_BOF( short db );
long DB_EOF( short db );
long DB_RecNo( short db );

void Str255ToStr( Str255 st1, char * st2);
void Str63Copy( Str255 st1, Str255 st2);

CODE4	codeBase;

struct dbStruct
{
	short 	status;
	DATA4	*datafile;
	Str63	filename;
} dbinfo[20];

void DB_Init()
{
	short a;
	code4init( &codeBase );
	for(a=0;a<20;a++) 
	{
		dbinfo[a].status=0;
		dbinfo[a].datafile=nil;
		dbinfo[a].filename[0]=0;
	}
}

short DB_Open( Str63 fname )
{
	short a, b=-1;
	char	fname2[70];
	
	for(a=0;a<20;a++)  if( !dbinfo[a].status ) { b=a; break; }
	if( b==-1 ) return -1;
	
	Str255ToStr( fname, fname2);
	
	dbinfo[b].datafile= d4open( &codeBase, fname2 );
	if( dbinfo[b].datafile == nil ) return -1;
	
	dbinfo[b].status=1;
	Str63Copy( fname, dbinfo[b].filename );
	
	return b;
}

void DB_Close( short db )
{
		if( db<0 || db>19 ) return;
		if( !dbinfo[db].status || dbinfo[db].datafile==nil ) return;
		
		d4close( dbinfo[db].datafile );
		dbinfo[db].status=0;
		dbinfo[db].datafile=nil;
		dbinfo[db].filename[0]=0;
}

void DB_Goto( short db, long recNo )
{
		if( db<0 || db>19 ) return;
		if( !dbinfo[db].status || dbinfo[db].datafile==nil ) return;
		
		d4go( dbinfo[db].datafile, recNo );
}

void DB_Skip( short db, long NoRecs )
{
		if( db<0 || db>19 ) return;
		if( !dbinfo[db].status || dbinfo[db].datafile==nil ) return;
		
		d4skip( dbinfo[db].datafile, NoRecs );
}

void DB_AppendBlank( short db )
{
		if( db<0 || db>19 ) return;
		if( !dbinfo[db].status || dbinfo[db].datafile==nil ) return;
		
		d4appendBlank( dbinfo[db].datafile );
}

void DB_Bottom( short db )
{
		if( db<0 || db>19 ) return;
		if( !dbinfo[db].status || dbinfo[db].datafile==nil ) return;
		
		d4bottom( dbinfo[db].datafile );
}

void DB_Top( short db )
{
		if( db<0 || db>19 ) return;
		if( !dbinfo[db].status || dbinfo[db].datafile==nil ) return;
		
		d4top( dbinfo[db].datafile );
}

void DB_Delete( short db )
{
		if( db<0 || db>19 ) return;
		if( !dbinfo[db].status || dbinfo[db].datafile==nil ) return;
		
		d4delete( dbinfo[db].datafile );
}

void DB_Undelete( short db )
{
		if( db<0 || db>19 ) return;
		if( !dbinfo[db].status || dbinfo[db].datafile==nil ) return;
		
		d4recall( dbinfo[db].datafile );
}

long DB_Deleted( short db )
{
		short b;
		
		if( db<0 || db>19 ) return;
		if( !dbinfo[db].status || dbinfo[db].datafile==nil ) return;
		
		b = d4deleted( dbinfo[db].datafile );
		if( b ) return 1;
		return 0;
}

long DB_BOF( short db )
{
		short b;
		
		if( db<0 || db>19 ) return;
		if( !dbinfo[db].status || dbinfo[db].datafile==nil ) return;
		
		b = d4bof( dbinfo[db].datafile );
		if( b>1 ) return 1;
		return 0;
}

long DB_EOF( short db )
{
		short b;
		
		if( db<0 || db>19 ) return;
		if( !dbinfo[db].status || dbinfo[db].datafile==nil ) return;
		
		b = d4eof( dbinfo[db].datafile );
		if( b>1 ) return 1;
		return 0;
}

long DB_RecNo( short db )
{
		long b;
		
		if( db<0 || db>19 ) return;
		if( !dbinfo[db].status || dbinfo[db].datafile==nil ) return;
		
		b = d4recNo( dbinfo[db].datafile );
		return b;
}
