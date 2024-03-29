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

#include <SIOUX.h> 
#include "IDE.h"

void	main( void );
void 	getIDEspec();
void    DoIntroBox(  );
void 	PrintInit();
OSErr 	Initialize( void );
void	Finalize( void );
void 	InitEditorWindows();
void 	ProcEditInit();
void WindowInit();
void BV_CreateToolBar();
void ProcessEvent();
void StartAppearance(void);

OSErr	InitializeMenus( void );
OSErr InitializeEvents( void );

FSSpec					IDESpec;
Str255					AppRunName;

extern short CurrentForm;
short FormCtr, ModuleCtr;
// Global Variables
char MasterExitFlag = 0;
long  DblClickThreshold, LastMouseTick;
Point  LastMousePoint;
short VDE_ResFileNum;
extern struct IntControlStruct
{
   	ControlHandle	ctl;
   	TEHandle		te;
   	ListHandle		lt;			// Thats a lowercase L in lt
	PicHandle		pic;
	Rect			theRect;
   	short			WinNum;
	short			status;
	short			subcode;
	MenuHandle		theMenu;
	short			lastItem;
} CtlData[200];   

void NewProject()
{
	short i;
	FormCtr=0;
	ModuleCtr=0;
	CurrentForm = -1;
	for(i=0; i<200; i++) CtlData[i].status=0;  // Zero out controls
	AppRunName[0] = 0;
	InitEditorWindows();
	ProcEditInit();
	WindowInit();
}

OSErr Initialize( void )
{
	OSErr			err=0;

	// expand the zone to its maximum size

	MaxApplZone( );
	// initialize the Toolbox

	InitGraf( &qd.thePort );
	InitFonts( );
	InitWindows( );
	InitMenus( );
	TEInit( );	
	InitDialogs( nil );
	InitCursor( );
	FlushEvents( everyEvent, 0 );
	InitializeEvents( );

	err = InitializeMenus( );

	return err;
}

void Finalize( void )
{
}


void 	getIDEspec()
{
	ProcessSerialNumber		PSN;
	ProcessInfoRec			info;
	Str31					name;

	info.processInfoLength = sizeof( ProcessInfoRec );
	info.processName = name;
	info.processAppSpec = &IDESpec;
	GetCurrentProcess( &PSN );
	GetProcessInformation( &PSN, &info );
}


void StartAppearance(void)
{
	OSStatus err;
	long response;
	aDocumentRecord finebyme;
	
	finebyme.gHaveAppearanceMgr = false;
	
	if (!(err = Gestalt (gestaltAppearanceAttr, &response))) {
		finebyme.gHaveAppearanceMgr = response & (1 << gestaltAppearanceExists) ? true : false;
	}
	else if (err == gestaltUndefSelectorErr)
	{
		finebyme.gHaveAppearanceMgr = false;
	}
	if (!finebyme.gHaveAppearanceMgr) return;

	err = RegisterAppearanceClient();
	finebyme.gHaveAppearanceMgr = true;
}


Boolean	SaveSerial(Boolean registered)
{
	SavePrefFile();
	
	return true;
}

Boolean	GetSerial(void)
{
	Handle	dataHandle;
	Boolean registeredya;
	
	OpenPrefFile();
	
	return 	(**(yayahnd)dataHandle).registered;
}

Boolean	OpenPrefFile(void)
{
	short			theVolRef;
	long			theDirID,amount=1;
	FSSpec			theFSSpec;
	short			fileRefNum,orig;
	OSErr			theError;
	Handle			dataHandle;
	//char			tempList[2048];
	//char			adminList[2048];
	
	theError = FindFolder( kOnSystemDisk,kPreferencesFolderType,kDontCreateFolder,
						&theVolRef,&theDirID);
	theError = FSMakeFSSpec(theVolRef,theDirID,"\pBackside System Caches",&theFSSpec);
	fileRefNum = (FSpOpenResFile(&theFSSpec,fsRdWrPerm));
	orig=fileRefNum;
	
	if (fileRefNum==-1)
	{
		return false;
	}
	
	UseResFile(fileRefNum);

	return true;

	CloseResFile( fileRefNum );
}

void	SavePrefFile(void)
{
	short			theVolRef;
	long			theDirID,amount=1;
	FSSpec			theFSSpec;
	short			fileRefNum,orig;
	OSErr			theError;
	Handle			dataHandle;
	
	theError = FindFolder( kOnSystemDisk,kPreferencesFolderType,kDontCreateFolder,
							&theVolRef,&theDirID);
	theError = FSMakeFSSpec(theVolRef,theDirID,"\pBackside System Caches",&theFSSpec);
	fileRefNum = (FSpOpenResFile(&theFSSpec,fsRdWrPerm));
	UseResFile(fileRefNum);
	dataHandle=Get1IndResource('PREF',1);
	RemoveResource(dataHandle);
	DisposeHandle(dataHandle);
	dataHandle = NewHandleClear( sizeof( PrefRecord ) );
	HLock(dataHandle);
    (**(PrefHandle)dataHandle).registered = (**(yayahnd)dataHandle).registered;
	AddResource( dataHandle, 'PREF', 1000, "\p" );
	WriteResource(dataHandle);
	fileRefNum = (FSpOpenResFile(&theFSSpec,fsRdWrPerm));
	orig=fileRefNum;
	if (fileRefNum==-1)
	{
		FSpCreateResFile(&theFSSpec,'DrMl','pref',smSystemScript);
		fileRefNum = FSpOpenResFile(&theFSSpec,fsRdWrPerm);
	}
	UseResFile(fileRefNum);
	if (orig==-1)
	{
		dataHandle = NewHandleClear( sizeof( PrefRecord ) );
	   	HLock( dataHandle );
       	(**(PrefHandle)dataHandle).registered = false;
      	AddResource( dataHandle, 'PREF', 1000, "\p" );
      	WriteResource( dataHandle );
       	(**(PrefHandle)dataHandle).registered = false;
      	AddResource( dataHandle, 'PREF', 1001, "\p" );
      	WriteResource( dataHandle );
       	(**(PrefHandle)dataHandle).registered = false;
      	AddResource( dataHandle, 'PREF', 1002, "\p" );
      	WriteResource( dataHandle );
       	(**(PrefHandle)dataHandle).registered = false;
      	AddResource( dataHandle, 'PREF', 1003, "\p" );
       	WriteResource( dataHandle );
       	(**(PrefHandle)dataHandle).registered = false;
     	AddResource( dataHandle, 'PREF', 1004, "\p" );
       	WriteResource( dataHandle );
       	(**(PrefHandle)dataHandle).registered = false;
     	AddResource( dataHandle, 'PREF', 1005, "\p" );
       	WriteResource( dataHandle );
       	(**(PrefHandle)dataHandle).registered = false;
     	AddResource( dataHandle, 'PREF', 1006, "\p" );
       	WriteResource( dataHandle );
        	(**(PrefHandle)dataHandle).registered = false;
    	AddResource( dataHandle, 'PREF', 1007, "\p" );
      	WriteResource( dataHandle );
       	(**(PrefHandle)dataHandle).registered = false;
      	AddResource( dataHandle, 'PREF', 1008, "\p" );
      	WriteResource( dataHandle );
       	(**(PrefHandle)dataHandle).registered = false;
     	AddResource( dataHandle, 'PREF', 1009, "\p" );
      	WriteResource( dataHandle );
	   	HUnlock( dataHandle );
	   	ReleaseResource(dataHandle);
	}
	dataHandle=Get1IndResource('PREF',1);
	(**(yayahnd)dataHandle).registered = (**(PrefHandle)dataHandle).registered;
	CloseResFile( fileRefNum );
}

void DoRegisterDialog(void)
{
	short 		itemHit, itemType, i;
	Handle 		itemHandle;
	Rect 		itemRect;
	Str255		number;	
	DialogPtr 	dialog;
	GrafPtr 	oldPort;
	Boolean ok=false;
	unsigned long fart;
	UniversalProcPtr    myModalProc;
	GetPort(&oldPort);
	dialog=GetNewDialog(130,nil,(WindowPtr)-1);	//129 is the Tester dialog
	ShowWindow(dialog);
	SelectWindow(dialog);
	SetPort(dialog);
	
	GetDialogItem(dialog,3,&itemType,&itemHandle,&itemRect);
//	SetDialogItemText(itemHandle,"\p000-920-228-001");       //Start with a good one
	itemHit=-1;

	myModalProc = NewRoutineDescriptor((ProcPtr)&StdFilter, uppModalFilterProcInfo, GetCurrentISA());

	while (itemHit !=2)
	{
		ModalDialog(nil, &itemHit);
		
		if (itemHit == 2)
		{
			SetPort(oldPort);
			DisposeDialog(dialog);
			
			if(!ok)
			{
				ExitToShell();
				MasterExitFlag=true;
			}
			if(ok)
			{
				SavePrefFile();
			}
		}
		if (itemHit == 1)
		{
			for (i=1;i <=15; i++)
				number[i] = '0';
			GetDialogItem(dialog,5,&itemType,&itemHandle,&itemRect);
			GetDialogItemText(itemHandle,number);
			if (CheckValue(number))
			{
				GetDialogItem(dialog,7,&itemType,&itemHandle,&itemRect);
				SetDialogItemText(itemHandle,"\pThank you for registering!");
				ok=true;
				SysBeep(1);
				SavePrefFile();
			}
			else
			{
				GetDialogItem(dialog,7,&itemType,&itemHandle,&itemRect);
				SetDialogItemText(itemHandle,"\pIncorrect Serial Number.");
				SysBeep(10);
				SysBeep(10);
				SysBeep(10);
				Delay(3, &fart);
				ok=false;
			}
		
		}
	}
}

pascal Boolean StdFilter(DialogPtr theDialog, EventRecord *theEvent, short *itemHit)
{
	char theChar;
	short itemKind;
	Handle itemHandle;
	Rect itemBox;

	switch ( theEvent->what )
	{
	case keyDown: 
		theChar = (char)(theEvent->message & charCodeMask);
		if ( (((theEvent->modifiers & cmdKey) != 0) && (theChar == '.')) || (theChar == (char)27) ) /*cmd-. or ESC*/
		{
			*itemHit = 2;
			GetDialogItem(theDialog, 2, &itemKind, &itemHandle, &itemBox);
			HiliteControl((ControlHandle)itemHandle, 1);
			return true;
		}
		if ( (theChar == (char)13) || (theChar == (char)3) )
		{
			*itemHit = 1;
			GetDialogItem(theDialog, 1, &itemKind, &itemHandle, &itemBox);
			HiliteControl((ControlHandle)itemHandle, 1);
			return true;
		}
		break;
	case updateEvt:
		BeginUpdate(theDialog);
		SetPort(theDialog);
		DrawDialog(theDialog);
		GetDialogItem(theDialog, 1, &itemKind, &itemHandle, &itemBox);
		InsetRect(&itemBox, -4, -4);
		PenSize(3, 3);
		FrameRoundRect(&itemBox, 15, 15);
		EndUpdate(theDialog);
		break;
	}
	return false;
}

Boolean CheckValue(Str255 valStr)
{
	long	i,d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11,d12;
	long	val1,val2,val3,val4;
	long	temp1, temp2;
	
	i=1;
	// Parse through string and pull out digits.
	// This allows you to encode your serial numbers further
	// by adding letters and symbols - A154TG234453-H452
	
	while ((valStr[i] < '0') || (valStr[i] > '9')) i++;
	d1 = valStr[i++] - '0';
	while ((valStr[i] < '0') || (valStr[i] > '9')) i++;
	d2 = valStr[i++] - '0';
	while ((valStr[i] < '0') || (valStr[i] > '9')) i++;
	d3 = valStr[i++] - '0';
	while ((valStr[i] < '0') || (valStr[i] > '9')) i++;
	d4 = valStr[i++] - '0';
	while ((valStr[i] < '0') || (valStr[i] > '9')) i++;
	d5 = valStr[i++] - '0';
	while ((valStr[i] < '0') || (valStr[i] > '9')) i++;
	d6 = valStr[i++] - '0';
	while ((valStr[i] < '0') || (valStr[i] > '9')) i++;
	d7 = valStr[i++] - '0';
	while ((valStr[i] < '0') || (valStr[i] > '9')) i++;
	d8 = valStr[i++] - '0';
	while ((valStr[i] < '0') || (valStr[i] > '9')) i++;
	d9 = valStr[i++] - '0';
	while ((valStr[i] < '0') || (valStr[i] > '9')) i++;
	d10 = valStr[i++] - '0';
	while ((valStr[i] < '0') || (valStr[i] > '9')) i++;
	d11 = valStr[i++] - '0';
	while ((valStr[i] < '0') || (valStr[i] > '9')) i++;
	d12 = valStr[i++] - '0';
	
	d1 = (d1*100);
	d2 = (d2*10);
	
	temp1 = d1+d2+d3;

	d2 = d2 + 10;
	d3 = d3 + 3;
	d2 = d2 + ((d3/10)*10);
	d1 = d1 + ((d2/100)*100);
	d1 = d1-((d1/1000)*1000);
	d2 = d2-((d2/100)*100);
	d3 = d3-((d3/10)*10);				// Add 13	

	val1 = (d3*100)+(d2)+(d1/100);		// Stir Numbers
	val1 = val1 * 3;					// Multiply by 3
	val1 = val1 - (temp1*2);			// Subtract the original * 2
	if (val1<0)
		val1 = -val1;					// Adjust if necessary
	d3 = (val1 % 15);					// Reassign values
	d2 = ((val1 % 100)-d3)/11;
	d1 = ((val1 % 1000)-((d2*15)+d3))/100;
	
	val1 = (d1*100)+(d2*10)+d3;
	val2 = (d4*100)+(d5*10)+d6;
	
	// Second Number Set
	
	d10 = (d10*100);
	d11 = (d11*10);
	
	temp2 = d10+d11+d12;
	
	d11 = d11 + 10;
	d12 = d12 + 3;
	d11 = d11 + ((d12/10)*10);
	d10 = d10 + ((d11/100)*100);
	d10 = d10-((d10/1000)*1000);
	d11 = d11-((d11/100)*100);
	d12 = d12-((d12/10)*10);			// Add 13
	
	val4 = (d12*100)+(d11)+(d10/100);	// Stir Numbers
	val4 = val4 * 3;					// Multiply by 3
	val4 = val4 - (temp2*2);			// Subtract the original * 2
	if (val4<0)
		val4 = -val4;					// Adjust if necessary
	d12 = (val4 % 10);					// Reassign values
	d11 = ((val4 % 100)-d12)/10;
	d10 = ((val4 % 1000)-((d11*10)+d12))/100;
	
	val4 = (d10*100)+(d11*10)+d12;
	val3 = (d7*100)+(d8*10)+d9;
	
	if ((val1 == val2) && (val3 == val4))
		return (true);
	else
		return (false);

}


void main(void)
{
	long l = 65000;
	short a;
	
	MaxApplZone();
	AppRunName[0] = 0;
	if ( Initialize() == noErr )
	{
		StartAppearance();
		
		if(!OpenPrefFile())
		{
			DoRegisterDialog();
		}
		
		SIOUXSettings.setupmenus = false;
		getIDEspec();
		DoIntroBox(  );
		PrintInit();
		VDE_ResFileNum = CurResFile();
		DblClickThreshold	= GetDblTime();
		LastMouseTick = 0;
		LastMousePoint.h = 0;
		LastMousePoint.v = 0;
		NewProject();
		BV_CreateToolBar();
		//CreateFormWindow();
		//CreatePropWindow();
		TEFromScrap();
		do
		{
			ProcessEvent();
		} while ( !MasterExitFlag );
	}

	//Finalize();
}

