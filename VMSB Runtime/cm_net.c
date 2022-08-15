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
// This file is new for 3.0 for doing web (SubWoofer) stuff
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <math.h>
#include        <Speech.h>
#include        <Sound.h>
#include        "Subwoofer.h"

#define         UCHAR   unsigned char
#define            kInFront    (WindowPtr) -1    /* put the new window in front */

short NET_GetURL(char *url, Str255 filename, short pbox);

short NET_GetURL(char *url, Str255 filename, short pbox) {
    short a, b = -1, rn;
    OSErr err = 0;
    FSSpec Ftemp;
    URLOpenFlags flags;
    flags = kURLReplaceExistingFileFlag + kURLExpandFileFlag + kURLDisplayAuthFlag;
    if (pbox) flags += kURLDisplayProgressFlag;
    err = FSMakeFSSpec(0, 0, filename, &Ftemp);
    a = URLDownload(url, &Ftemp, flags);
    return (0);
}
