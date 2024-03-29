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
#include        <serial.h>
#include        <Serial.h>

#define         UCHAR   unsigned char

short  SerInA=-1,SerOutA=-1,SerInB=-1,SerOutB=-1;
Ptr	SerBuf1, SerBuf2;

void ErrHandler( short ErrNum, Str255 AddMsg);

short SER_BaudConfig( long baud, short databits, short stopbits, short parity );
long SER_CharAvail(short portNum);
short SER_Open( short portNum, short parms );
short SER_SetParm( short portNum, short parms );
short SER_Write( short portNum, unsigned char *buf, long buflen );
long SER_Read( short portNum, unsigned char *buf, long numToRead );
short SER_HandShake( short portNum, unsigned char *buf );
short SER_Close( short portNum );
short SER_Check( short portNum );

EXTERN_API( OSErr )
SerGetBuf						(short 					refNum,
								 long *					count);
EXTERN_API( OSErr )
SerSetBuf						(short 					refNum,
								 Ptr 					serBPtr,
								 short 					serBLen);

EXTERN_API( OSErr )
SerReset						(short 					refNum,
								 short 					serConfig);
EXTERN_API( OSErr )
SerHShake						(short 					refNum,
								 const SerShk *			flags);

long SER_CharAvail(short portNum)
{
	long	count=0;
	OSErr	err;
	
	switch( portNum )
	{
		case 1:
			if( SerInA == -1 ) { ErrHandler( 1026, "\pin SerAvail function"); return(-1); }
			err = SerGetBuf( SerInA, &count );
			break;	
		case 2:
			if( SerInB == -1 ) { ErrHandler( 1026, "\pin SerAvail function"); return(-1); }
			err = SerGetBuf( SerInB, &count );
			break;	
		default:
			// Invalid Port #
			ErrHandler( 1026, "\pin SerAvail function");
			return(-1);
			break;
	}
	return(count); 
}

short SER_BaudConfig( long baud, short parity , short databits, short stopbits)
{
	short parm=0;

	switch( baud )
	{
		case 300:
			parm += 380;
			break;
		case 600:
			parm += 189;
			break;
		case 1200:
			parm += 94;
			break;
		case 1800:
			parm += 62;
			break;
		case 2400:
			parm += 46;
			break;
		case 3600:
			parm += 30;
			break;
		case 4800:
			parm += 22;
			break;
		case 7200:
			parm += 14;
			break;
		case 9600:
			parm += 10;
			break;
		case 19200:
			parm += 4;
			break;
		case 57600:
			parm += 0;
			break;
		default:
			return(-1);
			break;
	}	
	switch( databits )
	{
		case	5:
			parm += 0;
			break;
		case	6:
			parm += 2048;
			break;
		case	7:
			parm += 1024;
			break;
		case	8:
			parm += 3072;
			break;
		default:
			return(-1);
			break;
	}
	switch( parity )
	{
		case	0:
			parm += 0;
			break;
		case	1:
			parm += 2048;
			break;
		case	2:
			parm += 1024;
			break;
		default:
			return(-1);
			break;
	}
	switch( stopbits )
	{
		case	1:
			parm += 16384;
			break;
		case	2:
			parm += -16384;
			break;
		case	3:
			parm += -32768;
			break;
		default:
			return(-1);
			break;
	}
					
	return(parm);
}

short SER_Open( short portNum, short parms )
{
	short 	err;
	SerShk	flags;

	flags.fXOn = false;
	flags.fInX = false;
	flags.xOn  = 17;
	flags.xOff = 19;
	flags.errs = 0;
	flags.evts = 0;
	flags.fCTS = true;
	flags.fDTR = true;


	switch( portNum )
	{
		case 1:
			if( SerInA != -1 || SerOutA != -1 ) SER_Close( 1 );
			OpenDriver("\p.AIn", &SerInA);
			OpenDriver("\p.AOut", &SerOutA);
			// Setup baud etc...
			err = SerReset( SerInA, parms );
			 if(err == -1) { ErrHandler( 1022, "\pin SerOpen command"); return(-1); }
			err = SerReset( SerOutA, parms );
			 if(err == -1) { ErrHandler( 1022, "\pin SerOpen command"); return(-1); }
			// Setup input buffer size	
			SerBuf1 = NewPtr(1000);
			 if(!SerBuf1) { ErrHandler( 1001, "\pin SerOpen command"); return(-1); }
			err = SerSetBuf( SerInA, SerBuf1, 1000 );
			 if(err == -1){ ErrHandler( 1022, "\pin SerOpen command"); return(-1); }
			// Setup handshaking
			err = SerHShake( SerInA, &flags );
			 if(err == -1) { ErrHandler( 1022, "\pin SerOpen command"); return(-1); }
			break;	
		case 2:
			if( SerInB != -1 || SerOutB != -1 ) SER_Close( 2 );
			OpenDriver("\p.BIn", &SerInB);
			OpenDriver("\p.BOut", &SerOutB);
			// Setup baud etc...
			err = SerReset( SerInB, parms );
			 if(err == -1) { ErrHandler( 1022, "\pin SerOpen command"); return(-1); }
			err = SerReset( SerOutB, parms );
			 if(err == -1) { ErrHandler( 1022, "\pin SerOpen command"); return(-1); }
			// Setup input buffer size	
			SerBuf2 = NewPtr(1000);
			 if(!SerBuf2) { ErrHandler( 1001, "\pin SerOpen command"); return(-1); }
			err = SerSetBuf( SerInB, SerBuf2, 1000 );
			 if(err == -1) { ErrHandler( 1022, "\pin SerOpen command"); return(-1); }
			// Setup handshaking
			err = SerHShake( SerInB, &flags );
			 if(err == -1) { ErrHandler( 1022, "\pin SerOpen command"); return(-1); }
			break;	
		default:
			// Invalid Port #
			ErrHandler( 1027, "\pin SerOpen command");
			return(-1); 
			break;
	}
	return(0);
}

short SER_HandShake( short portNum, unsigned char *buf )
{

	short 	err;
	SerShk	flags;

	// Ok this is an explanation of buf
	// it is a 6 char string, as follows pos. 0-5
	// 0= Ascii char for XOn     1= Ascii char for XOff
	// 2= T=XOn flow control on  3= T=XOff flow control on
	// 4= T=CTS enable           5= T=DTR enable

	flags.fXOn = false;
	flags.fInX = false;
	flags.xOn  = 17;
	flags.xOff = 19;
	flags.errs = 0;
	flags.evts = 0;
	flags.fCTS = false;
	flags.fDTR = false;

	switch( portNum )
	{
		case 1:
			if( SerInA != -1 || SerOutA != -1 ) SER_Close( 1 );
			// Setup handshaking
			err = SerHShake( SerInA, &flags );
			 if(err == -1) return(-1);
			break;	
		case 2:
			if( SerInB != -1 || SerOutB != -1 ) SER_Close( 2 );
			// Setup handshaking
			err = SerHShake( SerInB, &flags );
			 if(err == -1) return(-1);
			break;	
		default:
			// Invalid Port #
			return(-1);
			break;
	}
	return(0);
}


short SER_SetParm( short portNum, short parms )
{
	short 	err;

	switch( portNum )
	{
		case 1:
			if( SerInA != -1 || SerOutA != -1 ) { ErrHandler( 1026, "\pin SerSet command"); return(-1); }
			// Setup baud etc...
			err = SerReset( SerInA, parms );
			 if(err == -1) { ErrHandler( 1022, "\pin SerSet command"); return(-1); }
			err = SerReset( SerOutA, parms );
			 if(err == -1) { ErrHandler( 1022, "\pin SerSet command"); return(-1); }
			break;	
		case 2:
			if( SerInB != -1 || SerOutB != -1 ) { ErrHandler( 1026, "\pin SerSet command"); return(-1); }
			// Setup baud etc...
			err = SerReset( SerInB, parms );
			 if(err == -1){ ErrHandler( 1022, "\pin SerSet command"); return(-1); }
			err = SerReset( SerOutB, parms );
			 if(err == -1) { ErrHandler( 1022, "\pin SerSet command"); return(-1); }
			break;	
		default:
			// Invalid Port #
			ErrHandler( 1027, "\pin SerOpen command");
			return(-1); 
			break;
	}
	return(0);
}


short SER_Write( short portNum, unsigned char *buf, long buflen )
{
	short		a;
	long		count;

	count	= buflen;
	switch( portNum )
	{
		case 1:
			if( SerOutA == -1 ) { ErrHandler( 1026, "\pin SerWrite command"); return(-1); }
			a = FSWrite( SerOutA, &count, buf );
			if(a) 	ErrHandler( 1036, "\pin SerWrite command");
			break;	
		case 2:
			if( SerOutB == -1 ){ ErrHandler( 1026, "\pin SerWrite command"); return(-1); }
			a = FSWrite( SerOutB, &count, buf );
			break;	
		default:
			// Invalid Port #
			ErrHandler( 1027, "\pin SerWrite command");
			return(-1); 
			break;
	}
	return(0);
}

long SER_Read( short portNum, unsigned char *buf, long numToRead )
{
	long		count;

	count	= numToRead;
	switch( portNum )
	{
		case 1:
			if( SerInA == -1 ) { ErrHandler( 1026, "\pin SerRead function"); return(-1); }
			FSRead( SerInA, &count, buf );
			break;	
		case 2:
			if( SerInB == -1 ){ ErrHandler( 1026, "\pin SerRead function"); return(-1); }
			FSRead( SerInB, &count, buf );
			break;	
		default:
			// Invalid Port #
			ErrHandler( 1027, "\pin SerRead function");
			return(-1); 
			break;
	}
	return(count);
}

short SER_Close( short portNum )
{

	switch( portNum )
	{
		case 1:
			if( SerInA == -1 && SerOutA == -1 ) return(-1);
			if( SerInA != -1 ) CloseDriver(SerInA);
			if( SerOutA != -1 ) CloseDriver(SerOutA);
			if( SerBuf1) DisposePtr( SerBuf1 );
			break;	
		case 2:
			if( SerInB == -1 && SerOutB == -1 ) return( -1 );
			if( SerInB != -1 ) CloseDriver(SerInB);
			if( SerOutB != -1 ) CloseDriver(SerOutB);
			if( SerBuf2) DisposePtr( SerBuf2 );
			break;	
		default:
			// Invalid Port #
			ErrHandler( 1027, "\pin SerClose command");
			return(-1); 
			break;
	}
	return(0);
}

short TestPortOut, TestPortIn;

short SER_Check( short portNum )
{
	OSErr	err;
	
	switch( portNum )
	{
		case 1:
			err = OpenDriver( "\p.AOut", &TestPortOut);
			if( err != noErr ) return 0;
			err = OpenDriver( "\p.AIn", &TestPortIn);
			if( err != noErr ) { CloseDriver( TestPortOut ); return 0; }
			CloseDriver( TestPortOut );
			CloseDriver( TestPortIn );
			return 1;
			break;		
		case 2:
			err = OpenDriver( "\p.BOut", &TestPortOut);
			if( err != noErr ) return 0;
			err = OpenDriver( "\p.BIn", &TestPortIn);
			if( err != noErr ) { CloseDriver( TestPortOut ); return 0; }
			CloseDriver( TestPortOut );
			CloseDriver( TestPortIn );
			return 1;
			break;		
	}
	return 0;
}
