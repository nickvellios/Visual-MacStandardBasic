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

#include <Icons.h>
#include <DCon.h>

WindowPtr	ToolPal=nil;
short		currTool=1;
extern short CurrentForm, VDE_ResFileNum;


void BV_FindTool(const EventRecord *currEvent)
{
	GrafPtr 	oldPort;
	Point 	tempPt;
	short	v,h,a;
	
	if( CurrentForm == -1 ) { {SysBeep(30); dprintf("Error @ " __FILE__" l. %d\n", __LINE__);} return; }
	GetPort(&oldPort);
	SetPort(ToolPal);
	tempPt.v = currEvent->where.v;
	tempPt.h = currEvent->where.h;
	GlobalToLocal(&tempPt);
	v = tempPt.v /32;
	h = tempPt.h /32;
	a = (v*2) + h + 1;
	if(a>0 && a<=12) currTool = a;
	SetPort(oldPort);
}

void BV_DrawToolBar() // if flag1=1 draw all  =0 just update
{
	short ctr,t1,col,row;
	GrafPtr 	oldPort;
	Rect 	tempRect;
	OSErr	err;
	
	GetPort(&oldPort);
	SetPort(ToolPal);

	UseResFile(VDE_ResFileNum);
	SetResLoad(true);
	
	row=0;
	for(ctr=1;ctr<=12;ctr++)
	{
		col=0;
		t1= 5000+ctr;
		if(ctr==currTool) t1+=100;
		// 1st Column
		SetRect(&tempRect,col,row,col+32,row+32);
		err = PlotIconID(&tempRect, atAbsoluteCenter, ttNone, t1);
		dAssert(err == noErr);
		//if( err) printf("Err=%i %x\n",err, err);
		ctr++;
		col=32;
		
		t1= 5000+ctr;
		if(ctr==currTool) t1+=100;
		// 2nd Column
		SetRect(&tempRect,col,row,col+32,row+32);
		err = PlotIconID(&tempRect, atAbsoluteCenter, ttNone, t1);
		dAssert(err == noErr);
		//if( err) printf("Err=%i %x\n",err, err);
		row+=32;
	}
	SetPort(oldPort);
	return;
}

OSErr BV_CreateToolBar(void)
{
	OSErr		error	= noErr;
	GrafPtr 	oldPort;
	Rect 	tempRect;
	
	
	if(ToolPal!=nil)
	{
		error=1;	//Already created
		return error;
	}
	
	GetPort(&oldPort);
	
	//SetRect(&tempRect,10,100,75,229);
	SetRect(&tempRect,10,100,74,292);
	ToolPal = NewCWindow( nil, &tempRect, "\pTools", true,/* noGrowDocProc*/floatProc, (WindowPtr) (-1), true, 0); 

	
	SetPort(ToolPal);
	
	BV_DrawToolBar();
	SetPort(oldPort);
	return error;

}
