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

#define            UCHAR    unsigned char
extern char *zbuffer;
extern char zline[512];
extern short zline_pos, zline_len;
extern long zbuf_pos, zsource_len, ztstart;
extern Str255 zparm1, zparm2;

void zsplit();

short znext_line();

void ztrimline();

void zsplit() {
    short a, b = 1, c = 1;
    a = 0;
    while (a < zline_len && b <= 255) {
        if (zline[a] == '=') break;
        else zparm1[b++] = zline[a++];
    }
    if (zline[a] == '=') a++;
    while (a < zline_len && c <= 255) {
        zparm2[c++] = zline[a++];
    }
    zparm1[0] = b - 1;
    zparm2[0] = c - 1;
    zparm1[b] = 0;
    zparm2[c] = 0;
}

void ztrimline() {
    short a;
    if (!zline_len) return;
    a = zline_len - 1;
    while (a >= 0 && (zline[a--] == ' ' || zline[a--] == 9)) zline_len--;
}

short znext_line() {
    long tc;
    short lpr;
    char loop = 0, a;
    zline_len = 0;
    zline_pos = 0;
    zline[zline_len] = 0;
    ztstart = zbuf_pos;
    while (!loop) {
        loop++;    //Set so this loop is a 1 timer
        //Ok first lets first ignore leading spaces (or tabs)
        while ((zbuffer[zbuf_pos] == ' ' || zbuffer[zbuf_pos] == 9) && zbuf_pos < zsource_len) zbuf_pos++;
        //Let's check for end of buffer
        if (zbuf_pos >= zsource_len) return (999);   //No more code
        //Let's check for blank line
        if (zbuffer[zbuf_pos] == 13) {
            zbuf_pos++;
            if (zbuffer[zbuf_pos] == 10) zbuf_pos++;  //Move past LF (if any )
            if (zbuf_pos >= zsource_len) return (999);   //No more code
            loop = 0;         //Blank line so go back to top
        }
        if (zbuffer[zbuf_pos] == 39) {
            while (zbuffer[zbuf_pos] != 13 && zbuf_pos < zsource_len) zbuf_pos++;
            zbuf_pos++;  // Skip CR
            if (zbuf_pos < zsource_len) if (zbuffer[zbuf_pos] == 10) zbuf_pos++;  //Move past LF (if any )
            loop = 0;   // This indicates a comment line
        }
        if (loop) {
            ztstart = zbuf_pos;
            while (zbuffer[zbuf_pos] != 13 && zbuf_pos < zsource_len) zline[zline_pos++] = zbuffer[zbuf_pos++];
            zline_len = zline_pos;        // Set line length
            zbuf_pos++;  // Skip CR
            if (zbuf_pos < zsource_len) if (zbuffer[zbuf_pos] == 10) zbuf_pos++;  //Move past LF (if any )
            ztrimline();        // Remove trailing spaces
            zline[zline_len] = 0;
        }
    }
    return (0);
}
