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


#include 		<Resources.h>
#include 		<MixedMode.h> 
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <math.h>
#include        <Palettes.h>
#define         UCHAR   unsigned char

//Types

typedef		long	(*MainProcPtr)( long, long, long, long);
//kThinkCStackBased
enum {
	uppMainProcInfo = kThinkCStackBased
		 | RESULT_SIZE(SIZE_CODE(sizeof(long)))
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(long)))
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(long)))
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(long)))
		 | STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(long)))
};

	//Using these macros lets us use the same calls for 68K and PPC
#if powerc
typedef UniversalProcPtr MainProcUPP;

#define CallMainProc(userRoutine, long, x, y, d)		\
		CallUniversalProc((UniversalProcPtr)(userRoutine), uppMainProcInfo, (long, x, y, d) )
#else
typedef MainProcPtr MainProcUPP;

//#define CallMainProc(userRoutine, long, long, long, long)		\
//		 (*(userRoutine))(long, long, long, long)
#endif


int CRES_CodeRes( short ResID, long v1, long v2, long v3, long v4 );



int CRES_CodeRes( short ResID, long v1, long v2, long v3, long v4 )  
{
	Handle	resource;
#if powerc
	MainProcUPP	mproc;
#else
	ProcPtr myProc;
#endif
	long result;
	
	SetResLoad( true );
	resource = GetResource('vmcr', ResID );
	if( resource == nil )
	{
	 	return -1;
	}
	HLock(resource);

#if powerc
	// PPC Code
	mproc = (MainProcUPP)*resource;
	result = CallUniversalProc((UniversalProcPtr)(mproc), uppMainProcInfo,  v1, v2, v3, v4 );
#
#else
	// 68K Code
	myProc = (ProcPtr)*resource;
	result = (myProc)( v1, v2, v3, v4);
#endif


	return result;
}

