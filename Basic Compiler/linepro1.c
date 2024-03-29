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
#define			UCHAR	unsigned char

short compile_log_update(short errno, char *special);

extern unsigned char *buffer, *project_bp, temp[100];
extern short pjt_ctr, pjt_pos, pjt_len;

extern unsigned char parsed_line[1024], line[260], RawLine[260], label_flag;
extern short line_pos,pline_pos,line_len;
extern long source_len, buf_pos, line_ctr;

short rem_cmd_chk();
short get_restofline();
short label_finder();
short next_line();
void trimline();

short nextCapFlag=0;
short CallTableFind( char *cmdname, short PassCallType );

void trimline()
{
	short a;
	a = line[0];
	while( line[a--] == ' ' || line[a--] == 9) line[0]--;
}

short rem_cmd_chk()
{
  if( ((buffer[buf_pos] & 223 ) =='R') &&  
       ((buffer[buf_pos+1] & 223 ) =='E') && 
	((buffer[buf_pos+2] & 223 ) =='M') && 
	 (buffer[buf_pos+3] ==' ' || buffer[buf_pos+3] == 13 ||
	   buf_pos+3>=source_len)  ) return(1);
  
  return(0);
}

short get_restofline()
{
  unsigned char quote_flag=0, ch;
  short a;

  while( buffer[buf_pos]!=13 && buf_pos<source_len) 
  {
    if(quote_flag)
    {
      line[line_pos++] = buffer[buf_pos];
      if( buffer[buf_pos] == '"' ) quote_flag=0;
    }
    else
    { 
      ch = buffer[buf_pos];
      if( (ch>='A' && ch<='Z') || (ch>='0' && ch<='9') ) 
      {
		if( rem_cmd_chk() )
		{
		  while( buffer[buf_pos]!=13 && buf_pos<source_len) buf_pos++;
		  if( buffer[++buf_pos]==10 ) buf_pos++;  //Move past LF
		  
		  return(0);
		}
		else line[line_pos++] = ch;
	  }
	  else if( ch>='a' && ch<='z' ) line[line_pos++] = ch-32;
	  else 
	  {
			switch(ch)
			{
			  case '"':
			    line[line_pos++] = ch;
			    quote_flag++;
			    break;
			  case '_':
			  case '!':
			  case '#':
			  case '%':
			  case '&':
			  case '$':
			  case ',':
			  case '|':
			  case ':':
			  case ';':
			  case '+':
			  case '-':
			  case '*':
			  case ' ':
			  case '.':
			  case '<':
			  case '>':
			  case '=':
			  case '(':
			  case ')':
			  case '[':
			  case ']':
			  case '?':
			  case  47:  // slash
			    line[line_pos++] = ch;
			    break;
			  case   9:  // tab
			    line[line_pos++] = ' ';
			    break;
			  case 92:  // backslash - auto convert to 
			    line[line_pos++] = 47;
			    break;
			  case 39:  // '
			    while( buffer[buf_pos]!=13 && buf_pos<source_len) buf_pos++;
			    if( buffer[++buf_pos]==10 ) buf_pos++;  //Move past LF
 	            line[0] = line_pos-1;
 	            trimline();
				a=line[0]+1;
  				line[a]=0;
			    return(0);
			    break;
			  default:
  			    line[line_pos++] = ch;
			    break;
  
			    /*//printf("\nError [#%i][%c]\n",ch,ch);
				compile_log_update(143, nil);
			    while( buffer[buf_pos]!=13 && buf_pos<source_len) buf_pos++;
			    if( buffer[++buf_pos]==10 ) buf_pos++;  //Move past LF
  		        line[0] = line_pos-1;
				a=line[0]+1;
  				line[a]=0;
 			    return(line_pos);  //Return location of error
			    break;*/
			}
    	}
    }
    buf_pos++;
  }
  
  if( quote_flag ) line[line_pos++] = '"';
  //line_ctr++;     //Inc line ctr
  line[0] = line_pos-1;
  line[line_pos]=0;
  trimline();
  a=line[0]+1;
  line[a]=0;
  if( buffer[++buf_pos]==10 ) buf_pos++;  //Move past LF
  return(0);
}

short label_finder()
{
   unsigned char temp[45],ctr=0, tctr, ch;  
   short result=0, a;
   long temp_buf_pos;
   
   temp_buf_pos = buf_pos;
   
   ch = buffer[buf_pos];

   if( (ch>='a' && ch<='z') || (ch>='A' && ch<='Z') )
   {
     // This means a const text str is being used as a label
     while(ctr<41)
     {
       ch = buffer[temp_buf_pos];
       
       if( (ch>='A' && ch<='Z') || (ch>='0' && ch<='9') || ch=='_')
       {
		 temp[ctr++] = buffer[temp_buf_pos++];  
       }
       else if( ch>='a' && ch<='z' )
       {
		 temp[ctr++] = buffer[temp_buf_pos++]-32;  
       }
       else if( ch==':')  //Yes this COULD be is a label check to it matches a command
       {
     	 temp[ctr]=0;
     	 a = CallTableFind( (char *) temp, 1 );
		 if( a >= 0 ) return 0; // No this is a command
		 buf_pos = temp_buf_pos+1;      //Update this ctr
		 // Now copy to line;
 		 line[line_pos++] = ':';
		 for( tctr=0; tctr < ctr; tctr++) line[line_pos++] = temp[tctr];
		 line[line_pos++] = ' ';
		 line[0] = line_pos-1;		// Set Line Len (in case only label on line
		 line[line_pos]= 0;	
		 return(1); //Valid label
       }  
       else // If it get here then no valid label
       {
	 	 return(0);
       }
     }
     // If it get here then no valid label
     return(0);
   }
   else if(ch>='0' && ch<='9') 
   {
     // This means a line number is being used as a label
	 line[line_pos++] = ':';
     while(ctr<40)
     {
       ch = buffer[buf_pos];
       
       if( ch>='0' && ch<='9' )
       {
		 line[line_pos++] = buffer[buf_pos++];  
       }
       else 
       {
		 line[line_pos++] = ' ';
		 return(1); //Valid label
       }
     }  
   }	
   else return(0);
   return(0);	
}


short next_line()
{
  long tc;
  short lpr;
  // This sub loads line from source buffer in proper syntax (if possible)
  //

  char loop=0, a;

  // First let's copy line in buffer to RawLine
  tc = buf_pos;
 
  line[0]=0;    // Pascal type string, so line len=0; 
  line_pos=1; 

  while(!loop)
  {
     loop++;    //Set so this loop is a 1 timer
     
     //Ok first lets first ignore leading spaces (or tabs)
     while( (buffer[buf_pos]==' ' || buffer[buf_pos]==9) && buf_pos<source_len) buf_pos++;

     //Let's check for end of buffer
     if( buf_pos>=source_len) return(999);   //No more code
     
     //Let's check for blank line 
     if( buffer[buf_pos]==13 ) 
     {
		if( buffer[++buf_pos]==10 ) buf_pos++;  //Move past LF
		line_ctr++;     //Inc line ctr
		if( buf_pos>=source_len) return(999);   //No more code
		tc = buf_pos;
		loop=0;         //Blank line so go back to top 
     }
     
     //Let's check for remark only line
     if( buffer[buf_pos]==39 || rem_cmd_chk() )   //Check for '
     {
		while( buffer[buf_pos]!=13 && buf_pos<source_len) buf_pos++;
		line_ctr++;     //Inc line ctr
		if( buf_pos>=source_len) return(999);   //No more code
		if( buffer[++buf_pos]==10 ) buf_pos++;  //Move past LF
		tc = buf_pos;
		loop=0;         //Blank line so go back to top 
     }
  }

  //Ok if we get to this point then there is code to process on the line
  lpr = 0;
  while( (buffer[tc]==' ' || buffer[tc]==9) && tc<source_len) tc++;
  while( lpr<256 && buffer[tc]!=13 && tc<source_len) RawLine[lpr++] = buffer[tc++];
  RawLine[lpr] = 0;	//Terminate with Zero

  //Check if there is a label at the start of the line
  label_flag=label_finder();    //Will put label in 'line' plus space
  
  if(label_flag)   
  {
     //Ok lets first ignore any spaces after label that was fnd
     while( (buffer[buf_pos]==' ' || buffer[buf_pos]==9) && buf_pos<source_len) buf_pos++;
     //Let's check for EOL 
     if( buffer[buf_pos]==13 ) 
     {
		if( buffer[++buf_pos]==10 ) buf_pos++;  //Move past LF
		return 0;   //Line only has a label on it
	 }
  }
  line_ctr++;     //Inc line ctr
  
  // Get cmd and rest of line, capitalized, ignore '
  //Ok lets first ignore any spaces after label that was fnd
  a = get_restofline();
  return(0);

}

