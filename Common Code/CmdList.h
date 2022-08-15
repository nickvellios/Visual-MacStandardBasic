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

struct  cmd_func1       //These are built-in cmds, there will also be user defined subs
{
	short (*func_ptr)();
}       cmd_func[200]=
	{
		cmd_dummy,      // 100
		cmd_let,        // 101
		cmd_none,      	// 102
		cmd_end,        // 103
		cmd_dummy,      // 104
		cmd_goto,       // 105
		cmd_beep,       // 106
		cmd_if,      	// 107
		cmd_else,		// 108
		cmd_endif,      // 109
		cmd_do,     	// 110
		cmd_loop,		// 111
		cmd_dummy,      // 112
		cmd_gosub,      // 113
		cmd_return,     // 114
		cmd_exitloop,   // 115
		cmd_exitsub,    // 116
		cmd_exitfunc,   // 117
		cmd_for,        // 118
		cmd_next,       // 119
		cmd_print,      // 120
		cmd_window,     // 121
		cmd_closewindow,// 122
		cmd_line,       // 123
		cmd_box,        // 124
		cmd_circle,     // 125
		cmd_ellipse,    // 126
		cmd_button,		// 127
		cmd_checkbox,	// 128
		cmd_radio,		// 129
		cmd_scrollbar,	// 130
		cmd_moveto,		// 131
		cmd_point,		// 132
		cmd_textbox,	// 133
		cmd_addmenu,	// 134
		cmd_addapple,	// 135
		cmd_addmenuitem,// 136
		cmd_adddesk,	// 137
		cmd_select,		// 138
		cmd_case,		// 139
		cmd_endcase,	// 140
		cmd_default,	// 141
		cmd_movewindow,	// 142
		cmd_sizewindow,	// 143
		cmd_windowtitle,// 144
		cmd_say,		// 145
		cmd_ctlremove,	// 146
		cmd_ctlhide,	// 147
		cmd_ctlshow,	// 148
		cmd_ctldisable,	// 149
		cmd_ctlenable,	// 150
		cmd_ctlsize,	// 151
		cmd_ctlmove,	// 152
		cmd_hidewindow,	// 153
		cmd_showwindow,	// 154
		cmd_qtopen,		// 155
		cmd_qtplay,		// 156
		cmd_qtclose,	// 157
		cmd_qtmove,		// 158
		cmd_pict,		// 159
		cmd_label,		// 160
		cmd_sndplay,	// 161
		cmd_ctltext,	// 162
		cmd_seropen,	// 163
		cmd_serclose,	// 164
		cmd_serwrite,	// 165
		cmd_fileopen,	// 166
		cmd_fileclose,	// 167
		cmd_filewrite,	// 168
		cmd_filesetloc,	// 169
		cmd_filecreate,	// 170
		cmd_spriteframe,// 171
		cmd_spritedraw, // 172
		cmd_spritecopy,	// 173
		cmd_spritedrawsize, //174
		cmd_spritecopysize, //175
		cmd_spritebegin,// 176
		cmd_spritedone, // 177
		cmd_spriteend,  // 178
		cmd_spriterelease,  // 179
		cmd_spriteclear,// 180
		cmd_spritedisplay,	// 181
		cmd_spriteload,	// 182
		cmd_list,		// 183
		cmd_listadd,    // 184
		cmd_droplist,   // 185
		cmd_licon,    	// 186
		cmd_dummy,    	// 187
		cmd_dummy,    	// 188
		cmd_setcursor, 	// 189
		cmd_showcursor,	// 190
		cmd_hidecursor,	// 191
		cmd_clearerror,	// 192
		cmd_onerror,	// 193
		cmd_boxfill,    // 194
		cmd_circlefill, // 195
		cmd_ellipsefill,// 196
		cmd_lineto,     // 197
		cmd_drawtext,   // 198
		cmd_drawfont,   // 199
		cmd_drawsize,   // 200
		cmd_drawstyle,  // 201
		cmd_setcolor,	// 202
		cmd_backcolor,	// 203
		cmd_setRGBcolor,// 204
		cmd_backRGBcolor,// 205
		cmd_redrawwindow,//206
		cmd_events,		// 207
		cmd_pensize,	// 208
		cmd_penpattern,	// 209
		cmd_penmode,	// 210
		cmd_pennormal,	// 211
		cmd_listclear,  // 212
		cmd_listremove, // 213
		cmd_textboxro,  // 214
		cmd_printopen,  // 215
		cmd_printclose, // 216
		cmd_newpage,    // 217
		cmd_poke,   	// 218
		cmd_pokeint,   	// 219
		cmd_pokelong,  	// 220
		cmd_memalloc,  	// 221
		cmd_memfree,  	// 222
		cmd_memresize,  // 223
		cmd_memfill,  	// 224
		cmd_memint,  	// 225
		cmd_memlong,  	// 226
		cmd_memsingle, 	// 227
		cmd_memdouble, 	// 228
		cmd_memstring,  // 229
		cmd_textselect, // 230
		cmd_listselect, // 231
		cmd_listdeselect, // 232
		cmd_setvalue,	// 233
		cmd_cut,		// 234
		cmd_copy,		// 235
		cmd_paste,		// 236
		cmd_clear,		// 237
		cmd_menublank,	// 238
		cmd_menuclear, 	// 239
		cmd_menudelete,	// 240
		cmd_menudeleteitem, // 241
		cmd_menudisable,// 242
		cmd_menuenable,	// 243
		cmd_menuinsertitem, // 244
		cmd_menumark,	// 245
		cmd_menuredraw,	// 246
		cmd_menutext,	// 247
		cmd_submenu,	// 248
		cmd_menuprepare,// 249
		cmd_hilite,		// 250
		cmd_stretchline,// 251	
		cmd_stretchbox, // 252	
		cmd_stretchoval,// 253	
		cmd_stretchcircle, // 254		
		cmd_sndload,	// 255
		cmd_sndunload,	// 256
		cmd_delay,		// 257
		cmd_ctlalign,	// 258
		cmd_clipwindow,	// 259
		cmd_serset,		// 260
		cmd_console,	// 261
		cmd_picturebegin,//262		
		cmd_picturedone,// 263		
		cmd_picturesave,// 264
		cmd_cleargraphics, //265
		cmd_clearcontrols, //266
		cmd_clearwindow,//267
		cmd_filedelete,//268
		cmd_drawupdate,  // 269
		cmd_setcmdkey,	// 270
		cmd_filewritebuffer, //271
		cmd_scrollset,	// 272
		cmd_alertbox,	// 273
		cmd_filecopy,	// 274
		cmd_filerename, // 275
		cmd_filelock,   // 276
		cmd_fileunlock, // 277
		cmd_loadpal, 	// 278
		cmd_winpal, 	// 279
		cmd_setdepth,	// 280
		cmd_winorigin,	// 281
		cmd_drawpicture,// 282
		cmd_input,		// 283
		cmd_formshow,	// 284
		cmd_timer,		// 285
		cmd_conmsg,		// 286
		cmd_spritedup, 	// 287
		cmd_menufonts,	// 288
		cmd_createfolder, // 289
		cmd_conclear,	//290
		cmd_contitle,	//291
};
