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
#include        <Speech.h>
#include 		<Sound.h>
#define         UCHAR   unsigned char
#define		    kInFront 	(WindowPtr) -1	/* put the new window in front */

short FILE_CreateFileData( Str255 filename, long creator, long filetype, long script );
short FILE_OpenFileData( short perm, Str255 filename );
short FILE_CloseFile( short filenum );
short FILE_Read( short filenum, long ReadLen  );
short FILE_Write( short filenum );
short FILE_Exist( Str255 filename );
long FILE_LenOpen( short filenum );
short FILE_SetLoc(short filenum, long location);
long FILE_GetLoc(short filenum);
void FILE_Init();
long FILE_IfEOF(short filenum);
short FILE_Delete( Str255 filename );
short FILE_Rename( Str255 filename1, Str255 filename2 );
short FILE_Lock( Str255 filename );
short FILE_Unlock( Str255 filename );
short FILE_OpenDialog( Str255 filetypes );
short FILE_SaveDialog( Str255 filename, Str255 prompt);
short FILE_LoadPictFile( Str255 filename ); 
short FILE_SavePictFile( Str255 filename );
long  FILE_ReadBuffer( short filenum, short bufferID, long Start, long ReadLen  );
short FILE_WriteBuffer( short filenum, short bufferID, long Start, long WriteLen );
long  FILE_Size( Str255 filename );
short FILE_Copy( Str255 filename1, Str255 filename2 );
short FILE_LineInput( short filenum  );
short FILE_AppPath( );
short FILE_Dir( Str255 filename );
short FILE_CreateFolder( Str255 filename );


short CopyFile( FSSpec *fspec1,  FSSpec *fspec2 );

void RStrToStr255( UCHAR * st1, long slen, Str255 st2);
short FreeReturnStr();
short Allocate_ReturnStr( long);

void pstrcat( StringPtr dst, StringPtr src);
void pstrinsert( StringPtr dst, StringPtr src );
void PathNameFromDirID( long dirID, short vRefNum, StringPtr fullPathName);
void PathNameFromWD( long vRefNum, StringPtr pathName );
void ErrHandler( short ErrNum, Str255 AddMsg);

extern PicHandle	openPictureHandle;

struct filedatastruct
{
	FSSpec	fs;
	short	refnum;
} filedata[100];	

extern Handle  Buffers[250];
extern long	BufferSize[250];
extern UCHAR	*ReturnStr, ReturnStrFlag;
extern long		ReturnStrLen;
PicHandle		Picts[100];

void FILE_Init()
{
	short a;
	
	for(a=0; a<100; a++)
	{
		filedata[a].refnum = -1;
		Picts[a] = nil;
	}
}


short FILE_LoadPictFile( Str255 filename )  
{
	OSErr	err;
	short fRefNum, a, b=-1,c ;
	long 	filesize, pictlen;
	FSSpec	Ftemp;
	PicHandle	hPict;

	for( a=0; a<100; a++) if( Picts[a]==nil ) { b = a; break; }
	if( b==-1) return(-1);
	err = FSMakeFSSpec( 0, 0, filename, &Ftemp);
	if( err )
	{
		ErrHandler( 1036, nil);
		return(-1);
	}

	err = FSpOpenDF( &Ftemp, fsRdWrPerm, & fRefNum );
	if( err ) return(-1);
	GetEOF( fRefNum, &filesize);
	if( filesize < 512 ) { FSClose( fRefNum ); return(-1); }
	SetFPos( fRefNum, fsFromStart, 512 );
	pictlen = filesize - 512;
	hPict = (PicHandle) NewHandle( pictlen );
	HLock( (Handle) hPict );
	err = FSRead( fRefNum, &pictlen, *hPict );
	HUnlock( (Handle) hPict );
	FSClose( fRefNum );
	Picts[b] = hPict;
	c = (long) b;  c+=30000;
	return( c );
}

short FILE_SavePictFile( Str255 filename )  
{
	OSErr	err;
	FSSpec	Ftemp;
	short	i, fRefNum;
	long	zDL, pictlen, zD=0;
	PicHandle	hPict;
	

	if( openPictureHandle == nil ) return(-1);
	hPict = openPictureHandle;

	err = FSMakeFSSpec( 0, 0, filename, &Ftemp);
	if( !err )
	{
		// File Exists so delete it
		err = FSpDelete( &Ftemp ); 
	}

	err = FSpCreate( &Ftemp, 'TEST', 'PICT', smSystemScript );
	err = FSpOpenDF( &Ftemp, fsRdWrPerm, & fRefNum );
	if( err ) return(-1);
	
	zDL = sizeof( long );
	for( i=0;	i< 512 / zDL; i++)
	{
		err = FSWrite( fRefNum, &zDL, &zD); 
	}
	pictlen = GetHandleSize (( Handle ) hPict);
	HLock( ( Handle ) hPict);
	if(pictlen>0) err = FSWrite( fRefNum, &pictlen, *hPict); 
	HUnlock( ( Handle ) hPict);
	FSClose( fRefNum );
	return(err);
}

short FILE_CreateFileData( Str255 filename, long creator, long filetype, long script )
{
	OSErr	err;
	FSSpec	Ftemp;
	
	
	err = FSMakeFSSpec( 0, 0, filename, &Ftemp);
	if( !err )
	{
		// File Exists so delete it
		err = FSpDelete( &Ftemp ); 
	}
	err = FSpCreate( &Ftemp, creator, filetype, script );
	
	return(err);
}


short FILE_OpenFileData( short perm, Str255 filename )
{
	short	a, b=-1, rn;
	OSErr	err;
	
	for(a=0;a<100;a++)
	{
		if( filedata[a].refnum == -1 ) { b=a; break; }
	}
	if(b==-1) return(-1);
	
	err = FSMakeFSSpec( 0, 0, filename, &filedata[a].fs);
	if( err ) return(err);
	
	err = FSpOpenDF( &filedata[b].fs, perm, &rn );
	if( !err )  filedata[b].refnum = rn;
	return(b);
}

short FILE_CloseFile( short filenum )
{
	if( filedata[filenum].refnum!=-1)
	{
		 FSClose( filedata[filenum].refnum );
		 filedata[filenum].refnum = -1;
		return(0);
	}
	return(-1);
}

short FILE_LineInput( short filenum  )
{
	long	c=0, d, rsl, first;
	OSErr	err;
	
	if( filedata[filenum].refnum!=-1)
	{
		if( FILE_IfEOF(filenum) ) { ReturnStrLen = 0; return (0); }
		first = FILE_GetLoc( filenum );
		rsl = 300;
		err = FSRead( filedata[filenum].refnum, &rsl, ReturnStr );
		if( err == -39) err = 0;
		
		while( c<rsl && c<300 && ReturnStr[c]!=13 && ReturnStr[c]!=10) c++;
		if( ReturnStr[c]==13 || ReturnStr[c]==10)
		{
			d = c;
			if( ReturnStr[c]==13) c++;
			if( ReturnStr[c+1]==10) c++;
			FILE_SetLoc( filenum, first+c );
		}
		ReturnStrLen = d;
		ReturnStr[d]=0;
		return(0);
	} 
	return(-1);
}

short FILE_Read( short filenum, long ReadLen  )
{
	long	rsl;
	OSErr	err;
	
	if( filedata[filenum].refnum!=-1)
	{
		rsl = ReadLen;
		err = FSRead( filedata[filenum].refnum, &rsl, ReturnStr );
		if( err == -39) err = 0;
		if(!err) ReturnStrLen = rsl;
		return(err);
	} 
	return(-1);
}

short FILE_Write( short filenum )
{
	long	rsl=0 ;
	OSErr	err=0;
	
	if( filedata[filenum].refnum!=-1)
	{
		rsl = ReturnStrLen;
		err = FSWrite( filedata[filenum].refnum, &rsl, ReturnStr );
		if( !err && rsl != ReturnStrLen) err = -22;
		return(err);
	}
	return(-1);
}

long FILE_ReadBuffer( short filenum, short bufferID, long Start, long ReadLen  )
{
	long	rsl, a;
	OSErr	err;

	if( Buffers[bufferID] == nil ) { ErrHandler( 1002, "\pin ReadBuffer function"); return(-1); }

	if( BufferSize[bufferID] >= Start+ReadLen ) a = ReadLen;
	else
	{
		a = BufferSize[bufferID] - Start;
		if( a <= 0 ) return(0); 
	}
	if( filedata[filenum].refnum!=-1)
	{
		rsl = a;
    	HLock( Buffers[bufferID] );
		err = FSRead( filedata[filenum].refnum, &rsl, (*Buffers[bufferID]) + Start );
		HUnlock( Buffers[bufferID] );
		if( err == -39) err = 0;
		return(rsl);
	} 
	return(0);
}

short FILE_WriteBuffer( short filenum, short bufferID, long Start, long WriteLen )
{
	long	rsl=0, a ;
	OSErr	err=0;
	
	if( Buffers[bufferID] == nil ) { ErrHandler( 1002, "\pin WriteBuffer command"); return(-1); }

	if( BufferSize[bufferID] >= Start+WriteLen ) a = WriteLen;
	else
	{
		a = BufferSize[bufferID] - Start;
		if( a <= 0 ) return(0); 
	}
	
	if( filedata[filenum].refnum!=-1)
	{
		rsl = a;
    	HLock( Buffers[bufferID] );
		err = FSWrite( filedata[filenum].refnum, &rsl, (*Buffers[bufferID]) + Start  );
		HUnlock( Buffers[bufferID] );
		if( !err && rsl != ReturnStrLen) err = -22;
		return(err);
	}
	return(-1);
}

short FILE_AppPath( )
{
	Str255	pathName;
	short 	b, c, d=0;
	
	// Allocate ReturnStr Space
	b = Allocate_ReturnStr( 260 );
	if(b) return(-1); // Buffer allocation error

	PathNameFromDirID( 0, 0, pathName);

	d = pathName[0];

	for( c=0; c<d; c++) ReturnStr[c] = pathName[c+1];	
	ReturnStrLen = d;
	ReturnStr[ReturnStrLen] = 0;
	
	return(	0 );
}


short FILE_OpenDialog( Str255 filetypes )
{
	Str255	pathName;
	StandardFileReply	rep1;
	short 	numTypes=0, a, b, c, d=0;
	unsigned long	v1,v2,v3,v4,v5;
	SFTypeList	Tlist;
	
	// First convert file types
	if( filetypes[0]>=4 )
	{
		numTypes = filetypes[0] / 4 ; // Len
		if(numTypes>4) numTypes = 4;
		for( b=0; b<numTypes;b++)
		{
			a = (b*4)+1;
			v1 = filetypes[a];   v1 <<= 24;
			v2 = filetypes[a+1]; v2 <<= 16;
			v3 = filetypes[a+2]; v3 <<=  8;
			v4 = filetypes[a+3];
			v5 = v1 +v2 +v3 + v4;
			Tlist[b] = v5; //(SFTypeList) v1;
		}
	}
	
	StandardGetFile( nil, numTypes, Tlist, &rep1);
	// Now 
	if( rep1.sfGood)
	{
	
		// Allocate ReturnStr Space
		b = Allocate_ReturnStr( 260 );
		if(b) return(-1); // Buffer allocation error
	
		PathNameFromDirID( rep1.sfFile.parID, rep1.sfFile.vRefNum, pathName);
		pstrcat( pathName, rep1.sfFile.name);

		d = pathName[0];
	
		for( c=0; c<d; c++) ReturnStr[c] = pathName[c+1];	
		ReturnStrLen = d;
		ReturnStr[ReturnStrLen] = 0;
	}
	else ReturnStrLen = 0;
	
	return(	0 );
}

short FILE_SaveDialog( Str255 filename, Str255 prompt)
{
	Str255	pathName;
	StandardFileReply	rep1;
	short	b, c, d;
	
	StandardPutFile( prompt, filename, &rep1);
	// Now 
	if( rep1.sfGood)
	{
	
		// Allocate ReturnStr Space
		b = Allocate_ReturnStr( 260 );
		if(b) return(-1); // Buffer allocation error
	
		PathNameFromDirID( rep1.sfFile.parID, rep1.sfFile.vRefNum, pathName);
		pstrcat( pathName, rep1.sfFile.name);

		d = pathName[0];

	
		for( c=0; c<d; c++) ReturnStr[c] = pathName[c+1];	
		ReturnStrLen = d;
		ReturnStr[ReturnStrLen] = 0;
	}
	else ReturnStrLen = 0;
	
	return(	0 );
}

short FILE_Delete( Str255 filename )
{
	OSErr	err;
	FSSpec	Ftemp;
	
	err = FSMakeFSSpec( 0, 0, filename, &Ftemp);
	if( err ) return(err);
	FSpDelete( &Ftemp );
	return(	0 );
}

short FILE_CreateFolder( Str255 filename )
{
	OSErr	err;
	FSSpec	Ftemp;
	long	m1;
	
	err = DirCreate( 0, 0, filename, &m1);
	if( err ) return(err);
	return(	0 );
}

short FILE_Rename( Str255 filename1, Str255 filename2 )
{
	OSErr	err;
	FSSpec	Ftemp;
	
	err = FSMakeFSSpec( 0, 0, filename1, &Ftemp);
	if( err ) return(err);
	FSpRename( &Ftemp, filename2 );
	return(	0 );
}

short FILE_Copy( Str255 filename1, Str255 filename2 )
{
	OSErr	err;
	FSSpec	Ftemp1, Ftemp2;
	
	err = FSMakeFSSpec( 0, 0, filename1, &Ftemp1);
	if( err ) return(err);
	err = FSMakeFSSpec( 0, 0, filename2, &Ftemp2);
	if( !err ) 	FSpDelete( &Ftemp2 );

	CopyFile( &Ftemp1, &Ftemp2 );
	return(	0 );
}

short FILE_Lock( Str255 filename )
{
	OSErr	err;
	FSSpec	Ftemp;
	
	err = FSMakeFSSpec( 0, 0, filename, &Ftemp);
	if( err ) return(err);
	FSpSetFLock( &Ftemp );
	return(	0 );
}

short FILE_Unlock( Str255 filename )
{
	OSErr	err;
	FSSpec	Ftemp;
	
	err = FSMakeFSSpec( 0, 0, filename, &Ftemp);
	if( err ) return(err);
	FSpRstFLock( &Ftemp );
	return(	0 );
}


short FILE_Exist( Str255 filename )
{
	OSErr	err;
	FSSpec	Ftemp;
	
	err = FSMakeFSSpec( 0, 0, filename, &Ftemp);
	if( err ) return(0);
	return(	1 );
}

long FILE_Size( Str255 filename )
{
	OSErr	err;
	FSSpec	Ftemp;
	short	ref;
	long	sz=0, tsz;
	
	err = FSMakeFSSpec( 0, 0, filename, &Ftemp);
	if( err ) return(-1);

	err = FSOpen( filename, 0, &ref );
	if( !err ) { GetEOF( ref, &tsz ); sz = tsz; }
	FSClose( ref );
	
	err = OpenRF( filename, 0, &ref );
	if( !err ) { GetEOF( ref, &tsz ); sz += tsz; }
	FSClose( ref );
	
	return(	sz );
}


long FILE_LenOpen(short filenum)
{
	long ln;
	OSErr	err;
	
	if( filedata[filenum].refnum!=-1)
	{
		err = GetEOF( filedata[filenum].refnum, &ln);
		if( err < 0) ln = err;
		return( ln );
	}
	return(-1);
}

short FILE_SetLoc(short filenum, long location)
{
	long ln;
	OSErr	err;

	if( filedata[filenum].refnum!=-1)
	{
		if(location==-1)
		{
			ln = 0;
			err = SetFPos( filedata[filenum].refnum, 2, ln);	// Set at end of file
			if( err == -63) err = 0;
		}
		else
		{
			ln = location;
			err = SetFPos( filedata[filenum].refnum, 1, ln);	// Set at desig loc.
			if( err == -63) err = 0;
		}
		return( err );
	}
	return(-1);
}

long FILE_GetLoc(short filenum)
{
	long ln;
	OSErr	err;
	
	if( filedata[filenum].refnum!=-1)
	{
		err = GetFPos( filedata[filenum].refnum, &ln);
		if( err < 0) ln = err;
		return( ln );
	}
	return(-1);
}

long FILE_IfEOF(short filenum)
{
	long ln,loc;
	OSErr	err;
	
	if( filedata[filenum].refnum!=-1)
	{
		err = GetFPos( filedata[filenum].refnum, &loc);
		if( err < 0) {ln = err;	return( ln );}
		err = GetEOF( filedata[filenum].refnum, &ln);
		if( err < 0) {ln = err;	return( ln );}
		if( loc >= ln ) return(1);
		else return(0);
	}
	return(-1);
}


void pstrcat( StringPtr dst, StringPtr src)
{
	BlockMove( src+1, dst + *dst + 1, *src);
	*dst +=*src;
}

void pstrinsert( StringPtr dst, StringPtr src )
{
	BlockMove( dst+1, dst + *src +1, *dst );
	BlockMove( src +1, dst +1, *src );
	*dst += *src;
}


void PathNameFromDirID( long dirID, short vRefNum, StringPtr fullPathName)
{
	CInfoPBRec	pb;
	DirInfo		*block = (DirInfo*)&pb;
	Str255		directoryName;
	OSErr		err;
	short		a, b=0;
	
	fullPathName[0] = 0;
	directoryName[0] = 0;
	
	block->ioDrParID = dirID;
	block->ioNamePtr = directoryName;
	do{
		block->ioVRefNum = vRefNum;
		block->ioFDirIndex = -1;
		block->ioDrDirID = block->ioDrParID;
		err = PBGetCatInfoSync( &pb );
		
	
		pstrcat( directoryName, (StringPtr) "\p:");
		pstrinsert( fullPathName, directoryName );
	} while(block->ioDrDirID != 2);
	a = fullPathName[0];
	fullPathName[a+1] = 0;
	
}

void PathNameFromWD( long vRefNum, StringPtr pathName )
{
	WDPBRec	myBlock;
	OSErr	err;
	
	myBlock.ioNamePtr = nil;
	myBlock.ioVRefNum = vRefNum;
	myBlock.ioWDIndex = 0;
	myBlock.ioWDProcID = 0;
	
	err = PBGetWDInfoSync( &myBlock );
	if( err != noErr ) return;
	PathNameFromDirID( myBlock.ioWDDirID, myBlock.ioWDVRefNum, pathName);

}

short CopyFile2( short src, short dest );

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
		  if ( a ) { return(-1);}
		if( curPos >= srcSize )break;
		count = 512;
		err = FSRead( src, &count, theBuf );
		a = FSWrite( dest, &count, theBuf );
		if( err == eofErr ) break;
		else if(err) {return(-1);}
	}       
	SetEOF( dest, srcSize );
	return(0);
}

short CopyFile( FSSpec *fspec1,  FSSpec *fspec2 )
{
	short a, srcRef, destRef;
		
	FSpCreateResFile( fspec2, 'MSBA', 'APPL', smSystemScript  );
	
	// Copying Data Fork
	a = FSpOpenDF( fspec1, fsRdWrPerm, &srcRef );
	  if( a ) return(-3);
	a = FSpOpenDF( fspec2, fsRdWrPerm, &destRef );
	  if( a ) return(-4);
	CopyFile2( srcRef, destRef );
	a = FSClose( srcRef );
	  if( a ) return(-5);
	a = FSClose( destRef );
	  if( a ) return(-6);

	// Copying resource fork
	a = FSpOpenRF( fspec1, fsRdWrPerm, &srcRef );
	  if( a ) return(-7);
	a = FSpOpenRF( fspec2, fsRdWrPerm, &destRef );
	  if( a ) return(-8);
	
	CopyFile2( srcRef, destRef );

	a = FSClose( srcRef );
	  if( a ) return(-9);
	a = FSClose( destRef );
	  if( a ) return(-10);

	return(0);      
}

// Directory Search Code 4.0

Str255	tempFilename;		/* global buffer */

short FILE_Dir( Str255 filename )
{
	CInfoPBRec	cipbr;				/* local pb */
	HFileInfo	*fpb = (HFileInfo *)&cipbr;	/* to pointers */
	DirInfo	*dpb = (DirInfo *) &cipbr;
	short	rc, idx, err,b;
	long 	c=0,d=0;
	FSSpec	rt;
	
	// Allocate ReturnStr Space
	b = Allocate_ReturnStr( 32100 );  //up to 500 filenames of 64 bytes each
	if(b) return(-1); // Buffer allocation error

	err = FSMakeFSSpec(0,0, filename, &rt);
	if( err ) return(err);

	fpb->ioVRefNum = rt.vRefNum;		/* default volume */
	fpb->ioNamePtr = tempFilename;	/* buffer to receive name */

	for( idx=1; TRUE; idx++)
	{	/* indexing loop */
		fpb->ioDirID = rt.parID;		/* must set on each loop */
		fpb->ioFDirIndex = idx;

		rc = PBGetCatInfoSync( &cipbr );
		if (rc) break;	/* exit when no more entries */

		ReturnStr[d++] =  ':'; // this is separator
		if(fpb->ioFlAttrib & 16) ReturnStr[d++] = ':'; //add extra to indicate dir
		for( c=1; c<=tempFilename[0]; c++) ReturnStr[d++] = tempFilename[c];
		if( d>32010) break;
	}
	ReturnStrLen = d;
	ReturnStr[ReturnStrLen] = 0;
	return(	0 );

}
