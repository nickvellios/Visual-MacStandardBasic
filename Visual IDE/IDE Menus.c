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

#include "IDE.h"
#include "TextEdit.h"
#include "Scrap.h"
#include <Files.h>
#include <string.h>
#include <stdio.h>
#include "ICAPI.h"
#include <DCon.h>

void PropDispose( );
short StrPartCmp( char * st1, char * st2);
void Str255Copy ( Str255 st1, Str255 st2);
void Str63Copy ( Str255 st1, Str255 st2);
short Str255Cmp(  Str255 st1, Str255 st2);

void TextFix( WindowPtr window );
short CreateEditorWindow();
short CreatePropWindow();
short CreateFormWindow();
OSErr BV_CreateToolBar(void);
short SaveSourceFile( WindowPtr window );
void SetEdWinStatus( short wn, short st);
void SetFormStatus( short fn, short st );
short ModuleEditorCreate(short, Str255  );
short LoadSourceFile( short fddi, Str255 fname );
short ResLaunch( unsigned char * rfile);
short SaveEditBox( short b);
short WhichWindow( WindowPtr window);
short CheckForEdit(WindowPtr window);
short CloseEditWindow( WindowPtr window);
short CloseForm( WindowPtr window);
void NewProject();
short FindFormOrEdit( WindowPtr window );
short  CloseAllWindows( );
void 	UndoText( WindowPtr window);
void AddCustomLibs();
short updatethescreen( );
short SafeForm( WindowPtr window);
void MarkChangeTE( );
void DoQCheck();

extern char MasterExitFlag;
extern short CurrentForm;
extern MenuHandle FormsSubMenu;
extern MenuHandle ModulesSubMenu;
extern MenuHandle ResourcesSubMenu;
extern Str255		WinTitle;
short	undoCode=0, undoStart=-1, undoEnd = -1;
extern FSSpec	IDESpec;


short DoTextFind( /*WindowPtr window*/ );
short TextReplace( WindowPtr window );

/*#ifndef __ALIASES__
#include <Aliases.h>
#endif

#ifndef __DEVICES__
#include <Devices.h>
#endif

#ifndef __LOWMEM__
#include <LowMem.h>
#endif

#ifndef __STANDARDFILE__
#include <StandardFile.h>
#endif

#ifndef __FILETYPESANDCREATORS__
#include <FileTypesAndCreators.h>
#endif

#ifndef __TOOLUTILS__
#include <ToolUtils.h>
#endif

#ifndef __WEDEMOAPP__
#include "WEDemoIntf.h"
#endif

#include "WETabs.h"*/
/*
void PrintSetup();
int GetMemSize(  );
int AddFileToProject( unsigned char * fname );
int GetProjectName( );
int LoadProject( );
void PrintStuff( WindowRef window );
OSErr OpenSource( unsigned char *fname );
void DoProjectChoice( short menuItem );
OSErr DoAdd( void );
int SaveProject( );
int TextFind( WEReference we );
int TextReplace(WEReference we );
short DoTextFind(WEReference we );
short TextMatch( short ch1, short ch2 );
int LaunchRT(  );
int LaunchCompiler(  );
int LaunchResedit( unsigned char * medit );
void DoRunChoice( short menuItem );
int GetAppExe(  );
void DoPrefChoice( short menuItem );
OSErr SaveBeforeCompile( ClosingOption closing, SavingOption saving, WindowRef window );
OSErr SaveAll( SavingOption saving );
void DoHelpChoice( short menuItem );
int LaunchHelp( );
*/
void DoAboutBox( short dialogID );
int GetAppExe(  );
int GetMemSize(  );
short GotoLine( long linenum );
short GotoLineBox( WindowPtr window );
void SaveProject( );
int LaunchRT( );
short NewProjectName( );
OSErr DoOpenPjt( void );
OSErr DoCloseProject( SavingOption saving );
OSErr DoClose( ClosingOption closing, SavingOption saving, WindowRef window );
void  DoFormMenuChoice( short menuItem );
void  DoModuleMenuChoice( short menuItem );
void  DoResourceMenuChoice( short menuItem );
void DoAdd( );
short LaunchHelp1( );
short LaunchHelp2( );
void PrintStuff( short ModNum, short EdProc );
void OpenResourceFiles();

int LoadProject( );
void PrintSetup();
int LaunchCompiler(  );
char				FindFlag=0, ReplaceFlag=0, FindOptions[]="00000";
Str255				FindText, ReplaceTextWith;
Boolean secret;
Str255				Target;
extern MenuHandle	ProjectMenu;
extern MenuHandle	FormsSubMenu;
extern MenuHandle	ModulesSubMenu;
extern MenuHandle	ResourcesSubMenu;

char				ProjectFlag=0;
extern char			CmdKeyFlag;
extern FSSpec		PjtFS;
Str255				LastFileName;
short				GridShow=1, GridSnap=1, SaveAsFlag=false;
extern WindowPtr	ToolPal;
extern WindowPtr	PropWin;
extern TEHandle		ActiveTE, PropTE;

extern WindowPtr	FormWin[20];
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
/*
// resource types

#define kTypeMenuColorTable		'mctb'

// static variables

static MenuCRsrcHandle		sColors;		// handle to the 'mctb' resource for the Color menu

// constants used by DoClose()

enum {
	kButtonSave			= 1,
	kButtonCancel,
	kButtonDontSave
};

void SetDefaultDirectory( const FSSpec *spec )
{
	LMSetCurDirStore( spec->parID );
	LMSetSFSaveDisk( -spec->vRefNum );
}

static pascal Boolean MySFDialogFilter( DialogRef dialog, EventRecord *event, short *item, void *yourData )
{
#pragma unused ( item, yourData )

	// 	intercept window events directed to windows behind the dialog
	if ( ( event->what == updateEvt ) || ( event->what == activateEvt ) )
	{
		if ( (WindowRef) event->message != GetDialogWindow( dialog ) )
		{
			DoWindowEvent( event );
		}
	}

	return false;
}

static ModalFilterYDUPP GetMySFDialogFilter( void )
{
#ifdef __cplusplus
	static ModalFilterYDUPP sFilterUPP = NewModalFilterYDProc( MySFDialogFilter );
#else
	static ModalFilterYDUPP sFilterUPP = nil;
	if ( sFilterUPP == nil )
	{
		sFilterUPP = NewModalFilterYDProc( MySFDialogFilter );
	}
#endif

	return sFilterUPP;
}

short	FindMenuItemText( MenuRef menu, ConstStr255Param stringToFind )
{
	short		item;
	Str255		itemString;

	for ( item = CountMItems( menu ); item >= 1; item-- )
	{
		GetMenuItemText( menu, item, itemString );
		if ( EqualString( itemString, stringToFind, false, false ) )
			break;
	}

	return item;
}

Boolean	EqualColor( const RGBColor *rgb1, const RGBColor *rgb2 )
{
	return ( (rgb1->red == rgb2->red) && (rgb1->green == rgb2->green) && (rgb1->blue == rgb2->blue) );
}
*/

/*void OpenResourceFiles()
{
	Str255	temp1;
	short a=0, b=1, c;
	
	c = CountMItems( ResourcesSubMenu );
	while( a<100 && b<=c)
	{
			GetMenuItemText( ResourcesSubMenu, b, temp1);
			ResRef[a] = HOpenResFile( PjtFS.vRefNum,PjtFS.parID, temp1,4 );
			if( ResRef[a] == -1 ) ResRef[a] = -2;
			else 
			b++;
			a++;
	}
}*/

void 	UndoText( WindowPtr window )
{
		short a;
		if( ActiveTE == nil || ActiveTE == PropTE) return;
		switch( undoCode )
		{
			case 0:
				return;
				break;
			case 1:  // undo cut
				a = TEGetScrapLength();
				TESetSelect( undoStart, undoStart, ActiveTE );
				TEPaste(ActiveTE);
				TESetSelect( undoStart, undoStart+a, ActiveTE );
				break;
			case 2:  // undo paste
				TESetSelect( undoStart, undoEnd, ActiveTE );
				TECut(ActiveTE);
				break;
			case 3: // undo delete & paste
				TESetSelect( undoStart, undoEnd, ActiveTE );
				TEDelete(ActiveTE);
				a = TEGetScrapLength();
				TEPaste(ActiveTE);
				TESetSelect( undoStart, undoStart+a, ActiveTE );
				break;
			case 4:  // undo delete
				a = TEGetScrapLength();
				TESetSelect( undoStart, undoStart, ActiveTE );
				TEPaste(ActiveTE);
				TESetSelect( undoStart, undoStart+a, ActiveTE );
				break;
			case 5:  // undo typing
				a = TEGetScrapLength();
				TESetSelect( undoStart, undoStart, ActiveTE );
				TEPaste(ActiveTE);
				TESetSelect( undoStart, undoStart+a, ActiveTE );
				break;
		}
		TextFix( window );
		undoCode = 0;
}

void	PrepareMenus( void )
{
	WindowRef		window;
	//WEReference		we;
	MenuRef			menu;
	short			item, winNum=-1, a;
	Str255			itemText;
	long			selStart=0, selEnd=0;
	PScrapStuff		pss;
	
	// get a pointer to the frontmost window, if any
	window = FrontWindow();

	// get associated WE instance
	if( window == PropWin ) winNum = CurrentForm;
	else if( window !=nil ) winNum = FindFormOrEdit( window );

	// *** FILE MENU ***

	menu = GetMenuHandle( kMenuFile );

	// first disable all items

	for ( item = CountMItems( menu); item >= 1; item-- ) DisableItem( menu, item );

	// New, Open, and Quit are always enabled
	EnableItem( menu, kItemNewPjt );
	EnableItem( menu, kItemOpenPjt );
	EnableItem( menu, kItemPSetup );
	EnableItem( menu, kItemQuit );

	if( ProjectFlag )
	{
		EnableItem( menu, kItemClosePjt );
		EnableItem( menu, kItemNewForm );
		EnableItem( menu, kItemNew );
		EnableItem( menu, kItemAdd );
		EnableItem( menu, kItemRemove );
		
		if( winNum>=0 )
		{
			EnableItem( menu, kItemCloseWin );
			EnableItem( menu, kItemSaveAs );
			EnableItem( menu, kItemPrint );
			if( EditWin[winNum].status==1 || EditWin[winNum].change )
			{
			 	EnableItem( menu, kItemSave );
			}
		}
	}

	// *** Run MENU ***

	menu = GetMenuHandle( kMenuRun );

	// first disable all items

	for ( item = CountMItems( menu); item >= 1; item-- ) DisableItem( menu, item );

	if( ProjectFlag )
	{
		EnableItem( menu, 1 );
		EnableItem( menu, 2 );
		EnableItem( menu, 3 );
	}

	// *** PREF MENU ***
	menu = GetMenuHandle( kMenuTools );

	// first disable all items

	for ( item = CountMItems( menu); item >= 1; item-- ) DisableItem( menu, item );

	if( ProjectFlag )
	{
		EnableItem( menu, 1 );
		EnableItem( menu, 2 );
		if( ToolPal == nil )	EnableItem( menu, 4 );
		if( PropWin == nil )
		{
			for( a=0; a<20;a++ ) if( FormWin[a] !=nil ) { EnableItem( menu, 5 ); break; }
		}
		for ( item = CountMItems( menu); item >= 7; item-- )
		{
			EnableItem( menu, item );
			GetMenuItemText( menu, item, itemText );
			a = Target[0]; Target[a+1]=0;
			a = itemText[0]; itemText[a+1]=0;
			if(	Str255Cmp(  Target, itemText) ) SetItemMark( menu, item, checkMark );
			else SetItemMark( menu, item, 0 );
		}
	} 

	// *** EDIT MENU ***

	menu = GetMenuHandle( kMenuEdit );

	// first, disable all items

	for ( item = CountMItems( menu ); item >= 1; item-- ) DisableItem( menu, item );

	// by default, the Undo menu item should read "Can't Undo"

	GetIndString( itemText, kUndoStringsID, 1 );
	SetMenuItemText( menu, kItemUndo, itemText );

	if ( ActiveTE!=nil && ActiveTE != PropTE )
	{
		if( (*ActiveTE)->selStart != (*ActiveTE)->selEnd )
		{
			EnableItem( menu, kItemCut );
			EnableItem( menu, kItemCopy );
			EnableItem( menu, kItemClear );
		}
		if( ActiveTE != nil )
		{
			pss = InfoScrap();
			if( pss->scrapSize > 0 ) EnableItem( menu, kItemPaste );
		}
		EnableItem( menu, kItemSelectAll );
		EnableItem( menu, kItemFind );
		EnableItem( menu, kItemReplace );
		EnableItem( menu, 14 );
		if( FindFlag) 		EnableItem( menu, kItemFindNext );
		if( ReplaceFlag) 	EnableItem( menu, kItemReplaceNext );
		switch( undoCode )
		{
			case 0:
				SetMenuItemText( menu, 1, "\pCan't Undo" );
				break;
			case 1:
				EnableItem( menu, 1 );
				SetMenuItemText( menu, 1, "\pUndo Cut" );
				break;
			case 2:
				EnableItem( menu, 1 );
				SetMenuItemText( menu, 1, "\pUndo Paste" );
				break;
			case 3:
				EnableItem( menu, 1 );
				SetMenuItemText( menu, 1, "\pUndo Delete & Paste" );
				break;
			case 4:
				EnableItem( menu, 1 );
				SetMenuItemText( menu, 1, "\pUndo Delete" );
				break;
			case 5:
				EnableItem( menu, 1 );
				SetMenuItemText( menu, 1, "\pUndo Typing Delete" );
				break;
		}

	}
}


void DoDeskAcc( short menuItem )
{
	Str255 deskAccessoryName;

	GetMenuItemText( GetMenuHandle( kMenuApple ), menuItem, deskAccessoryName );
	OpenDeskAcc( deskAccessoryName );
}

/*
OSErr DoNew( void )
{
	OSErr					err = noErr;
	// create a new window from scratch
	WinNewFlag = true;
	err = CreateWindow( nil );
 	//AddFileToProject( "\puntitled");
	return err;
}

OSErr DoOpen( void )
{
	StandardFileReply		reply;
	SFTypeList				typeList;
	OSErr					err = noErr;
	Point					where = { -1, -1 };  // auto center dialog

	// set up a list of file types we can open for StandardGetFile
	typeList[0] = kTypeText;
	typeList[1] = ftSimpleTextDocument;

	// put up the standard open dialog box.
	// (we use CustomGetFile instead of StandardGetFile because we want to provide
	// our own dialog filter procedure that takes care of updating our windows)
	CustomGetFile( nil, 2, typeList, &reply, 0, where, nil, GetMySFDialogFilter( ), nil, nil, nil );

	// if the user ok'ed the dialog, create a new window from the specified file
	if ( reply.sfGood )
	{
		WinNewFlag = false;
		err = CreateWindow( &reply.sfFile );
	}
	else
		err = userCanceledErr;

	return err;
}
*/


OSErr DoOpenPjt( void )
{
	StandardFileReply		reply;
	SFTypeList				typeList;
	OSErr					err = noErr;
	Point					where = { -1, -1 };  // auto center dialog

	// set up a list of file types we can open for StandardGetFile
	typeList[0] = 'BPjt';  //kTypeText;
	typeList[1] = ftSimpleTextDocument;

	// put up the standard open dialog box.
	// (we use CustomGetFile instead of StandardGetFile because we want to provide
	// our own dialog filter procedure that takes care of updating our windows)
	//CustomGetFile( nil, 2, typeList, &reply, 0, where, nil, GetMySFDialogFilter( ), nil, nil, nil );
	StandardGetFile( nil, 2, typeList, &reply );

	// if the user ok'ed the dialog, create a new window from the specified file
	if ( reply.sfGood )
	{
		//DoCloseProject( savingAsk );
		//PjtFS  = reply.sfFile;
		PjtFS.vRefNum  = reply.sfFile.vRefNum;
		PjtFS.parID  = reply.sfFile.parID;
		Str255Copy( reply.sfFile.name, PjtFS.name );
		LoadProject( );

		//err = CreateWindow( &reply.sfFile );
	}
	return err;
}


OSErr DoCloseProject(SavingOption saving )
{
	short	a, b;
	
	// close all windows
	for(a=0; a<20; a++ )
	{
		if(FormWin[a]!=nil )
		{
			b =  CloseForm( FormWin[a] );
			if( b==-2 ) return 0;
		}
	}
	for(a=20; a<100; a++ )
	{
		if(EditWin[a].theWindow != nil )
		{
			b = CloseEditWindow( EditWin[a].theWindow );
			if( b==-2 ) return 0;
		}
	}
	
	DeleteMenu( 101 );	
	DeleteMenu( 102 );	
	DeleteMenu( 103 );	
	DeleteMenu( 7 );	
	DisposeMenu( FormsSubMenu);	
	DisposeMenu( ModulesSubMenu);	
	DisposeMenu( ResourcesSubMenu);	
	DisposeMenu( ProjectMenu);	
	ProjectFlag = 0;
	if( PropWin!=nil )
	{
		DisposeWindow( PropWin );
		PropWin = nil;
	}
	NewProject();

	DrawMenuBar();

	return 1;
}

short  CloseAllWindows( )
{
	short	a, b;
	
	// close all windows
	for(a=0; a<20; a++ )
	{
		if(FormWin[a]!=nil )
		{
			b =  SafeForm( FormWin[a] );
			if( b==-2 ) return 0;
		}
	}
	for(a=20; a<100; a++ )
	{
		if(EditWin[a].theWindow != nil )
		{
			b = CloseEditWindow( EditWin[a].theWindow );
			if( b==-2 ) return 0;
		}
	}
	
	return 1;
}


void DoAdd( )
{
	DialogPtr				theDialog;
	StandardFileReply		reply;
	SFTypeList				typeList;
	OSErr					err = noErr,error;
	Point					where = { -1, -1 };  // auto center dialog
	short					looper=0,a,cc;
	char					lbuf[120];
	long					bl=100, FLEN;
	Str255					text1;
	
	// set up a list of file types we can open for StandardGetFile
	typeList[0] = kTypeText;
	typeList[1] = ftSimpleTextDocument;
	typeList[2] = sigResEdit;
	typeList[3] = ftResEditResourceFile;
	//typeList[4] = 'FORM';


	while( !looper )
	{
		// put up the standard open dialog box.
		StandardGetFile( nil, -1, typeList, &reply );

		// if the user ok'ed the dialog, create a new window from the specified file
		if ( reply.sfGood )
		{

			if( reply.sfFile.vRefNum != PjtFS.vRefNum || reply.sfFile.parID != PjtFS.parID ) 
			{
				{SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);}				theDialog = GetNewDialog( 280, nil, (WindowPtr) -1L );
				ModalDialog( 0, &cc );
				DisposeDialog( theDialog);
			}
			else
			{
				error = FSpOpenDF( &reply.sfFile, fsRdWrShPerm, &cc); 	if(error) { return; }
				error = GetEOF( cc, &FLEN);  if(error) { FSClose(cc); return; }
				if( !FLEN)
				{	// Resource
					FSClose( cc);
					Str63Copy ( reply.sfFile.name, text1 );
					
					AppendMenu( ResourcesSubMenu, text1 );
					SaveProject( );
					return;
				}
				else
				{
					error = FSRead( cc, &bl, lbuf );  if(error) { return; }
					FSClose( cc);
					Str63Copy ( reply.sfFile.name, text1 );
					for( a=0;a<100;a++) if( lbuf[a]==13 ) { lbuf[a]=0; break; }
					if( StrPartCmp( lbuf, "MacStandardBasic Form" ) )
					{	// module
						AppendMenu( FormsSubMenu, text1 );
					}
					else
					{	// module
						AppendMenu( ModulesSubMenu, text1 );
					}
					SaveProject( );
					return;
				}
			}
		}
		else return;
	}
	return;
}

/*
OSErr OpenSource( unsigned char *fname )
{
	//StandardFileReply		reply;
	//SFTypeList				typeList;
	OSErr					err = noErr;
	//Point					where = { -1, -1 };  // auto center dialog
	FSSpec					fs1;
	
	// set up a list of file types we can open for StandardGetFile
	//typeList[0] = kTypeText;
	//typeList[1] = ftSimpleTextDocument;

	// put up the standard open dialog box.
	// (we use CustomGetFile instead of StandardGetFile because we want to provide
	// our own dialog filter procedure that takes care of updating our windows)
	//CustomGetFile( nil, 2, typeList, &reply, 0, where, nil, GetMySFDialogFilter( ), nil, nil, nil );

	// if the user ok'ed the dialog, create a new window from the specified file
	
	err = FSMakeFSSpec(PjtFS.vRefNum,PjtFS.parID, fname, &fs1 );
	if ( !err )
		err = CreateWindow( &fs1 );
	else
	{	 
		{SysBeep(30); dprintf("Error @ " __FILE__" l. %d
", __LINE__);}	}
	return err;
}


OSErr SaveWindow( const FSSpec *pFileSpec, WindowRef window )
{
	DocumentHandle	hDocument;
	AliasHandle		alias = nil;
	OSErr			err;

	hDocument = GetWindowDocument(window);
	ForgetHandle( & (*hDocument)->fileAlias );

	// save the text

	if ( ( err = WriteTextFile( pFileSpec, (*hDocument)->we ) ) == noErr )
	{
		SetWTitle( window, pFileSpec->name );

		// replace the old window alias (if any) with a new one created from pFileSpec
		NewAlias( nil, pFileSpec, &alias );

		// if err, alias will be nil, and it's not fatal, just will make subsequent saves annoying
		(* hDocument)->fileAlias = (Handle) alias;
	}

	return err;
}


OSErr	DoSaveAs( const FSSpec *suggestedTarget, WindowRef window )
{
	DialogPtr			theDialog;
	StringHandle		hPrompt;
	Str255				defaultName;
	StandardFileReply	reply;
	Point				where = { -1, -1 }; // autocenter's dialog
	OSErr				err;
	short				i, cc=0, WinNewFlag = -1;

	// get the prompt string for CustomPutFile from a string resource and lock it
	hPrompt = GetString( kPromptStringID );
	HLockHi( (Handle) hPrompt );

	// if a suggested target file is provided, use its name as the default name
	if ( suggestedTarget != nil )
	{
		PStringCopy( suggestedTarget->name, defaultName );
		SetDefaultDirectory( suggestedTarget );
	}
	else
	{
		// otherwise use the window title as default name for CustomPutFile
		GetWTitle( window, defaultName );
	}
	// Check if this was an Open File so it shouldn'tbe added to pjt
	for(i=0; i<100; i++ )
		if( WinBank[i] == window )
		{
			 if( WinNew[i] ) 	WinNewFlag = i;
			 break;
		}
	// put up the standard Save dialog box
	while( !cc )
	{
		CustomPutFile( *hPrompt, defaultName, &reply, 0, where, nil, GetMySFDialogFilter(), nil, nil, nil );
		cc++;
		if( WinNewFlag >= 0 && reply.sfGood && (reply.sfFile.vRefNum != PjtFS.vRefNum || reply.sfFile.parID != PjtFS.parID ) )
		{
			{SysBeep(30); dprintf("Error @ " __FILE__" l. %d
", __LINE__);}			theDialog = GetNewDialog( 280, nil, (WindowPtr) -1L );
			ModalDialog( 0, &cc );
			cc = 0;
			DisposDialog( theDialog);
		}
	}

	// unlock the string resource
	HUnlock( (Handle)hPrompt );

	// if the user ok'ed the dialog, save the window to the specified file
	if ( reply.sfGood )
	{
		err = SaveWindow( &reply.sfFile, window );
		PStringCopy( reply.sfFile.name, LastFileName );
		if(!err)
		{
			if( WinNewFlag>=0 )
			{
				AddFileToProject( LastFileName );
		 		WinNew[WinNewFlag] = false;
			}
		}
	}
	else
		err = userCanceledErr;

	return err;
}

OSErr	DoSave( WindowRef window )
{
	FSSpec		spec;
	FSSpecPtr	suggestedTarget = nil;
	Boolean		promptForNewFile = true;
	Boolean		aliasTargetWasChanged;
	OSErr		err;
	short 		i;
	// resolve the alias associated with this window, if any

	if ( (* GetWindowDocument(window) )->fileAlias != nil )
	{
		if ( ( ResolveAlias( nil, (AliasHandle) (* GetWindowDocument(window))->fileAlias, &spec, &aliasTargetWasChanged ) == noErr ) )
		{
			if ( aliasTargetWasChanged )
				suggestedTarget = &spec;
			else
				promptForNewFile = false;
		}
	}

	// if no file has been previously associated with this window, or if the
	// alias resolution has failed, or if the alias target was changed
	// prompt the user for a new destination

	if ( promptForNewFile )
	{	err = DoSaveAs( suggestedTarget, window );
		if(!err)
		{
				for(i=0; i<100; i++ )
					if( WinBank[i] == window )
					{
						 if( WinNew[i] )
						 {
							AddFileToProject( LastFileName );
		 					WinNew[i] = false;
						 }
					}
		}
	}
	else
		err = SaveWindow( &spec, window );

	return err;
}
*/

/*static pascal Boolean SaveChangesDialogFilter( DialogRef dialog, EventRecord *event, short *item )
{
	//	map command + D to the "Don't Save" button
	if ( ( event->what == keyDown ) && ( event->modifiers & cmdKey ) && ( ( event->message & charCodeMask ) == 'd' ) )
	{
		//	flash the button briefly
		FlashButton( dialog, kButtonDontSave );

		//	fake an event in the button
		*item = kButtonDontSave;
		return true;
	}

	//	route everything else to our default handler
	return CallModalFilterProc( GetMyStandardDialogFilter( ), dialog, event, item );
}*/

/*
OSErr DoClose( ClosingOption closing, SavingOption saving, WindowRef window )
{
	Str255		s1, s2;
	short		alertResult;
	OSErr		err;
//#ifdef __cplusplus
	static ModalFilterUPP sFilterProc = NewModalFilterProc( SaveChangesDialogFilter );
#else
	static ModalFilterUPP sFilterProc = nil;
	if ( sFilterProc == nil )
	{
		sFilterProc = NewModalFilterProc( SaveChangesDialogFilter );
	}
//#endif

	// is this window dirty?
	if ( WEGetModCount( GetWindowWE(window) ) > 0 )
	{
		// do we have to ask the user whether to save changes?

		if ( saving == savingAsk )
		{
			// prepare the parametric strings to be used in the Save Changes alert box

			GetWTitle( window, s1 );

			GetIndString( s2, kClosingQuittingStringsID, 1 + closing );
			ParamText( s1, s2, nil, nil );

			// put up the Save Changes? alert box

			SetCursor( &qd.arrow );

			alertResult = Alert( kAlertSaveChanges, sFilterProc );

			// exit if the user canceled the alert box

			if ( alertResult == kButtonCancel )
				return userCanceledErr;

			if ( alertResult == kButtonSave )
				saving = savingYes;
			else
				saving = savingNo;
		}

		if ( saving == savingYes )
		{
			if ( ( err = DoSave( window ) ) != noErr )
				return err;
		}
	}

	// destroy the window
	DestroyWindow( window );

	return noErr;
}

OSErr DoQuit( SavingOption saving )
{
	WindowRef window;
	OSErr err;

	// close all windows
	do
	{
		if ( ( window = FrontWindow( ) ) != nil )
		{
			if ( ( err = DoClose( closingApplication, saving, window ) ) != noErr )
				return err;
		}
	} while ( window != nil );

	// set a flag so we drop out of the event loop
	gExiting = true;

	return noErr;
}


OSErr SaveAll( SavingOption saving )
{
	WindowRef window, window1;
	OSErr err;

	// close all windows
	window1 = FrontWindow( );
	do
	{
		if ( ( window = FrontWindow( ) ) != nil )
		{
			if ( ( err = SaveBeforeCompile( closingApplication, saving, window ) ) != noErr )
				return err;
				SendBehind( window, 0 );
		}
		window = FrontWindow( ) ;
		if( window == window1 ) return noErr;
	} while ( window != nil );

	return noErr;
}


OSErr SaveBeforeCompile( ClosingOption closing, SavingOption saving, WindowRef window )
{
	Str255		s1; //, s2;
	short		alertResult;
	OSErr		err;
	
#ifdef __cplusplus
	static ModalFilterUPP sFilterProc = NewModalFilterProc( SaveChangesDialogFilter );
#else
	static ModalFilterUPP sFilterProc = nil;
	if ( sFilterProc == nil )
	{
		sFilterProc = NewModalFilterProc( SaveChangesDialogFilter );
	}
#endif
	alertResult = closing;

	// is this window dirty?
	if ( WEGetModCount( GetWindowWE(window) ) > 0 )
	{
		// do we have to ask the user whether to save changes?

		if ( saving == savingAsk )
		{
			// prepare the parametric strings to be used in the Save Changes alert box

			GetWTitle( window, s1 );

			//GetIndString( s2, kClosingQuittingStringsID, 1 + closing );
			ParamText( s1, "\pcompiling", nil, nil );

			// put up the Save Changes? alert box

			SetCursor( &qd.arrow );

			alertResult = Alert( kAlertSaveChanges, sFilterProc );

			// exit if the user canceled the alert box

			if ( alertResult == kButtonCancel )
				return userCanceledErr;

			if ( alertResult == kButtonSave )
				saving = savingYes;
			else
				saving = savingNo;
		}

		if ( saving == savingYes )
		{
			if ( ( err = DoSave( window ) ) != noErr )
				return err;
		}
	}


	return noErr;
}
*/

short GotoLine( long linenum )
{
	long			textLen, offset, ctr=1;
	short			ch;
	Handle			chnd;
	
	chnd = TEGetText( ActiveTE );
	if( linenum <=0 ) linenum=1;
	offset = 0;
	
	//textLen = WEGetTextLength( we );
	textLen = (*ActiveTE)->teLength ;
	if( !textLen ) return(-1);

	ctr = 1;  // line ctr
	
	while( offset<textLen )
	{
		if( linenum<=ctr ) break;
		
		ch = *((*chnd) +offset);  //ch = WEGetChar( offset,we );
		if( ch == 13 )
		{
		 	ctr++;
			ch = *((*chnd) +offset);  //ch = WEGetChar( offset,we );
			if( ch == 10 ) offset++;
		}
		offset++;
	}
	//WESetSelection( offset, offset, we );
	TESetSelect( offset, offset, ActiveTE );

	return 0;
}


short TextMatch( short ch1, short ch2 )
{
	if( FindOptions[0] == '0' )
	{
		if( ch1 == ch2 ) return( true );
		if( ch1>='A' && ch1<='Z') ch1+=32; // Make lowercase
		if( ch1 == ch2 ) return( true );
		if( ch1>='a' && ch1<='z') ch1-=32; // Make lowercase
		if( ch1 == ch2 ) return( true );
	}
	else
	{
		if( ch1 == ch2 ) return( true );
	}
	return(false);
}

short DoTextFind( )
{
	long			selStart, selEnd, textLen, offset, first;
	short			ch, ctr;
	char			TopFlag=0;
	Handle			chnd;
	
	if( !FindText[0] ) return(-1);
	
	chnd = TEGetText( ActiveTE );
	
	//WEGetSelection(&selStart, &selEnd , we);
	offset = (*ActiveTE)->selEnd;
	selEnd = offset;
	
	if( !selEnd ) TopFlag=1;
	
	//textLen = WEGetTextLength( we );
	textLen = (*ActiveTE)->teLength ;
	if( !textLen ) return(-1);
	
	while( TopFlag<2 )
	{
		while( offset<textLen )
		{
			ctr = 1;
			
			ch = *((*chnd) +offset); //WEGetChar( offset,we );
			if( TextMatch( ch, FindText[ctr]) )
			{
				first = offset;
				while( TextMatch( ch, (short) FindText[ctr]) && offset < textLen )
				{
					offset++;
					ctr++;
					ch = *((*chnd) +offset); //WEGetChar( offset,we );
				}
				if( ctr == FindText[0]+1 )
				{
					// Match found
					//WESetSelection( offset, first, we );
					TESetSelect( first, offset, ActiveTE );
					return( 1 );
				}
				else
				{
					
				}
			}
			else offset++; 
		}
		TopFlag++;
		offset = 0;
		textLen = selEnd;
		
	}
	
	return 0;
}


short TextFind( WindowPtr window )
{
	GrafPtr		gp;
	DialogPtr	theDialog;
	Handle		theHandle;
	PenState	ps;
	Rect		theRect;
	short		theItem, theType, Lp=0, value, a, b;
	Str255		nFindText;

	GetPort( &gp);
	InitCursor();
	theDialog = GetNewDialog( 258, 0, (WindowPtr) -1L );
	if( theDialog == nil ) 
	{
		return -1;
	}
	SetPort( theDialog );
	GetPenState( &ps );
	ps.pnSize.h = 3;
	ps.pnSize.v = 3;
	SetPenState( &ps );
	SetRect( & theRect, 203, 96, 269, 124 );
	FrameRoundRect( &theRect, 15, 15 );
	PenNormal();
	
	
    nFindText[0] = 0;
	if( FindFlag )
	{
		GetDialogItem( theDialog, 3, &theType, &theHandle, &theRect );
		SetDialogItemText( theHandle, FindText );
		//GetDItem( theDialog, 2, &theType, &theHandle, &theRect );
		//SetIText( theHandle, nFindText );
		SelectDialogItemText( theDialog, 3, 0, 32767 );
	}
	if(FindOptions[0] == '1') // case sensitive
	{
		GetDialogItem( theDialog, 4, &theType, &theHandle, &theRect );
	  	SetControlValue( (ControlHandle) theHandle, 1 );
	}
	
	while( !Lp)
	{
		ModalDialog( 0, &theItem );
		switch( theItem)
		{
			case 1: // Find
				//theHandle = GetDialogItemHandle( theDialog, 3 );
				GetDialogItem( theDialog, 3, &theType, &theHandle, &theRect );
				GetDialogItemText( theHandle, nFindText );
				//SetPort( gp);
				if( nFindText[0] )
				{
					Str255Copy( nFindText, FindText );
					value = DoTextFind( );
					FindFlag =1;
					if( value == 1 ) 
					{
						DisposeDialog( theDialog);
						SetPort( gp);
						TESelView( ActiveTE );
						TextFix( window );
						return 0;
					}
				}
				{SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);}				break;
			case 2:	// Cancel
				Lp++;
				break;
			case 4:
				GetDialogItem( theDialog, theItem, &theType, &theHandle, &theRect );
				value = GetControlValue( (ControlHandle) theHandle );
				if(! value)
				{
				  	SetControlValue( (ControlHandle) theHandle, 1 );
					FindOptions[theItem-4] = '1';
				}
				else
				{
				 	SetControlValue( (ControlHandle) theHandle, 0 );
					FindOptions[theItem-4] = '0';
				}
				break;
		}
	}
	DisposeDialog( theDialog);
	SetPort( gp);

	return 0;
}

short TextReplace( WindowPtr window )
{
	GrafPtr		gp;
	DialogPtr	theDialog;
	Handle		theHandle;
	PenState	ps;
	Rect		theRect;
	short		theItem, theType, Lp=0, value, a,b;
	Str255		nFindText, nReplaceTextWith;
	
/*#ifdef __cplusplus
	static ModalFilterUPP sFilterProc = NewModalFilterProc( SaveChangesDialogFilter );
#else
	static ModalFilterUPP sFilterProc = nil;
	if ( sFilterProc == nil )
	{
		sFilterProc = NewModalFilterProc( SaveChangesDialogFilter );
	}
#endif*/

	GetPort( &gp);
	value= 0;
	value =1;
	InitCursor();
	theDialog = GetNewDialog( 260, nil, (WindowPtr) -1L );
	if( theDialog == nil ) 
	{
		return -1;
	}
	SetPort( theDialog );
	GetPenState( &ps );
	ps.pnSize.h = 3;
	ps.pnSize.v = 3;
	SetPenState( &ps );
	SetRect( & theRect, 191, 166, 257, 194 );
	FrameRoundRect( &theRect, 15, 15 );
	PenNormal();

 	nFindText[0] = 0;
	nReplaceTextWith[0] = 0;

	if( ReplaceFlag )
	{
		GetDialogItem( theDialog, 3, &theType, &theHandle, &theRect );
		SetDialogItemText( theHandle, FindText );
		GetDialogItem( theDialog, 4, &theType, &theHandle, &theRect );
		SetDialogItemText( theHandle, ReplaceTextWith );
	}
	
	if(FindOptions[0] == '1') // case sensitive
	{
		GetDialogItem( theDialog, 5, &theType, &theHandle, &theRect );
	  	SetControlValue( (ControlHandle) theHandle, 1 );
	}
	FindOptions[1] = '0';
	
	while( !Lp)
	{
		ModalDialog( nil, &theItem );
		switch( theItem)
		{
			case 1: // Replace
				GetDialogItem( theDialog, 3, &theType, &theHandle, &theRect );
				GetDialogItemText( theHandle, nFindText );
				GetDialogItem( theDialog, 4, &theType, &theHandle, &theRect );
				GetDialogItemText( theHandle, nReplaceTextWith );
				if( nFindText[0] )
				{
					Str255Copy( nFindText, FindText );
					Str255Copy( nReplaceTextWith, ReplaceTextWith );
					value = DoTextFind( );
					ReplaceFlag =1;
					if( value == 1 ) 
					{
					  DisposeDialog( theDialog);
					  SetPort( gp);
					  TESelView( ActiveTE );
					  if( FindOptions[1] =='0' )
					  {	// FInd & Verify
						ParamText( FindText, ReplaceTextWith, nil, nil );
						SetCursor( &qd.arrow );
						value = Alert( 129, nil );

						// exit if the user canceled the alert box
						switch( value )
						{
							case 1:	// Yes
								TECut( ActiveTE);
								TEInsert( ReplaceTextWith+1, (long) ReplaceTextWith[0], ActiveTE );
								TESelView( ActiveTE );
								TextFix( window );
								break;
							case 2: // No
								break;
							case 3:	// Cancel
								break;
						}

						return 0;
					  }
					  else
					  { // Replace All
					  		while( value == 1 )
					  		{
								//WECut( we);
								//WEInsert( ReplaceTextWith+1, (long) ReplaceTextWith[0], nil, nil, we );
								TECut( ActiveTE);
								TEInsert( ReplaceTextWith+1, (long) ReplaceTextWith[0], ActiveTE );
								value = DoTextFind( );
					 			if( value ==1 )
					 			{
					 				TESelView( ActiveTE );
									TextFix( window );
								}
								else return 0;
					  		}
					  }
					}
				}
				{SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);}				break;
			case 2:	// Cancel
				Lp++;
				break;
			case 5:
			case 6:
				GetDialogItem( theDialog, theItem, &theType, &theHandle, &theRect );
				value = GetControlValue( (ControlHandle) theHandle );
				if(! value)
				{
				  	SetControlValue( (ControlHandle) theHandle, 1 );
					FindOptions[theItem-5] = '1';
				}
				else
				{
				 	SetControlValue( (ControlHandle) theHandle, 0 );
					FindOptions[theItem-5] = '0';
				}
				break;
		}
	}
	DisposeDialog( theDialog);
	SetPort( gp);

	return 0;
}


short GotoLineBox( WindowPtr window  )
{
	GrafPtr		gp;
	DialogPtr	theDialog;
	Handle		theHandle;
	PenState	ps;
	Rect		theRect;
	short		theItem, theType, Lp=0, a,b,c;
	Str255		LineText, txt3,txt4;
	long 		lnum;
	
	GetPort( &gp);
	InitCursor();
	theDialog = GetNewDialog( 290, 0, (WindowPtr) -1L );
	if( theDialog == nil ) 
	{
		return -1;
	}
	SetPort( theDialog );
	GetPenState( &ps );
	ps.pnSize.h = 3;
	ps.pnSize.v = 3;
	SetPenState( &ps );
	SetRect( & theRect, 193, 73, 259, 101 );
	FrameRoundRect( &theRect, 15, 15 );
	PenNormal();
	
	
	GetDialogItem( theDialog, 3, &theType, &theHandle, &theRect );
	SetDialogItemText( theHandle, "\p1" );
	//SelIText( theDialog, 3, 0, 32767 );
	SelectDialogItemText( theDialog, 3, 0, 32767 );

	while( !Lp)
	{
		ModalDialog( 0, &theItem );
		switch( theItem)
		{
			case 1: // Goto
				//theHandle = GetDialogItemHandle( theDialog, 3 );
				GetDialogItem( theDialog, 3, &theType, &theHandle, &theRect );
				GetDialogItemText( theHandle, LineText );
				if( LineText[0] )
				{
					StringToNum( LineText, &lnum);
					if( lnum<0) lnum=1;
					DisposeDialog( theDialog);
					SetPort( gp);
					GotoLine( lnum);
					TESelView( ActiveTE );
					TextFix( window );
					return 0;
				}
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
		}
	}
	DisposeDialog( theDialog);
	SetPort( gp);

	return 0;
}

// ***Time Limit Routine
short updatethescreen( )
{
	short a;
	DateTimeRec	now;
	
	GetTime( &now);
	if( now.year !=1997 ) return 0;
	if( now.month != 10  ) return 0;
	
	return(1);
}


void DoAppleChoice( short menuItem, const EventRecord *theEvent )
{
	switch( menuItem )
	{
		case 1:
			if (secret)
			{
				DoAboutBox( 800 );
				secret=false;
			}
			else
			{
				DoAboutBox( kDialogAboutBox );
			}
			break;
		/*case 2:
		 	LaunchHelp1(  );
		 	break;
		case 3:
		 	LaunchHelp2(  );
		 	break;
		case 4:
		 	LaunchHelp3(  );
		 	break;*/
		default:
			DoDeskAcc( menuItem );
			break;
	}
}

OSStatus LaunchURL(ConstStr255Param urlStr)
{
	OSStatus err;
	ICInstance inst;
	long startSel;
	long endSel;
	
	err = ICStart(&inst, 'MLib');
	if (err == noErr)
	{
		err = ICFindConfigFile(inst, 0, nil);
		if (err == noErr)
		{
			startSel = 0;
			endSel = urlStr[0];
			err = ICLaunchURL(inst, "\p", (char *) &urlStr[1], urlStr[0], &startSel, &endSel);
		}
		(void) ICStop(inst);
	}

	return (err);
}

void DoWebChoice( short menuItem )
{
	switch( menuItem )
	{
		case 1:
			LaunchURL("\phttp://www.squirrelsw.com");
			break;
		case 2:
		 	LaunchURL("\phttp://www.squirrelsw.com/vmsb");
		 	break;
		case 3:
		 	LaunchURL("\pmailto:nick@squirrelsw.com");
		 	break;
		case 4:
		 	LaunchURL("\phttp://www.squirrelsw.com/vmsb/docs.html");
		 	break;
		default:
			DoDeskAcc( menuItem );
			break;
	}
}

void DoFileChoice( short menuItem )
{
	WindowRef window;
	short a,b,c;
	DialogPtr				theDialog;
	
	window = FrontWindow();
	switch( menuItem )
	{
		case kItemNewPjt:
			if(ProjectFlag)
			{
				a = DoCloseProject( savingAsk );
				if( !a ) return;
			}
			Str255Copy( "\pStandard FAT Library", Target);
			NewProjectName( );
			break;
			
		case kItemOpenPjt:
			PropDispose( );
			if(ProjectFlag)
			{
				a = DoCloseProject( savingAsk );
				if( !a ) return;
			}
  			DoOpenPjt( );
			break;

		case kItemClosePjt:
			PropDispose( );
			if( ProjectFlag ) DoCloseProject( savingAsk );
			break;
			
		case kItemNew:
			PropDispose( );
			ModuleEditorCreate( 1, "\p" );
			break;

		case kItemNewForm:
			PropDispose( );
		    CreateFormWindow();
			SetFormStatus( CurrentForm, 1 );  //Set as new window
		    CreatePropWindow();
			break;

		case kItemSave:
			PropDispose( );
			if( CurrentForm >= 0 )
			{
				a = CheckForEdit( window);
				if( a >=0 )
				{
				 	b = SaveEditBox( a);
					if( b == -1 ) return;
				} 	
				SaveSourceFile( window );
			}
			else
			{
				a = CheckForEdit( window);
				if( a >=0 )
				{
					b = SaveEditBox( a);
					if( b == -1 ) return;
					SaveSourceFile( window );
				}
			}
			break;

		case kItemSaveAs:
			PropDispose( );
			SaveAsFlag=true;
			if( CurrentForm >= 0 )
			{
				a = CheckForEdit( window);
				if( a >=0 )
				{
				 	b = SaveEditBox( a);
					if( b == -1 ) return;
				} 	
				SaveSourceFile( window );
			}
			else
			{
				a = CheckForEdit( window);
				if( a >=0 )
				{
					b = SaveEditBox( a);
					if( b == -1 ) return;
					SaveSourceFile( window );
				}
			}
			SaveAsFlag=false;
			break;

		case kItemCloseWin:
			PropDispose( );
			a = CloseEditWindow( window);
			if( a==-1 ) CloseForm( window);
			break;

		case kItemAdd:
			PropDispose( );
			DoAdd(  );
			break;

		case kItemRemove:
			PropDispose( );
			Alert( 136, nil );
			break;

		case kItemPSetup:		
		//PropDispose( );
		   PrintSetup();
			//printf("CurrentForm = %i\n", CurrentForm);
			//DoQCheck();

			break;
			
		case kItemPrint:
			PropDispose( );
			a = CheckForEdit( window);
			if( a >=0 || CurrentForm >= 0 )
 			{
 				if( a>=0 )
				{
					b = SaveEditBox( a);
					if( b == -1 ) return;
					GetWTitle(EditWin[a].theWindow, WinTitle);
					theDialog = GetNewDialog( 400, nil, (WindowPtr) -1L );
					ModalDialog( 0, &c );
					DisposeDialog( theDialog );
					if( c== 1 ) PrintStuff( a, EditWin[a].currentProc );
					else if( c ==2 ) PrintStuff( a, -1 );
				}
				else
				{
					a = CurrentForm;
					GetWTitle(FormWin[a], WinTitle);
					PrintStuff( a, -1 );
				}
			}
			break;

		case kItemQuit:
			PropDispose( );
			if( ProjectFlag )
			{
				a = DoCloseProject( savingAsk );
				if( !a ) return;
			}
			MasterExitFlag++;
			break;
	}
}


void DoEditChoice( short menuItem )
{
	WindowRef window;
	short a,b;

	window = FrontWindow();
	if( menuItem != kItemUndo )	undoCode = 0;
	switch ( menuItem )
	{
		case kItemUndo:
			UndoText( window);
			break;

		case kItemCut:
 			if( ActiveTE != nil )
 			{
 				if( (*ActiveTE)->selStart != (*ActiveTE)->selEnd )
 				{
	 				undoCode = 1;
	 				undoStart = (*ActiveTE)->selStart;
	 				undoEnd = (*ActiveTE)->selEnd;
					TECut(ActiveTE);
					ZeroScrap();
					TEToScrap();
					TextFix( window );
					MarkChangeTE();
				}
     	 	}
      	 	break;

		case  kItemCopy:
	 		if( ActiveTE != nil )
	 		{
	 			TECopy(ActiveTE); 	ZeroScrap(); TEToScrap();
 				undoCode = 0;
	 		}
			break;

		case kItemPaste:
			TEFromScrap();
			if( ActiveTE != nil && TEGetScrapLength() )
 			{
				undoStart = (*ActiveTE)->selStart;
				if(  (*ActiveTE)->selStart != (*ActiveTE)->selEnd )
				{
					undoEnd = (*ActiveTE)->selEnd;
					TESetSelect( undoEnd, undoEnd, ActiveTE );
					undoCode = 3;   // delete and paste
 				}
 				else undoCode = 2; // just paste
				//TEFromScrap();
				b = TEGetScrapLength();
				TEPaste(ActiveTE);
				if( undoCode == 3 ) 
				{	// put old selected text in local scrap
					TESetSelect( undoStart, undoEnd, ActiveTE );
					TECut(ActiveTE);
				}
				undoEnd = undoStart+b;
				TESetSelect( undoEnd, undoEnd, ActiveTE );
				TextFix( window );
 				MarkChangeTE( );
    	 	}
      	 	break;

		case kItemClear:
 			if( ActiveTE != nil )
 			{
 				if( (*ActiveTE)->selStart != (*ActiveTE)->selEnd )
 				{
	 				undoCode = 4;  // delete
	 				undoStart = (*ActiveTE)->selStart;
	 				undoEnd = (*ActiveTE)->selEnd;
					TECut(ActiveTE);
					TextFix( window );
					MarkChangeTE( );
				}
    	 	}
			break;

		case kItemSelectAll:
			if( ActiveTE != nil ) TESetSelect( 0, 32767, ActiveTE);
			break;

		case kItemFind:
			if( ActiveTE != nil ) TextFind( window );
			break;
			
		case kItemFindNext:
			if( FindFlag == 1 )
			{
				a = DoTextFind( /*window*/ );
				//if( a == 1 ) TESelView( ActiveTE );
			}
			break;

		case kItemReplace:
			TextReplace( window );
			MarkChangeTE( );
			break;

		case kItemReplaceNext:
			if( ReplaceFlag == 1 )
			{
				MarkChangeTE( );
				a = DoTextFind( /*window*/ );
				if( a == 1 )
				{
					TESelView( ActiveTE );
					ParamText( FindText, ReplaceTextWith, nil, nil );
					SetCursor( &qd.arrow );
					a = Alert( 129, nil );

					// exit if the user canceled the alert box
					switch( a )
					{
						case 1:	// Yes
							TECut( ActiveTE);
							TEInsert( ReplaceTextWith+1, (long) ReplaceTextWith[0], ActiveTE );
							TextFix( window );
							break;
						case 2: // No
							break;
						case 3:	// Cancel
							break;
					}

				}
			}
			break;

		case 14:	// Goto Line
			GotoLineBox( window );	
			break;
					
		/*case kItemFindNext:
			if( FindFlag == 1 )
			{
				a = DoTextFind( we );
				if( a == 1 ) WESelView( we );
			}
			break;
			


		case kItemReplaceNext:
			if( ReplaceFlag == 1 )
			{
				a = DoTextFind( we );
				if( a == 1 )
				{
					WESelView( we );
					ParamText( FindText, ReplaceTextWith, nil, nil );
					SetCursor( &qd.arrow );
					a = Alert( 129, sFilterProc );

					// exit if the user canceled the alert box
					switch( a )
					{
						case 1:	// Yes
							WECut( we);
							WEInsert( ReplaceTextWith+1, (long) ReplaceTextWith[0], nil, nil, we );
							break;
						case 2: // No
							break;
						case 3:	// Cancel
							break;
					}

				}
			}
			break;
		*/
	}
}

extern char		AppLaunchFlag;

void DoRunChoice( short menuItem )
{
	WindowRef window = FrontWindow();
	short a;
	
	switch( menuItem )
	{
		case kItemRunComp:
			a = CloseAllWindows( );
			if( !a ) return;
			AppLaunchFlag = 44;
		 	LaunchCompiler(  );
			break;
		case kItemCompile:
			a = CloseAllWindows( );
			if( !a ) return;
			AppLaunchFlag = 45;
		 	LaunchCompiler(  );
			break;
		case kItemRun:
			LaunchRT(  );
			break;
	}
}			
/*
void DoHelpChoice( short menuItem )
{
	WindowRef window = FrontWindow();

	switch( menuItem )
	{
		case 1:
		 	LaunchHelp(  );
			break;
	}
}*/			

void DoPrefChoice( short menuItem )
{
	Str255		mtext;
	WindowRef window = FrontWindow();
	MenuRef			menu;

	menu = GetMenuHandle( kMenuTools );

	if( menuItem>=7) // Custom Library
	{
		SetItemMark( menu, menuItem, checkMark );
		GetMenuItemText( menu, menuItem, mtext );
		Str255Copy( mtext, Target);
		SaveProject( );
		return;
	}
	switch( menuItem )
	{
		case 1:
			GetAppExe();
			break;
		case 2:
			GetMemSize(  );
			break;
		case 4:
			BV_CreateToolBar();
			break;
		case 5:
			CreatePropWindow();
			break;
	}
}			



/*void DoProjectChoice( short menuItem )
{
	short		a=2, b=1;
	Str255		mtext, m2;

	GetMenuItemText( ProjectMenu, menuItem, mtext );

short LoadSourceFile( short fddi, Str255 fname )
	
	if( !CmdKeyFlag )
	{
		if(mtext[1]=='[' ) 
		{
			// Heh a resource file
			while( mtext[a] !=']' && mtext[a] ) 
			{
				m2[b++] = mtext[a++];
			}
			m2[b]=0;
			b--;
			m2[0]=b;
			a = ResLaunch( m2 );
		}
		else OpenSource( mtext );
	}
	else
	{
		// Remove file from project
		DelMenuItem( ProjectMenu, menuItem );
		SaveProject( );
	}
}*/

void DoFormMenuChoice( short menuItem )
{
	short		a=2, b=1;
	Str255		mtext, m2;

	GetMenuItemText( FormsSubMenu, menuItem, mtext );
	
	if( !CmdKeyFlag )
	{
		LoadSourceFile( 1, mtext );
	}
	else
	{
		// Remove file from project
		DeleteMenuItem( FormsSubMenu, menuItem );
		SaveProject( );
	}
}

void DoModuleMenuChoice( short menuItem )
{
	short		a=2, b=1;
	Str255		mtext, m2;

	GetMenuItemText( ModulesSubMenu, menuItem, mtext );
	
	if( !CmdKeyFlag )
	{
		LoadSourceFile( 2, mtext );
	}
	else
	{
		// Remove file from project
		DeleteMenuItem( ModulesSubMenu, menuItem );
		SaveProject( );
	}
}

void DoResourceMenuChoice( short menuItem )
{
	short		b;
	Str255		mtext, m2;

	GetMenuItemText( ResourcesSubMenu, menuItem, mtext );
	
	if( !CmdKeyFlag )
	{
		// Heh a resource file
		b =  mtext[0];
		mtext[b+1]=0;
		b = ResLaunch( mtext );
	}
	else
	{
		// Remove file from project
		DeleteMenuItem( ResourcesSubMenu, menuItem );
		SaveProject( );
	}
}

void DoMenuChoice( long menuChoice, const EventRecord *theEvent )
{
	short menuID, menuItem;

	// extract menu ID and menu item from menuChoice

	menuID = HiWord( menuChoice );
	menuItem = LoWord( menuChoice );

	// dispatch on menuID

	if( menuID	!= kMenuEdit )		undoCode = 0;

	switch( menuID )
	{
		case kMenuApple:
			if (theEvent->modifiers & optionKey)
			{
				if (theEvent->modifiers & controlKey)
				{
					if (theEvent->modifiers & shiftKey)
					{
						secret=true;
					}
				}
			}
			DoAppleChoice( menuItem, theEvent );
			break;

		case kMenuFile:
			DoFileChoice( menuItem );
			break;

		case kMenuEdit:
			DoEditChoice( menuItem );
			break;

		case kMenuRun:
			DoRunChoice( menuItem );
			break;

		case kMenuTools: // Now Prefs
			DoPrefChoice( menuItem );
			break;
			
		case 101:
			DoFormMenuChoice( menuItem );
			break;

		case 102:
			DoModuleMenuChoice( menuItem );
			break;
			
		case 103:
			DoResourceMenuChoice( menuItem );
			break;
		
		case 130:
			DoWebChoice( menuItem );
			break;
			
		//case kMenuHelp:
			//DoHelpChoice( menuItem );
			//break;

		case 6:		// ProjectMenu
			//DoProjectChoice( menuItem );
			break;
	}
	HiliteMenu( 0 );
}


OSErr	InitializeMenus( void )
{
	OSErr		err = noErr;

	// build up the whole menu bar from the 'MBAR' resource

	MenuBarHandle	mbar = GetNewMBar(kMenuBarID);
	dAssert(mbar != NULL);
	SetMenuBar(mbar);
	
	MenuHandle appleMenu = GetMenuHandle(kMenuApple);
	AppendResMenu(appleMenu, kTypeDeskAccessory);
	
		err = ResError();
	if ( err != noErr ) 	return err;
	
	// Add Custom Library Files Names Here 4.0
	AddCustomLibs();
	
	// draw the menu bar
	DrawMenuBar();

	return err;
}

// Directory Search Code 4.0

Str255	tempFilename;		/* some global buffers */

void AddCustomLibs()
{
	MenuRef	menu;
	CInfoPBRec	cipbr;				/* local pb */
	HFileInfo	*fpb = (HFileInfo *)&cipbr;	/* to pointers */
	DirInfo	*dpb = (DirInfo *) &cipbr;
	short	rc, idx, err;
	FSSpec	rt;

	menu = GetMenuHandle( kMenuTools );
	err = FSMakeFSSpec(IDESpec.vRefNum,IDESpec.parID, "\p:Tools:Libraries:Standard FAT Library", &rt);
	if( err ) { {SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);}return; }

	fpb->ioVRefNum = rt.vRefNum;		/* default volume */
	fpb->ioNamePtr = tempFilename;	/* buffer to receive name */

	for( idx=1; TRUE; idx++) {	/* indexing loop */
		fpb->ioDirID = rt.parID;		/* must set on each loop */
		fpb->ioFDirIndex = idx;

		rc = PBGetCatInfoSync( &cipbr );
		if (rc) break;	/* exit when no more entries */

		if(fpb->ioFlFndrInfo.fdType =='APPL')
		{
			if(	!Str255Cmp(  tempFilename, "\pStandard FAT Library") && 
			!Str255Cmp(  tempFilename, "\pStandard PPC Library") && 
			!Str255Cmp(  tempFilename, "\pStandard 68K Library") )
			AppendMenu( menu, tempFilename);
		}
	}
}

