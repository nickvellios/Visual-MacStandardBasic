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

short compile_project( ); //unsigned char *project );
void compile_error(void);
short isnumber(short);
short compile_argument( short argtype );
long get_longarg_val();
short show_source_file( unsigned char * fname);
short isvalidvar(short ch);
short rem_cmd_chk();
short get_restofline();
short label_finder();
short next_line();
short load_source_file( unsigned char * fname);
short compile_line();
short load_project_list( ); // unsigned char * fname);
short next_pjt_file();
short next_res_file();
short add_CallTable_page();
short add_VarTable_page();
short add_LabelTable_page();
short add_CodeTable_page();
short add_StringTable_page();
short add_SelectTable_page();
void ReleaseTables();
short getspot_InCallTable();
short getspot_InVarTable();
short getspot_InLabelTable();
short AddToVarTable( char *varname, short vartype );
short AddToCallTable( char *callname, short calltype );
short AddToLabelTable( char *labelname);
short compile_init();
void memory_error();
short getspot_InVarTable();
short pass1_compile_file( void );
short pass1_findtype();
short pass2_compile_file( void );
short process_dims( short scope );
short CallTableFind( char * CmdText, short CallType );
short VarTableFind( char * varname, short vartype  );
short compile_command();
short compile_module();
short register_sub();
short register_func();
short register_params(short tp);
short OneOfThem( short ch, char *src );
short PassOne();
short PassTwo();
short RunProgram();
long GrabLongConst();

