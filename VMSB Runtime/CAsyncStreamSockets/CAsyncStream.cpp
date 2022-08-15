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

#include "CAsyncStream.h"
#include <String.h>

CAsyncStream::CAsyncStream()
{
	mEndpointRef	= nil;
	fConnected		= false;
}

CAsyncStream::~CAsyncStream ()
{
}

#pragma mark -

void CAsyncStream::DoConnect (char* server, UInt16 port)
{
	OSStatus 		result;
	
	mConnectRef.used 	= true;
	mConnectRef.ready 	= false;
	mConnectRef.address = new char[256];
	mConnectRef.current 	= this;
	
	DoLookupName(server);
	OTInitInetAddress(&mDestAddress, port, mHostAddress);
	
	result = OTAsyncOpenEndpoint(OTCreateConfiguration("tcp"), 0L, &mEndpointInfo, &Notifier, &mConnectRef);
	
	if (result != kOTNoError)
	{
		ExitToShell();
	}
}

void CAsyncStream::DoClose ()
{	
	fConnected 		= false;
	mEndpointRef 	= nil;
}

void
CAsyncStream::DoLookupName(char *host_name)
{
	OSStatus 		err;
	InetHostInfo	response;

	mInetSvcRef = OTOpenInternetServices(kDefaultInternetServicesPath, 0, &err);
	
	if (err != kOTNoError)
	{
		ExitToShell();
	}
	
	memset(&response, 0, sizeof(InetHostInfo));
	err = OTInetStringToAddress(mInetSvcRef, host_name, &response);
	
	if (err == noErr)
	{
		mHostAddress = response.addrs[0];
	}
}

void CAsyncStream::DoBind()
{
}

void CAsyncStream::DoUnbind()
{
}

pascal void CAsyncStream::Notifier(void* context, OTEventCode event, OTResult result, void* cookie)
{
	OSStatus		err;
	EndpointRef		ep;
	EPInfo			*eInfo;
	TDiscon 		discon;
	static Boolean	oe = false;
	InetAddress		inAddr;

	eInfo = (EPInfo*)context;
	ep = eInfo->ep;
	
	switch (event)
	{
		case T_BINDCOMPLETE:
			TCall			sndCall;
		
			if (result != kOTNoError)
			{
				ExitToShell();
			}
	
			OTInitInetAddress(&inAddr, eInfo->current->mPort, eInfo->current->mHostAddress);
			OTMemzero(&sndCall, sizeof(TCall));
			sndCall.addr.len = sizeof(InetAddress);
			sndCall.addr.buf = (UInt8*) &(eInfo->current->mDestAddress);
			sndCall.addr.maxlen = sizeof(InetAddress);
			sndCall.opt.buf 	= nil;		// no connection options
			sndCall.opt.len 	= 0;
			sndCall.opt.maxlen 	= 0;
			sndCall.udata.buf 	= nil;		// no connection data
			sndCall.udata.len 	= 0;
			sndCall.sequence 	= 0;
			
			err = OTConnect(ep, &sndCall, nil);
			
			if (err != kOTNoDataErr)
			{
				ExitToShell();
			}
			break;
			
		//returns when a connection to the server is established
		case T_CONNECT:
			TCall 		call;
			InetAddress	caddr;
			
			if (result != kOTNoError)
			{
				ExitToShell();
			}
			
			OTMemzero(&call, sizeof(TCall));
			call.addr.maxlen	= sizeof(InetAddress);
			call.addr.buf 		= (unsigned char*) &caddr;
			
			err = OTRcvConnect(ep, &call);
			if (err != kOTNoError)
			{
				ExitToShell();
			}
			
			eInfo->current->Connected();
			break;
		
		//returns when data is available to be read IMPORTANT: wont get another until data is read
		case T_DATA:
			if (result != kOTNoError)
			{
				ExitToShell();
			}
			
			//handle read
			eInfo->current->DataAvailable(eInfo->ep);
			break;
		
		//usually returns when the server send abortive disconnect
		case T_DISCONNECT:
			err = OTRcvDisconnect(ep, &discon);
			break;
			
		//returns when connection to server is closed
		case T_DISCONNECTCOMPLETE:
			break;
			
		//returns when OTAsyncOpenEndpoint() completes
		case T_OPENCOMPLETE:
			if (result != kOTNoError)
			{
				ExitToShell();
			}
			
			ep = (EndpointRef)cookie;
			eInfo->ep = ep;
			eInfo->current->mEndpointRef = ep;
			
			err = OTSetBlocking(ep);
			
			if (err != kOTNoError)
			{
				ExitToShell();
			}
			
			err = OTDontAckSends(ep);
			
			if (err != kOTNoError)
			{
				ExitToShell();
			}
	
			err = OTBind(ep, nil, nil);
			
			if (err != kOTNoError)
			{
				ExitToShell();
			}
			break;
			
		//returns when an orderly release has been received
		case T_ORDREL:
			err = OTRcvOrderlyDisconnect(ep);
			
			if (err != kOTNoError)
			{
				ExitToShell();
			}
			break;
		
		//returns upon completion of an OTUnbind()
		case T_UNBINDCOMPLETE:
			break;
			
		//unknown/unhandled OT event
		default:
			break;
	
	}
}


#pragma mark -
UInt32 CAsyncStream::AmountAvailable ()
{
	UInt32		unreadData;
	OSStatus	result;

	result = OTCountDataBytes(mEndpointRef, &unreadData);
	
	if (result == kOTNoDataErr)
	{
		return 0;
	}
	
	return unreadData;
}

void CAsyncStream::LocalAddress (void *dataPtr)
{
	#pragma unused (dataPtr)
}

void CAsyncStream::LookAhead (void* dataPtr, UInt32 dataLen)
{
	#pragma unused (dataPtr, dataLen)
}

UInt32 CAsyncStream::Read (void *dataPtr, UInt32 dataLen)
{
	OTResult	result;
	OTFlags		flags;

	result = OTRcv(mEndpointRef, dataPtr, dataLen, &flags);
	if (result > 0)
	{
		return(result);
	}
	else
	{
	}
}

UInt32 CAsyncStream::ReadAll (void* dataPtr)
{
	OTResult	result;
	OTFlags		flags = 0;

	result = OTRcv(mEndpointRef, dataPtr, AmountAvailable(), &flags);
	if (result > 0) {
		return(result);
	} else {
	}
}

void CAsyncStream::Write (void* dataPtr, UInt32 dataLen)
{
	OSStatus result;

	result = OTSnd(mEndpointRef, dataPtr, dataLen, 0);
	if (result == dataLen)
	{
		return;
	}
	else if (result < 0)
	{
	}
	else
	{
	}
}

#pragma mark -

void CAsyncStream::Connected()
{
}

void CAsyncStream::DataAvailable(EndpointRef ep)
{
}

void CAsyncStream::Error()
{
}