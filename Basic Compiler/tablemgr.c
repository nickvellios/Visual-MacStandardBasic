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

#include	"Structs1.h"

struct ObjectPropsStruct1  *PropPtr1;    //global now
struct ObjectPropsStruct2  *PropPtr2;	 //global now


extern unsigned char *buffer, *project_bp, temp[100], CurrSrc[65];
extern short pjt_ctr, pjt_pos, pjt_len, current_module_num;
extern unsigned char parsed_line[1024];
extern short pline_pos;
extern long StrConCtr;
extern long VarSzCtr;
extern unsigned char ParList[12], DimNum;
extern long ParVar[12], DimList[12];
extern short ReturnType;
extern short SelectCtr, SelectArray[32], SelectCurr;
extern short p2_startflag;



// Start Runtime Structures  *******************************

/*extern struct RunVarStruct // This Data Structure holds runtime info
{       
	long    NumOfVars, NumOfCalls, NumOfLabels, CodePtr;
	struct  CallTableStruct1 *CallTable1;  
	struct  CallTableStruct2 *CallTable2;  
	struct  VarTableStruct1 *VarTable1;    
	struct  VarTableStruct2 *VarTable2;    
	struct  LabelTableStruct *LabelTable;
	struct  SelectTableStruct *SelectTable;
	unsigned char * StrTable;
	unsigned char * CodeTable;
	unsigned char * GlobalVarSpace;
} RunVar;
*/
// End of Runtime Structures  *******************************

short add_CallTable_page();
short add_VarTable_page();
short add_LabelTable_page();
short add_CodeTable_page();
short add_StringTable_page();
short getspot_InLabelTable();
void ReleaseTables();
short getspot_InCallTable();
short getspot_InVarTable();
short getspot_InSelectTable();
short AddToVarTable( char *varname, short vartype );
short AddToCallTable( char *callname, short calltype );
short AddToLabelTable( char *labelname);
short AddToSelectTable( short status, short casenum, long value );
short SetLabel( short labelnum, long location );
long GetLabelLoc( short labelnum );
short FindLabel( char * lbl );
short WriteParseLine();
long StoreStrCon(unsigned char * st1);
unsigned char * GetStrCon( long);
short GetCallPars( short d );
short UpdateCodePage(long location, long where);
long GetCallVMS( short d );
short SetCallVMS( short d, long value99 );
short SetCallLocation( short d, long value99 );
short add_SelectTable_page();
short add_PropTable_page();
short getspot_InPropTable();
short AddToPropTable( );


short add_CallTable_page()
{
	// Allocates another CallTable page
	// Returns: 0 if no page available  or  1=OK
	if(GlobalData.Num_CTP>99) return(0);    // Too many pages
	
	GlobalData.CallTable1Page[GlobalData.Num_CTP] = (struct CallTableStruct1 *) malloc( sizeof( CallTable1 ) * 100 );
	
	// Was memory alloc OK?
	if( GlobalData.CallTable1Page[GlobalData.Num_CTP] == NULL ) return(0);  //No

	GlobalData.CallTable2Page[GlobalData.Num_CTP] = (struct CallTableStruct2 *) malloc( sizeof( CallTable2 ) * 100 );

	if( GlobalData.CallTable2Page[GlobalData.Num_CTP] == NULL )
	{
		// Memory Alloc Error
		// So, 1st free up tablepage that was allocated
		free( GlobalData.CallTable1Page[GlobalData.Num_CTP] );  
		return(0);  //No
	}
	GlobalData.Num_CTP++;
	return(1);              
}

short add_VarTable_page()
{
	// Allocates another VarTable page
	// Returns: 0 if no page available  or  1=OK

	if(GlobalData.Num_VTP>99) return(0);    // Too many pages
	
	GlobalData.VarTable1Page[GlobalData.Num_VTP] = (struct  VarTableStruct1 *) malloc( sizeof( VarTable1 ) * 100 );
	
	// Was memory alloc OK?
	if( GlobalData.VarTable1Page[GlobalData.Num_VTP] == NULL ) return(0);  //No

	GlobalData.VarTable2Page[GlobalData.Num_VTP] = (struct  VarTableStruct2 *) malloc( sizeof( VarTable2 ) * 100 );
	// Was memory alloc OK?
	if( GlobalData.VarTable2Page[GlobalData.Num_VTP] == NULL )
	{
		// Memory Alloc Error
		// So, 1st free up tablepage that was allocated
		free( GlobalData.VarTable1Page[GlobalData.Num_VTP] );  
		return(0);  //No
	}

	GlobalData.Num_VTP++;
	return(1);              
}

short add_LabelTable_page()
{
	// Allocates another LabelTable page
	// Returns: 0 if no page available  or  1=OK
	if(GlobalData.Num_LTP>99) return(0);    // Too many pages
	
	GlobalData.LabelTablePage[GlobalData.Num_LTP] = (struct LabelTableStruct *) malloc( sizeof( LabelTable ) * 100 );
	
	// Was memory alloc OK?
	if( GlobalData.LabelTablePage[GlobalData.Num_LTP] == NULL ) return(0);  //No

	GlobalData.LabelLocPage[GlobalData.Num_LTP] = (long *) malloc( sizeof( long ) * 100 );

	if( GlobalData.LabelLocPage[GlobalData.Num_LTP] == NULL )
	{
		// Memory Alloc Error
		// So, 1st free up tablepage that was allocated
		free( GlobalData.LabelTablePage[GlobalData.Num_LTP] );  
		return(0);  //No
	}
	// Mem Alloc OK 
	GlobalData.Num_LTP++;

	return(1);              
}

short add_SelectTable_page()
{
	// Allocates another LabelTable page
	// Returns: 0 if no page available  or  1=OK
	if(GlobalData.Num_LTP>99) return(0);    // Too many pages
	
	GlobalData.SelectTablePage[GlobalData.Num_SCTP] = (struct SelectTableStruct *) malloc( sizeof( SelectTable ) * 100 );
	
	// Was memory alloc OK?
	if( GlobalData.SelectTablePage[GlobalData.Num_SCTP] == NULL ) return(0);  //No

	GlobalData.Num_SCTP++;

	return(1);              
}


short add_CodeTable_page()
{
	// Allocates another CodeTable page
	// Returns: 0 if no page available  or  1=OK
	if(GlobalData.Num_CDTP>99) return(0);    // Too many pages
	
	GlobalData.CodeTablePage[GlobalData.Num_CDTP] = (unsigned char *) malloc( 16384 );
	
	// Was memory alloc OK?
	if( GlobalData.CodeTablePage[GlobalData.Num_CDTP] == NULL ) return(0);  //No
	
	GlobalData.Num_CDTP++;
	return(1);              
}

short add_StringTable_page()
{
	// Allocates another CodeTable page
	// Returns: 0 if no page available  or  1=OK
	if(GlobalData.Num_STP>99) return(0);    // Too many pages
	
	GlobalData.StringTablePage[GlobalData.Num_STP] = (unsigned char *) malloc( 16384 );
	
	// Was memory alloc OK?
	if( GlobalData.StringTablePage[GlobalData.Num_STP] == NULL ) return(0);  //No
	
	GlobalData.Num_STP++;
	return(1);              
}

short add_PropTable_page()
{
	// Allocates another PropTable page
	// Returns: 0 if no page available  or  1=OK

	if(GlobalData.Num_PTP>99) return(0);    // Too many pages
	
	GlobalData.PropTable1Page[GlobalData.Num_PTP] = (struct  ObjectPropsStruct1 *) malloc( sizeof( PropTable1 ) * 100 );
	
	// Was memory alloc OK?
	if( GlobalData.PropTable1Page[GlobalData.Num_PTP] == NULL ) return(0);  //No

	GlobalData.PropTable2Page[GlobalData.Num_PTP] = (struct  ObjectPropsStruct2 *) malloc( sizeof( PropTable2 ) * 100 );
	// Was memory alloc OK?
	if( GlobalData.PropTable2Page[GlobalData.Num_PTP] == NULL )
	{
		// Memory Alloc Error
		// So, 1st free up tablepage that was allocated
		free( GlobalData.PropTable1Page[GlobalData.Num_PTP] );  
		return(0);  //No
	}

	GlobalData.Num_PTP++;
	return(1);              
}


short getspot_InLabelTable()               // Returns short position in table or -1 if error
{ 
	// This function finds open spot for Func/Sub in CallTable 
	// Returns: -1 if none   or  # of spot available
	short a;
	a = GlobalData.NumOfLabels % 100; 
	if( GlobalData.NumOfLabels>0 && !a)         // Need new page?
	{
		if( GlobalData.Num_LTP > 99 ) return(-1);       // Too many
		a = add_LabelTable_page();
		if( !a ) return(-1);
	}
	a = GlobalData.NumOfLabels;
	GlobalData.NumOfLabels++;
	return(a); 
}

void ReleaseTables()
{
	// This function releases memory 'pages' reserved for CallTable's & VarTable's
	
	short a;
	// Let's release the SelectTable's now
	for( a = 0; a < GlobalData.Num_SCTP; a++)
	{
		free( GlobalData.SelectTablePage[a]);
	}
	// Let's release the StrTable's now
	for( a = 0; a < GlobalData.Num_STP; a++)
	{
		free( GlobalData.StringTablePage[a]);
	}
	// Let's release the CodeTable's now
	for( a = 0; a < GlobalData.Num_CDTP; a++)
	{
		 free(GlobalData.CodeTablePage[a]);
	}
	// Let's release the LabelTable's now
	for( a = 0; a < GlobalData.Num_LTP; a++)
	{
		 free( GlobalData.LabelTablePage[a] );
		 free( GlobalData.LabelLocPage[a] );
	}
	// Let's release the VarTable's now
	for( a = 0; a < GlobalData.Num_VTP; a++)
	{
		 free( GlobalData.VarTable1Page[a] );
		 free( GlobalData.VarTable2Page[a] );
	}
	// Let's release the CallTable's 
	for( a = 0; a < GlobalData.Num_CTP; a++)
	{
		free( GlobalData.CallTable1Page[a] );
		free( GlobalData.CallTable2Page[a] );
	}
	// Let's release the PropTable's 
	for( a = 0; a < GlobalData.Num_PTP; a++)
	{
		free( GlobalData.PropTable1Page[a] );
		free( GlobalData.PropTable2Page[a] );
	}
	return;
 }

short getspot_InCallTable()               // Returns short position in table or -1 if error
{ 
	// This function finds open spot for Func/Sub in CallTable 
	// Returns: -1 if none   or  # of spot available
	short a;
	a = GlobalData.NumOfCalls % 100; 
	if( GlobalData.NumOfCalls > 0 && !a)            // Need new page?
	{
		if( GlobalData.Num_CTP > 99 ) return(-1);       // Too many
		a = add_CallTable_page();
		if( !a ) return(-1);
	}
	a = GlobalData.NumOfCalls;
	GlobalData.NumOfCalls++;
	return(a); 
}

short getspot_InVarTable()                // Returns short position in table or -1 if error
{ 
	// This function finds open spot for var in VarTable 
	// Returns: -1 if none   or  # of spot available
	short a;
	a = GlobalData.NumOfVars % 100; 
	if( GlobalData.NumOfVars>0 && !a)               // Need new page?
	{
		if( GlobalData.Num_VTP > 99 ) return(-1);       // Too many
		a = add_VarTable_page();
		if( !a ) return(-1);
	}
	a = GlobalData.NumOfVars;
	GlobalData.NumOfVars++;
	return(a); 
}

short getspot_InSelectTable()                // Returns short position in table or -1 if error
{ 
	// This function finds open spot for var in SelectTable 
	// Returns: -1 if none   or  # of spot available
	short a;
	a = GlobalData.NumOfSelects % 100; 
	if( GlobalData.NumOfSelects>0 && !a)               // Need new page?
	{
		if( GlobalData.Num_SCTP > 99 ) return(-1);       // Too many
		a = add_SelectTable_page();
		if( !a ) return(-1);
	}
	a = GlobalData.NumOfSelects;
	GlobalData.NumOfSelects++;
	return(a); 
}

short getspot_InPropTable()                // Returns short position in table or -1 if error
{ 
	// This function finds open spot for var in PropTable 
	// Returns: -1 if none   or  # of spot available
	short a;
	a = GlobalData.NumOfProps % 100; 
	if( GlobalData.NumOfProps>0 && !a)               // Need new page?
	{
		if( GlobalData.Num_PTP > 99 ) return(-1);       // Too many
		a = add_PropTable_page();
		if( !a ) return(-1);
	}
	a = GlobalData.NumOfProps;
	GlobalData.NumOfProps++;
	return(a); 
}


short AddToVarTable( char *varname, short vartype)
{
	// Add var to table from dim_process
	short     a, b, scope;
	struct VarTableStruct1  *BPtr1;
	struct VarTableStruct2  *BPtr2;
	
	// Need to check for dups here
	
	// No dups so grab a spot       
	a = getspot_InVarTable();
	if( a == -1 ) return( -1);   // -1 means room,tell em' "get more memory dude"
	
	b = a % 100; 
	BPtr1 = GlobalData.VarTable1Page[GlobalData.Num_VTP-1];
	BPtr2 = GlobalData.VarTable2Page[GlobalData.Num_VTP-1];
	strcpy( BPtr1[b].varname, varname );
	BPtr2[b].vartype = vartype; 
	BPtr2[b].offset = VarSzCtr; 	// Set offset
	BPtr2[b].dims = DimNum; 		// Set offset
	for(a=0;a<DimNum;a++) BPtr2[b].dimsize[a] = DimList[a];
	if(GlobalData.CompMode==21) BPtr2[b].modnum = -2;
	else if(GlobalData.CompMode==13) BPtr2[b].modnum = -1;
	else BPtr2[b].modnum = current_module_num - 1;
	//printf("Added Var to module # %i %i\n", BPtr2[b].modnum, current_module_num  );
	return(0);

}

short AddToCallTable( char *callname, short calltype )
{
	// Add var to table from dim_process
	short     a, b, scope;
	struct CallTableStruct1 *BPtr1;
	struct CallTableStruct2 *BPtr2;
	
	// No dups so grab a spot       
	a = getspot_InCallTable();
	if( a == -1 ) return( -1);   // -1 means room,tell em' "get more memory dude"
	
	b = a % 100; 
	BPtr1 = GlobalData.CallTable1Page[GlobalData.Num_CTP-1];
	BPtr2 = GlobalData.CallTable2Page[GlobalData.Num_CTP-1];
	strcpy( BPtr1[b].callname, callname );
	BPtr1[b].pjt_filenum = pjt_ctr-1;
	BPtr2[b].calltype = (char) calltype; 
	BPtr2[b].location = GlobalData.CodeCtr; 
	BPtr2[b].returntype = ReturnType;  // Used only for functions 
	for(a=0;a<10;a++)
	{
		BPtr2[b].params[a] = ParList[a];
		BPtr2[b].par_vars[a] = ParVar[a];
	}			
	return(0);
}

short AddToLabelTable( char *labelname )
{
	// Add var to table from dim_process
	short     a, b, scope;
	struct LabelTableStruct *BPtr;
	long    *BLong;

	// No dups so grab a spot       
	a = getspot_InLabelTable();
	if( a == -1 ) return( -1);   // -1 means room,tell em' "get more memory dude"
	// DEBUG CODE printf("Adding to label table %s\n", labelname);
	b = a % 100; 
	BPtr = GlobalData.LabelTablePage[GlobalData.Num_LTP-1];
	BLong = GlobalData.LabelLocPage[GlobalData.Num_LTP-1];
	strcpy( BPtr[b].labelname, labelname );
	BPtr[b].status = 1; 
	if(GlobalData.CompMode==21) BPtr[b].modnum = -1; // New for 2.3
	else BPtr[b].modnum = current_module_num;
	BLong[b] = -1;          // se to null location
	return(0);
}

short AddToSelectTable( short status, short casenum, long value )
{
	// Add var to table from dim_process
	short     a, b, scope;
	struct SelectTableStruct *BPtr;

	// No dups so grab a spot       
	a = getspot_InSelectTable();
	if( a == -1 ) return( -1);   // -1 means room,tell em' "get more memory dude"
	b = a % 100; 
	BPtr = GlobalData.SelectTablePage[GlobalData.Num_SCTP-1];
	BPtr[b].casenum = casenum; 
	BPtr[b].status = status; 
	BPtr[b].value = value; 
	BPtr[b].location = GlobalData.CodeCtr;
	//GlobalData.NumOfSelects++;
	return(0);
}

short AddToPropTable( )
{	// New to 3.0 this function will return with ptr's in global ptr's and place in table in ret value
	// Add var to table from dim_process
	short     a, b, scope;
	//struct ObjectPropsStruct1  *PropPtr1;  global now
	//struct ObjectPropsStruct2  *PropPtr2;	 global now
	
	// No dups so grab a spot       
	a = getspot_InPropTable();
	if( a == -1 ) return( -1);   // -1 means room,tell em' "get more memory dude"
	
	b = a % 100; 
	PropPtr1 = GlobalData.PropTable1Page[GlobalData.Num_PTP-1];
	PropPtr2 = GlobalData.PropTable2Page[GlobalData.Num_PTP-1];
	/*strcpy( BPtr1[b].varname, varname );
	BPtr2[b].vartype = vartype; 
	BPtr2[b].offset = VarSzCtr; 	// Set offset
	BPtr2[b].dims = DimNum; 		// Set offset
	*/
	return(b);

}

short FindLabel( char * lbl )
{
	short a, b, c, setflag=0, cm;
	struct LabelTableStruct *BPtr;
		
	//BPtr = GlobalData.LabelTablePage[GlobalData.Num_LTP-1];
	// DEBUG CODE printf("Running FindLabel [%i]\n", (short) GlobalData.NumOfLabels);
	if(p2_startflag) cm = -1;
	else cm = current_module_num+1;
	
	c = 0;
	for(a = 0; a< GlobalData.Num_LTP; a++ )
	{
		BPtr = GlobalData.LabelTablePage[a];
		for(b=0; b<100; b++)
		{
			//printf("Comparing %i with %i\n", BPtr[b].modnum, cm);
			if ( c >= GlobalData.NumOfLabels ) return(-1);
			
			if( /*1==1)*/ BPtr[b].modnum == cm )
			{
				setflag = 1;
				// DEBUG CODE 
				//printf("Comparing %s with %s\n", BPtr[b].labelname, lbl);
				if( !strcmp( BPtr[b].labelname, lbl) ) return(c);       
			}
			else
			{
				if(setflag) return(-1); 
			}
			c++;
		}
	}
	return(-1);
}

short SetLabel( short labelnum, long location )
{
	short b;
	long    *BLong;

	b = labelnum / 100; 
	if( b > GlobalData.Num_LTP-1 ) { return(-1);}

	BLong = GlobalData.LabelLocPage[b];

	b = labelnum % 100; 
	BLong[b] = GlobalData.CodeCtr;          // location
	// DEBUG CODE printf("Label Loc stored %i\n", (short)GlobalData.CodeCtr );
	return(0);
}

long GetLabelLoc( short labelnum )
{
	short b;
	long    *BLong, result;

	b = labelnum / 100; 
	if( b > GlobalData.Num_LTP-1 ) return(-1);

	BLong = GlobalData.LabelLocPage[b];

	b = labelnum % 100; 
	result = BLong[b];          // location
	
	return(result);
}


short WriteParseLine()
{
	long tctr;
	unsigned char *CPtr;
	short wtr1, wtr2, a, b;
	
	CPtr = GlobalData.CodeTablePage[GlobalData.Num_CDTP-1];
	tctr = GlobalData.CodeCtr % 16384;

	/*if(!GlobalData.CodeCtr) for(a=0;a<25;a++) CPtr[a]=33;*/
	//for(a=0;a<GlobalData.CodeCtr;a++) printf("{%i}",(short) CPtr[a]);
	//printf("\n");

	//printf("\nPg#%i  At %i \n",GlobalData.Num_CDTP-1,a);
	wtr1 = (short) tctr + pline_pos;

	if( wtr1 > 16384 )  //Multipage write
	{
		// Yes, 2 writes necessary
		// No, all on single page
		a=0;
		for(wtr2=(short) tctr; wtr2< 16384; wtr2++)  CPtr[wtr2]=parsed_line[a++];         

		b = add_CodeTable_page();
		if( !b ) SysBeep(30);
		CPtr = GlobalData.CodeTablePage[GlobalData.Num_CDTP-1];
		for(wtr2=0; wtr2 < wtr1+-16384; wtr2++)  CPtr[wtr2]=parsed_line[a++];         
	}
	else
	{
		// No, all on single page
		// DEBUG CODE printf("Single table write of %i bytes\n",pline_pos);
		wtr1 = (short) tctr;
		for(a=0; a < pline_pos; a++) CPtr[wtr1++] = parsed_line[a];             
	}
	GlobalData.CodeCtr += pline_pos;
	return( 0 );
}

short UpdateCodePage(long location, long where)
{
	long cp,g, *jm;
	unsigned char *mbuf, *CPtr;
	short tctr, a;
	
	// DEBUG CODE printf("UCP %li  %li\n", location, where);
	cp = location / 16384;
	
	CPtr = GlobalData.CodeTablePage[ cp ];
	tctr = location % 16384;

	mbuf = (unsigned char*) &where;
	//jm = (long*) (CPtr + location);
	//*jm = where;
	
	
	for(a=0; a<4; a++)
	{
	 	CPtr[tctr++] = mbuf[a];
	
		if( tctr > 16384 )
		{
			cp++;
			CPtr = GlobalData.CodeTablePage[ cp ];
			tctr = 0;
		}
	}

	return( 0 );
}


long StoreStrCon(unsigned char * st1)
{
	// st1 is a pascal type string
	unsigned char *CPtr;
	long tctr;
	short wtr1, wtr2, a, TL, ip;
	
	CPtr = GlobalData.StringTablePage[GlobalData.Num_STP-1];
	tctr = StrConCtr % 16384;
	ip = st1[0];       
	TL = st1[0] + 1;        
	wtr1 = (short) tctr + TL;

	if( wtr1 > 16384 )  //Multipage write
	{
		// Yes, 2 writes necessary
		// No, all on single page
		a=0;
		for(wtr2=(short) tctr; wtr2< 16384; wtr2++)
		    CPtr[wtr2]=st1[a++];                

		add_StringTable_page();
		CPtr = GlobalData.StringTablePage[GlobalData.Num_STP-1];
		for(wtr2=0; wtr2 < wtr1-16384; wtr2++)
		    CPtr[wtr2]=st1[a++];                
	}
	else
	{
		// No, all on single page
		wtr1 = (short) tctr;
		for(a=0; a< TL; a++) CPtr[wtr1++] = st1[a];               
	}

	a = ip + 1;    
	tctr = StrConCtr;
	StrConCtr += a; 
	return( tctr );
}

short GetCallPars( short d )
{
	short a,b;
	struct CallTableStruct2 *BPtr2;

	b = (d-1000) / 100; 
	if( b > GlobalData.Num_CTP-1 ) return(-1);

	BPtr2 = GlobalData.CallTable2Page[b];
	b = (d-1000) % 100; 
	for(a=0;a<10;a++)  ParList[a] = BPtr2[b].params[a];

	return(0);
}

long GetCallVMS( short d )
{
	short b;
	long value99;
	struct CallTableStruct2 *BPtr2;

	b = d / 100; 
	if( b > GlobalData.Num_CTP-1 ) return(-1);

	BPtr2 = GlobalData.CallTable2Page[b];
	b = d % 100; 
	value99 = BPtr2[b].VarMemSz;

	return(value99);
}

short SetCallVMS( short d, long value99 )
{
	short b;
	struct CallTableStruct2 *BPtr2;

	b = d / 100; 
	if( b > GlobalData.Num_CTP-1 ) return(-1);

	BPtr2 = GlobalData.CallTable2Page[b];
	b = d % 100; 
	BPtr2[b].VarMemSz = value99;

	return(0);
}

short SetCallLocation( short d, long value99 )
{
	short a,b;
	struct CallTableStruct2 *BPtr2;

	a = d / 100; 
	if( a > GlobalData.Num_CTP-1 ) return(-1);

	BPtr2 = GlobalData.CallTable2Page[a];
	b = d % 100; 
	BPtr2[b].location = value99;
	//printf("Setting Call %i CodePtr to %li a=%i b=%i\n", d, value99,a,b ); 

	return(0);
}


/*
unsigned char * GetStrCon( long str_pos)
{
	// st1 is a pascal type string
	unsigned char *CPtr, s1;
	long tctr;
	short wtr1, wtr2, a, TL, *ip;
	
	CPtr = GlobalData.StringTablePage[0];
	
	ip = (short *) st1;       
	TL = st1[0] + 1;        
	wtr1 = (short) tctr + TL;

	if( wtr1 > 16384 )  //Multipage write
	{
		// Yes, 2 writes necessary
		// No, all on single page
		a=0;
		for(wtr2=(short) tctr; wtr2< 16384; wtr2++)
		    CPtr[wtr2]=st1[a++];                

		add_StringTable_page();
		CPtr = GlobalData.StringTablePage[GlobalData.Num_STP-1];
		for(wtr2=0; wtr2 < wtr1-16384; wtr2++)
		    CPtr[wtr2]=st1[a++];                
	}
	else
	{
		// No, all on single page
		wtr1 = (short) tctr;
		for(a=0; a< TL; a++) CPtr[wtr1] = st1[a];               
	}

	a = 2 + *ip;    
	tctr = StrConCtr;
	StrConCtr += a; 
	return( tctr );
}
*/

 
