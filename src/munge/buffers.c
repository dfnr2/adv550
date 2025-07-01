/*
**		    Copyright (c) 1985	Ken Wellsch
**
**     Permission is hereby granted to all users to possess, use, copy,
**     distribute, and modify the programs and files in this package
**     provided it is not for direct commercial benefit and secondly,
**     that this notice and all modification information be kept and
**     maintained in the package.
**
*/

#include "mdefs.h"

#define MAXCBUF		420
#define MAXTBUF		3000

static char tbuf[MAXTBUF] ;
static short int tbp = 0 ;

static short int cbuf[MAXCBUF] ;
static short int cbp = 0 ;

void clrcode ()
{
	cbp = 0 ;
}

void appcode (code)
  short int code ;
{
	if ( cbp >= MAXCBUF )
		error ("Appcode","code buffer overflow (%d)!",MAXCBUF) ;
	cbuf[cbp++] = code ;
}

void outcode (key)
  int key ;
{
	if ( cbp < 1 )
		error ("Outcode","null code buffer!") ;
	if ( writek(dbunit,key,(char *)cbuf,cbp*sizeof(short int)) == ERROR )
		error ("Outcode","write error on key %d",key) ;
}

void clrtext ()
{
	tbp = 0 ;
}

void apptext (line)
  char line[] ;
{
	register int i ;

	for ( i = 0 ; tbp < MAXTBUF ; i++ )
	{
		if ( line[i] == EOS )
			break ;
		tbuf[tbp++] = line[i] ;
	}
	if ( tbp >= MAXTBUF )
		error ("Apptext","text buffer overflow (%d)!",MAXTBUF) ;
}

void outtext (key)
  int key ;
{
	if ( tbp < 1 )
		return ;
	if ( writek(dbunit,key,tbuf,tbp) == ERROR )
		error ("Outtext","write error on key %d",key) ;
}
