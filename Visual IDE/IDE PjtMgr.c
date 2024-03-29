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

// Assumes inclusion of <MacHeaders>
//#include <PrintTraps.h>
//#include <stdio.h>
//#include <pascal.h>

#include "IDE.h"
#include <DCon.h>

#define         UCHAR   unsigned char
#define		   kInFront 	(WindowPtr) -1	/* put the new window in front */
#include <stdio.h>
void AddStationary();
void Str255Copy ( Str255 st1, Str255 st2);
short GetProjectName( );
short NewProjectName( );
short SaveProject( );
short LoadProject( );
short LaunchRT(  );
short LaunchCompiler( );
short LaunchResedit( unsigned char * medit );
void  LaunchAnApplication (StandardFileReply *mySFReply, short tp);
short AddFileToProject( UCHAR * fname );
short CreateProjectWindow();
short Str255Cpy(	UCHAR *st1, UCHAR *st2);
short SaveCompileFS( );
short GetAppExe(  );
short GetMemSize(  );
short CompilerErrorLog(  );
void  KillErrorWindow();
short LaunchHelp1( );
short LaunchHelp2( );
short LaunchHelp3( );
short CreateFormWindow();
short CreatePropWindow();
short CreateErrorWindow( );
short FindNextPntProc( short ModNum, short startctr );
void PrintStuff( short ModNum, short EdProc );
void CloseErrorWindow( );

MenuHandle	ProjectMenu;
MenuHandle	FormsSubMenu;
MenuHandle	ModulesSubMenu;
MenuHandle	ResourcesSubMenu;
WindowPtr	ProjectWin;
ListHandle	PjtList;
FSSpec		PjtFS;
extern char		ProjectFlag;
extern FSSpec	IDESpec;
extern char		AppLaunchFlag;
extern Str255	AppRunName, Target;
Str255		WinTitle;
short 		PrintPageNum, gFlag;
long		MemMin = 1024, MemPref = 2048, PRinit=0;
THPrint		hPrint;
extern short FormCtr, ModuleCtr;
short StrPartCmp( char * st1, char * st2);
void zsplit();
short znext_line();
extern char *zbuffer, zline[512];
extern short zline_pos,zline_len;
extern long  zbuf_pos, zsource_len;
extern Str255	zparm1, zparm2;

extern struct EditProcStruct
{
	Handle	ProcHandle;
	short	EditWinNum;
	long	size;
}	
EditProc[1000];


// Var Declares
extern struct	EditWinStruct  // 0-19 for forms - 20-99 reserved for modules
{
	Str63			FileName;
	WindowPtr		theWindow;
   	TEHandle		te;
   	ControlHandle	theControl;		//scrollbar
   	ControlHandle	dropList;		//dropList
	MenuHandle		theMenu;
	short			status;
	Rect			theRect;
	short			ht;
	short			FormNum;
	short			change;
	short			editFlag;   // 1=New Object // 2=New Ind. Proc  // 3 existing proc
	short			currentProc;
	FSSpec			fs;
	Handle			GlobalVar;
	long			GlobalVarSize;
}
EditWin[100];


short LaunchRT(  )
{
	OSErr			error;
 	StandardFileReply  theReply;
 	//short 			sc_result;
 	
 	//sc_result = SaveCompileFS();
 	//if(sc_result < 0 ) return( -1 );
 	
	error = FSMakeFSSpec(PjtFS.vRefNum,PjtFS.parID, AppRunName, &theReply.sfFile);
	if(error)
	{
	 	return(-1); 
	}
	// Found the compiler, so launch it
	LaunchAnApplication ( &theReply, 3 );
	return(0);
}

short LaunchCompiler(  )
{
	OSErr			error;
 	StandardFileReply  theReply;
 	short 			sc_result, a;
 	FSSpec			rt;
 	
	sc_result = SaveCompileFS();
 	if(sc_result < 0 )
	{
		Alert( 143, nil );   // Program is probably running
		return -1;
	}

	// Delete old runtime first, if any
 	error = FSMakeFSSpec(PjtFS.vRefNum,PjtFS.parID, AppRunName, &rt);
	if(!error)
	{
		// Ok file exist so delete it
		error = FSpDelete( &rt);
		if( error ) 
		{
			Alert( 142, nil );   // Program is probably running
			return -1;
		}
	}
	
	CloseErrorWindow( );

	
	error = FSMakeFSSpec(IDESpec.vRefNum, IDESpec.parID, "\p:Tools:Visual MSB Compiler", &theReply.sfFile);
	if(error)
	{
		Alert( 133, nil );
	 	return(-1); 
	}
	
	// Need to close all open res files before calling compiler, we will reopen them
	// when the compiler is done.
	/*for( a =0; a<100;a++ )
	{
		if( ResRef[a] == -1 ) break;
		else if( ResRef[a] != -2 )  { CloseResFile( ResRef[a] ); ResRef[a] = -1; }
	} Not needed anymore */

	// Found the compiler, so launch it
	LaunchAnApplication ( &theReply, 2 );

	return(0);
}

// ***Time Limit Routine
short	grabdate2( )
{
	short a;
	DateTimeRec	now;
	
	GetTime( &now);
	if( now.year !=1997 ) return 0;
	if( now.month != 10 ) return 0;
	
	return(1);
}

short LaunchResedit( unsigned char * medit )
{
	OSErr			error;
 	StandardFileReply  theReply;
 	short 			 a=1;
 	Str255			fn;
	
	while( medit[a] !=']' ) { fn[a] = medit[a+1]; a++;}
	fn[0] = medit[0]-2;
 	
	error = FSMakeFSSpec(PjtFS.vRefNum,PjtFS.parID, fn, &theReply.sfFile);
	if(error)
	{
		{SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);}	 	return(-1); 
	}
	// Found the compiler, so launch it
	LaunchAnApplication ( &theReply, 4 );

	return(0);
}

/*short LaunchHelp1( )
{
	OSErr			error;
 	StandardFileReply  theReply;
 	
	error = FSMakeFSSpec( IDESpec.vRefNum,IDESpec.parID, "\pVisual MSB User's Guide", &theReply.sfFile);
	if(error)
	{
		Alert( 135, nil );
	 	return(-1); 
	}
	// Found the compiler, so launch it
	LaunchAnApplication ( &theReply, 1 );

	return(0);
}

short LaunchHelp2( )
{
	OSErr			error;
 	StandardFileReply  theReply;
 	
	error = FSMakeFSSpec( IDESpec.vRefNum,IDESpec.parID, "\pVisual MSB Reference", &theReply.sfFile);
	if(error)
	{
		Alert( 135, nil );
	 	return(-1); 
	}
	// Found the compiler, so launch it
	LaunchAnApplication ( &theReply, 1 );

	return(0);
}

short LaunchHelp3( )
{
	OSErr			error;
 	StandardFileReply  theReply;
 	
	error = FSMakeFSSpec( IDESpec.vRefNum,IDESpec.parID, "\pVisual MSB Tutorial", &theReply.sfFile);
	if(error)
	{
		Alert( 135, nil );
	 	return(-1); 
	}
	// Found the compiler, so launch it
	LaunchAnApplication ( &theReply, 1 );

	return(0);
}*/

void LaunchAnApplication (StandardFileReply *mySFReply, short tp)
{
	LaunchParamBlockRec		myLaunchParams;
	ProcessSerialNumber		launchedProcessSN;
	OSErr	launchErr;
	long		prefSize;
	long		minSize;
	long		availSize;

	myLaunchParams.launchBlockID = extendedBlock;
	myLaunchParams.launchEPBLength = extendedBlockLen;
	myLaunchParams.launchFileFlags = 0;
	myLaunchParams.launchControlFlags = launchContinue + launchNoFileFlags;
	myLaunchParams.launchAppSpec = &(mySFReply->sfFile);
	myLaunchParams.launchAppParameters = nil;

	launchErr = LaunchApplication(&myLaunchParams);
	prefSize  = myLaunchParams.launchPreferredSize;
	minSize   = myLaunchParams.launchMinimumSize;
	if (!launchErr)
		launchedProcessSN = myLaunchParams.launchProcessSN;
	else
	{
		switch( tp )
		{
			case 1:  //help
				Alert( 139, nil );
				break;
			case 2:  //comp
				Alert( 138, nil );
				break;
			case 3:  //rt
				Alert( 140, nil );
				break;
			case 4:  //resedit
				Alert( 141, nil );
				break;
		}
	}
		/*  if (launchErr == memFullErr)
		availSize = myLaunchParams.launchAvailableSize;*/
   //	else
		//DoError(launchErr);
}

short AddFileToProject( UCHAR * fname )
{
	// Returns # of file in project that was added
	int		NumOfItems;

	// Let's add item to Project Menu
	AppendMenu( ProjectMenu, fname );
	NumOfItems = CountMItems( ProjectMenu );	

	SaveProject( );	
	
	return( NumOfItems );
}

short CompilerErrorLog(  )
{
	OSErr			error;
 	FSSpec			rt;
 	
	//KillErrorWindow();

	error = FSMakeFSSpec(IDESpec.vRefNum,IDESpec.parID, "\p:Tools:ERRORLOG", &rt);
	if(error)
	{
		// This means no errors during compile
	 	return(0); 
	}

	// Found so open it
	CreateErrorWindow( );
	

	FSpDelete( &rt);  // Now it is displayed so delete it
	{SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);}
	return(0);
}



short GetProjectName( )
{
	SFTypeList		typeList;
	short			numTypes;
 	StandardFileReply  theReply;
 	
	typeList[0] = 'VmSB';
	numTypes = 1;
	StandardGetFile( nil, numTypes, typeList, &theReply );
	if( theReply.sfGood )
	{
		if(ProjectFlag)
		{
			// Ok a project is already open
		}
		
		
		//CreateFormWin();
		//load_source_file( &theReply, CurrWin );
	
	}	
	return(0);

}

short NewProjectName( )
{
	short a;
	StandardFileReply	theReply;
	
	MemMin = 1024;
	MemPref = 2048;
	// Pop Up Save Window
	StandardPutFile("\pEnter New Project Name", (UCHAR*) "\p", &theReply ); 
	if( theReply.sfGood )
	{
		Str255Cpy( AppRunName, "\pAppName" );
		//a = GetAppExe();
		//if(!a) return(0);
		PjtFS.vRefNum  = theReply.sfFile.vRefNum;
		PjtFS.parID  = theReply.sfFile.parID;
		Str255Copy( theReply.sfFile.name, PjtFS.name );

		ProjectMenu = NewMenu( 7, PjtFS.name);
		InsertMenu( ProjectMenu, 0 );

		FormsSubMenu = NewMenu( 101, "\pForms");
		//AppendMenu( FormsSubMenu, "\pCreate New Form");
		//SetItemStyle( FormsSubMenu, 1, 2 );
		InsertMenu( FormsSubMenu, -1 );

		ModulesSubMenu = NewMenu( 102, "\pModules");
		//AppendMenu( ModulesSubMenu, "\pCreate New Module");
		//SetItemStyle( ModulesSubMenu, 1, 2 );
		InsertMenu( ModulesSubMenu, -1 );

		ResourcesSubMenu = NewMenu( 103, "\pResources");
		InsertMenu( ResourcesSubMenu, -1 );


		AppendMenu( ProjectMenu, "\pForms" );
		AppendMenu( ProjectMenu, "\pModules" );
		AppendMenu( ProjectMenu, "\pResources" );


		SetItemCmd( ProjectMenu, 1, 0x1b );
		SetItemMark( ProjectMenu, 1, 101 );
		SetItemCmd( ProjectMenu, 2, 0x1b );
		SetItemMark( ProjectMenu, 2, 102 );
		SetItemCmd( ProjectMenu, 3, 0x1b );
		SetItemMark( ProjectMenu, 3, 103 );
		
		ProjectFlag = 1;
		DrawMenuBar();
		CreateFormWindow();
		CreatePropWindow();
		AddStationary();
		SaveProject();
		return(1);
	}
	else return(0);	
    
}

short SaveProject( )
{
	// This function saves project file
	short   rn, a, b, NumOfFiles, *sp;
	long	bl=38;
	OSErr   error;
	Str255	sc;
	char buf[512], hdr1[]="MacStandardBasic Project Version 4.1\x0d\x0d";
	
	//Time Limit
	//if( !grabdate2( ) ) exit(0);
	
	error = FSpOpenDF( &PjtFS, fsRdWrShPerm, &rn);
	if(error)
	{
		if( error == -43 )
		{
			//{SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);}
			FSpCreateResFile( &PjtFS, 'VmSB', 'BPjt', smSystemScript);
			error = FSpOpenDF( &PjtFS, fsRdWrShPerm, &rn);
			if(error) return(-1);
		}
		else return(-1);
	}
	else
	{
		// File Already Exists
		// So close & delete it
		error = FSClose( rn);
		error = FSpDelete( &PjtFS );

		FSpCreateResFile( &PjtFS, 'VmSB', 'BPjt', smSystemScript);
		error = FSpOpenDF( &PjtFS, fsRdWrShPerm, &rn);
		if(error) return(-1);
	}

	// Let's write out filenames of all project

	error = FSWrite( rn, &bl, hdr1);
	
	a = AppRunName[0]; AppRunName[a+1]=0;
	bl = sprintf( buf, "    AppName=%s\x0d", AppRunName+1);
	error = FSWrite( rn, &bl, buf);

	a = Target[0]; Target[a+1]=0;
	bl = sprintf( buf, "    Target=%s\x0d", Target+1);
	error = FSWrite( rn, &bl, buf);

	bl = sprintf( buf, "    MemMin=%i\x0d", MemMin);
	error = FSWrite( rn, &bl, buf);
	bl = sprintf( buf, "    MemPref=%i\x0d", MemPref);
	error = FSWrite( rn, &bl, buf);

	bl = sprintf( buf, "    FormCtr=%i\x0d", FormCtr);
	error = FSWrite( rn, &bl, buf);
	bl = sprintf( buf, "    ModuleCtr=%i\x0d", ModuleCtr);
	error = FSWrite( rn, &bl, buf);

	
	NumOfFiles = CountMItems( FormsSubMenu );
	for( a = 1; a <= NumOfFiles; a++)
	{
		//GetItem( ProjectMenu, a, sc);
		GetMenuItemText( FormsSubMenu, a, sc);
		b = sc[0]; sc[b+1]=0;
		bl = sprintf( buf, "    Form=%s\x0d", sc+1);
		error = FSWrite( rn, &bl, buf);
	}

	NumOfFiles = CountMItems( ModulesSubMenu );
	for( a = 1; a <= NumOfFiles; a++)
	{
		//GetItem( ProjectMenu, a, sc);
		GetMenuItemText( ModulesSubMenu, a, sc);
		b = sc[0]; sc[b+1]=0;
		bl = sprintf( buf, "    Module=%s\x0d", sc+1);
		error = FSWrite( rn, &bl, buf);
	}
	
	NumOfFiles = CountMItems( ResourcesSubMenu );
	for( a = 1; a <= NumOfFiles; a++)
	{
		//GetItem( ProjectMenu, a, sc);
		GetMenuItemText( ResourcesSubMenu, a, sc);
		b = sc[0]; sc[b+1]=0;
		bl = sprintf( buf, "    Resource=%s\x0d", sc+1);
		error = FSWrite( rn, &bl, buf);
	}
	error = FSClose( rn);
	
	return(0);

}

short LoadProject( )
{
	long    FLen;
	short   rn, *sp, a, z1, marker=0;
	OSErr   error;
	UCHAR	sc[66];
	long bl;
	Handle	hbuf;
	
	error = FSpOpenDF( &PjtFS, fsRdWrShPerm, &rn);
	if(error)
	{
		return(-1);
	}

	// Build Project Menu Structure
	ProjectMenu = NewMenu( 7, PjtFS.name);
	InsertMenu( ProjectMenu, 0 );

	FormsSubMenu = NewMenu( 101, "\pForms");
	//AppendMenu( FormsSubMenu, "\pCreate New Form");
	//SetItemStyle( FormsSubMenu, 1, 2 );
	InsertMenu( FormsSubMenu, -1 );

	ModulesSubMenu = NewMenu( 102, "\pModules");
	//AppendMenu( ModulesSubMenu, "\pCreate New Module");
	//SetItemStyle( ModulesSubMenu, 1, 2 );
	InsertMenu( ModulesSubMenu, -1 );

	ResourcesSubMenu = NewMenu( 103, "\pResources");
	InsertMenu( ResourcesSubMenu, -1 );

	AppendMenu( ProjectMenu, "\pForms" );
	AppendMenu( ProjectMenu, "\pModules" );
	AppendMenu( ProjectMenu, "\pResources" );

	SetItemCmd( ProjectMenu, 1, 0x1b );
	SetItemMark( ProjectMenu, 1, 101 );
	SetItemCmd( ProjectMenu, 2, 0x1b );
	SetItemMark( ProjectMenu, 2, 102 );
	SetItemCmd( ProjectMenu, 3, 0x1b );
	SetItemMark( ProjectMenu, 3, 103 );
	

	GetEOF( rn, &bl );
	zsource_len = bl;

	hbuf = NewHandle( bl );
	if( hbuf == nil )
	{
		FSClose( rn );	
		return -1;
	}
	HLock( hbuf );
	zbuffer = (*hbuf );
	FSRead( rn, &bl, zbuffer );	   // Write header
	FSClose( rn );
	zbuf_pos = 0;
	znext_line();
	if( !StrPartCmp( zline,"MacStandardBasic Project" ) ) { {SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);}return -1; }

	a = znext_line();
	if( !a) if( StrPartCmp( zline,"AppName" ) )  { zsplit(); Str255Copy( zparm2, AppRunName ); }
	a = znext_line();
	if( !a) if( StrPartCmp( zline,"Target" ) )
	{
		zsplit();
		Str255Copy( zparm2, Target );
		// Do check to convert from < Ver4.0
		if( Target[0] == 1 ) // probably so
		{
			if( Target[1] == '1' ) { marker=1; Str255Copy( "\pStandard 68K Library", Target ); ProjectFlag=1; }
			if( Target[1] == '2' ) { marker=1; Str255Copy( "\pStandard PPC Library", Target ); ProjectFlag=1; }
			if( Target[1] == '3' ) { marker=1; Str255Copy( "\pStandard FAT Library", Target ); ProjectFlag=1; }
		}
	}
	//  { zsplit(); StringToNum( zparm2, &bl ); Target = (short) bl;}
	a = znext_line();
	if( !a) if( StrPartCmp( zline,"MemMin" ) )  { zsplit(); StringToNum( zparm2, &bl ); MemMin = (short) bl;}
	a = znext_line();
	if( !a) if( StrPartCmp( zline,"MemPref" ) )  { zsplit(); StringToNum( zparm2, &bl ); MemPref = (short) bl;}
	a = znext_line();
	if( !a) if( StrPartCmp( zline,"FormCtr" ) )  { zsplit(); StringToNum( zparm2, &bl ); FormCtr = (short) bl;}
	a = znext_line();
	if( !a) if( StrPartCmp( zline,"ModuleCtr" ) )  { zsplit(); StringToNum( zparm2, &bl ); ModuleCtr = (short) bl;}

	a = znext_line();
	while( !a && StrPartCmp( zline,"Form" ) )
	{
		zsplit();
		if( zparm2[0] ) AppendMenu( FormsSubMenu, zparm2 );
		a = znext_line();
	}

	while( !a && StrPartCmp( zline,"Module" ) )
	{
		zsplit();
		if( zparm2[0] ) AppendMenu( ModulesSubMenu, zparm2 );
		a = znext_line();
	}

	z1=0;
	while( !a && StrPartCmp( zline,"Resource" ) )
	{
		zsplit();
		if( zparm2[0] )
		{
			AppendMenu( ResourcesSubMenu, zparm2 );
			z1++;
		}
		a = znext_line();
	}

	ProjectFlag = 1;
	if( marker) SaveProject();
	DrawMenuBar();
	
	return(0);

}

short SaveCompileFS( )
{
	// This function saves project file
	short   rn;
	long	FLen;
	OSErr   error;
	FSSpec	CplFS;
	
	CplFS.vRefNum = IDESpec.vRefNum;
	CplFS.parID = IDESpec.parID;
	Str255Cpy(CplFS.name, "\p:Tools:MSB__PJT__FS");
	
	error = FSpOpenDF( &CplFS, fsRdWrShPerm, &rn);
	if(error)
	{
		if( error == -43 )
		{
			//{SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);}
			FSpCreateResFile( &CplFS, 'VmSB', 'BPjt', smSystemScript);
			error = FSpOpenDF( &CplFS, fsRdWrShPerm, &rn);
			if(error) return(-1);
		}
		else return(-1);
	}
	else
	{
		// File Already Exists
	}


	// Let's write out project FS
	
	FLen = sizeof(FSSpec);
	error = FSWrite( rn, &FLen, (void *) &PjtFS);
	if(error) return(-1);

	error = FSClose( rn);
	if(error) return(-1);
	
	return(0);

}


//____________________________________________________________

short Str255Cpy(	UCHAR *st1, UCHAR *st2)
{
	// Copy st2 to  st1 string, both are Str255, returns len

	short a;
	for(a=0; a <= st2[0]; a++)	st1[a] = st2[a]; 
	return( st2[0] );
}  

//____________________________________________________________
//  Printing



/* Do you want to prompt for the page setup? */
Boolean	DoPageSetUp  = FALSE;

long	PrintPos;
Str255	PrintStringOfText;
Handle	hPrintText = nil;
Size	PrintTextSize;
short   OnPrintLine;

short CJ=0;

/* Prototypes */
short	GetPrintLine();
Boolean DoneDrawingPagesOfATextFile( );
void PrintInit();
void PrintSetup();

short	GetPrintLine()
{
	short PSpos=1 ,a ,b;
	unsigned char	ch;
	
	while( PSpos<=255 && PrintPos < PrintTextSize )
	{
		ch = *(*hPrintText + PrintPos);
		PrintPos++;
		if( ch==13 ) // CR
		{
			PrintStringOfText[0] = PSpos-1;
			PrintStringOfText[PSpos] = 0;
			return(1);	
		}
		else
		{
			if( ch == 9 )
			{
				a = (((PSpos/4) +1) * 4) - (PSpos-1);
				for( b = 0; b < a; b++ ) PrintStringOfText[PSpos++] = ' ';
				PSpos--;
			}
			else PrintStringOfText[PSpos] = ch;
		}
		PSpos++;
	}

	PrintStringOfText[0] = PSpos-1;
	PrintStringOfText[PSpos] = 0;
	if( PSpos ==1 ) return(0);	
	return(1);

}
void  CreatePageHdr();
void AppendStrNum( unsigned char * cp1, long num );

void AppendStrNum( unsigned char * cp1, long num )
{
	
	Str255	theNum;
	short a, b, c, d=1;
	
	NumToString( num, theNum );
	
	a = cp1[0]+1;
	b = a + theNum[0];
	for(c=a; c<b; c++) cp1[c] = theNum[d++]; 
	cp1[0] = b;
}
	
void CreatePageHdr()
{
	short	a;
	Str255	theLine;
	DateTimeRec		dt;
	
	GetTime(&dt);
	
	for(a=1;a<255;a++) theLine[a] = ' ';
	Str255Cpy( theLine, "\pVisual MacStandardBasic                                          Page # ");
	AppendStrNum( theLine, PrintPageNum);
	MoveTo(10, 14 );
	DrawString(	theLine );	
	PrintPageNum++;	
	
	for(a=1;a<255;a++) theLine[a] = ' ';
	Str255Cpy( theLine, WinTitle );
	theLine[0]=55;

	AppendStrNum( theLine, dt.month);
	a = theLine[0];
	AppendStrNum( theLine, dt.day);
	theLine[a] = '/';
	a = theLine[0];
	AppendStrNum( theLine, dt.year);
	theLine[a] = '/';
	
	a = dt.hour; if( a>12 ) a-=12;
	AppendStrNum( theLine, a);
	a = theLine[0];
	AppendStrNum( theLine, dt.minute);
	theLine[a] = ':';
	a = theLine[0]++;
	theLine[a+1] = ' ';
	if( dt.hour > 12 ) {a = theLine[0]++; theLine[a] = 'P';}
	else {a = theLine[0]++; theLine[a] = 'A';}
	a = theLine[0]++; theLine[a] = 'M';

	MoveTo(10, 26 );
	DrawString(	theLine );	
	
}

Boolean DoneDrawingPagesOfATextFile( )
{
	short	i, a;
	TextSize(10);
	//TextFont(courier);
	TextFace( bold );
	if( OnPrintLine == 4 || OnPrintLine >56 )
	{
		OnPrintLine = 4;
		CreatePageHdr();
	}
	TextFace( 0 );

	if( gFlag==1 )  // print global title
	{
		MoveTo(10, 12 * OnPrintLine);
		DrawString( "\pGlobal");
		OnPrintLine++;
		gFlag=2;
	}

	while( OnPrintLine <=56 )
	{		
			a = GetPrintLine();
			if (!a)
			{
				if( gFlag==2 )  // print global title
				{
					MoveTo(10, 12 * OnPrintLine);
					DrawString( "\pEndGlobal");
					OnPrintLine++;
					gFlag=0;
				}
				return TRUE; 
			}
			/*
			 * Carrige return characters mess up DrawString, so they are removed.
			 * Also, tabs are not handled correctly.  They are left as an exercise
			 * for the programmer!
			 */
			//if (aStringOfText[aStringOfText[0]] == '\n')	//		aStringOfText[aStringOfText[0]] = ' ';
			MoveTo(10, 12 * OnPrintLine);
			DrawString( PrintStringOfText);
			//printf("%s\n",PrintStringOfText+1);
			OnPrintLine++;
	}
	return FALSE;
}


void PrintInit()
{
	PrOpen();
	hPrint = (THPrint) NewHandle( sizeof(TPrint) );/* *not* sizeof(THPrint) */
	//if( hPrint == nil ) {SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);}
	//PrintDefault(hPrint);
}

void PrintSetup()
{
	//if( PRinit == 1) PrintDefault(hPrint);
	PRinit = 1;
	PrOpen();
	PrStlDialog(hPrint);
	PrClose();
}

short FindNextPntProc( short ModNum, short startctr )
{
	short a;

	for( a=startctr;a<1000; a++ )
	{
		if( EditProc[a].EditWinNum == ModNum)
			if( EditProc[a].ProcHandle !=nil )	return a;
	}
	return -1;
}

void PrintStuff( short ModNum, short EdProc ) 
{
	GrafPtr	savedPort;
	TPrStatus	prStatus;
	TPPrPort	printPort;
	Boolean	ok;
	DocumentHandle	hDocument;
	long 		nol;
	short 		startctr=0, ep, sf;
	
	gFlag=0;
	PrintPageNum =1;
	PrintPos = 0;
	OnPrintLine = 4;
	if( EdProc >=0 )
	{
		if( EdProc == 9999 ) // Globals
		{
			if( EditWin[ModNum].GlobalVar == nil ) return;
			hPrintText = EditWin[ModNum].GlobalVar;
			PrintTextSize = EditWin[ModNum].GlobalVarSize;
			gFlag=1;
		}
		else
		{
			if( EditProc[EdProc].ProcHandle == nil ) return;
			hPrintText = EditProc[EdProc].ProcHandle;
			PrintTextSize = EditProc[EdProc].size;
		}
	}
	else
	{
		if( EditWin[ModNum].GlobalVar != nil )
		{	// Do Global first
			hPrintText = EditWin[ModNum].GlobalVar;
			PrintTextSize = EditWin[ModNum].GlobalVarSize;
			gFlag=1;
		}
		else
		{
			ep = FindNextPntProc( ModNum, startctr );
			if( ep==-1 ) return;
			startctr = ep+1;
			hPrintText = EditProc[ep].ProcHandle;
			PrintTextSize = EditProc[ep].size;
		}
	}
	
	GetPort(&savedPort);
	PrOpen();
	//if( PRinit == 1) PrintDefault(hPrint);
	InitCursor();
	//if (DoPageSetUp) ok = PrStlDialog(hPrint);
	//else ok = PrValidate(hPrint);

	ok = PrJobDialog(hPrint);
	if (ok)
	{
			printPort = PrOpenDoc(hPrint, nil, nil);
			SetPort(&printPort->gPort);
			PrOpenPage(printPort, nil);

			while( ok )
			{
					sf = DoneDrawingPagesOfATextFile(  );
					if( sf )
					{
						if( EdProc != -1 ) ok=0;
						else
						{
							OnPrintLine++; // extra cr
							ep = FindNextPntProc( ModNum, startctr );
							if( ep==-1 ) ok=0;
							else
							{
								startctr = ep+1;
								hPrintText = EditProc[ep].ProcHandle;
								PrintTextSize = EditProc[ep].size;
								PrintPos =0;
							}
						
						}
					}
					else
					{
						PrClosePage(printPort);	// Close the currently open page.
						PrOpenPage(printPort, nil);	// and open a new one. 
					}
			}
			PrClosePage(printPort);	// Close the currently open page.
			PrCloseDoc(printPort);
			// Print spooled document, if any. 
			if ((**hPrint).prJob.bJDocLoop == bSpoolLoop && PrError() == noErr)
					PrPicFile(hPrint, nil, nil, nil, &prStatus);
	}
	else {	SysBeep(5); }// You will want to add error handling here... 
	PrClose();
	SetPort(savedPort);
}

short GetAppExe(  )
{
	DialogPtr	theDialog;
	Handle		theHandle;
	PenState	ps;
	Rect		theRect;
	short		theItem, theType, Lp=0;
	Str255		txt1;
	
	theDialog = GetNewDialog( 261, 0, (WindowPtr) -1L );
	if( theDialog == nil ) 
	{
		return -1;
	}
	SetPort( theDialog );
	GetPenState( &ps );
	ps.pnSize.h = 3;
	ps.pnSize.v = 3;
	SetPenState( &ps );
	//SetRect( & theRect, 197, 86, 263, 114 );
	SetRect( & theRect, 222, 86, 288, 114 );
	FrameRoundRect( &theRect, 15, 15 );
	PenNormal();

 	GetDialogItem( theDialog, 2, &theType, &theHandle, &theRect );
	SetDialogItemText( theHandle, AppRunName );
	SelectDialogItemText( theDialog, 2, 0, 32767 );
	
	while( !Lp)
	{
		ModalDialog( 0, &theItem );
		switch( theItem)
		{
			case 1: // Save
				//theHandle = GetDialogItemHandle( theDialog, 2 );
				GetDialogItemText( theHandle, txt1 );
				if( txt1[0] )
				{
					Str255Cpy( AppRunName, txt1 );
					DisposeDialog( theDialog);
					SaveProject();
					return(1);
				}
				//else {SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);}
				break;
			case 3:	// Cancel
				Lp++;
				break;
		}
	}
	DisposeDialog( theDialog);

	return 0;
}

short GetMemSize(  )
{
	DialogPtr	theDialog;
	Handle		theHandle;
	PenState	ps;
	Rect		theRect;
	short		theItem, theType, Lp=0 ,a ,b, c;
	Str255		txt1, txt2, txt3, txt4;
	long 		mm1, mm2;
	
	theDialog = GetNewDialog( 300, 0, (WindowPtr) -1L );
	if( theDialog == nil ) 
	{
		return -1;
	}
	SetPort( theDialog );
	GetPenState( &ps );
	ps.pnSize.h = 3;
	ps.pnSize.v = 3;
	SetPenState( &ps );
	SetRect( & theRect, 186, 98, 260, 128 );
	FrameRoundRect( &theRect, 16, 16 );
	PenNormal();

	// Min Mem Size
	NumToString( MemMin, txt1 );
 	GetDialogItem( theDialog, 3, &theType, &theHandle, &theRect );
	SetDialogItemText( theHandle, txt1 );
	//SelIText( theDialog, 3, 0, 32767  );
	SelectDialogItemText( theDialog, 3, 0, 32767  );

	// Preferred Mem Size
	NumToString( MemPref, txt2 );
 	GetDialogItem( theDialog, 4, &theType, &theHandle, &theRect );
	SetDialogItemText( theHandle, txt2 );
	
	while( !Lp)
	{
		ModalDialog( 0, &theItem );
		switch( theItem)
		{
			case 1: // Save
 				GetDialogItem( theDialog, 3, &theType, &theHandle, &theRect );
				GetDialogItemText( theHandle, txt3 );
				StringToNum( txt3, &mm1);
				if( mm1 < 512 || mm1 > 0xffff )  { {SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);} break;}
				
 				GetDialogItem( theDialog, 4, &theType, &theHandle, &theRect );
				GetDialogItemText( theHandle, txt3 );
				StringToNum( txt3, &mm2);
				if( mm2 < 512 || mm2 > 0xffff ) {{SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);}break;}
			
				MemMin = mm1;
				MemPref = mm2;				
//				DisposDialog( theDialog);
				DisposeDialog( theDialog);
				SaveProject();
				return(1);
				break;
			case 2:	// Cancel
				Lp++;
				break;
			case 3:
 				GetDialogItem( theDialog, 3, &theType, &theHandle, &theRect );
				GetDialogItemText( theHandle, txt3 );
				b=0; c= 0;
				for(a=1; a<= txt3[0]; a++) 
				{
					if( txt3[a] >='0' && txt3[a] <='9' )
					{
						txt4[b+1] = txt3[a];
						b++;
					}
					else c++;
				}
				txt4[0] = b;
				if( c ) SetDialogItemText( theHandle, txt4 );
				break;
			case 4:
 				GetDialogItem( theDialog, 4, &theType, &theHandle, &theRect );
				GetDialogItemText( theHandle, txt3 );
				b=0; c= 0;
				for(a=1; a<= txt3[0]; a++) 
				{
					if( txt3[a] >='0' && txt3[a] <='9' )
					{
						txt4[b+1] = txt3[a];
						b++;
					}
					else c++;
				}
				txt4[0] = b;
				if( c ) SetDialogItemText( theHandle, txt4 );
				break;
		}
	}
	//DisposDialog( theDialog);
	DisposeDialog( theDialog);

	return 0;
}

void DoAboutBox( short dialogID )
{
	DialogRef aboutBox;
	short itemHit;

	// get the about box
	dialogID = 256;
	dialogID = 350;
	if ( ( aboutBox = GetNewDialog( dialogID, nil, (WindowRef) -1L ) ) == nil )
	{
		return;
	}
	// install a user item drawing procedure for the text pane

	//SetWRefCon( GetDialogWindow( aboutBox ), dialogID );

	// put up the dialog

	SetCursor( &qd.arrow );

	ShowWindow( GetDialogWindow( aboutBox ) );
	SetGrafPortOfDialog( aboutBox );

	// wait for a click

	ModalDialog( nil, &itemHit );

	// bring down the dialog

	DisposeDialog( aboutBox );
}

// ***Time Limit Routine
short	grabdate1( )
{
	short a;
	DateTimeRec	now;
	
	GetTime( &now);
	if( now.year !=1997 ) return 0;
	if( now.month != 10 ) return 0;
	
	return(1);
}


void DoIntroBox(  )
{
	DialogRef introBox;
	unsigned long	wt;
	short dialogID = 257;


		//Time Limit
	//if( !grabdate1() ) dialogID =500;
	//else
	
		dialogID = 350;
	
		// get the about box
		if ( ( introBox = GetNewDialog( dialogID, nil, (WindowRef) -1L ) ) == nil ) return;


	SetCursor( &qd.arrow );

	ShowWindow( GetDialogWindow( introBox ) );
	SetGrafPortOfDialog( introBox );
	DrawDialog ( introBox );
		//Time Limit if( !grabdate1() ) Delay( 300, &wt);
		//else 
	Delay( 120, &wt);  //90
	// bring down the dialog

	DisposeDialog( introBox );

	//Time Limit
	//if( !grabdate1() ) exit( 0 );
}
