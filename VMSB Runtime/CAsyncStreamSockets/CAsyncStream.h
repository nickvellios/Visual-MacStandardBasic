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
#pragma once

#include <OpenTransport.h>
#include <OpenTptInternet.h>
#include <stdlib.h>
#include <stdio.h>

struct EPInfo {
    EndpointRef ep;
    char *address;
    Boolean used;
    Boolean ready;

    class CAsyncStream *current;
};
enum ConnectionStatus {
    kNotConnected = 1,
    kNotRecognized = 2,
    kConnected = 3
};

class CAsyncStream {
public:
    CAsyncStream();

    virtual    ~CAsyncStream();

    virtual UInt32 Read(void *, UInt32);

    virtual UInt32 ReadAll(void *);

    virtual void Write(void *, UInt32);

    virtual void LookAhead(void *, UInt32);

    virtual UInt32 AmountAvailable();

    virtual void LocalAddress(void *);

protected:
    EndpointRef mEndpointRef;
    TEndpointInfo mEndpointInfo;
    InetSvcRef mInetSvcRef;
    InetHost mHostAddress;
    InetAddress mDestAddress;
    char *mAddress;
    UInt16 mPort;
    EPInfo mConnectRef;
    Boolean fConnected;
    Boolean fError;

    void DoConnect(char *, UInt16);

    void DoClose();

    void DoLookupName(char *);

    void DoBind();

    void DoUnbind();

    static pascal void Notifier(void *, OTEventCode, OTResult, void *);

    //Events to override
    virtual void Connected();

    virtual void DataAvailable(EndpointRef);

    virtual void Error();
};