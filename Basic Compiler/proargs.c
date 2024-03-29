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

#define         UCHAR   unsigned char

#define         ARGSTART        251

#define         ARGSTOP         252



extern unsigned char parsed_line[1024], line[260], label_flag;

extern short line_pos,pline_pos,line_len;

extern long source_len, buf_pos, line_ctr;

extern short 
htype;

extern UCHAR ParList[12], DimNum;

extern short ArgTypeChecker;

extern short ReturnType;



short compile_log_update(short errno, char *special);

short CallTableFind( char * CmdText, short CallType );

short isvalidvar(short ch);

short OneOfThem( short ch, char *src );

short VarTableFind( char * varname, short vartype);

short GetCallPars( short d );

void COD( UCHAR *msg );

short ObjectTableFind( Str255 objname, short objtype );

short FormTableFind( Str255 formname );



short compile_argument( short argtype );

long get_longarg_val();

short isvalidvar(short ch);

short isnumber(short ch);

void compile_error(void);

short StoreStrCon(unsigned char * st1);

short compile_logfile_update( char * txt);



extern struct  fun_list

{

	short	num;

	char    name[20];

	char    params[10];

	char 	ret;		//#11 is return (actually10)

}       func_list[150];





short compile_argument( short argtype )

{

	char first=0,temp[258],temp_ctr,fp_flag,looper,ch, par[12], singleFlag=0, doubleFlag=0;

	short *ip, last_type=0, beginArg, LArgTypeChecker, compareFlag=0, overFP=0, funcFlag=0, dims, cb, parCtr=0,m,n;

	long *lp,lv;

	float *fp,fv;

	short double	*dp, dv;

	short a, b, c, d, e, f, LL, array_flag,tres;

	Str255	temp3;

	char temp2[45];

	

	if( argtype == 'U' ) argtype= 'L';

	else if( argtype == 'X' ) argtype= 'D';



	if( argtype == 'L'+100 || argtype == 'D'+100 || argtype == 'S'+100 ||  argtype == 'a'+100) { argtype-=100; funcFlag++; }

	//if( argtype >=100 ) { argtype-=100; funcFlag++; }

	LArgTypeChecker = 0;  // Default=0 : 1=Long, 2=FP, 3=String

	array_flag=-1;

	LL = line[0]+1;

	
hile(line_pos<LL  && line[line_pos]==' ') line_pos++;

	if(line_pos>=LL) return(-2);



	// Let's check for 'nothingness'



	beginArg = pline_pos;	// store the start pos

	if( argtype == 'S') parsed_line[pline_pos++] = 249;   // Designates a string arg

	else if( argtype == 'D') parsed_line[pline_pos++] = 250;   // Designates a double

	else parsed_line[pline_pos++] = ARGSTART;  // else presume long



	// Don't forget to uppercase

	looper=0;

	
hile(!looper && line_pos<LL)

	{

		if(line[line_pos]>='A' && line[line_pos]<='Z')  // Variable or Function

		{

			tres = 0;

			//if( type_flag > 777 )

			

			// Check for THEN, TO, STEP, or ELSE

			if( OneOfThem( line[line_pos], "TESAON" ) )

			{

				a = line_pos;

				b = 0;

				
hile(line[a]>='A' && line[a]<='Z') temp[b++] = line[a++];

				temp[b]=0;

				if( line[a] !='(' )

				{

					if( !strcmp(temp,"TO") || !strcmp(temp,"THEN") || !strcmp(temp,"STEP") || !strcmp(temp,"ELSE" ))

					{

						parsed_line[pline_pos++] = 252;

						//if( parCtr > 0  ) { compile_log_update(160, nil); return(-1); }

						//if( parCtr < 0  ) { compile_log_update(161, nil);  return(-1); }

						return(0);     

					}

					if( !strcmp(temp,"AND") )

					{

						// Insert AND into code

						parsed_line[pline_pos++] = 46;

						last_type = 0;  // Refresh 

						line_pos = a;

						tres++;

					}

					if( !strcmp(temp,"OR") )

					{

						// Insert OR into code

						parsed_line[pline_pos++] = 47;

						last_type = 0;  // Refresh 

						line_pos = a;

						tres++;

					}

					if( !strcmp(temp,"NOT") )

					{

						// Insert NOT into code

						parsed_line[pline_pos++] = 48;

						line_pos = a;

						tres++;

					}

				}

			}

			if(!tres)

			{

				temp_ctr=0;

				
hile( isvalidvar( line[line_pos]) ) temp[temp_ctr++] = line[line_pos++];

				temp[temp_ctr]=0; //Zero Term. String

			

				
hile( line[line_pos] ==' ' && line_pos<LL ) line_pos++;  // Skip extra spaces

				

				lv = 0;

				if( temp[0] == 'P' )

				{

					if( temp[1] == 'R' && temp[2] == 'N' && !temp[3] )

					{

						parsed_line[pline_pos++] = 2;                                           //Indicate Long Constant

						lp = (long*) (parsed_line+pline_pos);                   //Store 4 Byte Long Value

						lv = 30000;

						*lp = lv;                                                                               //No
 Store it

						pline_pos+=4;                                                                   //Update pos. ptr

						last_type = 0x10;

					}

				}

				if( temp[0] == 'T' )

				{

					if( temp[1] == 'R' && temp[2] == 'U' && temp[3] == 'E' && !temp[4] )

					{

						parsed_line[pline_pos++] = 2;                                           //Indicate Long Constant

						lp = (long*) (parsed_line+pline_pos);                   //Store 4 Byte Long Value

						lv = 1;

						*lp = lv;                                                                               //No
 Store it

						pline_pos+=4;                                                                   //Update pos. ptr

						last_type = 0x10;

					}

				}

				if( temp[0] == 'F' )

				{

					if( temp[1] == 'A' && temp[2] == 'L' && temp[3] == 'S' && temp[4] == 'E' && !temp[5] )

					{

						parsed_line[pline_pos++] = 2;                                           //Indicate Long Constant

						lp = (long*) (parsed_line+pline_pos);                   //Store 4 Byte Long Value

						lv = 0;

						*lp = lv;                                                                               //No
 Store it

						pline_pos+=4;  

						lv = 555;                                                                 //Update pos. ptr

						last_type = 0x10;

					}

				}

			   if(!lv)

			   {

				if( line[line_pos] =='(' ) // Check for function

				{

					// DEBUG CODE	printf("Yes it's a function\n");

					// Ok it's a function call

					a = CallTableFind( (char *) temp, 2 );

					if( a<0 )

					{

						// Function call name not found, generate error

						//printf("ERROR - Function [%s] not found.", temp);

				     	compile_log_update(104, temp);

				     	return(-1);



					}

					else

					{

						// Ok function name found

						// DEBUG CODE	printf("function processing %i-%i\n", LArgTypeChecker, 
htype );

						cb = 
htype;

						parsed_line[pline_pos++] = 19;		// function

						ip = (short*) (parsed_line+pline_pos);

						*ip = a;                    //No
 Store func # 2 bytes

						pline_pos+=2;               //Update pos. ptr

						

						//Ok no
 process args (if any)

						if( a < 1000 ) strcpy( par, func_list[a-100].params );

						else 

						{

							b = GetCallPars( a );

							if(!b) strcpy( par, (char*) ParList );  // Temp 

							else

							{

								//printf("Error - Getting par list\n");

								strcpy( par, "Z");

							}       

						}

						line_pos++;	// Skip past (

						parCtr++;

						b=0;

						e=0;

						
hile(!e)

						{

							s
itch( par[b] )

							{

								case    'Z':

								case      0:                                    

									// All Done

									e++;

									break;

								case    '
':  // form id's

								case    'c':  // objects

									if(line[line_pos]==',') line_pos++;

									c=0;

									
hile( line[line_pos]==' ' && line_pos <= line[0] )line_pos++; // skip spaces

									if( isalpha( line[line_pos] ) )

									{

										m = line_pos;	f = 1;

										
hile( ( isvalidvar(line[m]) || line[m] =='.') && m <= line[0] )

											temp3[f++] = line[m++];

										temp3[f]=0; temp3[0] = f-1;

										if( par[b] == '
' ) n = FormTableFind( temp3 );

										else n = ObjectTableFind( temp3, 1 );

										if( n>=0 )

										{

											line_pos = m;

											
hile( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++; // skip spaces

											if(line[line_pos]==')') line_pos++;

											parsed_line[pline_pos++] = 2; // ID as ctl

											ip = (short*) (parsed_line + pline_pos);

											*ip = n;

											pline_pos+=2;

											c++;

											//parsed_line[pline_pos++] = 252;

										}

									}

									if( !c )

									{

										parsed_line[pline_pos++] = 1; // argument

										c = compile_argument( 'L'+100 );

										if(c <0) compile_log_update(117, nil);

										if(line[line_pos-1]==')') parCtr--;

									}

									break;

								case	'U': // Integer

									if(line[line_pos]==',') line_pos++;

									c = compile_argument( 'L'+100 );

									if(c <0) compile_log_update(117, nil);

									if(line[line_pos-1]==')') parCtr--;

									break;

								case    'X':  //Single

									if(line[line_pos]==',') line_pos++;

									c = compile_argument( 'D'+100 );

									if(c <0) compile_log_update(117, nil);

									if(line[line_pos-1]==')') parCtr--;

									break;

								case	'S':

								case    'L':

								case    'D':

									if(line[line_pos]==',') line_pos++;

									c = compile_argument( par[b]+100 );

									if(c <0) compile_log_update(117, nil);

									if(line[line_pos-1]==')') parCtr--;

									break;

								case    'l':

									if(line[line_pos]==',' && c!=')')

									{

										line_pos++;

										c = compile_argument( 'L'+100 );

										if(c <0) compile_log_update(117, nil);

										if(line[line_pos-1]==')') parCtr--;

									}

									break;

								case 'V':    // Var Only

									//printf("Here\n"); getchar();

									
hile( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;

									c = 0;  // Let's get the variable to assign

									
hile( isvalidvar(line[line_pos]) && line_pos <= line[0] )

									{

										temp2[c++] = line[line_pos++];

										if(c>40)  return(-1); // Varname too long...}

									}

									temp2[c]=0;

									DimNum=0;

									

									c = OneOfThem( line[line_pos], "%&!#$" );

									if( c == 3 ) c = 4;  // Ne
 float to double conversion

									if( c ) line_pos++;  // Type Decl, so skip it

			

									c = VarTableFind(temp2, 0);        //Find It

									//printf("VR = %i\n", c); getchar();

									if( c>=0 )

									{

										ip = (short*) (parsed_line + pline_pos);

										*ip = c;

										ip++;

										pline_pos+=2;

										
hile( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;

			

										if( (DimNum==1 && 
htype!=5) || DimNum>1 )

										{

											if(
htype==5) DimNum--;

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

												
hile( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;

												if( line[line_pos++] != ']' )

												{

											     	compile_log_update(114, nil);

											     	return(-1);

												}

												
hile( line[line_pos]==' ' && line_pos <= line[0] ) line_pos++;

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



							}

							b++;

						}

						
htype = cb;  // reset it back

						if(!LArgTypeChecker)

						{

							s
itch(
htype)

							{

								case 1:

								case 2:

									if( argtype == 'S' ) {compile_log_update(110, nil); return(-1); }

									LArgTypeChecker = 1;

									break;

								case 3:

								case 4:

									if( argtype == 'S' ) {compile_log_update(110, nil); return(-1); }

									LArgTypeChecker = 2;

									break;

								case 5:

									if( argtype == 'L' || argtype == 'D'  )

									{

										
hile( line_pos < LL && line[line_pos] == ' ' ) line_pos++;

										if(compareFlag || OneOfThem( line[line_pos], "=<>" ) ) compareFlag = 1;

										else

										{

											//for( c = -4; c<=0; c++) printf("%c", line[line_pos+c]);

											//printf(">>> %i %c\n", LArgTypeChecker, argtype); getchar();

											compile_log_update(111, nil); return(-1); }

									}

									else

									{

										LArgTypeChecker = 3;

										parsed_line[beginArg] = 249;

									}

									break;

							}

						}



					}

				}

				else

				{

					// Ok it's a variable (of some type)

					ch = OneOfThem( line[line_pos], "%&!#$" );

					if( ch == 3 ) ch = 4;  // Ne
 float to double conversion

					if( ch ) line_pos++;  // Type Decl, so skip it

					a = VarTableFind( (char *) temp, ch );

					if( a== -1)

					{

						compile_log_update(103, (char *) temp); return(-1);

						//printf("ERROR - Variable [%s] not found\n", temp);

					}

					else

					{

						parsed_line[pline_pos++] = 
htype+10;

						ip = (short*) (parsed_line+pline_pos);                   

						*ip = a;                                                                               //No
 Store it

						pline_pos+=2;             //Update pos. ptr

						// DEBUG CODE printf("VTF = %i and Type=%i\n", a, 
htype);

						if( 
htype< 5 ) last_type = 0x10;

						else last_type = 0x20;  // String 

						if( 
htype == 3 || 
htype == 4)  // Check for fp type vars

						{

							overFP++;

							if( parsed_line[beginArg] == 251) parsed_line[beginArg] = 250;                                                

						}

						// Let's check to see if this is an array

						if( DimNum>1 || (DimNum==1 && 
htype!=5)  )

						{

							// Yes it is

							dims = DimNum;

							if(
htype == 5) dims--;

							for( e=0; e<dims; e++ )

							{

								if(line[line_pos]=='[')

								{

									line_pos++;

									f = 
htype;

									c = compile_argument( 'L' );  //111

									
htype = f;

									if(line[line_pos]==']') line_pos++;

								}

								else

								{

									// 
e have an error, repeat Houston 
e have an error

							     	compile_log_update(114, nil);

							     	return(-1);

								}

							}

						}

						if(line[line_pos]=='[')

						{

							// 
e have too many elements

					     	compile_log_update(118, nil);

					     	line_pos = LL;

					     	return(-1);

						}



						s
itch(
htype)

						{

							case 1:

							case 2:

								if( argtype == 'S' ) {compile_log_update(110, nil); return(-1); }

								LArgTypeChecker = 1;

								break;

							case 3:

							case 4:

								if( argtype == 'S' ) {compile_log_update(110, nil); return(-1); }

								LArgTypeChecker = 2;

								break;

							case 5:

								if( argtype == 'L' || argtype == 'D' )

								{

									
hile( line_pos < LL && line[line_pos] == ' ' ) line_pos++;

									if(compareFlag || OneOfThem( line[line_pos], "=<>" ) ) compareFlag = 1;

									else { compile_log_update(110, nil);  return(-1); }

								}

								else

								{

									LArgTypeChecker = 3;

									parsed_line[beginArg] = 249;

								}

								break;

						}



					}

				}

			   }   // part of !lv              

				//type_flag=1;  // Expecting op no


			}

		}

		else

		{       // #, operator, string constant 

		  s
itch(line[line_pos])

		  {

			case      0:    //Line Term

				parsed_line[pline_pos++] = 253;

				//printf("\nDoing line term\n");

				ArgTypeChecker = LArgTypeChecker;

				//if( parCtr > 0  ) { compile_log_update(160, nil); return(-1); }

				//if( parCtr < 0  ) { compile_log_update(161, nil);  return(-1); }

				return(0);

				break;

			case    ':':

			case    ',':

			case    ';':

				// End of argument

				parsed_line[pline_pos++] = ARGSTOP;

				ArgTypeChecker = LArgTypeChecker;

				//if( parCtr > 0  ) { compile_log_update(160, nil); return(-1); }

				//if( parCtr < 0  ) { compile_log_update(161, nil);  return(-1); }

				return(0);

				break;

			case    '0':    // Numbers & decimal point

			case    '1':

			case    '2':

			case    '3':

			case    '4':

			case    '5':

			case    '6':

			case    '7':

			case    '8':

			case    '9':

			case    '.':    // Convert # to constant code

				if( last_type == 0x10 ) return(-1); // Syntax Error

				temp_ctr=0;

				fp_flag=0;

				
hile(isnumber(line[line_pos]))

				{

					if(line[line_pos]=='.') fp_flag++;

					if(fp_flag>1) compile_error();  //Too many decimal points, so call error

					temp[temp_ctr++]=line[line_pos++];

				}

				temp[temp_ctr]=0; //Zero Term. String

				if( OneOfThem( last_type, "/x10/x20" )) { compile_log_update(102, temp); return(-1);} // Syntax Error

				if( !LArgTypeChecker && argtype == 'S' ) { compile_log_update(111, nil);  return(-1); }

				if(fp_flag)

				{

					overFP++;

					//This is a floating point #, mean double constant

					if(!LArgTypeChecker) LArgTypeChecker = 2;

					dv = (short double) atof(temp);

					//dv=11.123456;

					//printf(">> %13.13f\n", dv);

					parsed_line[pline_pos++] = 4;           			//Indicate Double Constant

					dp = (short double*) (parsed_line+pline_pos);                  //Store 8 Byte Dbl Value

					*dp = dv;                                                                               //No
 Store it

					//printf("=> %15.13g\n", *dp);

					pline_pos+=8;

				    if( parsed_line[beginArg] == 251) parsed_line[beginArg] = 250;                                                

				}

				else

				{

					//  This is either an int or a long, since all ops are performed as longs

					//to avoid overflo
, int's & longs are stored as longs (4 bytes)

					if(!LArgTypeChecker) LArgTypeChecker = 1;

					lv=atol(temp);

					parsed_line[pline_pos++] = 2;                                           //Indicate Long Constant

					lp = (long*) (parsed_line+pline_pos);                   //Store 4 Byte Long Value

					*lp = lv;                                                                               //No
 Store it

					pline_pos+=4;                                                                   //Update pos. ptr

				}

				last_type = 0x10;

				//type_flag = 1;  // No
 expect op

				break;

			case    '-':    // Could be subtraction or negation

				parsed_line[pline_pos++] = 21;

				line_pos++;

				last_type = 4;

				break;          

			case    '+':    // Addition

				parsed_line[pline_pos++] = 20;

				line_pos++;

				last_type = 3;

				break;

			case    '*':    // Multiplication

				if( OneOfThem( last_type, "/x05/x20" )) return(-1); // Syntax Error

				parsed_line[pline_pos++] = 25;

				line_pos++;

				last_type = 5;

				break;

			/*case    '%':    // Modula Division

				if( OneOfThem( last_type, "/x05/x20" )) return(-1); // Syntax Error

				parsed_line[pline_pos++] = 27;

				line_pos++;

				last_type = 5;

				break;*/

			case    '=':

				parsed_line[pline_pos++] = 40;

				line_pos++;

				if( OneOfThem( last_type, "/x20/x21" ))	last_type =0x25; // str comp

				else last_type = 8; // Numeric comp

				compareFlag++;

				if( parsed_line[beginArg] == 249 )

				{

					if(!overFP) parsed_line[beginArg] = 251;

					else parsed_line[beginArg] = 250;

				}

				break;

			case    '<':

				s
itch( line[line_pos+1] )

				{

					case '>':

					 parsed_line[pline_pos++] = 41;

					 line_pos+=2;

					 break;

					case '=':

					 parsed_line[pline_pos++] = 43;

					 line_pos+=2;

					 break;

					default:

					 parsed_line[pline_pos++] = 42;

					 line_pos++;

					 break;

				}

				if( OneOfThem( last_type, "/x20/x21" ))	last_type =0x25; // str comp

				else last_type = 8; // Numeric comp

				compareFlag++;

				if( parsed_line[beginArg] == 249 )

				{

					if(!overFP) parsed_line[beginArg] = 251;

					else parsed_line[beginArg] = 250;

				}

				break;

			case    '>':

				if( line[line_pos+1] == '=' )

				{

					parsed_line[pline_pos++] = 45;

					line_pos+=2;

				}

				else

				{

				  	parsed_line[pline_pos++] = 44;

				  	line_pos++;

				}

				if( OneOfThem( last_type, "/x20/x21" ))	last_type =0x25; // str comp

				else last_type = 8; // Numeric comp

				compareFlag++;

				if( parsed_line[beginArg] == 249 )

				{

					if(!overFP) parsed_line[beginArg] = 251;

					else parsed_line[beginArg] = 250;

				}

				break;

			case    '(':

				parsed_line[pline_pos++] = 36; //'(';

				line_pos++;

				parCtr++;

				break;

			case    ')':

				parsed_line[pline_pos++] = 37; //')';

				line_pos++;

				if( funcFlag)

				{

					//if( parCtr > 0  ) { compile_log_update(160, nil); return(-1); }

					//if( parCtr < 0  ) { compile_log_update(161, nil);  return(-1); }

					return(')');

				}

				parCtr--;

				break;

			case    '[':

				// Need to check some var status to confirm an array and ho


				// many dimensions

				parsed_line[pline_pos++] = 251; //'[';

				line_pos++;

				break;

			case    ']':

				// Need to check some var status to confirm an array and ho


				// many dimensions

				//if( argtype == 111 )  // var in assign statement

				//{

				//	parsed_line[pline_pos++] = 252;

				//	return(0);

				//}

				parsed_line[pline_pos++] = 252;

				//if( argtype == 111 )

				//{

					//if( parCtr > 0  ) { compile_log_update(160, nil); return(-1); }

					//if( parCtr < 0  ) { compile_log_update(161, nil);  return(-1); }

				 	return(0);

				//}

				//line_pos++;  // Put here on purpose

				break;

			case    47:     // Both slash & backslash mean divide

			case    92:     // allo
s for less typos from users

				if( OneOfThem( last_type, "/x05/x20" )) return(-1); // Syntax Error

				parsed_line[pline_pos++] = 26;

				line_pos++;

				last_type = 5;

				break;

			case    '"':    // String Constant

				if( OneOfThem( last_type, "/x04/x05/x20" )) return(-1); // Syntax Error

				// Ok store string constant

				line_pos++;

				a=1;

				
hile( line_pos < LL && line[line_pos] != '"' )

					temp[a++] = line[line_pos++];

				temp[0] = a - 1;

				temp[a]=0; //Shouldn't need this, just for c debug

				line_pos++;

				
hile( line_pos < LL && line[line_pos] == ' ' ) line_pos++;

				if( LArgTypeChecker && ( argtype == 'L' || argtype == 'D' ))

				{

					if(compareFlag || OneOfThem( line[line_pos], "=<>" ) )

					{

						compareFlag=1;

					}

					else

					{

						temp[0] = '"'; temp[a]='"'; temp[a+1]=0;

						compile_log_update(110, nil ); //temp);

						return(-1);

					}

				}

				lv = StoreStrCon( (unsigned char *) temp );

				if( lv == -1 ) return( -1 );

				parsed_line[pline_pos++] = 7; //Token Code

				lp = (long*) (parsed_line+pline_pos);                   //Store 4 Byte Long Value

				*lp = lv;                                                                               //No
 Store it

				pline_pos+=4;                                                                   //Update pos. ptr

				last_type = 0x20;

				if(!compareFlag) parsed_line[beginArg] = 249; 

				if(!LArgTypeChecker && argtype != 'L' && argtype != 'D' ) LArgTypeChecker = 3;

				break;

			case    ' ':    // Spaces are skipped

				line_pos++;

				break;

			default:                // Character unkno
n so return means

				return(-1);

				break;

				

			

		  }

		}

	}

	ArgTypeChecker = LArgTypeChecker;

	//if( parCtr > 0  ) { compile_log_update(160, nil); return(-1); }

	//if( parCtr < 0  ) { compile_log_update(161, nil);  return(-1); }

	return(0);

}   



