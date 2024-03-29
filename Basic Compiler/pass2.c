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
#include        <ctype.h>
#include		"pass2.h"

#define         UCHAR   unsigned char
#define         CMDEND  253

char *imod[]=
{       "START", "SUB", "FUNCTION", "GLOBAL", "FINISH", "ENDSUB", "ENDFUNCTION", "ENDGLOBAL", "DIM" };

short ArgNumber;
void COD( UCHAR *msg );

extern unsigned char *buffer, *project_bp, temp[100];
extern short pjt_ctr, pjt_pos, pjt_len;

extern unsigned char parsed_line[1024], line[260], label_flag;
extern short line_pos,pline_pos,line_len, LabelCtr, whtype, current_module_num, p2_startflag, pjt_ctr;
extern long source_len, buf_pos, line_ctr, VarSzCtr;
extern UCHAR ParList[12], DimNum;
short LoopStackPtr;
long  ErrorCtr=0;
extern short SelectCtr, SelectArray[32], SelectCurr;
 short ArgTypeChecker;
short	SelectCurrType, SelectTypeArray[32], SingleLineIfFlag=false;
extern long ProcLineCtr;
extern unsigned char CurrSrc[65], CurrProc[260];
extern short	srcFileType;

#include        "Structs1.h"

extern struct PjtFilesStruct
{
	Str63	fname;
	short	tp;  	// 1= form  2= module 3= resource
	long    codeStart;
	short	lineCtr;
	short	FormID;
} PjtFileList[800];

extern struct  cmd_list //These are built-in cmds, there will also be user defined subs
{
	short   num;
	char    name[20];
	char    params[10];
}       cmd_list[800];

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

struct  CmpLoopStruct    // Serves different functions in compile & run 
{
	long          location;       
	char          looptype;         // 1=For, 2=If, 3=else, 4=while, 5=until
} CmpLoop[500];

short next_line();
short process_dims( short scope );
short register_sub();
short AddToLabelTable( char *labelname );
short compile_argument( short argtype );
short ObjectTableFind( Str255 objname, short objtype );
short FormTableFind( Str255 formname );

short p2_cmd_compile( ); //short ll);
short p2_line_compile();
short p2_skip_global();
short p2_compile_module( short modtype);
short pass2_compile_file();

short CallTableFind( char * CmdText, short CallType );
short isvalidvar(short ch);
short OneOfThem( short ch, char *src );
short FindLabel( char * lbl );
short SetLabel( short labelnum, long location );
short FindVar( char * var );
short WriteParseLine();
short GetCallPars( short d );
short VarTableFind( char * varname, short vartype );
short UpdateCodePage(long location, long where);
long GetCallVMS( short d );
short SetCallVMS( short d, long value99 );
short SetCallLocation( short d, long value99 );
short AddToSelectTable( short status, short casenum, long value );
void DoLoopStackReport();
short compile_logfile_update( char * txt);
short compile_log_update(short errno, char *special);
short FormTableFind( Str255 formname );

short whdata;

short p2_cmd_compile( ) //short ll)
{
	short a, b, bu, df=0;
	char temp2[85];
	
	// Skip lead spacing
	while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
	
	// Store
	bu = line_pos;
	//  Cmd or var
	if(line[line_pos] == '?' ) // Check for ? Print Command SPECIAL
	{
		strcpy(temp2, "PRINT");
		line_pos++;
	}
	else
	{
		a = 0;
		while( ( isvalidvar(line[line_pos]) ||  line[line_pos]=='.' ) && line_pos <= line[0] ) // . new for 3.0
		{
			if( line[line_pos]=='.' ) df++;
			temp2[a++] = line[line_pos++];
			if(a>40)
			{
				if( !df )
				{
					while( ( isvalidvar(line[line_pos]) ||  line[line_pos]=='.' ) && line_pos <= line[0] ) line_pos++;
					while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
					if( line[line_pos]=='=' )  return(-1); // Varname too long...
					else return(-2);  // procname too long...
				}
				if( a>80)  return(-2);  // procname too long...
			}
		}
		temp2[a]=0;
	}
	if( df>1 ) return -3;
	while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
	// Ok, test for implied let
	if( OneOfThem( line[line_pos], "%&!#$[="))
	{
		line_pos = bu;  // Restore
		whdata = 1;
		//printf("Found Implied Let\n");
		return( 101 );   // Let Cmd #  was 101
	}
	a = CallTableFind( temp2, 1 );
	// DEBUG CODE printf( "<%s><%i>\n", temp2, a);
	if(a < 0 )
	{
		
		// None of those, so check for module identifier
		// Search for:  SUB, ENDSUB, GLOBAL, ENDGLOBAL, FUNCTION, ENDFUNCTION, & DIM
		for(b=0; b<9; b++) if( !strcmp( temp2, imod[b] ))  return(b); 

		return(-4); // Error - No cmd match and not implied let
	}
	return(a);

}

short p2_line_compile()
{
	short a, b, c, d,e, f, bn, m, n;
	char temp2[45], par[12], ctemp[20];
	short   *ip;
	long    *lp, nl, CCT;
	Str255	temp3;
	
	//printf("%s - Code Ptr = %li\n",line+1,GlobalData.CodeCtr);
	
	ip = (short*) (parsed_line);
	*ip = 0;                        // Set line len to 0
	pline_pos=2;                    //Update pos. ptr

	a = line[0];
	line_pos = 1;
	
	if( line[1] == ':')             // Label?
	{
		c=0;
		while(line[line_pos]!=' ' && line[line_pos]) line_pos++;
		// This puts code addr in the Label Table
		SetLabel( LabelCtr, GlobalData.CodeCtr );
		line_pos++;                 // Skip space
		LabelCtr++;
	}               

	// Now compile line after label(if any)
	while( line_pos <= a)   
	{
		
		d = p2_cmd_compile( a );
	
		// Check for Dim Statement
		if( d < 0 )
		{
			// Error 
		    switch( d)
		    {
		    	case -1: // varname too long
		    		compile_log_update(165, nil);
					break;
		    	case -2: // procname too long
		    		compile_log_update(164, nil);
					break;
		    	case -3:
		    	case -4:
		    		compile_log_update(101, nil);
					break;
			}
			return(-1);
		}
		if( d == 51 )
		{
			while( line_pos <= a && line[line_pos] != ':' ) line_pos++;
			if(line[line_pos] == ':' ) line_pos++;
			else return(0);
			d = -1;
		}
		
		if(d >= 0)
		{
			ip = (short*) (parsed_line+pline_pos);
			*ip = d;                    //Now Store cmd 2 bytes
			pline_pos+=2;               //Update pos. ptr
		
			//Ok now process args (if any)
			if( d < 1000 ) strcpy( par, cmd_list[whdata].params );
			else 
			{
				b = GetCallPars( d );
				if(!b) strcpy( par, (char*) ParList );  // Temp 
				else
				{
					//printf("Error - Getting par list\n");
					//compile_log_update(153, nil);zzzzzz
					strcpy( par, "Z");
				}       
			}
			b=0;
			e=0;
			while(!e)
			{
				ArgNumber = b+1;
				//printf("==> %c %i %s\n", par[b], whdata, cmd_list[whdata].name);
				switch( par[b] )
				{
					case    'Z':
					case      0:                                    
						// All Done
						parsed_line[pline_pos++] = CMDEND;
						e++;
						while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
						
						/*if(SingleLineIfFlag)
						{
							UpdateCodePage( CmpLoop[LoopStackPtr].location+4, GlobalData.CodeCtr + 5);
							LoopStackPtr--;
							//printf("ZZZZZ > 0\n");
							//SingleLineIfFlag=false;
						}*/
						//else
						 if( line[line_pos]!=':' && line_pos <= line[0] ) 
						{
							compile_log_update(121, nil);
							//return(0);
						}
						//line_pos=a+1;           
						break;
					case    'a':
						while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
						if( line[line_pos]!=':' && line_pos <= line[0] ) 
						{
							do
							{
								c = compile_argument( par[b] );
								if( c == -1 )
								{
									if( par[b]=='S') compile_log_update(111, nil);
									else compile_log_update(110, nil);
									return -1;
								}
								c = 0;
								//printf("====> %c\n", line[line_pos]);
								//if( (line[line_pos]==',' || line[line_pos]==';') && line_pos <= line[0]) { line_pos++; c++;}
								if( (line[line_pos]==',' ) && line_pos <= line[0]) { line_pos++; c++;}
							} while( c );
							if( line[line_pos]==';' && line_pos <= line[0]) 
							{ line_pos++; parsed_line[pline_pos++] = 255;}
						}
						break;
					case	'E':
						c = compile_argument( 'E' );
						if( c == -1 ) return(-1);
						if( c == -2 )
						{
							if( par[b]=='S') compile_log_update(111, nil); //ctemp);
							else compile_log_update(110, nil); //ctemp);
						}
						break;
					case	'l':  // small L   This is for an optional extra # parm
						c = compile_argument( 'L' );
						if( c<0 ) return -1;
						//if( c<0 ) parsed_line[pline_pos++] = 252;
						break;
					case	'S':
					case    'L':
					case    'X':
					case    'U':
					case    'D':
						c = compile_argument( par[b] );
						if( c == -1 ) return(-1);
						if( c == -2 )
						{
							if( par[b]=='S') compile_log_update(111, nil); //ctemp);
							else compile_log_update(110, nil); //ctemp);
						}
						break;
					case	'c':  // small c   This is for ctl id
						while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
						c = 0;  // Let's see i
						if( isalpha( line[line_pos] ) )
						{
							m = line_pos;
							f = 1;
							while( ( isvalidvar(line[m]) || line[m] =='.') && m <= line[0] ) 
								temp3[f++] = line[m++];
							temp3[f]=0; temp3[0] = f-1;
							n = ObjectTableFind( temp3, 1 );
							if( n>=0 )
							{
								line_pos = m;
								parsed_line[pline_pos++] = 2; // ID as ctl
								ip = (short*) (parsed_line + pline_pos);
								*ip = n;
								pline_pos+=2;
								c++;
							}
						}							
						if( !c )
						{
							parsed_line[pline_pos++] = 1; // ID as var
							c = compile_argument( 'L' );
							if( c<0 ) return -1;
						}
						//if( c<0 ) parsed_line[pline_pos++] = 252;
						break;
/*					case	'w':  // small c   This is for form id
						while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
						c = 0;  // Let's see i
						if( isalpha( line[line_pos] ) )
						{
							m = line_pos;
							f = 1;
							while( ( isvalidvar(line[m]) || line[m] =='.') && m <= line[0] )
							{
						 		temp3[f++] = line[m++];
							}
							temp3[f]=0; temp3[0] = f-1;
							//printf("-->>%s\n", temp3+1 );  getchar();
							//n = ObjectTableFind( temp3, 1 );
							n = FormTableFind( temp3 );
							//printf("++>>%i\n", m );  getchar();
							//if( m == -1 ) compile_log_update(163, temp3+1);
							if( n>=0 )
							{
								line_pos = m;
								parsed_line[pline_pos++] = 2; // ID as ctl
								ip = (short*) (parsed_line + pline_pos);
								*ip = n;
								pline_pos+=2;
								c++;
							}
						}							
						if( !c )
						{
							parsed_line[pline_pos++] = 1; // ID as var
							c = compile_argument( 'L' );
						}
						//if( c<0 ) parsed_line[pline_pos++] = 252;
						break;
*/					case	'w':  // small c   This is for form id
						while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
						c = 0;  // Let's see i
						if( isalpha( line[line_pos] ) )
						{
							m = line_pos;
							f = 1;
							while( ( isvalidvar(line[m]) || line[m] =='.') && m <= line[0] )
							{
						 		temp3[f++] = line[m++];
							}
							temp3[f]=0; temp3[0] = f-1;
							//printf("-->>%s\n", temp3+1 );  getchar();
							//n = ObjectTableFind( temp3, 2 );
							n = FormTableFind( temp3 );
							//printf("++>>%i\n", n );  getchar();
							//if( m == -1 ) compile_log_update(163, temp3+1);
							if( n>=0 )
							{
								line_pos = m;
								parsed_line[pline_pos++] = 2; // ID as form
								ip = (short*) (parsed_line + pline_pos);
								*ip = n;
								pline_pos+=2;
								c++;
								//parsed_line[pline_pos++] = 252;
							}
						}							
						if( !c )
						{
							parsed_line[pline_pos++] = 1; // ID as var
							c = compile_argument( 'L' );
						}
						//if( c<0 ) parsed_line[pline_pos++] = 252;
						break;
					case    'R':	// User defined Sub
						c = p2_cmd_compile( par[b] );
						if( c >= 1000 || c==102)	// 102 = NONE cmd
						{
							if( c == 102 ) c=0;
							ip = (short*) (parsed_line + pline_pos);
							*ip = c;
							pline_pos+=2;
						}
						else
						{
							compile_log_update(109, nil);//" ");
							return(-1);
						}
						break;
					case    'T':    // THEN
						if( line[line_pos] == 'T' && line[line_pos+1] == 'H'  && line[line_pos+2] == 'E' && line[line_pos+3] == 'N' ) 
						{
							line_pos+=4;
							// Now find out if any cmds after THEN
							while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
							if( line_pos <= line[0] ) 
							{
								SingleLineIfFlag=true;
								parsed_line[pline_pos++] = CMDEND;
								e++;
							}
							//parsed_line[pline_pos++] = ;  
						}
						else
						{
							compile_log_update(125, nil);
							//printf("ERROR - Expecting THEN\n");
						}
						break;
					case    '=':    // equal sign
						if( line[line_pos] == '=' ) line_pos++;
						//else error
						break;
					case    't':    // TO
						if( line[line_pos] == 'T' && line[line_pos+1] == 'O' ) 
						{
							line_pos+=2;
						}
						//else error
						break;
					case    's':    // STEP (Optional)
						if( line[line_pos] == 'S' && line[line_pos+1] == 'T'  && line[line_pos+2] == 'E' && line[line_pos+3] == 'P' ) 
						{
							line_pos+=4;
							while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
							c = compile_argument( 'L' );
							if( c<0 ) return -1;

							//parsed_line[pline_pos++] = ;  
						}
						
						break;
					case    'J':
						// DEBUG CODE printf("Gosub Spot is %li\n", GlobalData.CodeCtr);
						while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
						c = 0;
						while( isvalidvar(line[line_pos]) && line_pos <= line[0] )
						{
							temp2[c++] = line[line_pos++];
							if(c>40)  return(-1); // Label too long...}
						}
						temp2[c]=0;

						c = FindLabel(temp2);
						if( c>=0 )
						{
							ip = (short*) (parsed_line + pline_pos);
							*ip = c;
							// DEBUG CODE printf("Label stuff %i\n",c);
							pline_pos+=2;
						}
						else
						{
							compile_log_update(126, temp2);
							//printf( "ERROR - Label [%s] Not Found\n",temp2);
						}
						break;
						
					case    'm':    // Let Assignment  varnum|vartype|expression
						while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
						c = 0;  // Let's get the variable to assign
						while( isvalidvar(line[line_pos]) && line_pos <= line[0] )
						{
							temp2[c++] = line[line_pos++];
							if(c>40)
							{
								temp2[38] = '+'; temp2[39] = '+'; temp2[40] = '+';
								compile_log_update(127, (char*)temp2);
							  	return(-1); // Varname too long...}
							}
						}
						temp2[c]=0;
						DimNum=0;
						c = VarTableFind(temp2, 0);        //Find It
						if( c>=0 )
						{
							ip = (short*) (parsed_line + pline_pos);
							*ip = c;
							ip++;
							// DEBUG CODE printf("Var to be assigned %s\n",temp2);
							pline_pos+=2;
							//parseline[pline_pos++] = whtype;      //what type
							while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
							
							if( OneOfThem( line[line_pos], "%&!#$" ) ) line_pos++;
							bn = whtype;
							// Let's check for arrays							
							if( (DimNum==1 && whtype!=5) || DimNum>1 )
							{
								// DEBUG CODE
								//Ok lets go thru the array args
								if(whtype==5) DimNum--;
								f = DimNum;
								for(e=0; e<f; e++)					
								{
									if( line[line_pos++] != '[' ) { compile_log_update(119, nil); return(-1);	}
									c = compile_argument( 'L' );	//Looking for long 111
									if( c<0 ) return -1;

									while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
									if( line[line_pos++] != ']' )
									{
										// We have an error, repeat Houston we have an error
								     	compile_log_update(114, nil);
								     	return(-1);
									}
									while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
								}
								if( line[line_pos] == '[' )
								{
									// We have an error, repeat Houston we have an error
							     	compile_log_update(118, nil);
							     	return(-1);
								}

							}						
						}
						break;
					case    'Q':    // Let Assignment  varnum|vartype|expression
						while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
						c = 0;  // Let's get the variable to assign
						while( isvalidvar(line[line_pos]) && line_pos <= line[0] )
						{
							temp2[c++] = line[line_pos++];
							if(c>40)
							{
								temp2[38] = '+'; temp2[39] = '+'; temp2[40] = '+';
								compile_log_update(127, (char*)temp2);
							  	return(-1); // Varname too long...}
							}
						}
						temp2[c]=0;
						DimNum=0;
						c = VarTableFind(temp2, 0);        //Find It
						if( c>=0 )
						{
							ip = (short*) (parsed_line + pline_pos);
							*ip = c;
							ip++;
							// DEBUG CODE printf("Var to be assigned %s\n",temp2);
							pline_pos+=2;
							//parseline[pline_pos++] = whtype;      //what type
							while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
							
							if( OneOfThem( line[line_pos], "%&!#$" ) ) line_pos++;
							bn = whtype;
							// Let's check for arrays							
							if( (DimNum==1 && whtype!=5) || DimNum>1 )
							{
								// DEBUG CODE
								//Ok lets go thru the array args
								if(whtype==5) DimNum--;
								f = DimNum;
								for(e=0; e<f; e++)					
								{
									if( line[line_pos++] != '[' ) { compile_log_update(119, nil); return(-1);	}
									c = compile_argument( 'L' );	//Looking for long 111
									if( c<0 ) return(-1);
									while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
									if( line[line_pos++] != ']' )
									{
										// We have an error, repeat Houston we have an error
								     	compile_log_update(114, nil);
								     	return(-1);
									}
									while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
								}
								if( line[line_pos] == '[' )
								{
									// We have an error, repeat Houston we have an error
							     	compile_log_update(118, nil);
							     	return(-1);
								}

							}						

							while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;

							if( line[line_pos++]!='=')              // Skip ='s
							{
							   	compile_log_update(120, nil);
							   	return(-1);
							}
							else
							{
								whtype = bn;
								if( whtype == 5 ) c = compile_argument( 'S');
								else
								{
									if( whtype< 3 ) c = compile_argument( 'L'); //whtype );
									else c = compile_argument( 'D'); //whtype );  // double
								}
								if( c<0 ) return -1;
							}
							e = 0;
						}
						else
						{
							compile_log_update(103, temp2);
							return(-1);

							//printf( "ERROR - Variable [%s] Not Found\n",temp2);
						}
						break;
					case 'V':    // Var Only
						//printf("Here\n"); getchar();
						while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
						c = 0;  // Let's get the variable to assign
						while( isvalidvar(line[line_pos]) && line_pos <= line[0] )
						{
							temp2[c++] = line[line_pos++];
							if(c>40)  return(-1); // Varname too long...}
						}
						temp2[c]=0;
						DimNum=0;
						
						c = OneOfThem( line[line_pos], "%&!#$" );
						if( c == 3 ) c = 4;  // New float to double conversion
						if( c ) line_pos++;  // Type Decl, so skip it

						c = VarTableFind(temp2, 0);        //Find It
						//printf("VR = %i\n", c); getchar();
						if( c>=0 )
						{
							ip = (short*) (parsed_line + pline_pos);
							*ip = c;
							ip++;
							pline_pos+=2;
							while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;

							if( (DimNum==1 && whtype!=5) || DimNum>1 )
							{
								if(whtype==5) DimNum--;
								//Ok lets go thru the array args
								for(f=0; f<DimNum; f++)					
								{
									if( line[line_pos++] != '[' )
									{
								     	compile_log_update(114, nil);
								     	return(-1);
									}
									c = compile_argument( 'L' );	//Looking for long 111
									if( c<0 ) return -1;
									while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
									if( line[line_pos++] != ']' )
									{
								     	compile_log_update(114, nil);
								     	return(-1);
									}
									while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
								}
							}						
						}
						else
						{
							compile_log_update(103, temp2);
							return(-1);
							//printf( "ERROR - Variable [%s] Not Found\n",temp2);
						}
						break;

					case 'F':       // For cmd,  so skip reserved space next location 4 bytes
						// DEBUG CODE printf("For Spot is %li\n", GlobalData.CodeCtr);
						parsed_line[pline_pos++]=0;
						parsed_line[pline_pos++]=0;
						parsed_line[pline_pos++]=0;
						parsed_line[pline_pos++]=0;
						LoopStackPtr++;
						CmpLoop[LoopStackPtr].location = GlobalData.CodeCtr;
						CmpLoop[LoopStackPtr].looptype = 1;
						break;
					case 'j':
						// Now reserve
						lp = (long*) (parsed_line + pline_pos);
						if( p2_startflag)
						{
							nl = RunHdr.StartVarSz;
							*lp = nl;
							//printf("START Just stored %li as offset FOR\n", nl);
							//RunHdr.StartVarSz+=8; // 1-26-97
							RunHdr.StartVarSz+=16;
						}
						else
						{
							nl = GetCallVMS( current_module_num );
							*lp = nl;
							//printf("Sub/Func Just stored %li as offset FOR CMN#%i\n", nl, current_module_num);
							// SetCallVMS( current_module_num, nl+8 ); 1-26-97
							SetCallVMS( current_module_num, nl+16 );
						}
						pline_pos+=4;
						break;
					case 'N':
						// DEBUG CODE printf("Next Spot is %li\n", GlobalData.CodeCtr);
						if( LoopStackPtr < 0 ) 
						{
							compile_log_update(128, nil);
							return(-1);
							//printf("Error - Next without a For\n");
						}

						if( CmpLoop[LoopStackPtr].looptype != 1)
						{
							compile_log_update(128, nil);
							return(-1);
							//printf("Error - Next without a For\n");
						}
						// Ok this is a next statement
						lp = (long*) (parsed_line + pline_pos);
						*lp = CmpLoop[LoopStackPtr].location;
						pline_pos+=4;
						UpdateCodePage( CmpLoop[LoopStackPtr].location+4, GlobalData.CodeCtr + 11);
						LoopStackPtr--;
						break;
					case 'W':       // Do cmd,  so skip reserved space next location 4 bytes
						pline_pos+=4;           
						while( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;
						
						c = 0;  // Let's get the while or until 
						while( line[line_pos]>='A' && line[line_pos]<='Z' && line_pos <= line[0] )	
							temp2[c++] = line[line_pos++];
						temp2[c]=0;
						if(!strcmp(temp2, "WHILE"))
						{						
							parsed_line[pline_pos++]=3;
							LoopStackPtr++;
							CmpLoop[LoopStackPtr].location = GlobalData.CodeCtr;
							CmpLoop[LoopStackPtr].looptype = kLoopTypeWhile;
						}	
						else if(!strcmp(temp2, "UNTIL"))
						{						
							parsed_line[pline_pos++]=4;
							LoopStackPtr++;
							CmpLoop[LoopStackPtr].location = GlobalData.CodeCtr;
							CmpLoop[LoopStackPtr].looptype = kLoopTypeUntil;
						}	
						else
						{
							compile_log_update(116, nil);
							return(-1);
						}
						break;
					case 'B':
						if( LoopStackPtr < 0 ) 
						{
							//printf("Error - Loop without a Do\n");
							compile_log_update(129, nil);
							return(-1);
						}

						if( CmpLoop[LoopStackPtr].looptype < 3 || CmpLoop[LoopStackPtr].looptype > 4)
						{
							//printf("Error - Loop without a Do\n");
							compile_log_update(129, nil);
							return(-1);
						}
						// Ok this is a loop statement
						lp = (long*) (parsed_line + pline_pos);
						*lp = CmpLoop[LoopStackPtr].location;
						pline_pos+=4;
						UpdateCodePage( CmpLoop[LoopStackPtr].location+4, GlobalData.CodeCtr + 9);
						LoopStackPtr--;
						break;
					case 'b':
						if( LoopStackPtr < 0 ) 
						{
							//printf("Error - ExitLoop without a Do\n");
							compile_log_update(162, nil);
							return(-1);
						}
						if( CmpLoop[LoopStackPtr].looptype < 3 || CmpLoop[LoopStackPtr].looptype > 4)
						{
							// OK find 
							c = LoopStackPtr;
							f = 0;
							while( c>=0 & !f )
							{
								if( CmpLoop[c].looptype ==3 || CmpLoop[c].looptype == 4) f = 1;
								else c--;
							}
							if( !f )
							{					
								compile_log_update(162, nil);
								return(-1);
							}
						}
						// Ok this is a loop statement
						lp = (long*) (parsed_line + pline_pos);
						*lp = CmpLoop[c].location;
						pline_pos+=4;
						break;
					case 'C':       // For IF cmd,  so skip reserved space next location 4 bytes
						parsed_line[pline_pos++] =0;
						parsed_line[pline_pos++] =0;
						parsed_line[pline_pos++] =0;
						parsed_line[pline_pos++] =0;
						//pline_pos+=4;           
						LoopStackPtr++;
						CmpLoop[LoopStackPtr].location = GlobalData.CodeCtr;
						CmpLoop[LoopStackPtr].looptype = 5;
						break;
					case 'e':	// Else
						if( LoopStackPtr < 0 ) 
						{
							compile_log_update(122, nil);
							return(-1);
							//printf("Error - ELSE without an IF\n");
						}

						if( CmpLoop[LoopStackPtr].looptype != kLoopUntil)
						{
							compile_log_update(122, nil);
							return(-1);
							//printf("Error - ELSE without an IF\n");
						}
						// Ok this is a loop statement
						lp = (long*) (parsed_line + pline_pos);
						*lp = CmpLoop[LoopStackPtr].location;
						pline_pos+=4;
						UpdateCodePage( CmpLoop[LoopStackPtr].location+4, GlobalData.CodeCtr + 9);
						CmpLoop[LoopStackPtr].location = GlobalData.CodeCtr;
						SingleLineIfFlag=false;
						// Leave LoopStackCtr as is so to wait for ENDIF
						break;
					case 'f':
						if( LoopStackPtr < 0 ) 
						{
							compile_log_update(123, nil);
							return(-1);
							//printf("Error - ENDIF without an IF\n");
						}

						if( CmpLoop[LoopStackPtr].looptype !=5)
						{
							compile_log_update(123, nil);
							return(-1);
							//printf("Error - ENDIF without an IF\n");
						}
						// Ok this is a endif statement
						UpdateCodePage( CmpLoop[LoopStackPtr].location+4, GlobalData.CodeCtr + 5);
						LoopStackPtr--;
						SingleLineIfFlag=false;
						break;
					case 'g':  // Select Cmd Special
						// This means a new Select Statement	
						SelectCtr++;	// Increment Select #
							
						// First let's find a spot & be sure the nesting is <30
						f = -1;
						for(c=0;c<30;c++) if( !SelectArray[c]) { f = c; break;}
						if( f == -1) 
						{
							// Error nesting overflow
							compile_log_update(130, nil);
							return(-1);
						}	
						else
						{
							SelectArray[f] = SelectCtr;  // Add to active que
							SelectCurr = SelectCtr;		//Set current ptr
							ip = (short*) (parsed_line + pline_pos);  
							*ip = SelectCtr;  // short This Case #
							pline_pos+=2;
							lp = (long*) (parsed_line + pline_pos);
							*lp = GlobalData.NumOfSelects;  // long Starting Location of select
							pline_pos+=4;
							c = compile_argument( par[b] );  //Compile the argument
							if( c == -1 ) { compile_log_update(102, nil); return -1; }
							else
							{
								SelectCurrType = 1;
								if( ArgTypeChecker == 3) SelectCurrType = 3; // String
								SelectTypeArray[f] = SelectCurrType;
												//status, case#, value
								AddToSelectTable( SelectCurrType, SelectCtr, 0 );
							}
						}
						break;
					case 'h':  // Case Cmd Special
					  // This means a new case Statement
					  if( SelectCurr )
					  {
						ip = (short*) (parsed_line + pline_pos);  
						*ip = SelectCtr;  // short This Case #
						pline_pos+=2;
						f = pline_pos;
						c = compile_argument( 'E' );
						if( c<0 ) return -1;
						parsed_line[pline_pos] = 253;  // Add end of cmd line
						if( c == -1 ) {compile_log_update(102, nil); return(-1);}
						else
						{
						  if( f == pline_pos )
						  {
								if( SelectCurrType == 1 ) compile_log_update(110, nil);
								else compile_log_update(111, nil);
						  }
						  else
						  {
							c = 0;
							nl = 0;
							// Now let's check to see if this is a proper case
							if( parsed_line[f] == 251 ||parsed_line[f] == 250) // Long or FP
							{
								switch( parsed_line[f+1] )
								{
									case 2: //Long Constant
										if( parsed_line[f+6] == 253 )
										{
											c=12;
											lp = (long*) (parsed_line + f + 2 );  
											nl = *lp;
										}
										break;
									case 3: //FP constants
									case 4:
										compile_log_update(112, nil);  //Fp not allowed
										c = -1;
										break;
									case 11:	//int Var
										if( parsed_line[f+4] == 253 )
										{
											ip = (short*) (parsed_line + f +2);  
											c = *ip;
											nl = (long) c;
											c=14;
										}
										break;	
									case 12:	//Long Var
										if( parsed_line[f+4] == 253 )
										{
											ip = (short*) (parsed_line + f +2);  
											c = *ip;
											nl = (long) c;
											c=13;
										}
								}
								if( !c ) { compile_log_update(113, nil); return(-1);}
							}
							else
							{
								// String then
								c=0;
								switch( parsed_line[f+1] )
								{
									case 7: //String Constant
										if( parsed_line[f+6] == 253 )
										{
											c=15;
											lp = (long*) (parsed_line + f + 2);  
											nl = *lp;
											//printf("string const added %li\n", GlobalData.NumOfSelects);
										}
										break;
									case 15: // String Var
										if( parsed_line[f+4] == 253 )
										{
											ip = (short*) (parsed_line + f + 2);  
											c = *ip;
											nl = (long) c;
											c=16;
											//printf("string var added %li\n", GlobalData.NumOfSelects);
										}
										break;	
								}
								if( !c ) compile_log_update(113, nil);
						  	} 
							if( c > 0)
							{
						 		pline_pos = f;		// Don't need argument
								CCT = GlobalData.CodeCtr;
								GlobalData.CodeCtr += pline_pos+1;
								AddToSelectTable( c, SelectCtr, nl );  // Ok add to table
								GlobalData.CodeCtr = CCT;
						 		//printf("Added value was %li\n", nl);
						 	}
						  }
						}
					  }
					  break;
					case 'i':  // EndCase Cmd Special
						// This means a end of a Select Statement	
						if( SelectCurr)
						{
							//printf( "EndCase\n");
							ip = (short*) (parsed_line + pline_pos);  
							*ip = SelectCtr;  // short This Case #
							pline_pos+=2;
							CCT = GlobalData.CodeCtr;
							GlobalData.CodeCtr += pline_pos+1;
							AddToSelectTable( 99, SelectCtr, 0 );
							GlobalData.CodeCtr = CCT;
							f = -1;
							for(c=0;c<30;c++) if( SelectArray[c] == SelectCurr) { f = c; break;}
							if( f > 0)
							{
								SelectCurr = SelectArray[ f -1];
								SelectCurrType = SelectTypeArray[ f-1 ];
							}
							else
							{
								SelectCurr = 0;
								SelectCurrType = 0;
							}
							
							if( f >= 0 )
							{
								
							 	SelectArray[ f ] = 0;
							 	SelectTypeArray[ f ] = 0;
							}
						}
						break;
					case 'k':  // Default Cmd Special
						// This means a end of a Select Statement	
						if( SelectCurr)
						{
							ip = (short*) (parsed_line + pline_pos);  
							*ip = SelectCtr;  // short This Case #
							pline_pos+=2;
							CCT = GlobalData.CodeCtr;
							GlobalData.CodeCtr += pline_pos+1;
							AddToSelectTable( 50, SelectCtr, 0 );
							GlobalData.CodeCtr = CCT;
						}
						break;
					default:
							printf("Unknown %c\n", par[b] );
						break;
							
							
				}
				b++;
				if( line[line_pos] == ',' ) line_pos++;
			}
					//}
			while( line[line_pos]==' ' && line_pos <= a ) line_pos++;
	
			if( line_pos > a || line[line_pos]==':' || SingleLineIfFlag)
			{
				ip = (short*) (parsed_line);
				*ip = pline_pos;        // Set line len
				// DEBUG CODE ##################################################
	////printf("Line - %s [%li]\n", line+1, line_ctr );
				
				c = WriteParseLine();
				
	////for(c=0; c< pline_pos;c++) printf("[%i]",parsed_line[c]);
	////printf("\n\n");  // code print

				if(line_pos > a && SingleLineIfFlag)  // This is for same line then
				{
					UpdateCodePage( CmpLoop[LoopStackPtr].location+4, GlobalData.CodeCtr); //bobby+ 5);
					LoopStackPtr--;
					SingleLineIfFlag=false;
				}
				
				if( line_pos <= a && ( line[line_pos]==':' || SingleLineIfFlag))
				{
					if(line[line_pos]==':') line_pos++; //Skip colon
					ip = (short*) (parsed_line);
					*ip = 0;                        // Set line len to 0
					pline_pos=2;                    //Update pos. ptr
				}
				else
				{
				 	if(!SingleLineIfFlag) return(0); 
				}
			}
			else { compile_log_update(131, nil); return(-1);} // Syntax Error
		}
	}

	return(0);
}

short p2_skip_global()
{
	// This function scans thru global module
	// because all dim statements were processed on Pass1
	short a;
			
	//printf("Debug - scanning thru global\n");
	while(1)
	{
		a = next_line();
		if(a) return(1);
		if( !strcmp( (char*)line+1, "ENDGLOBAL" ))
		{
		 //printf("DEBUG - ENDGLOBAL FOUND\n");
		 return(0); // Done     
		}
	}
	return(0);
}

short p2_compile_module( short modtype)
{
		// This function scans thru global module
	// because all dim statements were processed on Pass1
	short a;
	
	// Check for START
	if( !modtype )  // START?
	{
		RunHdr.CodeStart = GlobalData.CodeCtr;
		p2_startflag =1;
		//printf("Debug - CodeStart set to %i\n", (short) RunHdr.CodeStart);
	}
	
			
	LoopStackPtr = -1;      //Reset
	//printf("Debug - scanning thru module\n");
	while(1)
	{
		a = next_line();
		//printf("[%s]\n", line+1 );
		/*if(a)	// Checking for nesting error
		{
			if( LoopStackPtr != -1 ) {  DoLoopStackReport(); return(1); }
		}*/
		if( !modtype )  // START?
		{
				
			if( !strcmp( (char*)line+1, "FINISH" ))
			{
				p2_startflag = 0;
				// Add END command
				parsed_line[0] =0;
				parsed_line[1] =5;
				parsed_line[2] =0;
				parsed_line[3] =103;
				parsed_line[4] =253;
				pline_pos = 4;
				//printf("Writing EXTRA END \n" );
				a = WriteParseLine();
				//printf("Finish LSP = %i\n", LoopStackPtr );
				if( LoopStackPtr != -1 ) {  DoLoopStackReport(); return(1); }
				
				//printf("DEBUG - FINISH of Module FOUND\n");
				return(0); // Done      
			}
			else
		    {
				// DEBUG CODE printf("line compile\n");
				p2_line_compile();
			}
		}
		else
		{
			//line[4]=0;
			if( !strncmp( (char*)line+1, "END" , 3 ) )
			{
				if( !strcmp( (char*)line+1, "ENDSUB" ))
				{
					// Add Return command
					parsed_line[0] =0;
					parsed_line[1] =5;
					parsed_line[2] =0;
					parsed_line[3] =116;
					parsed_line[4] =253;
					pline_pos = 4;
					//printf("Writing EXTRA ExitSub \n" );
					a = WriteParseLine();
					//printf("EndSub LSP = %i\n", LoopStackPtr );
					if( LoopStackPtr != -1 ) {  DoLoopStackReport(); return(1); }
					//printf("DEBUG - ENDSUB of Module FOUND\n");
					return(0); // Done      
				}
				else if( !strcmp( (char*)line+1, "ENDFUNCTION" ))
				{
					// Add Return command
					parsed_line[0] =0;
					parsed_line[1] =5;
					parsed_line[2] =0;
					parsed_line[3] =117;
					parsed_line[4] =253;
					pline_pos = 4;
					//printf("Writing EXTRA ExitFunction \n" );
					a = WriteParseLine();
					//printf("ExitF LSP = %i\n", LoopStackPtr );
					if( LoopStackPtr != -1 ) {  DoLoopStackReport(); return(1); }
					return(0); // Done      
				}
				else
				{
					//printf("DEBUG - END of Module FOUND\n");
					//printf("line compile\n");
					p2_line_compile();
					//return(0); // Done      
				}
			}
			else
			{
				//printf("line compile\n");
				p2_line_compile();
			}
		}
		
	}
	return(0);
}

short pass2_compile_file()
{
	short a=0, b, c;
	char cmdstr[15];
	
	line_ctr = 0;
	srcFileType = PjtFileList[pjt_ctr-1].tp;
	buf_pos = PjtFileList[pjt_ctr-1].codeStart;  // shortcut for pass 2
	line_ctr = PjtFileList[pjt_ctr-1].lineCtr;     // shortcut for pass 2

	while(!a)
	{
		b = next_line();
		//printf("[%s]\n",line+1);
		if(!b)
		{
			// ("Looking for module\n");
			// Check line should be a module ident.
			c = 0;
			b = 1;
			while(b <= line[0] && c < 13)
			{
				if( line[b] != ' ')
				{
					cmdstr[c++] = line[b++];
				}
				else b=99;
			}
			cmdstr[c]=0;

			if(c)
			{
				//printf("Looking for [%s][%i]\n",cmdstr,b);
				c=-1;
				for(b=0; b<4; b++) if( !strcmp( cmdstr, imod[b] ))  { c=b; b=99;}
				//printf("{{%i}}\n",c);
				switch(c)
				{
					case 0: //Start
					 	ProcLineCtr=line_ctr;
					  	strcpy( (char *) CurrProc, (char *) line+1);
						VarSzCtr=0;
						p2_compile_module( c );
						//current_module_num++;
						break;
					case 1:	//Sub
					case 2: //Function
					 	ProcLineCtr=line_ctr;
					  	strcpy( (char *) CurrProc, (char *) line+1);
						VarSzCtr=0;
						SetCallLocation( current_module_num, GlobalData.CodeCtr );
						p2_compile_module( c );
						current_module_num++;
						break;
					case 3:         // Global, Processed in Pass so skip module
					 	ProcLineCtr=line_ctr;
					  	strcpy( (char *) CurrProc, (char *) line+1);
						b = p2_skip_global();
						if( b ) return(0);  //End of file                                               
						break;
					default:
						//ERROR! - Pass 1 let something bad slide thru
						//printf("Baddy slid thru %s %i/n",cmdstr,c);
						//SysBeep(30);
						compile_log_update(131, nil);
						return(-1);
						break;
				}                               
			}
			else
			{
				//ERROR! - Pass 1 let something bad slide thru
				return(-1);
			}       
		}
		else
		{
			//printf("Next_Line Done\n");
			 a++;
		}
	}
	return(0);
	
}

void DoLoopStackReport()
{
	short 	a;
	for( a=0; a<=LoopStackPtr; a++)
	{
		switch(	CmpLoop[a].looptype )
		{
			case 1:	// FOR
				compile_log_update(133, nil);
				break;
			case 3:	// DO While
				compile_log_update(134, nil);
				break;
			case 4:	// DO Until
				compile_log_update(135, nil);
				break;
			case 5:	// IF
				//compile_logfile_update("ERROR -\x0d");  
				compile_log_update(136, nil);
				break;
		}
	}
}
