/*
	File:		Subwoofer.h

	Contains:	<contents>

	Written by:	Frederic Artru

		Copyright:	� 1997 by Apple Computer, Inc., all rights reserved.

     You may incorporate this sample code into your applications without
    restriction, though the sample code has been provided "AS IS" and the
    responsibility for its operation is 100% yours.  However, what you are
    not permitted to do is to redistribute the source as "DSC Sample Code"
    after having made changes. If you're going to re-distribute the source,
    we require that you make it clear in the source that the code was
    descended from Apple Sample Code, but that you've made changes.

	Change History (most recent first):

        <24>     3/13/97    FA      no stuffit error. Adding kURLInvalidCallError
        <23>     3/12/97    FA      adding periodic event
        <22>     3/11/97    FA      typo in errors
        <21>     3/10/97    FA      States aren't flags anymore
        <20>      3/7/97    FA      adding kURLResourceFoundStateFlag and kURLResourceFoundEvent.
                                    Removing URLIsValid. Removing BuffersAreLow event and state.
        <19>      3/7/97    FA      adding kURLResourceNotFoundError
        <18>      3/5/97    FA      adding kURLFileEmptyError
        <17>      3/4/97    FA      adding kURLUserCancelledError
        <16>      3/3/97    FA      Error in URLGetError
        <15>     2/27/97    FA      API changes for post processing
        <14>     2/26/97    FA      states are flags
        <13>     2/26/97    FA      new properties and flag change
        <12>     2/20/97    FA      making more generic
        <11>     2/20/97    FA      URLGetError now returns URL error as a parameter. changed some
                                    types. const char* defined as extern.
        <10>     2/17/97    FA      removing URLSupportsScheme
         <9>     2/12/97    FA      wrong error code base
         <8>     2/12/97    FA      New changes to API
         <7>      2/7/97    FA      More implementation
         <6>     1/29/97    FA      Nee API changes
         <5>     1/24/97    FA      Updated after new comments to API design.
         <4>     1/23/97    FA      Incorporated Nick Kledzik's comments.
         <3>     1/21/97    FA      Added new calls and properties.
         <2>     1/17/97    FA      Added API
         <1>     1/15/97    FA      first checked in
*/

#ifndef __SUBWOOFER__
#define __SUBWOOFER__

#include <Types.h>
#include <Files.h>

#ifdef __cplusplus
extern "C" {
#endif

//                                    ERRORS
enum
{
	kURLSubwooferErrorCodeBase				= -30770, // ��� lower limit is -30789
	kURLNoErr								= noErr,
	
	kURLInvalidURLReferenceError			= kURLSubwooferErrorCodeBase - 0,
	kURLInvalidParameterError				= kURLSubwooferErrorCodeBase - 1,
	kURLProgressAlreadyDisplayedError		= kURLSubwooferErrorCodeBase - 2,
	kURLFileExistsError						= kURLSubwooferErrorCodeBase - 3,
	kURLInvalidURLError						= kURLSubwooferErrorCodeBase - 4,
	kURLUnsupportedSchemeError				= kURLSubwooferErrorCodeBase - 5,
	kURLServerBusyError						= kURLSubwooferErrorCodeBase - 6,
	kURLAuthenticationError					= kURLSubwooferErrorCodeBase - 7,
	kURLUnkownPropertyError					= kURLSubwooferErrorCodeBase - 8,
	kURLPropertyBufferToSmallError			= kURLSubwooferErrorCodeBase - 9,
	kURLUnsetablePropertyError				= kURLSubwooferErrorCodeBase - 10,
	kURLInvalidCallError					= kURLSubwooferErrorCodeBase - 11,
	kURLNoInternetConfigError				= kURLSubwooferErrorCodeBase - 12,
	kURLUserCancelledError					= kURLSubwooferErrorCodeBase - 13,
	kURLFileEmptyError						= kURLSubwooferErrorCodeBase - 14,
	kURLResourceNotFoundError				= kURLSubwooferErrorCodeBase - 15
};

// 											TYPES

typedef struct OpaqueURLReference* URLReference;

// download flags
typedef UInt32 URLOpenFlags;
enum
{
	kURLReplaceExistingFileFlag				= 0x00000001,
	kURLExpandFileFlag						= 0x00000002,
	kURLDisplayProgressFlag					= 0x00000004,
	kURLDisplayAuthFlag						= 0x00000008
};

// state flags
typedef UInt32 URLState;
enum
{
	kURLNullState							= 0,
	kURLInitiatingState						= 1,
	kURLLookingUpHostState					= 2,	// NOT IN EXT HEADER
	kURLConnectingState						= 3,	// NOT IN EXT HEADER
	kURLResourceFoundState					= 4,	
	kURLDownloadingState					= 5,
	kURLDataAvailableState					= 0x10 + kURLDownloadingState,
	kURLDownloadCompleteState				= 6,
	kURLErrorOccurredState					= 7,
	kURLAbortingState						= 8,
	kURLCompletedState						= 9
};

// events
typedef UInt32 URLEvent;
enum
{
	kURLInitiatedEvent						= kURLInitiatingState,
	kURLResourceFoundEvent					= kURLResourceFoundState,
	kURLDownloadingEvent					= kURLDownloadingState,
	kURLAbortInitiatedEvent					= kURLAbortingState,
	kURLCompletedEvent						= kURLCompletedState,
	kURLErrorOccurredEvent					= kURLErrorOccurredState,

	kURLDataAvailableEvent					= kURLDataAvailableState,
	kURLDownloadCompleteEvent				= kURLDownloadCompleteState,
	
	kURLPercentEvent						= 30,
	kURLPeriodicEvent						= 31,
	kURLPropertyChangedEvent				= 32	
};

// event flags
typedef UInt32 URLEventFlags;
enum
{
	kURLinitiatedEventFlag					= 1 << (kURLInitiatedEvent - 1),
	kURLResourceFoundEventFlag				= 1 << (kURLResourceFoundEvent - 1),
	kURLDownloadingFlag						= 1 << (kURLDownloadingEvent - 1),
	kURLAbortInitiatedFlag					= 1 << (kURLAbortInitiatedEvent - 1),
	kURLCompletedEventFlag					= 1 << (kURLCompletedEvent - 1),
	kURLErrorOccurredEventFlag				= 1 << (kURLErrorOccurredEvent - 1),

	kURLDataAvailableEventFlag				= 1 << (kURLDataAvailableEvent - 1),
	kURLDownloadCompleteEventFlag			= 1 << (kURLDownloadCompleteEvent - 1),

	kURLPercentEventFlag					= 1 << (kURLPercentEvent - 1),
	kURLPeriodicEventFlag					= 1 << (kURLPeriodicEvent - 1),
	kURLPropertyChangedEventFlag			= 1 << (kURLPropertyChangedEvent - 1),
	
	kURLAllBufferEventsFlag					= kURLDataAvailableEventFlag
											+ kURLDownloadCompleteEventFlag,
	kURLAllNonBufferEventsFlag				= kURLinitiatedEventFlag
											+ kURLDownloadingFlag
											+ kURLAbortInitiatedFlag
											+ kURLCompletedEventFlag
											+ kURLErrorOccurredEventFlag
											+ kURLPercentEventFlag
											+ kURLPeriodicEventFlag
											+ kURLPropertyChangedEventFlag,
	kURLAllEventsFlag						= kURLAllBufferEventsFlag
											+ kURLAllNonBufferEventsFlag

};

// universal properties
extern const char* kURLURL;
extern const char* kURLResourceSize;
extern const char* kURLLastModifiedTime;
extern const char* kURLMIMEType;
extern const char* kURLFileType;
extern const char* kURLFileCreator;
extern const char* kURLCharacterSet;
extern const char* kURLResourceName;
extern const char* kURLHost;
extern const char* kURLAuthType;
extern const char* kURLUserName;
extern const char* kURLPassword;
extern const char* kURLStatusString;
extern const char* kURLIsSecure;
extern const char* kURLCertificate;

// http and https properties
extern const char* kURLHTTPRequestMethod;
extern const char* kURLHTTPRequestHeader;
extern const char* kURLHTTPRequestBody;
extern const char* kURLHTTPRespHeader;

// authentication type flags
enum
{
	kUserNameAndPaswordFlag					= 0x00000001
};

//                                    CALLBACK ROUTINES

typedef OSStatus (*URLNotifyProcPtr)(		UInt32 refCon,
											URLReference urlRef,
											URLEvent event,
											const char* changed,
											UInt32 downloadSize);

//                                    URLDOWNLOAD ROUTINE

extern OSStatus URLDownload(				const char* url,
											FSSpec *destination,
											URLOpenFlags openFlags);

//                                    URL REFERENCE ROUTINES

extern OSStatus URLNewReference(			const char* url,
											URLReference *urlRef);
extern OSStatus URLDisposeReference(		URLReference urlRef);

//                                    DOWNLOAD ROUTINES

extern OSStatus URLOpen(					URLReference urlRef,
											FSSpec* file,
											URLOpenFlags openFlags,
											URLNotifyProcPtr notifyProc,
											URLEventFlags eventRegister,
											UInt32 refCon);
extern OSStatus URLAbort(					URLReference urlRef);
extern OSStatus URLGetDataAvailable(		URLReference urlRef,
											Size *dataSize);
extern OSStatus URLGetBuffer(				URLReference urlRef,
											void** buffer,
											Size *bufferSize);
extern OSStatus URLReleaseBuffer(			URLReference urlRef,
											void* buffer);
extern OSStatus URLReadBuffer(				URLReference urlRef,
											void* buffer,
											Size requestedSize,
											Size *actualSize);

//                                    URL INFORMATION ROUTINES

extern OSStatus URLGetProperty(				URLReference urlRef,
											const char* property,
											void* propertyBuffer,
											Size bufferSize);
extern OSStatus URLGetPropertySize(			URLReference urlRef,
											const char* property,
											Size *propertySize);
extern OSStatus URLSetProperty(				URLReference urlRef,
											const char* property,
											void* propertyBuffer,
											Size bufferSize);

//                                    UTILITY ROUTINES

extern OSStatus URLGetCurrentState(			URLReference urlRef,
											URLState *state);
extern OSStatus URLGetError(				URLReference urlRef,
											OSStatus *urlError);

#ifdef __cplusplus
}
#endif

#endif /* __SUBWOOFER__ */
