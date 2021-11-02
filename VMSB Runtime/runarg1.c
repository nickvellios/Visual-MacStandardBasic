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
#define                 ARGSTART        251
#define                 ARGSTOP         252

extern short line_pos,pline_pos,line_len, CallStackPtr;
extern long source_len, buf_pos, line_ctr;

short Allocate_ReturnStr( long);
short FreeReturnStr();
short   ReturnStrStore( UCHAR * st1 );
void compile_argument();
long get_longarg_val();
short double get_doublearg_val();
short isvalidvar(short ch);
short isnumber(short ch);
void compile_error(void);
long grab_intvar(short varnum);
long grab_longvar(short varnum);
float grab_singlevar(short varnum);
short double grab_doublevar(short varnum);
short get_stringarg_val();
short get_function(short argtype);
short pass_assign( short vartype, short varnum);
short RunProgramLoop();
long longArg();
float singleArg();
short double doubleArg();
long grab_varaddress(short varnum);
short OneOfThem( short ch, unsigned char *src );
long ArrayCalc( short varnum );

long doStringCompare( );
long doCompare(short cmptype );
short getFuncReturnType( short tnum );


#include        "Structs1.h"

extern UCHAR   *CodeBuf;
extern long    CodePos, CodeLen, NLP;

extern short  *GIP, GIP2, GIP3, VR_Int, *VR_IntPtr;
extern long VR_Long, *VR_LongPtr, VR_Long2;
extern UCHAR    *VR_StrPtr;
extern short double	VR_Double, *VR_DoublePtr;
extern float  			VR_Single, *VR_SinglePtr;

extern UCHAR    *ReturnStr, ReturnStrFlag;
extern long     ReturnStrLen, ReturnStrSize;

extern long             Long_Function_Result;
extern float            Single_Function_Result;
extern short double     Double_Function_Result;

UCHAR	*cp1;
long	cp1len;
extern char		PassAssignFlag;

extern struct   CallStackStruct
{
	long    location;
	short     module;
	unsigned char * varbuf;                         
} CallStack[500];

extern struct RunVarStruct // This Data Structure holds runtime info 3.0
{       
	struct  CallTableStruct1 *CallTable1;  
	struct  CallTableStruct2 *CallTable2;  
	struct  VarTableStruct1 *VarTable1;    
	struct  VarTableStruct2 *VarTable2;    
	long	*LabelTable;   
	unsigned char * StrTable;
	unsigned char * CodeTable;
	unsigned char * GlobalVarSpace;
	struct  SelectTableStruct *SelectTable;  
	struct  ObjectPropsStruct2 *PropTable;  
} RunVar;

extern struct  cmd_func2       //These are built-in functions, there will also be user defined subs
{
	short (*func_ptr)(short rtype);
	char    returnType;
}       cmd_functions[150];

void ErrHandler( short ErrNum, Str255 AddMsg);

short Allocate_ReturnStr( long sz)
{
	if( ReturnStr !=nil )
	{
	 	return(-1);
	}
	ReturnStr = (UCHAR * ) NewPtr( sz );
	ReturnStrSize = sz;
	ReturnStrLen=0;
	return(0);
}


short FreeReturnStr()
{
	if(ReturnStr==nil) return(0); //None allocated so return
	DisposePtr( (Ptr) ReturnStr); 
	ReturnStrFlag = 0;
	ReturnStr = nil; // Point back to UCHAR Array
	ReturnStrLen=0;
	return(0);
}




long doStringCompare( )
{
	short	cmptype;
	long result;
	
	get_stringarg_val();
	cp1 = ReturnStr;
	cp1len = ReturnStrLen;			
	cmptype = CodeBuf[CodePos++]; //Cmp type
	if( cmptype<40 || cmptype>45)
	{
		FreeReturnStr();
		return(-1);
	}
	ReturnStr = nil;   // Reset so it can be used again
	get_stringarg_val();  // Get 2nd str
	result = doCompare(cmptype);	
	FreeReturnStr();
	ReturnStr = cp1;
	ReturnStrLen = cp1len;			
	FreeReturnStr();
	
	return(result);
}





long doCompare(short cmptype)
{
	long	result = 0, ctr, mx=0;

	// Now do compare result will 
	if( cmptype == 40 ) if(	cp1len != ReturnStrLen )  return(0L); // Fast check for =
	if( cmptype == 41 ) if(	cp1len != ReturnStrLen )  return(1L); // Fast check for =

	if(cp1len > ReturnStrLen ) mx = cp1len;
	else mx = ReturnStrLen;
	 
	for( ctr=0; ctr< mx; ctr++)
	{
		if( cp1[ctr] != ReturnStr[ctr] ) 
		{
			if( cmptype == 40 ) return(0L); // = is false
			if( cmptype == 41 ) return(1L); // <> is true
			switch(cmptype)
			{
				case 42:
				case 43:
					if( cp1[ctr] < ReturnStr[ctr] ) return(1L);
					else return(0L);
				case 44:
				case 45:
					if( cp1[ctr] > ReturnStr[ctr] ) return(1L);
					else return(0L);
			}

		}
	}				
	// Ok if we are here then the strings are equal up to length of the shorter string
	switch(cmptype)
	{
		case 40: // =
			if(	cp1len == ReturnStrLen )  return(1L); // Fast check for =
			return(0L);
			break;
		case 41: // <>
			if(	cp1len == ReturnStrLen )  return(0L); // Fast check for =
			return(1L);
			break;
		case 42: // <
			if(	cp1len == ReturnStrLen )  return(0L);
			if(	cp1len < ReturnStrLen )  return(1L);
			return( 0L );
			break;
		case 43: // <=
			if(	cp1len == ReturnStrLen )  return(1L);
			if(	cp1len < ReturnStrLen )  return(1L);
			return( 0L );
			break;
		case 44: // >
			if(	cp1len == ReturnStrLen )  return(0L);
			if(	cp1len > ReturnStrLen )  return(1L);
			return( 0L );
			break;
		case 45: // >=
			if(	cp1len == ReturnStrLen )  return(1L);
			if(	cp1len > ReturnStrLen )  return(1L);
			return( 0L );
			break;
	}
	return( 0L);
}				

			


short get_function(short argtype)
{
	long    result, NLP_Save;
	short   b,c,d;
	GIP = (short*) (CodeBuf+CodePos);               
	d = *GIP; 
	CodePos+=2;     
	if( d <1000 )                 // Internal or user defined
	{
		
		d-=100;
		cmd_functions[d].func_ptr(argtype);

		if(  CodeBuf[CodePos-1]!=37 && CodeBuf[CodePos]==37) CodePos++;	//Skip if there)
		
	}       
	else
	{
		// User Defined Function
		d-=1000;
			
		CallStackPtr++;
		if( CallStackPtr > 499 ) 
		{
			ErrHandler( 1029, nil);
		}
		CallStack[ CallStackPtr ].module = d;                
		result = RunVar.CallTable2[d].VarMemSz;
		if( RunVar.CallTable2[d].VarMemSz )
		{
			// Allocate memory for local vars                               
			CallStack[ CallStackPtr ].varbuf = (unsigned char *) NewPtrClear(  RunVar.CallTable2[d].VarMemSz );
			if( CallStack[ CallStackPtr ].varbuf == nil ) return(-1);
		}
		else CallStack[ CallStackPtr ].varbuf = nil;
			
		// Now assign passed values to variables (if any)
		b=0;
		while( RunVar.CallTable2[d].params[b] )
		{
			c=2;    //default
			switch( RunVar.CallTable2[d].params[b] )
			{
				case 'U':
					c=1;
					break;
				case 'L':
					c=2;
					break;
				case 'X':
					c=3;
					break;
				case 'D':
					c=4;
					break;
				case 'S':
					c=5;
					break;
			}
			pass_assign( c, RunVar.CallTable2[d].par_vars[b]);
			b++;
		}
		
		CallStack[ CallStackPtr ].location = CodePos;   //Return to next code position
		CodePos = RunVar.CallTable2[d].location;     // Call function
		NLP_Save = NLP;         // Save current end of line len global C var
		RunProgramLoop();
		NLP = NLP_Save;         // Restore old end of line len global C var
	}
	return(0);
}

long grab_varaddress(short varnum)
{
	short	a;
	long	sz1, sz2=0, result, dims=1;
	UCHAR 	*VIP;
	
	if( RunVar.VarTable2[varnum].modnum == -1) VIP = (UCHAR*) (RunVar.GlobalVarSpace +  RunVar.VarTable2[varnum].offset);
	else VIP = (UCHAR*) (CallStack[ CallStackPtr].varbuf +  RunVar.VarTable2[varnum].offset);
	
	sz1 =  ArrayCalc( varnum );
	result = (long) VIP+sz1;
	return( result );
}


long grab_intvar(short varnum)
{
	short	a, b ;
	long	sz1, sz2, mpl;
	short 	*VIP;
	
	if( RunVar.VarTable2[varnum].modnum == -1) VIP = (short*) (RunVar.GlobalVarSpace +  RunVar.VarTable2[varnum].offset);
	else VIP = (short*) (CallStack[ CallStackPtr-PassAssignFlag].varbuf +  RunVar.VarTable2[varnum].offset);
	
	if( RunVar.VarTable2[varnum].dims )
	{
		// This is an array variable  ; Reminder Base is always 1 
		sz1 = 0;
		for(a=0; a< RunVar.VarTable2[varnum].dims; a++ )
		{
			mpl = 1;
			if( CodeBuf[CodePos] == 250 || CodeBuf[CodePos] == 251) CodePos++;;
			sz2 = get_longarg_val();
			if( sz2<= 0 || sz2 > RunVar.VarTable2[varnum].dimsize[a] )
			{
				// Out of range, !!! Generate error
				if( sz2<= 0 ) ErrHandler( 1051, "\p in integer variable");
				else ErrHandler( 1007, "\p in integer variable");
				VR_Long = -1;
				return( VR_Long );
			}

			for(b=a+1; b< RunVar.VarTable2[varnum].dims; b++ ) mpl *= RunVar.VarTable2[varnum].dimsize[b];

			if (a+1 < RunVar.VarTable2[varnum].dims ) sz1 += (sz2-1) * mpl; 
			else sz1 += (sz2-1);  // Last dimension is special
		}

		VIP += sz1;	// times 4 bytes
	}
	a = *VIP;
	VR_Long = (long) a;
	return( VR_Long );
}

long grab_longvar(short varnum)
{
	short	a,b;
	long	sz1, sz2, *VLP, mpl=1;
	
	if( RunVar.VarTable2[varnum].modnum == -1) VLP = (long*) (RunVar.GlobalVarSpace +  RunVar.VarTable2[varnum].offset);
	else VLP = (long*) (CallStack[ CallStackPtr-PassAssignFlag].varbuf +  RunVar.VarTable2[varnum].offset);
	
	if( RunVar.VarTable2[varnum].dims )
	{
		// This is an array variable  ; Reminder Base is always 1 
		sz1 = 0;
		for(a=0; a< RunVar.VarTable2[varnum].dims; a++ )
		{
			mpl = 1;
			if( CodeBuf[CodePos] == 250 || CodeBuf[CodePos] == 251) CodePos++;
			sz2 = get_longarg_val();
			if( sz2<= 0 || sz2 > RunVar.VarTable2[varnum].dimsize[a] )
			{
				// Out of range, !!! Generate error
				if( sz2<= 0 ) ErrHandler( 1051, "\p in long variable");
				else ErrHandler( 1007, "\p in long variable");
				VR_Long = -1;
				return( VR_Long );
			}
			
			for(b=a+1; b< RunVar.VarTable2[varnum].dims; b++ ) mpl *= RunVar.VarTable2[varnum].dimsize[b];
			
			if (a+1 < RunVar.VarTable2[varnum].dims ) sz1 += (sz2-1) * mpl;  
			else sz1 += (sz2-1);  // Last dimension is special
		}

		VLP += sz1;	// times 4 bytes
	}
	VR_Long = *VLP;
	return( VR_Long );
}

float grab_singlevar(short varnum)
{
	short	a, b;
	long	sz1, sz2, mpl;
	float   *VSP,vv;
	
	if( RunVar.VarTable2[varnum].modnum == -1) VSP = (float*) (RunVar.GlobalVarSpace +  RunVar.VarTable2[varnum].offset);
	else VSP = (float*) (CallStack[ CallStackPtr-PassAssignFlag].varbuf +  RunVar.VarTable2[varnum].offset);
	
	if( RunVar.VarTable2[varnum].dims )
	{
		// This is an array variable  ; Reminder Base is always 1 
		sz1 = 0;
		for(a=0; a< RunVar.VarTable2[varnum].dims; a++ )
		{
			mpl = 1;
			if( CodeBuf[CodePos] == 250 || CodeBuf[CodePos] == 251) CodePos++;
			sz2 = get_longarg_val();
			if( sz2<= 0 || sz2 > RunVar.VarTable2[varnum].dimsize[a] )
			{
				// Out of range, !!! Generate error
				if( sz2<= 0 ) ErrHandler( 1051, "\p in single variable");
				else ErrHandler( 1007, "\p in single variable");
				VR_Single = -1;
				return( VR_Single );
			}
			
			for(b=a+1; b< RunVar.VarTable2[varnum].dims; b++ ) mpl *= RunVar.VarTable2[varnum].dimsize[b];

			if (a+1 < RunVar.VarTable2[varnum].dims ) sz1 += (sz2-1) * mpl;
			else sz1 += (sz2-1);  // Last dimension is special
		}
		VSP += sz1 ;	// times 4 bytes
	}
	vv = *VSP;
	return( vv );
}


short double grab_doublevar(short varnum)
{
	short	a,b;
	long	sz1, sz2,mpl;
	short double	*VSD;
	
	if( RunVar.VarTable2[varnum].modnum == -1) VSD = (short double*) (RunVar.GlobalVarSpace +  RunVar.VarTable2[varnum].offset);
	else VSD = (short double*) (CallStack[ CallStackPtr-PassAssignFlag].varbuf +  RunVar.VarTable2[varnum].offset);
	
	if( RunVar.VarTable2[varnum].dims )
	{
		// This is an array variable  ; Reminder Base is always 1 
		sz1 = 0;
		for(a=0; a< RunVar.VarTable2[varnum].dims; a++ )
		{
			mpl = 1;
			if( CodeBuf[CodePos] == 250 || CodeBuf[CodePos] == 251) CodePos++;
			sz2 = get_longarg_val();
			if( sz2<= 0 || sz2 > RunVar.VarTable2[varnum].dimsize[a] )
			{
				// Out of range, !!! Generate error
				if( sz2<= 0 ) ErrHandler( 1051, "\p in double variable");
				else ErrHandler( 1007, "\p in double variable");
				VR_Double = -1;
				return( VR_Double );
			}

			for(b=a+1; b< RunVar.VarTable2[varnum].dims; b++ ) mpl *= RunVar.VarTable2[varnum].dimsize[b];

			if (a+1 < RunVar.VarTable2[varnum].dims ) sz1 += (sz2-1) * mpl;
			else sz1 += (sz2-1);  // Last dimension is special
		}
		VSD += sz1;	// times 8 bytes
	}
	VR_Double = *VSD;
	return( VR_Double );
}


long longArg()
{
	long	answer;
	
	switch( CodeBuf[CodePos] )
	{
		case 251:  //long or short only
			CodePos++;
			answer = get_longarg_val();
			break;
		case 250:  // single/double
			CodePos++;
			answer = (long) get_doublearg_val();
			break;
	}
	return( answer );
}	

float singleArg()
{
	float	answer;
	
	switch( CodeBuf[CodePos] )
	{
		case 251:  //long or short only
			CodePos++;
			answer = (float) get_longarg_val();
			break;
		case 250:  // single/double
			CodePos++;
			answer = (float) get_doublearg_val();
			break;
	}
	return( answer );
}
	
short double doubleArg()
{
	
	short double	answer;
	
	switch( CodeBuf[CodePos] )
	{
		case 251:  //long or short only
			CodePos++;
			answer = (short double) get_longarg_val();
			break;
		case 250:  // single/double
			CodePos++;
			answer =  get_doublearg_val();
			break;
	}
	return( answer );
}	
	
short getFuncReturnType( short tnum )
{
	short num, rt;
	num = tnum;
	
	if( num <1000 )
	{
		num-=100;
		rt = cmd_functions[num].returnType;
	}
	else
	{
		num -=1000;
		rt = RunVar.CallTable2[num].returntype;
	}
	return(rt);
}	
		
long get_longarg_val()
{
	char looper, value_ctr=0,ng_flag=0,mdm_flag=0,pm_flag=0,cmp_flag=0,log_flag=0, coop=0, lastAct=0;
	char ops_list[]  ="                                  ";
	char ng_list[]   ="                                  ";
	long *lp,lv,temp_long[32];
	float *fp,fv;
	short double *dp,dv;
	
	// Don't forget to uppercase
	looper=0;
	while(!looper)
	{
		coop++;
		switch( CodeBuf[CodePos] )
		{
			case 252: // End of Arg
					ops_list[value_ctr-1]='T';
					CodePos++;
					looper++;
					lastAct = 0;
				break;
			case 253: // End of Cmd line
					ops_list[value_ctr-1]='T';
					looper++;
					lastAct = 0;
				break;
			case 37:  //')':       // End of Sub-Arg
					ops_list[value_ctr-1]='T';
					CodePos++;
					looper++;
					lastAct = 0;
				break;
			case 36: //'(':       // Start of Sub-Arg
					CodePos++;
					temp_long[value_ctr++]=get_longarg_val();
					lastAct = 0;
				break;
			case 19: //'(':       // Function Call
					CodePos++;
					GIP = (short*) (CodeBuf+CodePos);               
					GIP3 = *GIP;    
					                                                           
					// Allow use of different return types
					if(getFuncReturnType( GIP3 ) != 'S' )
					{
						get_function(2);
						temp_long[value_ctr++] = Long_Function_Result;
					}
					else
					{
						CodePos--; 
						temp_long[value_ctr++] = doStringCompare();
					}
					lastAct = 0;
				break;
			case 7:   // String const
			case 15: // String var  (Either of these two mean a string cmp
				temp_long[value_ctr++] = doStringCompare();
				lastAct = 0;
				break;
			case 2: // long const
					CodePos++;
					lp = (long*) (CodeBuf+CodePos);                   //Ptr to 4 Byte Long Value
					lv = *lp;                
					CodePos+=4;    
					temp_long[value_ctr++]=lv;
					lastAct = 0;
				break;
			case 3: // single const
					CodePos++;
					fp = (float*) (CodeBuf+CodePos);                   //Ptr to 4 Byte single Value
					fv = *fp;                                                                               //Now get it
					CodePos+=4;                                                                   //Update pos. ptr
					temp_long[value_ctr++]=(long) fv;
					lastAct = 0;
				break;
			case 4: // double const
					CodePos++;
					dp = (short double*) (CodeBuf+CodePos);                   //Ptr to 4 Byte single Value
					dv = *dp;                                                                               //Now get it
					CodePos+=8;                                                                   //Update pos. ptr
					temp_long[value_ctr++]=(long) dv;
					lastAct = 0;
				break;
			case 11: // int var
					CodePos++;
					GIP = (short*) (CodeBuf+CodePos);                   
					GIP3 = *GIP;                                                                               //Now get it
					CodePos+=2;  
					temp_long[value_ctr++] = grab_intvar( GIP3 );
					lastAct = 0;
				break;
			case 12: // long var
					CodePos++;
					GIP = (short*) (CodeBuf+CodePos);                   
					GIP3 = *GIP;                                                                               //Now get it
					CodePos+=2;  
					lv = grab_longvar( GIP3 );
					temp_long[value_ctr++] = lv; 
					lastAct = 0;
				break;
			case 13: // single var
					CodePos++;
					GIP = (short*) (CodeBuf+CodePos);                   
					GIP3 = *GIP;                                                                               //Now get it
					CodePos+=2;  
					temp_long[value_ctr++] = (long) grab_singlevar( GIP3 );
					lastAct = 0;
				break;
			case 14: // double var
					CodePos++;
					GIP = (short*) (CodeBuf+CodePos);                   
					GIP3 = *GIP;                                                                               //Now get it
					CodePos+=2;  
					temp_long[value_ctr++] = (long) grab_doublevar( GIP3 );
					lastAct = 0;
				break;
			case 20:  // '+':
					if(!value_ctr) temp_long[value_ctr++] = 0;
					ops_list[value_ctr-1]=CodeBuf[CodePos++];
					pm_flag++;              //Sets flag
					lastAct = 1;
					break;
			case 21:  // '-':
					if( !value_ctr || lastAct ) { ng_flag=1; ng_list[value_ctr]='~'; CodePos++;}
					else ops_list[value_ctr-1]=CodeBuf[CodePos++];
					pm_flag++;              //Sets flag
					lastAct = 1;
				break;
			case  25:  //'*':
			case  26:       //division
			case  27:  //'%':
					ops_list[value_ctr-1]=CodeBuf[CodePos++];
					mdm_flag++;             //Sets flag
					lastAct = 1;
				break;
			case  40:       // =
			case  41:       // <>
			case  42:       // <
			case  43:       // <=
			case  44:       // >
			case  45:       // >=
					ops_list[value_ctr-1]=CodeBuf[CodePos++];
					cmp_flag++;             //Sets flag
					lastAct = 1;
					break;
			case  46:       // AND
			case  47:       // OR
			case  48:       // NOT
					ops_list[value_ctr-1]=CodeBuf[CodePos++];
					log_flag++;             //Sets flag
					lastAct = 1;
					break;
			default:
				ErrHandler( 1042, "\p in long argument");
				looper++;
				break;
		}
	}
	// First check for negation
	if(ng_flag)     //Only run if needed
	{
		for(looper=0; looper < value_ctr; looper++)
		{
			if(ng_list[looper]=='~')
			{
				temp_long[looper]=-temp_long[looper];
			}       
		}
	}

	// Second check for */%
	if(mdm_flag)    //Only run if needed
	{
		for(looper=0; looper < value_ctr; looper++)
		{
			switch(ops_list[looper])
			{
				case  25:  //'*':
					temp_long[looper+1]*=temp_long[looper];
					break;
				case  26: //division
					temp_long[looper+1] = temp_long[looper] / temp_long[looper+1];
					break;
				case  27:  //'%': //modulo division
					temp_long[looper+1] = temp_long[looper] % temp_long[looper+1];
					break;
			}
					
		}
	}

	// Then do + -
	if(pm_flag)     //Only run if needed
	{
		mdm_flag=0;
		ng_flag=120;
		for(looper=0; looper < value_ctr; looper++)
		{
			switch(ops_list[looper])
			{
				case 40:  //== Special
				case 41:
				case 42:
				case 43:
				case 44:
				case 45:  
				case 46:  // AND
				case 47:  // OR 
				case 48:  // NOT
					if(ng_flag<120)
					{
						if(mdm_flag=='+') temp_long[looper]+=temp_long[ng_flag];
						else temp_long[looper] = temp_long[ng_flag] - temp_long[looper];
					}
					ng_flag=120;
					mdm_flag=0;
					break;
				case 20:  //'+':
					if(ng_flag<120)
					{
						if(mdm_flag=='+') temp_long[looper]+=temp_long[ng_flag];
						else temp_long[looper] = temp_long[ng_flag] - temp_long[looper];  // was rever looper&ng_flag

					}
					ng_flag=looper;
					mdm_flag='+';
					break;
				case  21:  //- 
					if(ng_flag<120)
					{
						if(mdm_flag=='+') temp_long[looper]+=temp_long[ng_flag];
						else temp_long[looper] = temp_long[ng_flag] - temp_long[looper]  ;
					}
					ng_flag=looper;
					mdm_flag='-';
					break;
				case  'T':
					if(ng_flag<120)
					{
						if(mdm_flag=='+') temp_long[looper]+=temp_long[ng_flag];
						else temp_long[looper] = temp_long[ng_flag] - temp_long[looper];
					}
					break;
					
			}
		
		}
	}

	// Then do = < > etc
	if(cmp_flag)     //Only run if needed
	{
		mdm_flag=0;
		ng_flag=120;

		for(looper=0; looper < value_ctr; looper++)
		{
			switch(ops_list[looper])
			{
				case 40:  //'=':
					if(ng_flag<120)
					{
						if( temp_long[looper] == temp_long[ng_flag]) temp_long[looper]=1; //true
						else temp_long[looper] = 0; //false
					}
					ng_flag=looper;
					mdm_flag = 40;
					break;
				case 41:  //'<>':
					if(ng_flag<120)
					{
						if( temp_long[looper] != temp_long[looper]) temp_long[looper]=1; //true
						else temp_long[looper] = 0; //false
					}
					ng_flag=looper;
					mdm_flag = 41;
					break;
				case 42:  //'<':
					if(ng_flag<120)
					{
						if( temp_long[ng_flag] < temp_long[looper]) temp_long[looper]=1; //true
						else temp_long[looper] = 0; //false
					}
					ng_flag=looper;
					mdm_flag = 42;
					break;
				case 43:  //'<=':
					if(ng_flag<120)
					{
						if( temp_long[ng_flag] <= temp_long[looper]) temp_long[looper]=1; //true
						else temp_long[looper] = 0; //false
					}
					ng_flag=looper;
					mdm_flag = 43;
					break;
				case 44:  //'>':
					if(ng_flag<120)
					{
						if( temp_long[ng_flag] > temp_long[looper]) temp_long[looper]=1; //true
						else temp_long[looper] = 0; //false
					}
					ng_flag=looper;
					mdm_flag = 44;
					break;
				case 45:  //'>=':
					if(ng_flag<120)
					{
						if( temp_long[ng_flag] >= temp_long[looper]) temp_long[looper]=1; //true
						else temp_long[looper] = 0; //false
					}
					ng_flag=looper;
					mdm_flag = 45;
					break;
				case 46:  // AND
				case 47:  // OR 
				case 48:  // NOT
				case  'T':
					if(ng_flag<120)
					{
						switch( mdm_flag )
						{
							case 40:  //'=':
								if( temp_long[looper] == temp_long[ng_flag]) temp_long[looper]=1; //true
								else temp_long[looper] = 0; //false
								break;
							case 41:  //'<>':
								if( temp_long[looper] != temp_long[ng_flag]) temp_long[looper]=1; //true
								else temp_long[looper] = 0; //false
								break;
							case 42:  //'<':
								if( temp_long[ng_flag] < temp_long[looper]) temp_long[looper]=1; //true
								else temp_long[looper] = 0; //false
								break;
							case 43:  //'<=':
								if( temp_long[ng_flag] <= temp_long[looper]) temp_long[looper]=1; //true
								else temp_long[looper] = 0; //false
								break;
							case 44:  //'>':
								if( temp_long[ng_flag] > temp_long[looper]) temp_long[looper]=1; //true
								else temp_long[looper] = 0; //false
								break;
							case 45:  //'>=':
								if( temp_long[ng_flag] >= temp_long[looper]) temp_long[looper]=1; //true
								else temp_long[looper] = 0; //false
								break;

						}
					}
					if (ops_list[looper] <=48)
					{
						ng_flag=120;
						mdm_flag=0;
					}
					break;
			}
		
		}
	}

	// Finally do AND OR NOT
	if(log_flag)     //Only run if needed
	{
		mdm_flag=0;
		ng_flag=120;
		for(looper=0; looper < value_ctr; looper++)
		{
			switch(ops_list[looper])
			{
				case 46:  // AND
					if(ng_flag<120)
					{
						if( temp_long[ng_flag] && temp_long[looper]) temp_long[looper]=1; //true
						else temp_long[looper] = 0; //false
					}
					ng_flag=looper;
					mdm_flag = 46;
					break;
				case 47:  // OR 
					if(ng_flag<120)
					{
						if( temp_long[ng_flag] || temp_long[looper]) temp_long[looper]=1; //true
						else temp_long[looper] = 0; //false
					}
					ng_flag=looper;
					mdm_flag = 47;
					break;
				case 48:  // NOT
					if(ng_flag<120)
					{
						if( !temp_long[looper]) temp_long[looper]=1; //true
						else temp_long[looper] = 0; //false
					}
					ng_flag=looper;
					mdm_flag = 48;
					break;
				case  'T':
					if(ng_flag<120)
					{
						switch( mdm_flag )
						{
							case 46:  // AND
								if( temp_long[ng_flag] && temp_long[looper]) temp_long[looper]=1; //true
								else temp_long[looper] = 0; //false
								break;
							case 47:  // OR 
								if( temp_long[ng_flag] || temp_long[looper]) temp_long[looper]=1; //true
								else temp_long[looper] = 0; //false
								break;
							case 48:  // NOT
								if( !temp_long[looper]) temp_long[looper]=1; //true
								else temp_long[looper] = 0; //false
								break;
						}
					}
					break;
					
			}
		
		}
	}


	// Answer will be in last value holder
	lv=temp_long[value_ctr-1];
	return(lv);             
}

// Much copied from get_longarg_val() & modified for short double (MSB DOUBLE's)

short double get_doublearg_val()
{
	char looper, value_ctr=0,ng_flag=0,mdm_flag=0,pm_flag=0,cmp_flag=0,log_flag=0;
	char ops_list[]  ="                                  ";
	char ng_list[]   ="                                  ";
	short	lastAct=0;
	long *lp,lv;
	float *fp,fv;
	short double *dp,dv,temp_double[32]; //, mod1 ,mod2;

	// Don't forget to uppercase
	looper=0;
	while(!looper)
	{
		switch( CodeBuf[CodePos] )
		{
			case 252: // End of Arg
					ops_list[value_ctr-1]='T';
					CodePos++;
					looper++;
					lastAct=0;
				break;
			case 253: // End of Cmd line
					ops_list[value_ctr-1]='T';
					looper++;
					lastAct=0;
				break;
			case 37:  //')':       // End of Sub-Arg
					ops_list[value_ctr-1]='T';
					CodePos++;
					looper++;
					lastAct=0;
				break;
			case 36: //'(':       // Start of Sub-Arg
					CodePos++;
					temp_double[value_ctr++]=get_doublearg_val();
					lastAct=0;
				break;
			case 19: //'(':       // Function Call
					CodePos++;
					GIP = (short*) (CodeBuf+CodePos);               
					GIP3 = *GIP;    
					                                                                           //Now get it
					// Allow use of different return types
					if(getFuncReturnType( GIP3 ) != 'S' )
					{
						get_function(4);
						temp_double[value_ctr++] = (short double) Double_Function_Result;
					}
					else temp_double[value_ctr++] = (short double) doStringCompare();
					lastAct=0;
				break;
			case 7:   // String const
			case 15: // String var  (Either of these two mean a string cmp
				temp_double[value_ctr++] = (short double) doStringCompare();
					lastAct=0;
				break;
			case 2: // long const
					CodePos++;
					lp = (long*) (CodeBuf+CodePos);                   //Ptr to 4 Byte Long Value
					lv = *lp;                                                                               //Now get it
					CodePos+=4;                                                                   //Update pos. ptr
					temp_double[value_ctr++]=(short double) lv;
					lastAct=0;
				break;
			case 3: // single const
					CodePos++;
					fp = (float*) (CodeBuf+CodePos);                   //Ptr to 4 Byte single Value
					fv = *fp;                                                                               //Now get it
					CodePos+=4;                                                                   //Update pos. ptr
					temp_double[value_ctr++]=(short double) fv;
					lastAct=0;
				break;
			case 4: // double const
					CodePos++;
					dp = (short double*) (CodeBuf+CodePos);                   //Ptr to 8 Byte single Value
					dv = *dp;  
					CodePos+=8;                                                                   //Update pos. ptr
					temp_double[value_ctr++] = (short double) dv;
					lastAct=0;
				break;
			case 11: // short var
					CodePos++;
					GIP = (short*) (CodeBuf+CodePos);                   
					GIP3 = *GIP;                                                                               //Now get it
					CodePos+=2;  
					temp_double[value_ctr++] = (short double) grab_intvar( GIP3 );
					lastAct=0;
				break;
			case 12: // long var
					CodePos++;
					GIP = (short*) (CodeBuf+CodePos);                   
					GIP3 = *GIP;                                                                               //Now get it
					CodePos+=2;  
					temp_double[value_ctr++] = (short double) grab_longvar( GIP3 );
					lastAct=0;
				break;
			case 13: // single var
					CodePos++;
					GIP = (short*) (CodeBuf+CodePos);                  
					GIP3 = *GIP;                                                                               //Now get it
					CodePos+=2;  
					fv= grab_singlevar( GIP3 );
					temp_double[value_ctr++] = (short double) fv;
					lastAct=0;
				break;
			case 14: // double var
					CodePos++;
					GIP = (short*) (CodeBuf+CodePos);               
					GIP3 = *GIP;                                                                               //Now get it
					CodePos+=2;  
					temp_double[value_ctr++] = (short double) grab_doublevar( GIP3 );
					lastAct=0;
				break;
			case 20:  // '+':
					if(!value_ctr)  temp_double[value_ctr] = 0;
					ops_list[value_ctr-1]=CodeBuf[CodePos++];
					pm_flag++;              //Sets flag
					lastAct=1;
				break;
			case 21:  // '-':
					if( !value_ctr || lastAct) { ng_flag=1; ng_list[value_ctr]='~'; CodePos++; }
					else ops_list[value_ctr-1]=CodeBuf[CodePos++];
					pm_flag++;              //Sets flag
					lastAct=1;
				break;
			case  25:  //'*':
			case  26:       //division
			case  27:  //'%':
					ops_list[value_ctr-1]=CodeBuf[CodePos++];
					mdm_flag++;             //Sets flag
					lastAct=1;
				break;
			case  40:       // =
			case  41:       // <>
			case  42:       // <
			case  43:       // <=
			case  44:       // >
			case  45:       // >=
					ops_list[value_ctr-1]=CodeBuf[CodePos++];
					cmp_flag++;             //Sets flag
					lastAct=1;
					break;
			case  46:       // AND
			case  47:       // OR
			case  48:       // NOT
					ops_list[value_ctr-1]=CodeBuf[CodePos++];
					log_flag++;             //Sets flag
					lastAct=1;
					break;
		}
	}

	// First check for negation
	if(ng_flag)     //Only run if needed
	{
		for(looper=0; looper < value_ctr; looper++)
		{
			if(ng_list[looper]=='~')
			{
				temp_double[looper]=-temp_double[looper];
			}       
		}
	}

	// Second check for */%
	if(mdm_flag)    //Only run if needed
	{
		for(looper=0; looper < value_ctr; looper++)
		{
			switch(ops_list[looper])
			{
				case  25:  //'*':
					temp_double[looper+1]*=temp_double[looper];
					break;
				case  26: //division
					temp_double[looper+1] = temp_double[looper] / temp_double[looper+1];
					break;
				case  27:  //'%': //modulo division
					temp_double[looper+1] = fmod(temp_double[looper] , temp_double[looper+1]);
					break;
			}
					
		}
	}

	// Then do + -
	if(pm_flag)     //Only run if needed
	{
		mdm_flag=0;
		ng_flag=120;
		for(looper=0; looper < value_ctr; looper++)
		{
			switch(ops_list[looper])
			{
				case 40:  //== Special
				case 41:
				case 42:
				case 43:
				case 44:
				case 45:  
				case 46:  // AND
				case 47:  // OR 
				case 48:  // NOT
					if(ng_flag<120)
					{
						if(mdm_flag=='+') temp_double[looper]+=temp_double[ng_flag];
						else temp_double[looper] = temp_double[ng_flag] - temp_double[looper];
					}
					ng_flag=120;
					mdm_flag=0;
					break;
				case 20:  //'+':
					if(ng_flag<120)
					{
						if(mdm_flag=='+') temp_double[looper]+=temp_double[ng_flag];
						else temp_double[looper] = temp_double[looper] - temp_double[ng_flag];

					}
					ng_flag=looper;
					mdm_flag='+';
					break;
				case  21:  //- 
					if(ng_flag<120)
					{
						if(mdm_flag=='+') temp_double[looper]+=temp_double[ng_flag];
						else temp_double[looper] = temp_double[ng_flag] - temp_double[looper]  ;
					}
					ng_flag=looper;
					mdm_flag='-';
					break;
				case  'T':
					if(ng_flag<120)
					{
						if(mdm_flag=='+') temp_double[looper]+=temp_double[ng_flag];
						else temp_double[looper] = temp_double[ng_flag] - temp_double[looper];
					}
					break;
					
			}
		
		}
	}

	// Then do = < > etc
	if(cmp_flag)     //Only run if needed
	{
		mdm_flag=0;
		ng_flag=120;
		for(looper=0; looper < value_ctr; looper++)
		{
			switch(ops_list[looper])
			{
				case 40:  //'=':
					if(ng_flag<120)
					{
						if( temp_double[looper] == temp_double[ng_flag]) temp_double[looper]=1; //true
						else temp_double[looper] = 0; //false
					}
					ng_flag=looper;
					mdm_flag = 40;
					break;
				case 41:  //'<>':
					if(ng_flag<120)
					{
						if( temp_double[looper] != temp_double[looper]) temp_double[looper]=1; //true
						else temp_double[looper] = 0; //false
					}
					ng_flag=looper;
					mdm_flag = 41;
					break;
				case 42:  //'<':
					if(ng_flag<120)
					{
						if( temp_double[ng_flag] < temp_double[looper]) temp_double[looper]=1; //true
						else temp_double[looper] = 0; //false
					}
					ng_flag=looper;
					mdm_flag = 42;
					break;
				case 43:  //'<=':
					if(ng_flag<120)
					{
						if( temp_double[ng_flag] <= temp_double[looper]) temp_double[looper]=1; //true
						else temp_double[looper] = 0; //false
					}
					ng_flag=looper;
					mdm_flag = 43;
					break;
				case 44:  //'>':
					if(ng_flag<120)
					{
						if( temp_double[ng_flag] > temp_double[looper]) temp_double[looper]=1; //true
						else temp_double[looper] = 0; //false
					}
					ng_flag=looper;
					mdm_flag = 44;
					break;
				case 45:  //'>=':
					if(ng_flag<120)
					{
						if( temp_double[ng_flag] >= temp_double[looper]) temp_double[looper]=1; //true
						else temp_double[looper] = 0; //false
					}
					ng_flag=looper;
					mdm_flag = 45;
					break;
				case 46:  // AND
				case 47:  // OR 
				case 48:  // NOT
				case  'T':
					if(ng_flag<120)
					{
						switch( mdm_flag )
						{
							case 40:  //'=':
								if( temp_double[looper] == temp_double[ng_flag]) temp_double[looper]=1; //true
								else temp_double[looper] = 0; //false
								break;
							case 41:  //'<>':
								if( temp_double[looper] != temp_double[ng_flag]) temp_double[looper]=1; //true
								else temp_double[looper] = 0; //false
								break;
							case 42:  //'<':
								if( temp_double[ng_flag] < temp_double[looper]) temp_double[looper]=1; //true
								else temp_double[looper] = 0; //false
								break;
							case 43:  //'<=':
								if( temp_double[ng_flag] <= temp_double[looper]) temp_double[looper]=1; //true
								else temp_double[looper] = 0; //false
								break;
							case 44:  //'>':
								if( temp_double[ng_flag] > temp_double[looper]) temp_double[looper]=1; //true
								else temp_double[looper] = 0; //false
								break;
							case 45:  //'>=':
								if( temp_double[ng_flag] >= temp_double[looper]) temp_double[looper]=1; //true
								else temp_double[looper] = 0; //false
								break;

						}
					}
					if (ops_list[looper] <=48)
					{
						ng_flag=120;
						mdm_flag=0;
					}
					break;
			}
		
		}
	}

	// Finally do AND OR NOT
	if(log_flag)     //Only run if needed
	{
		mdm_flag=0;
		ng_flag=120;
		for(looper=0; looper < value_ctr; looper++)
		{
			switch(ops_list[looper])
			{
				case 46:  // AND
					if(ng_flag<120)
					{
						if( temp_double[ng_flag] && temp_double[looper]) temp_double[looper]=1; //true
						else temp_double[looper] = 0; //false
					}
					ng_flag=looper;
					mdm_flag = 46;
					break;
				case 47:  // OR 
					if(ng_flag<120)
					{
						if( temp_double[ng_flag] || temp_double[looper]) temp_double[looper]=1; //true
						else temp_double[looper] = 0; //false
					}
					ng_flag=looper;
					mdm_flag = 47;
					break;
				case 48:  // NOT
					if(ng_flag<120)
					{
						if( !temp_double[looper]) temp_double[looper]=1; //true
						else temp_double[looper] = 0; //false
					}
					ng_flag=looper;
					mdm_flag = 48;
					break;
				case  'T':
					if(ng_flag<120)
					{
						switch( mdm_flag )
						{
							case 46:  // AND
								if( temp_double[ng_flag] && temp_double[looper]) temp_double[looper]=1; //true
								else temp_double[looper] = 0; //false
								break;
							case 47:  // OR 
								if( temp_double[ng_flag] || temp_double[looper]) temp_double[looper]=1; //true
								else temp_double[looper] = 0; //false
								break;
							case 48:  // NOT
								if( !temp_double[looper]) temp_double[looper]=1; //true
								else temp_double[looper] = 0; //false
								break;
						}
					}
					break;
					
			}
		
		}
	}


	// Answer will be in last value holder
	dv=temp_double[value_ctr-1];
	return(dv);             
}


short get_stringarg_val()
{
	// This returns a string with 4 byte prefix length & also zero byte termination
	unsigned char looper, *SA_Buffer, *SA_Temp;
	short	a, b, dims, vx;
	long SA_Len=0, SA_Size = 256, sz1, sz2;
	long RBC=4, *RBCptr;
	long *lp,lv, lm, lc, mpl;
	
	SA_Buffer = (UCHAR*) NewPtr ( SA_Size );   
		
	// Check if buffer allocated ok
	if( SA_Buffer == nil ) return(-1);
	
	RBCptr = (long*) SA_Buffer;
	lc = 0; //Len Ctr
	// Don't forget to uppercase
	looper=0;
	while(!looper)
	{
		switch( CodeBuf[CodePos] )
		{
			case 251:  // Start of #Arg
					looper++;
				break;
			case 252:  // End of Arg
					CodePos++;
					looper++;
				break;
			case 253:  // End of Cmd line
					looper++;
				break;
			case  20:  // Plus
					CodePos++; //Just Skip It
				break;
			case   7:  // String Constant
					CodePos++;
					lp = (long*) (CodeBuf+CodePos);                   //Ptr to 4 Byte Long Value
					lv = *lp;                                                                               //Now get it
					CodePos+=4;
					// Ok, now copy  
					lm = RunVar.StrTable[lv++] ;                                                                  //Update pos. ptr
					if( SA_Len+lm > SA_Size )
					{
						// Need more size, save ptr
						SA_Temp = SA_Buffer;
						
						SA_Size = SA_Len+lm+255;        // +255 is extra space
						SA_Buffer = (UCHAR*) NewPtr ( SA_Size );   
						// Check if buffer allocated ok
						if( SA_Buffer == nil ) return(-1);
						
						// Copy over contents of old smaller buffer
						memcpy( SA_Buffer, SA_Temp, SA_Len );
						DisposePtr( (Ptr) SA_Temp ); // Don't need this old buffer anymore
					}
					// Copy new stuff
					memcpy( SA_Buffer+SA_Len, RunVar.StrTable+lv, lm );

					// Update Len
					SA_Len += lm;
				break;
			case 19: //'(':       // Function Call
					CodePos++;
					// Allow use of different return types
					get_function(5);

					if( ReturnStrLen )
					{
					  // Ok, now copy  
		  			  if( SA_Len+ReturnStrLen > SA_Size )
					  {
						// Need more size, save ptr
						SA_Temp = SA_Buffer;
						
						SA_Size = SA_Len+ReturnStrLen+255;        // +255 is extra space
						SA_Buffer = (UCHAR*) NewPtr ( SA_Size );   
						// Check if buffer allocated ok
						if( SA_Buffer == nil ) return(-1);
						
						// Copy over contents of old smaller buffer
						memcpy( SA_Buffer, SA_Temp, SA_Len );
						DisposePtr( (Ptr) SA_Temp ); // Don't need this old buffer anymore
					  }
					  // Copy new stuff
					  memcpy( SA_Buffer+SA_Len, ReturnStr, ReturnStrLen );

					  // Update Len
					  SA_Len += ReturnStrLen;

					  // Ok we're done with ReturnStr, so free it up
					  DisposePtr( (Ptr) ReturnStr );
					  ReturnStr = nil;
					  ReturnStrLen =0;					  
					  
					}
				break;
			case  15:  // String Variable
					CodePos++;
					GIP = (short*) (CodeBuf+CodePos);                 
					vx = *GIP;                                                                               //Now get it
					CodePos+=2;     
					
					if( RunVar.VarTable2[vx].modnum == -1)  // Global ?
					   VR_StrPtr = (UCHAR*) (RunVar.GlobalVarSpace +  RunVar.VarTable2[vx].offset);
					else
					   VR_StrPtr = (UCHAR*) (CallStack[ CallStackPtr-PassAssignFlag].varbuf +  RunVar.VarTable2[vx].offset);


					// Is this string var an array
					dims = RunVar.VarTable2[vx].dims - 1 ;
					if( dims > 0 )
					{
						// This is an array variable  ; Reminder Base is always 1 
						sz1 = 0;
						for(a=0; a< dims; a++ )
						{
							mpl=1;
							if( CodeBuf[CodePos] == 250 || CodeBuf[CodePos] == 251) CodePos++;
							sz2 = get_longarg_val();
							if( sz2<= 0 || sz2 > RunVar.VarTable2[vx].dimsize[a] )
							{
								// Out of range, !!! Generate error
								if( sz2<= 0 ) ErrHandler( 1051, "\p in string variable");
								else ErrHandler( 1007, "\p in string variable");
								VR_Long = -1;
								return( VR_Long );
							}

							for(b=a+1; b< RunVar.VarTable2[vx].dims-1; b++ ) mpl *= RunVar.VarTable2[vx].dimsize[b];

							if( a+1 < dims ) sz1 += (sz2-1) * mpl;
							else sz1 += (sz2-1);  // Last dimension is special
						}
						sz1 *= RunVar.VarTable2[vx].dimsize[dims];  // Last dimension is special
						VR_StrPtr += sz1;
					
					}


					VR_LongPtr = (long*) VR_StrPtr;
					VR_Long = *VR_LongPtr;
					
					if( VR_Long )
					{
					  // Ok, now copy  
		  			  if( SA_Len+VR_Long > SA_Size )
					  {
						// Need more size, save ptr
						SA_Temp = SA_Buffer;
						
						SA_Size = SA_Len+VR_Long+255;        // +255 is extra space
						SA_Buffer = (UCHAR*) NewPtr ( SA_Size );   
						// Check if buffer allocated ok
						if( SA_Buffer == nil ) return(-1);
						
						// Copy over contents of old smaller buffer
						memcpy( SA_Buffer, SA_Temp, SA_Len );
						DisposePtr( (Ptr) SA_Temp ); // Don't need this old buffer anymore
					  }
					  // Copy new stuff
					  VR_StrPtr+=4;
					  memcpy( SA_Buffer+SA_Len, VR_StrPtr, VR_Long );

					  // Update Len
					  SA_Len += VR_Long;
					}
				break;
			case  37:  //')':       // End of Sub-Arg
					CodePos++;
					looper++;
				break;
			case  36:  //'(':       // Start of Sub-Arg
					CodePos++;
				break;
			case  40:  //= etc
			case  41:
			case  42:
			case  43:
			case  44:
			case  45:
			case  46:
			case  47:
			case  48:
				looper++;
				break;
			default:
				ErrHandler( 1042, "\p in string argument");

				looper++;
				break;
		}
	}
	ReturnStrLen = SA_Len;
	ReturnStr = SA_Buffer;
	ReturnStr[ReturnStrLen] = 0;  //Zero Term, just for programming convenience
	return(0);
}
