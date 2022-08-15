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
struct cmd_func2       //These are built-in functions, there will also be user defined subs
{
    short (*func_ptr)(short rtype);

    char returnType;
} cmd_functions[150] = {
        func_abs, 'L',        // 100
        func_val, 'L',        // 101
        func_chr, 'S',        // 102
        func_atan, 'L',        // 103
        func_acos, 'L',        // 104
        func_asin, 'L',        // 105
        func_cos, 'L',        // 106
        func_log, 'L',        // 107
        func_sin, 'L',        // 108
        func_sqr, 'L',        // 109
        func_tan, 'L',        // 110
        func_len, 'L',        // 111
        func_date, 'S',        // 112
        func_lcase, 'S',        // 113
        func_ucase, 'S',        // 114
        func_trim, 'S',        // 115
        func_ltrim, 'S',        // 116
        func_rtrim, 'S',        // 117
        func_space, 'S',        // 118
        func_str, 'S',        // 119
        func_string, 'S',        // 120
        func_rnd, 'L',        // 121
        func_ctext, 'S',        // 122
        func_serread, 'S',        // 123
        func_seravail, 'L',        // 124
        func_fileread, 'S',        // 125
        func_fileexist, 'L',    // 126
        func_filelenopen, 'L',    // 127
        func_fileloc, 'L',        // 128
        func_fileeof, 'L',        // 129
        func_spritewidth, 'L',    // 130
        func_spriteheight, 'L',    // 131
        func_seravail, 'L',        // 132
        func_time, 'S',        // 133
        func_day, 'S',            // 134
        func_wborder, 'L',        // 135
        func_sborder, 'L',        // 136
        func_timer, 'L',        // 137
        func_listcnt, 'L',        // 138
        func_errornum, 'L',        // 139
        func_errormsg, 'S',        // 140
        func_pagesetup, 'L',    // 141
        func_printsetup, 'L',    // 142
        func_launch, 'L',        // 143
        func_peek, 'L',            // 144
        func_peekint, 'L',        // 145
        func_peeklong, 'L',        // 146
        func_varaddr, 'L',        // 147
        func_getmemint, 'L',    // 148
        func_getmemlong, 'L',    // 149
        func_getmemsingle, 'L',    // 150
        func_getmemdouble, 'L',    // 151
        func_getmemstring, 'S',    // 152
        func_memsize, 'L',        // 153
        func_gettextlen, 'L',    // 154
        func_gettextstart, 'L',    // 155
        func_gettextend, 'L',    // 156
        func_gettextselect, 'S',// 157
        func_listselcount, 'L', // 158
        func_listselect, 'L',   // 159
        func_listitem, 'S',    // 160
        func_getctlvalue, 'L',  // 161
        func_mousex, 'L',        // 162
        func_mousey, 'L',        // 163
        func_getmousex, 'L',    // 164
        func_getmousey, 'L',    // 165
        func_mousedown, 'L',    // 166
        func_keysdown, 'L',    // 167
        func_ifkey, 'L',        // 168
        func_getkeys, 'L',        // 169
        func_fontnum, 'L',        // 170
        func_menucount, 'L',    // 171
        func_getmenumark, 'L',    // 172
        func_menutext, 'S',        // 173
        func_msgbox, 'L',        // 174
        func_eventcode, 'L',    // 175
        func_sndplaying, 'L',    // 176
        func_qtver, 'L',    // 177
        func_sndver, 'L',    // 178
        func_spkver, 'L',    // 179
        func_spkbusy, 'L',    // 180
        func_getctlborder, 'L',  // 181
        func_opendialog, 'S',    // 182
        func_savedialog, 'S',    // 183
        func_clipavail, 'L',        // 184
        func_strres, 'S',        // 185
        func_strreslen, 'L',        // 186
        func_filename, 'S',        // 187
        func_powerpc, 'L',        // 188
        func_mid, 'S',            // 189
        func_left, 'S',            // 190
        func_right, 'S',            // 191
        func_spritehit, 'L',        // 192
        func_pictfile, 'L',        // 193
        func_and, 'L',            // 194
        func_or, 'L',            // 195
        func_xor, 'L',            // 196
        func_exp, 'L',            // 197
        func_pow, 'L',            // 198
        func_filesize, 'L',        // 199
        func_typecvt, 'L',        // 200
        func_mod, 'L',            // 201
        func_asc, 'L',            // 202
        func_filereadbuffer, 'L',// 203
        func_pi, 'L',            // 204
        func_deg2rad, 'L',        // 205
        func_rad2deg, 'L',        // 206
        func_getdepth, 'L',        // 207
        func_hasdepth, 'L',        // 208
        func_lineinput, 'S',        // 209
        func_call, 'L',            // 210
        func_winhandle, 'L',        // 211
        func_ctlhandle, 'L',        // 212
        func_memhandle, 'L',        // 213
        func_winexist, 'L',        // 214
        func_ctlexist, 'L',        // 215
        func_not, 'L',            // 216
        func_winnum, 'L',        // 217
        func_ctlnum, 'L',        // 218
        func_frontwindow, 'S',    // 219
        func_wintitle, 'S',        // 220
        func_getpixel, 'L',        // 221
        func_rgb, 'L',            // 222
        func_apppath, 'S',        // 223
        func_hex, 'S',            // 224
        func_int, 'L',            // 225
        func_dir, 'S',            // 226
        func_numfiles, 'L',        // 227
        func_getfilename, 'S',    // 228
        func_instr, 'L',        // 229
        func_sercheck, 'L',        // 230
};
