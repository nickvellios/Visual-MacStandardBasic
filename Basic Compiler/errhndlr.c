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
#include		<SIOUX.h>
#define         UCHAR   unsigned char

short compile_log_init();
short compile_log_create();
short compile_logfile_update( char * txt);
short compile_log_update(short errno, char *special);
short lde(short errno, short lineno);

FSSpec  CompileLogFS;
short	CompileLogFN, ErrorLogExist;
unsigned char LastErrSrc[65], LastErrProc[65];
short	srcFileType=1;

extern unsigned char RawLine[260];
extern long line_ctr;
extern unsigned char CurrSrc[65], CurrProc[260];
extern long  ErrorCtr;
extern FSSpec	PjtSpec, IDESpec;
extern short ArgNumber;
long ProcLineCtr;
extern Str255 TargetDef;

short compile_log_init()
{
	// This function deletes ERRORLOG if it exists
	// Returns 0 if none available or 1 if it has one
	
	OSErr   error;
	LastErrSrc[0] = 0;
	LastErrProc[0] = 0;
	// Create final app file
	ErrorLogExist = 0;
	error = FSMakeFSSpec( IDESpec.vRefNum, IDESpec.parID, "\pERRORLOG", &CompileLogFS);
	if(error == -43 ) return(0);
	FSpDelete( &CompileLogFS);    // Let's get rid of existing file
	return(0);
}


short compile_log_create()
{
	// This function loads next file into buffer
	// Returns 0 if none available or 1 if it has one
	
	OSErr   error;

	// Create final app file
	error = FSMakeFSSpec(IDESpec.vRefNum, IDESpec.parID, "\pERRORLOG", &CompileLogFS);
	//printf("Creating Error Log\n");
	FSpCreate( &CompileLogFS, 'MSB1', 'TEXT', smSystemScript);
	ErrorLogExist = 1;
	return(0);
}

short compile_logfile_update( char * txt)
{
	long slen;
	OSErr   error;


	if( !ErrorLogExist ) compile_log_create();
	error = FSpOpenDF( &CompileLogFS, fsRdWrShPerm, &CompileLogFN);
	if(error) SysBeep(30);
	SetFPos( CompileLogFN, 2, 0);  // Append 
	for(slen=0;slen<255;slen++) if(!txt[slen]) break;
	error = FSWrite( CompileLogFN, &slen, txt );
	if(error) SysBeep(30);
	error = FSClose( CompileLogFN);
	//if(error) SysBeep(30);
	//FlushVol( nil, 0 );
	return(0);
}

short compile_log_update(short errno, char *special)
{
	short llen;
	char st1[260];
	char errmsg[][80]=
	{
		"Invalid BASIC Command Or Sub Not Found", //101
		"Argument Syntax Error", 
		"Variable is not defined ->",
		"Function not found ->",
		"Module Nesting Error, Expecting ENDSUB", //105
		"Module Nesting Error, Expecting ENDGLOBAL",
		"Module Nesting Error, Expecting ENDFUNCTION",
		"Module Nesting Error, Expecting FINISH",
		"Expecting Required Sub Name or NONE in argument #",
		"Expecting Numeric in argument #", //110
		"Expecting String in argument #",
		"Floating Point Values Not Allowed in Select Case statements",
		"Only a constant or variable name is allowed in a Case Statement",
		"Error in array variable in argument #",  
		"Expecting parenthesis '('", //115
		"Expecting WHILE or UNTIL in Do Statement",
		"Error in Function Argument",
		"Too many array elements",
		"Expecting array element(s) in argument #",
		"Expecting assignment equal sign '='", //120
		"Too many parameters", 	
		"ELSE without a IF", 	
		"ENDIF without a IF", 	
		"Error in array variable trying to be assigned",  
		"Expecting THEN", //125
		"Label not found ->",
		"Variable name is too long ->",
		"NEXT without a FOR", 	
		"LOOP without a DO", 	
		"SELECT nesting overflow", //130	
		"Syntax Error", 
		"No Start/Finish Module Defined",
		"Procedure closed with a FOR without a NEXT", 	
		"Procedure closed with a DO WHILE without a LOOP", 	
		"Procedure closed with a DO UNTIL without a LOOP", //135 
		"Procedure closed with an IF without an ENDIF", 
		"Duplicate Sub or Function Procedure name found",
		"Module Nesting Error, Unexpected ENDSUB", //138
		"Module Nesting Error, Unexpected ENDGLOBAL",
		"Module Nesting Error, Unexpected ENDFUNCTION",
		"Module Nesting Error, Unexpected FINISH", // 141
		"Unknown Compiler Error: Check syntax of statement", 
		"Invalid character in statement", // 143
		"Array specification error for ",  //144
		"DIM variable error",  //145
		"Variable type mismatch",  //146
		"Expecting AS parameter",  //147
		"Attempt to define an invalid Function name ",  //148
		"MacStandardBasic IDE and Compiler need to be in the same folder!", //149
		"Could not load specified project",  // 150
		"Project source file not found > ",  // 151
		"Command statement outside of a procedure",  // 152
		"No command statements allowed in Global module",  // 153
		"More than than one START procedure in project",  // 154
		"Expecting end of line", //155
		"Memory allocation error: not enough memory available",  //156
		"Could not find resource file > ",  //157
		"Library not found ",  //158
		"Expecting return designator %&!#$,INTEGER,LONG,SINGLE,DOUBLE,or STRING after AS",  //159
		"Unbalanced parentheses ((...)",  //160
		"Unbalanced parentheses (...))",  //161
		"EXITLOOP without a DO", 	//162
		"Form or Control not found ->",  // new for 3.0
		"Sub or command name is too long", //164
		"Variable name to be assigned is too long", //165
		"Sub Proc. not found for object ->",  //166
		"Specified Library Definition File Not Found->",  //167
	};
		
	if( strcmp( (char*) LastErrSrc, (char*)CurrSrc ) ) 
	{
		strcpy(  (char*)LastErrSrc, (char*)CurrSrc );
		if( srcFileType == 1 ) sprintf( st1, "Errors in Form > %s\x0d\x0d", LastErrSrc);
		else sprintf( st1, "Errors in Module > %s\x0d\x0d", LastErrSrc);
		compile_logfile_update( st1 );

		// New for 3.0 proc name
		if( CurrProc[0] )
		{
			strcpy(  (char*)LastErrProc, (char*)CurrProc );
			sprintf( st1, "    %s\x0d\x0d", LastErrProc);
			compile_logfile_update( st1 );
		}
	}
	else
	{  // same as last
		if( strcmp( (char*) LastErrProc, (char*)CurrProc ) ) 
		{	// New for 3.0 proc name
			strcpy(  (char*)LastErrProc, (char*)CurrProc );
			sprintf( st1, "    %s\x0d\x0d", LastErrProc);
			compile_logfile_update( st1 );
		}
	}
		
	if( CurrProc[0] )
	{
		llen = sprintf( st1, "  [Line %li] %s\x0d", line_ctr- ProcLineCtr +1, RawLine);  //printf("%s\x0a", st1);	
		compile_logfile_update( st1 );
	}
	
	switch(errno)
	{
		case 109:
		case 110:
		case 111:
		case 114:
		case 119:
			llen = sprintf( st1, "  ERROR #%i - %s %i\x0d\x0d", errno, errmsg[errno-101], ArgNumber);
	 		break;
	 	default:
			if( special !=nil)	llen = sprintf( st1, "  ERROR #%i - %s [%s] \x0d\x0d", errno, errmsg[errno-101], special);
			else llen = sprintf( st1, "  ERROR #%i - %s\x0d\x0d", errno, errmsg[errno-101]);
			break;
	}
	
	//printf("%s\x0a", st1);	 
	compile_logfile_update( st1 );
	
	ErrorCtr++; 

	return(0);
}

short lde(short errno, short lineno)
{
	short llen;
	char st1[260];
	char errmsg[][80]=
	{
		"", // placeholder for 0
		"Truncated Library Def File", //1
		"Could not find line[Commands]", 
		"Could not find line[Functions]",
		"Syntax Error on Line #",
		"Invalid Command # on Line #",//5
	};
		
	sprintf( st1, "Errors in Library Def File > %s\x0d\x0d", TargetDef+1);
	compile_logfile_update( st1 );


	if( errno >= 4 ) llen = sprintf( st1, "%s %i\x0d\x0d", errmsg[errno], lineno);
	else  llen = sprintf( st1, "%s\x0d\x0d", errmsg[errno]);
	
	compile_logfile_update( st1 );
	
	ErrorCtr++; 

	return(0);
}
