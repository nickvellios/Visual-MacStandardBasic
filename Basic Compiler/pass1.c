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
#define                 UCHAR   unsigned char


extern unsigned char *buffer, *project_bp, temp[100];
extern short pjt_ctr, pjt_pos, pjt_len;

extern unsigned char parsed_line[1024], line[260], RawLine[260], label_flag;
extern short line_pos,pline_pos,line_len, current_module_num, pjt_ctr;
extern long source_len, buf_pos, line_ctr;
extern long VarSzCtr, GblSzCtr;
extern long extra_buf_pos, extra_line_ctr;
extern unsigned char CurrSrc[65], CurrProc[260];
extern long ProcLineCtr;
extern short	srcFileType;

extern struct PjtFilesStruct
{
	Str63	fname;
	short	tp;  	// 1= form  2= module 3= resource
	long    codeStart;
	short	lineCtr;
	short	FormID;
} PjtFileList[200];

#include	"Structs1.h"


extern struct RunHdrStruct // This Data Structure holds runtime info 
{       
	long    NumOfVars;
	long    NumOfCalls;
	long    NumOfLabels;
	long    StrLen;
	long    CodeLen;
	long    CodeStart;		// Where to start
	long	GlbVarSz;
	long	StartVarSz;
	long	CallTableOffset;	// Don't need VarTable Offset because it = 0
	long	LabelTableOffset;
	long	StrTableOffset;
	long	SelectTableOffset;
	long	NumOfSelects;
	long	NumOfProps;   		// New for 3.0
	long	PropTableOffset;    // New for 3.0

} RunHdr;	//ext to pass1.c & pass2.c

short pass1_findtype();
short pass1_compile_file( void );
short next_line();
short process_dims( short scope );
short register_sub();
short register_func();
short AddToLabelTable( char *labelname);
short StrPartCmp( char * st1, char * st2);
short LoadFormContents( );



short pass1_findtype()
{
	short a, b, c, ret;
	char cmdstr[15];
	a = line[0];
	b = 1;
	ret = 0;                                        // Mark as nothing, for now
	cmdstr[0] = 0;
	
	if( line[1] == ':')             // Label?
	{
		c=0;
		b=2;
		while(line[b]!=' ' && line[b]) temp[c++] = line[b++];
		temp[c] = 0;            //Terminate
		// This adds the Label to the Label Table
		AddToLabelTable( (char *) temp );
		b++;                 // Skip space
		ret = 100;
	}

	
	// Now let's grab cmd
	c = 0;
	while(b <= a && c < 13)
	{
		if( line[b] != ' ')
		{
			cmdstr[c++] = line[b++];
		}
		else b=99;
	}
	//printf( "==%s\n", cmdstr );
	if( cmdstr[0])
	{
		cmdstr[c]=0;
		// Search for:  SUB, ENDSUB, GLOBAL, ENDGLOBAL, FUNCTION, ENDFUNCTION, & DIM
		switch( cmdstr[0] )
		{
			case 'S':
				if( !strcmp( cmdstr, "START" )) { ProcLineCtr=line_ctr; strcpy( (char *) CurrProc, (char *) line+1); return(kLineStart + ret); }
				if( !strcmp( cmdstr, "SUB" )) { ProcLineCtr=line_ctr; strcpy( (char *) CurrProc, (char *) line+1); return(kLineSub + ret); }
				break;
			case 'E':
				if( !strcmp( cmdstr, "ENDSUB" )) { ProcLineCtr=line_ctr; strcpy( (char *) CurrProc, (char *) line+1); return(12 + ret); }
				if( !strcmp( cmdstr, "ENDGLOBAL" )) { ProcLineCtr=line_ctr; strcpy( (char *) CurrProc, (char *) line+1); return(14 + ret); }
				if( !strcmp( cmdstr, "ENDFUNCTION" )) { ProcLineCtr=line_ctr; strcpy( (char *) CurrProc, (char *) line+1); return(16 + ret); }
				break;
			case 'D':
				if( !strcmp( cmdstr, "DIM" )) { ProcLineCtr=line_ctr; strcpy( (char *) CurrProc, (char *) line+1); return(35 + ret); }
				break;
			case 'G':
				if( !strcmp( cmdstr, "GLOBAL" )) { ProcLineCtr=line_ctr; strcpy( (char *) CurrProc, (char *) line+1);  return(kCompInGlobal + ret); }
				break;
			case 'F':
				if( !strcmp( cmdstr, "FUNCTION" )) { ProcLineCtr=line_ctr; strcpy( (char *) CurrProc, (char *) line+1); return(15 + ret); }
				if( !strcmp( cmdstr, "FINISH" )) { ProcLineCtr=line_ctr; strcpy( (char *) CurrProc, (char *) line+1); return(22 + ret); }
				break;
		}       
	}
	return(ret);
}

short pass1_compile_file( void )
{
	// This function does a PASS 1 Compile on source file that is already
	// loaded into buffer.
	
	short result=0, ctr, passmode, linetype, looper, Tcmn, a, flag3;
	struct CallTableStruct2 *BPtr3;

	line_ctr = 0;        
	passmode = 0;
	Tcmn = current_module_num;   // Temp storage
	
	// New for Visual 3.0 check for form and if so load up prop info
	result = next_line();   if(result) return 0;
	srcFileType = 2;
	PjtFileList[pjt_ctr-1].tp=2;
	PjtFileList[pjt_ctr-1].FormID = -1;  // for pass2 
	if( StrPartCmp( (char*) line+1 ,  "MacStandardBasic Form" ) )
	{
		srcFileType =1;
		LoadFormContents( );
		flag3 = 0;
		PjtFileList[pjt_ctr-1].tp=1;
		PjtFileList[pjt_ctr-1].codeStart = extra_buf_pos;  // shortcut for pass 2
		PjtFileList[pjt_ctr-1].lineCtr = extra_line_ctr;   // shortcut for pass 2
	}
	else if( StrPartCmp( (char*) line+1 , "MacStandardBasic Module" ) )
	{
		flag3 = 1;
		PjtFileList[pjt_ctr-1].codeStart = buf_pos;  // shortcut for pass 2
		PjtFileList[pjt_ctr-1].lineCtr = line_ctr;   // shortcut for pass 2
	}
	else flag3 = 0;  // No header or form

	while(!result)
	{
	  if( flag3 ) 	result = next_line();
 	  else
 	  {
 	  		flag3 = 1; // Skip only first time because of no header
 	  		if( !strcmp( (char*) line+1, "FORMEND") || !strcmp( (char*) line+1, "CONTROLEND") ) result=1;
	  }
			
	
 	  if(!result) 
	  {
	    linetype = pass1_findtype();
	    switch( linetype )
	    {
		case 0:
		case 100:
			if( !GlobalData.CompMode )
			{
				// Error Code outside module
				compile_log_update(152, nil);

				//printf("#%li - %s\n", line_ctr, RawLine);
				//printf( "Error - Code outside of module\n");
				return(-1);
			}
			else
			{
				
				if( GlobalData.DimMode == 2)
				{
					if( GlobalData.CompMode == kCompInGlobal )
					{
						// Error Code in Global module
						//printf("#%li - %s\n", line_ctr, RawLine);
						//printf( "Error - No code allowed in GLOBAL module\n");
						compile_log_update(153, nil);
						return(-1);
					}
				}
				GlobalData.DimMode = 0;
			}       
			break;
		case kCompInStart:        //Start
			if(line[0]==5)
			{
				//Ok we have found START
				GlobalData.DimMode = 1;
				VarSzCtr = 0;
							
				if(GlobalData.CompMode) 
				{
					// Uh Oh we have a nesting problem - tell em
					//printf("#%li - %s\n", line_ctr, RawLine);
					switch(GlobalData.CompMode)
					{
					   case kCompInSub:
					     compile_log_update(105, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDSUB\n");
					     break;	
					   case kCompInGlobal:
					     compile_log_update(106, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDGLOBAL\n");
					     break;	
					   case kCompInFunction:
					     compile_log_update(107, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDFUNCTION\n");
					     break;	
					   case kCompInStart:
					     compile_log_update(108, nil);
					     //printf("Error - Module Nesting Error, Expecting FINISH\n");
					     break;	
					}
					return(-1);
				}

				if(GlobalData.StartFlag) 
				{
					// Uh Oh we already have a START in our code - tell em
					//printf("#%li - %s\n", line_ctr, RawLine);
					//printf("Error - More that one START module defined\n");
					compile_log_update(154, nil);
					return(-1);
				}
				GlobalData.StartFlag=1;
				GlobalData.CompMode=kCompInStart;
			}
			else
			{
					// Other stuff on line error
					//printf("#%li - %s\n", line_ctr, RawLine);
					//printf("Syntax Error - Expecting end of line\n");
					compile_log_update(155, nil);
					return(-1);
			}
			break;
		case 22:                // Finish
			if(line[0]==6)
			{
				GlobalData.DimMode = 0;
				RunHdr.StartVarSz = VarSzCtr;
				//printf("VSCtr = %li\n", VarSzCtr ); 
				if( GlobalData.CompMode == 21 )
				{
					GlobalData.CompMode = 0;                // Reset Mode
				}
					else
				{
					// Uh Oh we have a nesting problem - tell em
					//printf("#%li - %s\n", line_ctr, RawLine);
					switch(GlobalData.CompMode)
					{
					   case 11:
					     compile_log_update(105, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDSUB\n");
					     break;	
					   case 13:
					     compile_log_update(106, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDGLOBAL\n");
					     break;	
					   case 15:
					     compile_log_update(107, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDFUNCTION\n");
					     break;	
					   case 21:
					     compile_log_update(108, nil);
					     //printf("Error - Module Nesting Error, Expecting FINISH\n");
					     break;	
					   default:
					     compile_log_update(141, nil);
					     break;	
					}
					return(-1);
				}
			}
			else
			{
					// Other stuff on line error
					//printf("#%li - %s\n", line_ctr, RawLine);
					//printf("Syntax Error - Expecting end of line\n");
					compile_log_update(155, nil);
					return(-1);
			}
			break;
		case 13:        //GLOBAL
			if(line[0]==6)
			{
				VarSzCtr = 0;
				GlobalData.DimMode = 2;
				//Ok we have found Global
				if(GlobalData.CompMode) 
				{
					// Uh Oh we have a nesting problem - tell em
					//printf("#%li - %s\n", line_ctr, RawLine);
					switch(GlobalData.CompMode)
					{
					   case 11:
					     compile_log_update(105, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDSUB\n");
					     break;	
					   case 13:
					     compile_log_update(106, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDGLOBAL\n");
					     break;	
					   case 15:
					     compile_log_update(107, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDFUNCTION\n");
					     break;	
					   case 21:
					     compile_log_update(108, nil);
					     //printf("Error - Module Nesting Error, Expecting FINISH\n");
					     break;	
					}
					return(-1);
				}
				GlobalData.CompMode=13;
			}
			else
			{
					// Other stuff on line error
					//printf("#%li - %s\n", line_ctr, RawLine);
					//printf("Syntax Error - Expecting end of line\n");
					compile_log_update(155, nil);
					return(-1);
			}
			break;
		case 14:                // ENDGLOBAL
			if(line[0]==9)
			{
				GlobalData.DimMode = 0;
				RunHdr.GlbVarSz += VarSzCtr;
				VarSzCtr = 0;

				if( GlobalData.CompMode == 13 )
				{
					GlobalData.CompMode = 0;                // Reset Mode
				}
				else
				{
					// Uh Oh we have a nesting problem - tell em
					//printf("#%li - %s\n", line_ctr, RawLine);
					switch(GlobalData.CompMode)
					{
					   case 11:
					     compile_log_update(105, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDSUB\n");
					     break;	
					   case 13:
					     compile_log_update(106, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDGLOBAL\n");
					     break;	
					   case 15:
					     compile_log_update(107, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDFUNCTION\n");
					     break;	
					   case 21:
					     compile_log_update(108, nil);
					     //printf("Error - Module Nesting Error, Expecting FINISH\n");
					     break;	
					   default:
					     compile_log_update(139, nil);
					     break;	
					}
					return(-1);
				}
			}
			else
			{
					// Other stuff on line error
					//printf("#%li - %s\n", line_ctr, RawLine);
					//printf("Syntax Error - Expecting end of line\n");
					compile_log_update(155, nil);
					return(-1);
			}
			break;
		case 35:        // DIM
		case 135:
			if( !GlobalData.CompMode )
			{
				// Error Code outside module
				//printf("#%li - %s\n", line_ctr, RawLine);
				//printf( "Error - Code Outside of Module\n");
				compile_log_update(153, nil);
				return(-1);
			}
			else
			{
				if( !GlobalData.DimMode)
				{
					// Error Code outside module
					//printf("#%li - %s\n", line_ctr, RawLine);
					//printf( "Error - Declare variables at top of module\n");
					compile_log_update(153, nil);
					return(-1);
				}
				else
				{
					/*
					if( GlobalData.CompMode == 13 )
					{
						// current_module_num = 0;
						 //printf("\nProcessing Global Vars\n");
					}
					else  printf("\nProcessing Local Vars %i\n",current_module_num);
					*/
					line_len = line[0];
					line_pos=1;
					// Skip any 
					if(line[line_pos] == ':')  //Checking if label is here
					{
						//Process Label
						while(line[line_pos] != ' ')line_pos++;
						line_pos++;
					}
					looper = 0;
					while( !looper )
					{
						if( line[line_pos]=='D' && line[line_pos+1]=='I' && line[line_pos+2]=='M' && line[line_pos+3]==' ' ) 
						{
							line_pos+=4;
							process_dims( 0 );
							if( line[line_pos]==':' && line_pos<=line_len) 
							{
								looper=0;
								line_pos++;
							}
							else looper++;
						}
						else
						{
							GlobalData.DimMode = 0;
							looper++;
						}
						
					}
					//current_module_num = Tcmn; //Reset
						
				}
			}               
			break;
		case 11:                //Sub
			GlobalData.DimMode = 2;
			//Ok we have found Sub
			VarSzCtr = 0;           // Reset
			if(GlobalData.CompMode) 
			{
				// Uh Oh we have a nesting problem - tell em
				//printf("#%li - %s\n", line_ctr, RawLine);
				switch(GlobalData.CompMode)
				{
					   case 11:
					     compile_log_update(105, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDSUB\n");
					     break;	
					   case 13:
					     compile_log_update(106, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDGLOBAL\n");
					     break;	
					   case 15:
					     compile_log_update(107, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDFUNCTION\n");
					     break;	
					   case 21:
					     compile_log_update(108, nil);
					     //printf("Error - Module Nesting Error, Expecting FINISH\n");
					     break;	
				}
				return(-1);
			}
			GlobalData.CompMode=11;
			a = register_sub();         // Add sub name to call table
			if(a<0) return(-1);
			break;
		case 12:                //EndSub
			if(line[0]==6)
			{
				BPtr3 = GlobalData.CallTable2Page[GlobalData.Num_CTP-1];
				BPtr3[(GlobalData.NumOfCalls-1) % 100 ].VarMemSz = VarSzCtr;
				GlobalData.DimMode = 0;
				if( GlobalData.CompMode == 11 )
				{
					GlobalData.CompMode = 0;                // Reset Mode
				}
				else
				{
					// Uh Oh we have a nesting problem - tell em
					//printf("Extra Endsub#%li - %s\n", line_ctr, RawLine);
					switch(GlobalData.CompMode)
					{
					   case 11:
					     compile_log_update(105, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDSUB\n");
					     break;	
					   case 13:
					     compile_log_update(106, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDGLOBAL\n");
					     break;	
					   case 15:
					     compile_log_update(107, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDFUNCTION\n");
					     break;	
					   case 21:
					     compile_log_update(108, nil);
					     //printf("Error - Module Nesting Error, Expecting FINISH\n");
					     break;	
					   default:
					     compile_log_update(138, nil);
					     break;	
					   
					}
					return(-1);
				}
			}
			else
			{
					// Other stuff on line error
					//printf("#%li - %s\n", line_ctr, RawLine);
					//printf("Syntax Error - Expecting end of line\n");
					compile_log_update(155, nil);
					return(-1);
			}
			break;
		case 15:                //Function
			GlobalData.DimMode = 2;
			//Ok we have found Function
			VarSzCtr = 0;           // Reset
			if(GlobalData.CompMode) 
			{
				// Uh Oh we have a nesting problem - tell em
				//printf("#%li - %s\n", line_ctr, RawLine);
				switch(GlobalData.CompMode)
				{
					   case 11:
					     compile_log_update(105, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDSUB\n");
					     break;	
					   case 13:
					     compile_log_update(106, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDGLOBAL\n");
					     break;	
					   case 15:
					     compile_log_update(107, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDFUNCTION\n");
					     break;	
					   case 21:
					     compile_log_update(108, nil);
					     //printf("Error - Module Nesting Error, Expecting FINISH\n");
					     break;	
				}
				return(-1);
			}
			GlobalData.CompMode=15;
			register_func();              // Add func name to call table
			break;
		case 16:                //EndFunction
			if(line[0]==11)
			{
				BPtr3 = GlobalData.CallTable2Page[GlobalData.Num_CTP-1];
				BPtr3[( GlobalData.NumOfCalls-1) % 100].VarMemSz = VarSzCtr;
				GlobalData.DimMode = 0;
				if( GlobalData.CompMode == 15 )
				{
					GlobalData.CompMode = 0;                // Reset Mode
				}
				else
				{
					// Uh Oh we have a nesting problem - tell em
					//printf("#%li - %s\n", line_ctr, RawLine);
					switch(GlobalData.CompMode)
					{
					   case 11:
					     compile_log_update(105, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDSUB\n");
					     break;	
					   case 13:
					     compile_log_update(106, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDGLOBAL\n");
					     break;	
					   case 15:
					     compile_log_update(107, nil);
					     //printf("Error - Module Nesting Error, Expecting ENDFUNCTION\n");
					     break;	
					   case 21:
					     compile_log_update(108, nil);
					     //printf("Error - Module Nesting Error, Expecting FINISH\n");
					     break;	
					   default:
					     compile_log_update(140, nil);
					     break;	
					}
					return(-1);
				}
			}
			else
			{
					// Other stuff on line error
					//printf("#%li - %s\n", line_ctr, RawLine);
					//printf("Syntax Error - Expecting End of Line\n");
					compile_log_update(155, nil);
					return(-1);
			}
			break;
	    }
	    /* DEBUG CODE
	    printf("%li - %i {", line_ctr, linetype);
	    for(ctr=1; ctr<=line[0]; ctr++) putchar(line[ctr]);
	    printf("} [%li]\n",buf_pos);
		*/
	  }
	}
	

	return(0);
}
