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
#define	        kInFront 	(WindowPtr) -1	/* put the new window in front */
#define			MaxNumOfMenuItems 	500
#define			MaxMenus 	50

void  StrToStr255( char * st1, Str255 st2);
short FreeReturnStr();
short Allocate_ReturnStr( long);

short MENU_Add( short where, char * btext, short cmd );
short MENU_AddApple( );
short MENU_AddDesk( );
short MENU_AddFonts( short menu );
short MENU_Delete( short menu );
short MENU_DeleteItem( short menu, short where );
void MENU_Blank( );
void MENU_Redraw(  );
void MENU_Clear(  );
short MENU_Disable( short menu, short where );
short MENU_Enable( short menu, short where );
short MENU_SetMark( short menu, short where, short mark);
short MENU_SetText( short menu, short where, char * btext);
short MENU_SubMenu( short menu, short where, short cmd);
short MENU_AddItem( short menu,  char * btext, short cmd, short cmdchar );
short MENU_InsertItem( short menu, short where, char * btext, short cmd, short cmdchar );

short MENU_GetText( short menu, short where );
short MENU_Count( short menu );
short MENU_GetMark( short menu, short where );


short MenuChoiceHandler( long theChoice );
void ErrHandler( short ErrNum, Str255 AddMsg);

extern UCHAR	*ReturnStr, ReturnStrFlag;
extern long		ReturnStrLen;
extern short EventCode[4];
short  MasterMenuFlag=0;

struct MenuStruct
{
	MenuHandle	Menu;
	short		subcode;
} MenuData[52];   

struct MenuItemStruct
{
	short		menunum;
	short		itemnum;
	short		subcode;
} MenuItemData[MaxNumOfMenuItems];   



short MENU_Add( short where, char * btext, short cmd )
{
	short	a, b;
	Str255	txt;

	if(!where)
	{
		for(a=0;a<MaxMenus;a++) if( MenuData[a].Menu == nil ) break;
		if(a>=MaxMenus) { ErrHandler( 1033, nil); return( -1); }
		b = a;
	}
	else
	{
		b = where-1;
		// Move all Menus to the left
		for( a=MaxMenus-1; a > b; a--) MenuData[a].Menu = MenuData[a-1].Menu;
	}	
	
			
	StrToStr255( btext, txt);
	MenuData[b].Menu = NewMenu( b+1, txt );
	MenuData[b].subcode = cmd;
	InsertMenu( MenuData[b].Menu, where );
	DrawMenuBar();
	MasterMenuFlag =1;
	return(0);
}

short MENU_AddApple( )
{
	
	if(	MenuData[0].Menu != nil ) { ErrHandler( 1017, "\p in MenuApple command"); return( -1); }
	MenuData[0].Menu = NewMenu( 1, "\p\024" );
	InsertMenu( MenuData[0].Menu, 0 );
	DrawMenuBar();
	MasterMenuFlag =1;
	return(0);
}


short MENU_AddItem( short menu, char * btext, short cmd, short cmdchar )
{
	short a, b, c=0;
	Str255	txt;

	if(menu>MaxMenus) { ErrHandler( 1033, nil); return( -1); }
			
	StrToStr255( btext, txt);
	if( cmdchar )
	{
		a = txt[0]+1; //len
		txt[a++] = 0x2f;
		txt[a] = cmdchar;
		txt[0] = a; 
	}
	
	AppendMenu( MenuData[menu-1].Menu, txt );
	if(cmd)
	{
		a = CountMItems( MenuData[menu-1].Menu );
		for(b=0; b<MaxNumOfMenuItems; b++)
		{
			if( MenuItemData[b].menunum == -1 )
			{
				MenuItemData[b].menunum = menu;
				MenuItemData[b].itemnum = a;
				MenuItemData[b].subcode = cmd;
				c++;
				break;
			}
		} 
		if(!c) ErrHandler( 1032, nil);

	}
	return(0);
}


short MENU_InsertItem( short menu, short where, char * btext, short cmd, short cmdchar )
{
	short a, b, c=0;
	Str255	txt;

	if(menu>MaxMenus) { ErrHandler( 1033, nil); return( -1); }
			
	StrToStr255( btext, txt);
	if( cmdchar )
	{
		a = txt[0]+1; //len
		txt[a++] = 0x2f;
		txt[a] = cmdchar;
		txt[0] = a; 
	}

	InsertMenuItem( MenuData[menu-1].Menu, "\pAAA", where-1 );
	if(cmd)
	{
		a = CountMItems( MenuData[menu-1].Menu );
		for(b=0; b<MaxNumOfMenuItems; b++)
		{
			if( MenuItemData[b].menunum == menu && MenuItemData[b].itemnum >= where )
			{
				MenuItemData[b].itemnum++;
			}
			else if( MenuItemData[b].menunum == -1 )
			{
				MenuItemData[b].menunum = menu;
				MenuItemData[b].itemnum = where;
				MenuItemData[b].subcode = cmd;
				c++;
				break;
			}
		} 
		if(!c) ErrHandler( 1032, nil);
	}
	return(0);
}

short MENU_AddDesk( )
{
	if( MenuData[0].Menu != nil )
	{
		AppendResMenu( MenuData[0].Menu, 'DRVR' );
		return(0);
	}
	ErrHandler( 1017, "\p in MenuDesk command");
	return(-1);
}

short MENU_AddFonts( short menu )
{
	if( MenuData[menu-1].Menu != nil )
	{
		AppendResMenu( MenuData[menu-1].Menu, 'FONT' );
		return(0);
	}
	ErrHandler( 1017, "\p in MenuFonts command");
	return(-1);
}

short MENU_Delete( short menu )
{
	short b;

	if( MenuData[menu-1].Menu != nil )
	{
		DeleteMenu( menu );
		DisposeMenu( MenuData[menu-1].Menu );
		DrawMenuBar();
		MenuData[menu-1].Menu = nil;
		for(b=0; b<MaxNumOfMenuItems; b++)
		{
			if( MenuItemData[b].menunum == menu ) MenuItemData[b].menunum = -1;
		} 
	}
	return(0);
}


short MENU_DeleteItem( short menu, short where )
{
	short b;
	
	if(menu>MaxMenus) return( -1);
			
	DeleteMenuItem( MenuData[menu-1].Menu, where );
	DrawMenuBar();
	for(b=0; b<MaxNumOfMenuItems; b++)
	{
		if( MenuItemData[b].menunum == menu && MenuItemData[b].itemnum >= where )
		{
			if ( MenuItemData[b].itemnum == where ) MenuItemData[b].menunum = -1;
			else  MenuItemData[b].itemnum--;
		}
	} 
	return(0);
}

void MENU_Blank( )
{
	ClearMenuBar();
}

void MENU_Redraw(  )
{
	DrawMenuBar();
}

void MENU_Clear(  )
{
	short a;
	

	for(a=0;a<MaxMenus;a++)
	{
		if( MenuData[a].Menu != nil )
		{
			DeleteMenu( a+1 );
			DisposeMenu( MenuData[a].Menu );
			MenuData[a].Menu = nil;
		}
	}
	DrawMenuBar();
	for(a=0;a<MaxNumOfMenuItems;a++) MenuItemData[a].menunum=-1;		//Reset all Menu handles
}

short MENU_Disable( short menu, short where )
{
	
	if(menu>MaxMenus) return( -1);
			
	DisableItem( MenuData[menu-1].Menu, where );
	DrawMenuBar();
	return(0);
}

short MENU_Enable( short menu, short where )
{
	
	if(menu>MaxMenus) { ErrHandler( 1033, nil); return( -1); }
			
	EnableItem( MenuData[menu-1].Menu, where );
	DrawMenuBar();
	return(0);
}

short MENU_SetMark( short menu, short where, short mark )
{
	
	if(menu>MaxMenus) { ErrHandler( 1033, nil); return( -1); }
			
	SetItemMark( MenuData[menu-1].Menu, where, mark );
	DrawMenuBar();
	return(0);
}

short MENU_SetText( short menu, short where, char * btext )
{
	Str255	txt;
	
	if(menu>MaxMenus) { ErrHandler( 1033, nil); return( -1); }
	StrToStr255( btext, txt);
	SetMenuItemText( MenuData[menu-1].Menu, where, txt );
	DrawMenuBar();
	return(0);
}

short MENU_GetText( short menu, short where )
{
	short b, c, d;
	Str255	txt;
	
	if(menu>MaxMenus) { ErrHandler( 1033, nil); return( -1); }
	GetMenuItemText( MenuData[menu-1].Menu, where, txt );
	d = txt[0];
	
	// Allocate ReturnStr Space
	b = Allocate_ReturnStr( d+16 );
	if(b) return(-1); // Buffer allocation error

	for( c=0; c<d; c++)	ReturnStr[c] = txt[c+1];	
	ReturnStrLen = d;
	ReturnStr[ReturnStrLen] = 0;
	
	return(0);
}

short MENU_Count( short menu )
{
	short c = 0;
	
	if(menu>MaxMenus) { ErrHandler( 1033, nil); return( -1); }
	c = CountMItems( MenuData[menu-1].Menu );
	
	return(c);
}

short MENU_GetMark( short menu, short where )
{
	short c = 0;
	
	if(menu>MaxMenus) { ErrHandler( 1033, nil); return( -1); }
	GetItemMark( MenuData[menu-1].Menu, where, &c);

	return(c);
}


short MENU_SubMenu( short menu, short where, short cmd )
{
	short a, b, c=0;

	if(menu>MaxMenus || where < 1) return( -1);

	for(a=MaxMenus-1; a>=0; a--) if( MenuData[a].Menu == nil ) break;
	if(a<0) return( -1);
	b = a;
			
	MenuData[b].Menu = NewMenu( b+1, "\p" );
	if( MenuData[b].Menu == nil ) ErrHandler( 1017, "\p in SubMenu command");
	MenuData[b].subcode = cmd;
	InsertMenu( MenuData[b].Menu, -1);

	SetItemCmd( MenuData[menu-1].Menu, where, 0x1b );
	SetItemMark( MenuData[menu-1].Menu, where, b+1 );
	DrawMenuBar();
	return(b+1);
}


short MenuChoiceHandler( long theChoice )
{
	short menuID, menuItem, b;
	Str255	daName;
	
	menuID = HiWord( theChoice );
	menuItem = LoWord( theChoice );
	
	for(b=0; b<MaxNumOfMenuItems; b++)
	{
		if( MenuItemData[b].menunum == menuID && MenuItemData[b].itemnum == menuItem )
		{
			if( MenuItemData[b].subcode)
			{
		 	  	EventCode[0] = 13; 		EventCode[1] = MenuItemData[b].menunum;
				EventCode[2] = MenuItemData[b].itemnum;		EventCode[3] = 0;
			}
			return(  MenuItemData[b].subcode );
		}
	} 
	if(menuID == 1 && menuItem >1)
	{
		// Try desk accessory
		daName[0]=0;
		GetMenuItemText( MenuData[0].Menu, menuItem, daName );
		OpenDeskAcc( daName );	
	}
	return(0);
}
