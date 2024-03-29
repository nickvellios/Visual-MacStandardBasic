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

void COD( UCHAR *msg );

void ReleaseTables();

short create_program_file(  UCHAR * fname);
short create_app();
short copy_tables_to_resource();
short ReserveTempTableBuffers();
short create_temp_file();
short WriteHeaderResource();
short WriteTableResource();
short WriteCodeResource();
short ReadCodeResource();
long calc_tables_size();
short ReadCodeResources();
//short CopyFile( short svol, UCHAR* srcfile, short dvol, UCHAR* desfile );
short CopyFile( );
short CopyFile2( short src, short dest );
short next_res_file();
short CopyFileResources();
void  Str255Copy ( Str255 st1, Str255 st2);
void Str255Cat( Str255 st1, Str255 st2);

#include        "Structs1.h"
 
extern struct RunHdrStruct // This Data Structure holds runtime info 
{       
	long    NumOfVars;
	long    NumOfCalls;
	long    NumOfLabels;
	long    StrLen;
	long    CodeLen;
	long    CodeStart;              // Where to start
	long    GlbVarSz;
	long    StartVarSz;
	long    CallTableOffset;        // Don't need VarTable Offset because it = 0
	long    LabelTableOffset;
	long    StrTableOffset;
	long	SelectTableOffset;
	long	NumOfSelects;
	long	NumOfProps;   		// New for 3.0
	long	PropTableOffset;    // New for 3.0

} RunHdr;       //ext to pass1.c & pass2.c


FSSpec  pgm_fp, temp_fp, rtlSpec;
long    ctr_offset;
Handle  dataHandle;
extern unsigned char temp[100];
extern short	MinMem, PrefMem;

extern FSSpec	CplFS, IDESpec, PjtSpec;
extern long StrConCtr;
extern Str255 AppRunName, Target;

short CopyFile2( short src, short dest )
{
	long    curPos, srcSize, count;
	short   err, a;
	char    theBuf[512];

	a = GetEOF( src, &srcSize );
	if( !srcSize ) return(0);

	while(1)
	{
		a = GetFPos( src, &curPos );
		  if ( a ) { printf("Internal copy error\n"); return(-1);}
		if( curPos >= srcSize )break;
		count = 512;
		err = FSRead( src, &count, theBuf );
		a = FSWrite( dest, &count, theBuf );
		if( err == eofErr ) break;
		else if(err) { printf("Internal copy error\n"); return(-1);}
	}       
	SetEOF( dest, srcSize );
	return(0);
}

//short CopyFile( short svol, UCHAR* srcfile, short dvol, UCHAR* desfile )
short CopyFile(  )
{
	short a, srcRef, destRef;
	FInfo   fi;
	OSErr	error;
	
	//a = GetFInfo( srcfile, svol, &fi );        
	//  if( a ) return(-1);
	FSpDelete( &pgm_fp);
	//a = Create( desfile, dvol, fi.fdCreator, fi.fdType );
	// if( a ) return(-2);
	//CreateResFile( desfile );

	//error = FSMakeFSSpec( PjtSpec.vRefNum,  PjtSpec.parID, "\pAppRT", &pgm_fp);

	//FSpCreate( &pgm_fp, 'MSBA', 'APPL', smSystemScript  );
	FSpCreateResFile( &pgm_fp, 'MSBA', 'APPL', smSystemScript  );
	
	// Copying Data Fork
	a = FSpOpenDF( &rtlSpec, fsRdWrPerm, &srcRef );
	  if( a ) return(-3);
	a = FSpOpenDF( &pgm_fp, fsRdWrPerm, &destRef );
	  if( a ) return(-4);
	CopyFile2( srcRef, destRef );
	a = FSClose( srcRef );
	  if( a ) return(-5);
	a = FSClose( destRef );
	  if( a ) return(-6);

	// Copying resource fork
	a = FSpOpenRF( &rtlSpec, fsRdWrPerm, &srcRef );
	//a = OpenRF( srcfile, svol, &srcRef );
	  if( a ) return(-7);
	a = FSpOpenRF( &pgm_fp, fsRdWrPerm, &destRef );
	//a = OpenRF( desfile, dvol, &destRef );
	  if( a ) return(-8);
	CopyFile2( srcRef, destRef );
	a = FSClose( srcRef );
	  if( a ) return(-9);
	a = FSClose( destRef );
	  if( a ) return(-10);

	return(0);      
}


short copy_tables_to_resource()
{
	short   rc, a;
	UCHAR   *bptr; //, ty[]="Howdy";
	long    ctr1;

	ctr_offset=0;
	
	// Write out Var Table Pages
	for(a=0; a<GlobalData.Num_VTP ;a++)
	{
		bptr = (UCHAR *) GlobalData.VarTable2Page[a];
		ctr1 = sizeof( VarTable2 ) * 100;
		if( a == GlobalData.Num_VTP-1 ) ctr1 = sizeof( VarTable2 ) * (GlobalData.NumOfVars % 100);
		BlockMoveData( (UCHAR*) GlobalData.VarTable2Page[a],(UCHAR*) (*dataHandle)+ctr_offset, ctr1 );
		ctr_offset += ctr1;
	}

	// Set offset in Header
	RunHdr.CallTableOffset = ctr_offset;

	// Write out Call Table Pages
	for(a=0; a<GlobalData.Num_CTP ;a++)
	{
		bptr = (UCHAR *) GlobalData.CallTable2Page[a];
		ctr1 = sizeof( CallTable2 ) * 100;
		if( a == GlobalData.Num_CTP-1 ) ctr1 = sizeof( CallTable2 ) * (GlobalData.NumOfCalls % 100);
		BlockMoveData( (UCHAR*) GlobalData.CallTable2Page[a],(UCHAR*) (*dataHandle)+ctr_offset, ctr1 );
		ctr_offset += ctr1;
	}

	// Set offset in Header
	RunHdr.LabelTableOffset = ctr_offset;

	// Write out Label Table Pages
	for(a=0; a<GlobalData.Num_LTP ;a++)
	{
		bptr = (UCHAR *) GlobalData.LabelLocPage[a];
		ctr1 = sizeof( long ) * 100;
		if( a == GlobalData.Num_LTP-1 ) ctr1 = sizeof( long ) * (GlobalData.NumOfLabels % 100);
		BlockMoveData( (UCHAR*) GlobalData.LabelLocPage[a],(UCHAR*) (*dataHandle)+ctr_offset, ctr1 );
		ctr_offset += ctr1;
	}

	// Set offset in Header
	RunHdr.StrTableOffset = ctr_offset;

	// Write out String Constant Pages
	for(a=0; a<GlobalData.Num_STP ;a++)
	{
		bptr = GlobalData.StringTablePage[a];
		ctr1 = 16384;
		if( a == GlobalData.Num_STP-1 ) ctr1 = StrConCtr % 16384;
		BlockMoveData( (UCHAR*)bptr,(UCHAR*) (*dataHandle)+ctr_offset, ctr1 );
		ctr_offset += ctr1;
	}

	// Set offset in Header
	RunHdr.SelectTableOffset = ctr_offset;

	// Write out Select Table Pages
	for(a=0; a<GlobalData.Num_SCTP ;a++)
	{
		bptr = (UCHAR *) GlobalData.SelectTablePage[a];
		ctr1 = sizeof( SelectTable ) * 100;
		if( a == GlobalData.Num_SCTP-1 ) ctr1 = sizeof( SelectTable ) * (GlobalData.NumOfSelects % 100);
		BlockMoveData( (UCHAR*) GlobalData.SelectTablePage[a],(UCHAR*) (*dataHandle)+ctr_offset, ctr1 );
		ctr_offset += ctr1;
	}

	// Set offset in Header
	RunHdr.PropTableOffset = ctr_offset;

	// Write out Select Table Pages
	for(a=0; a<GlobalData.Num_PTP ;a++)
	{
		bptr = (UCHAR *) GlobalData.PropTable2Page[a];
		ctr1 = sizeof( PropTable2 ) * 100;
		if( a == GlobalData.Num_PTP-1 ) ctr1 = sizeof( PropTable2 ) * (GlobalData.NumOfProps % 100);
		BlockMoveData( (UCHAR*) GlobalData.PropTable2Page[a],(UCHAR*) (*dataHandle)+ctr_offset, ctr1 );
		ctr_offset += ctr1;
	}

	return(0);
}

long calc_tables_size()
{
	long    ctr1;

	ctr1  = (long) sizeof( VarTable2 ) * GlobalData.NumOfVars;
	//printf("VT2 = %li %li\n",(long) sizeof( VarTable2 ),  GlobalData.NumOfVars);
	ctr1 += sizeof( CallTable2 ) * GlobalData.NumOfCalls;
	//printf("VT3 = %li\n", GlobalData.NumOfCalls);
	ctr1 += sizeof( long ) * GlobalData.NumOfLabels;
	ctr1 += StrConCtr;
	ctr1 += sizeof( SelectTable ) * GlobalData.NumOfSelects;
	ctr1 += sizeof( PropTable2 ) * GlobalData.NumOfProps;

	return(ctr1);
}

short WriteHeaderResource()
{
	short             fRef;
	OSErr   error;
		
	// Open Resource File
	//error = FSMakeFSSpec(0,0, "\pAppRT", &pgm_fp);
	fRef = FSpOpenResFile( &pgm_fp, fsRdWrPerm);
	UseResFile( fRef );

	dataHandle = NewHandleClear( sizeof( RunHdr ) );
	if( dataHandle == nil )
	{
		compile_log_update(156, nil);
		//printf( "Bad Handle\n");
		return(-1);
	}
	
	HLock( dataHandle );
	//printf("Doing hdr blockmove of %li\n", sizeof( RunHdr ) );
	BlockMoveData( (UCHAR*) &RunHdr, (UCHAR*) *dataHandle, sizeof( RunHdr ) );
	//printf("Blockmove done\n");
	AddResource ( dataHandle, 'MSBH', 5503, "\pheader" );
	WriteResource( dataHandle );
	HUnlock( dataHandle );

	CloseResFile( fRef );
	DisposeHandle( dataHandle );
	return(0);
}


short WriteCodeResource()
{
	long    numBytes, a;
	short             fRef;
	OSErr   error;
	long ctr1;
		
	// Open Resource File
	//error = FSMakeFSSpec(0,0, "\pAppRT", &pgm_fp);
	fRef = FSpOpenResFile( &pgm_fp, fsRdWrPerm);
	UseResFile( fRef );

	dataHandle = NewHandleClear( RunHdr.CodeLen + 16);      //+16 is padding
	if( dataHandle == nil )
	{
		//printf( "Bad Handle\n");
		compile_log_update(156, nil);
		return(-1);
	}
	
	HLock( dataHandle );
	numBytes = RunHdr.CodeLen;      
	//printf("Doing code blockmove of %li\n", numBytes );
	ctr_offset = 0;
	for(a=0; a<GlobalData.Num_CDTP ;a++)
	{
		//bptr = GlobalData.CodeTablePage[a];
		ctr1 =16384;
		if( a == GlobalData.Num_CDTP-1 ) ctr1 = GlobalData.CodeCtr % 16384;
		BlockMoveData( (UCHAR*) GlobalData.CodeTablePage[a],(UCHAR*) (*dataHandle)+ctr_offset, ctr1 );
		ctr_offset+=ctr1;  
	}
	//printf("Blockmove done\n");
	//printf("Planned > %li : Actual > %li\n", numBytes, ctr_offset); 

	AddResource ( dataHandle, 'MSBC', 5501, "\ppcode" );
	WriteResource( dataHandle );
	HUnlock( dataHandle );

	CloseResFile( fRef );
	DisposeHandle( dataHandle );
	return(0);
}

short WriteTableResource()
{
	Handle  dataHandle;
	long    numBytes, a;
	short             fRef;
	OSErr   error;
		
	//printf("WriteCodeResources()\n" );
	// Open Resource File
	//error = FSMakeFSSpec(0,0, "\pAppRT", &pgm_fp);
	fRef = FSpOpenResFile( &pgm_fp, fsRdWrPerm);
	UseResFile( fRef );

	numBytes = calc_tables_size() + 16;     //+16 is padding
	//printf(">>%li\n", numBytes );
	dataHandle = NewHandleClear( numBytes);
	if( dataHandle == nil )
	{
		//printf( "Bad Handle\n");
		compile_log_update(156, nil);
		return(-1);
	}
	
	HLock( dataHandle );
	//numBytes = RunHdr.CodeLen;      
	//printf("Doing table blockmove of %li\n", numBytes );
      //          BlockMoveData( (UCHAR*) GlobalData.CodeTablePage[0],(UCHAR*) *dataHandle, numBytes );
	copy_tables_to_resource();
	//printf("Blockmove done\n");
	AddResource ( dataHandle, 'MSBT', 5502, "\ptables" );
	WriteResource( dataHandle );
	HUnlock( dataHandle );

	CloseResFile( fRef );
	DisposeHandle( dataHandle );
	return(0);
}

short CopyFileResources()
{
	short 	rCount, rID, j1, j2, ResTypes, fRef1, fRef2, IDnum;
	Str255 	fn, IDname;
	FSSpec	fsp;
	OSErr	error;
	ResType	IDrt;
	
	unsigned long stp, tp[]={ 'PICT', 'snd ', 'icl4', 'icl8','ICN#','ICON','ics4', 'ics8','STR ', 'clut', 'ALRT', 'DITL', 'vmcr' };
	ResTypes = 13;
	
	//printf("Opening AppRT\n");
	fRef1 = FSpOpenResFile( &pgm_fp, fsRdWrPerm);
	
	//printf("Trying [%s]\n", temp+1 );
	error = FSMakeFSSpec( PjtSpec.vRefNum, PjtSpec.parID, temp, &fsp);
	if( error )
	{
		//printf("error %i\n", error);
		compile_log_update(157, (char*) temp);
		 return(-1);
	}
	//printf("Opening Rsc File\n");
	fRef2 = FSpOpenResFile( &fsp, fsRdWrPerm);

	for(j1=0; j1 < ResTypes; j1++)
	{
		UseResFile( fRef2 );
		stp = tp[j1]; // Get Res Code
		rCount = Count1Resources( stp );
		//printf("#%i of resources\n", rCount ); 
		for(j2=1; j2 <= rCount; j2++ )
		{
			UseResFile( fRef2 );
			dataHandle = Get1IndResource( stp, j2 );
			HLock( dataHandle );
			GetResInfo( dataHandle, &IDnum, &IDrt, IDname ); 
			DetachResource ( dataHandle );
			
			UseResFile( fRef1 );
			AddResource ( dataHandle, IDrt, IDnum, IDname);
			WriteResource ( dataHandle );
			HUnlock( dataHandle );
			//printf("Writing Resource\n");
			ReleaseResource ( dataHandle );
			
		}
	}
	CloseResFile( fRef1 );
	CloseResFile( fRef2 );

	return(0);	
}

short ReadCodeResources()
{
	Handle  codeHandle, hdrHandle, tablesHandle;
	long    numBytes;
	short fRef;
	
	// Open Resource File
	fRef = FSpOpenResFile( &pgm_fp, fsRdWrPerm);
	UseResFile( fRef );

	// Load Code    
	codeHandle = Get1IndResource( 'MSBC', 1 );

	// Load Code    
	tablesHandle = Get1IndResource( 'MSBT', 1 );

	// Load Header
	hdrHandle = Get1IndResource( 'MSBH', 1 );

	HLock( codeHandle );
	HLock( tablesHandle );

	// Copy contents over then release resource
	HLock( hdrHandle );
	
	CloseResFile( fRef );
	return(0);

}

extern long pjt_pos;
extern char LibraryMode;

short create_app()
{
	short result, error, a, b=0, fRef2;
	FSSpec	fsp;
	UCHAR	*cp;
	long	*lp,lv;
	Str255	libdef;
	
	// Fill some of runvar table
	RunHdr.CodeLen = GlobalData.CodeCtr;
	RunHdr.NumOfVars = GlobalData.NumOfVars;
	RunHdr.NumOfCalls = GlobalData.NumOfCalls;
	RunHdr.NumOfLabels = GlobalData.NumOfLabels;
	RunHdr.StrLen = StrConCtr;
	RunHdr.NumOfSelects = GlobalData.NumOfSelects;
	RunHdr.NumOfProps = GlobalData.NumOfProps;
	// RunHdr.CodeStart assigned in pass2.c  p2_compile_module
	// RunHdr.GlbVarSz assigned in pass1.c
	// RunHdr.StartVarSz assigned in pass1.c
	
	//printf("NumOfVars = %li\n", GlobalData.NumOfVars );

	//result = create_program_file( "\pBobby");
	//result = CopyFile( IDESpec.vRefNum, "\pBasic Runtime", CplFS.vRefNum, "\pAppRT"); 
	//printf("CopyFile\n");
	//result = CopyFile( CplFS.parID, "\pBasic Runtime", PjtSpec.parID, "\pAppRT"); 
	
	//error = FSMakeFSSpec( CplFS.vRefNum, CplFS.parID, "\pBasic Runtime", &rtlSpec);
	/* 3.1 & < ver switch( LibraryMode )
	{
		case 1:
			error = FSMakeFSSpec(CplFS.vRefNum, CplFS.parID,   "\p:Libraries:MSB 68K Library", &rtlSpec);
			break;
		case 2:
			error = FSMakeFSSpec( CplFS.vRefNum, CplFS.parID,  "\p:Libraries:MSB PPC Library", &rtlSpec);
			break;
		default:
			error = FSMakeFSSpec(  CplFS.vRefNum, CplFS.parID, "\p:Libraries:MSB FAT Library", &rtlSpec);
			break;
	}*/
	
	//printf("Stage 1\n");
	// New 4.0 Method
	Str255Copy( "\p:Libraries:", libdef );
	Str255Cat( Target, libdef ); 
	error = FSMakeFSSpec(CplFS.vRefNum, CplFS.parID, libdef, &rtlSpec);


	COD( "\pWriting Application File..." );
	error = FSMakeFSSpec( PjtSpec.vRefNum,  PjtSpec.parID, AppRunName, &pgm_fp);
	result = CopyFile(); 
	if( result )
	{
		// printf("Error = %i\n", result);
		 //getchar();
		 compile_log_update(158, nil);
		 return(-1);
	}
	
	// Write code & tables to resources of file             
	WriteCodeResource();
	//printf("Stage 3\n");
	WriteTableResource();
	//printf("Stage 4\n");
	WriteHeaderResource();
	//printf("Stage 5\n");

	COD( "\pCopying Resources" );
	// Copy all relavent resource types from any resource files in the project
	pjt_pos = 0;  // Reset to 0
	b=0;
	while(!b)
	{
		a = next_res_file();
		if(a)
		{
			//printf("Res File Fnd\n");
			// Copy all necessary resources from this file
			CopyFileResources( );
			
		}
		else b++;
	}
	
	// Now update size resource in final app
	//error = FSMakeFSSpec( PjtSpec.vRefNum, PjtSpec.parID, temp, &fsp);
	//if( error ) { compile_log_update(157, (char*) temp); return(-1); }
	//error = FSMakeFSSpec( PjtSpec.vRefNum, PjtSpec.parID, AppRunName, &fsp);
	//if( error ) { compile_log_update(157, (char*) AppRunName); return(-1); }

	fRef2 = FSpOpenResFile( &pgm_fp, fsRdWrPerm);
	//printf("Stage 6 %i\n", fRef2);
	COD( "\pSetting Memory Allocations" );

	UseResFile( fRef2 );
	b = 1;
	dataHandle = Get1IndResource( 'SIZE', b );
	HLock( dataHandle );
	cp = (UCHAR*) *dataHandle;
	lp = (long *) (cp+2);
	lv = (long) PrefMem; lv*=1024;
	*lp = lv;
	lp++;
	lv = (long) MinMem; lv*=1024;
	*lp = lv;
	ChangedResource ( dataHandle );
	WriteResource ( dataHandle );
	HUnlock( dataHandle );
	CloseResFile( fRef2 );
	//printf("Stage 7\n");
	COD( "\pAll Operation Complete" );
	//getchar();


	return(0);
}

