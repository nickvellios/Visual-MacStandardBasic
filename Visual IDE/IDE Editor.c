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

extern struct	FormWins
{
	Str255		Name;
	Str255		Title;
	short		WinType;
	Rect		loc;
	short		minWidth, minHeight;
	short		maxWidth, maxHeight;
	short		gridSize, gridFlag, gridShow;
	short		ctlSelected;
} Forms[20];

extern WindowPtr	FormWin[20];

extern Rect	MasterRect;

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

extern struct ObjectPropsStruct
{
	Str63	name;
	Str255	text;
	short	left, top, width, height;
	Str63	proc;
	short	visible, value, enabled;
	Str63	fontname;
	short	fontsize, fontstyle;
	short	opt14, opt15, opt16, opt17, opt18, opt19, opt20;
	RGBColor BackRGB, ForeRGB;	
} ObjectProps[200], FormProps[20];

void CreateEditorWindow();

// -------------------------------------------------------------------------------------

void EditorCall( short ctlNum)
{
	CreateEditorWindow();
}
