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

#include "adefs.h"
#include <string.h>

/* Forward declaration */
int verb_takes_object() ;

void command ()
{
	register int i, k ;
	int cnt, val ;

	linlen = 0 ;
	for ( i = 0 ; i < LINELEN ; i++ )
		linewd[i] = -1 ;
	
	cnt = readln (lex) ;

	for ( i = 0 ; i < cnt ; i++ )
	{
		low2up (lex[i]) ;
		
		/* Special handling for "it" pronoun */
		if ( strcmp(lex[i], "IT") == 0 )
		{
			/* Check if we can substitute a previous object */
			if ( i > 0 && linlen > 0 && class(linewd[0]) == VERB && 
			     lastobject != -1 && lastverb != -1 && verb_takes_object(lastverb) )
			{
				linewd[linlen++] = lastobject ;
				continue ;
			}
			/* Otherwise skip "it" - it will cause an error later */
		}
		
		val = find(lex[i]) ;
		if ( class(val) == NULL_TYPE )
			continue ;
		linewd[linlen++] = val ;
	}


	/* Check if first word is "again" verb */
	if ( linlen > 0 && class(linewd[0]) == VERB )
	{
		static int again_verb = -1 ;
		if ( again_verb == -1 )
			again_verb = find("AGAIN") ;
		
		if ( linewd[0] == again_verb && again_verb != -1 )
		{
			if ( lastlinlen > 0 )
			{
				/* Restore last command */
				linlen = lastlinlen ;
				for ( i = 0 ; i < linlen ; i++ )
					linewd[i] = lastlinewd[i] ;
			}
			else
			{
				/* No previous command to repeat */
				linlen = 0 ;
			}
		}
		/* Save this command for future "again" (but not if it was "again") */
		else if ( linewd[0] != again_verb )
		{
			lastlinlen = linlen ;
			for ( i = 0 ; i < linlen ; i++ )
				lastlinewd[i] = linewd[i] ;
		}
	}
	/* Save non-verb commands too */
	else if ( linlen > 0 )
	{
		lastlinlen = linlen ;
		for ( i = 0 ; i < linlen ; i++ )
			lastlinewd[i] = linewd[i] ;
	}

	for ( i = 0 ; i < LINELEN ; i++ )
	{
		switch (class(linewd[i]))
		{
			case OBJECT:
			case PLACE:
				k = bitval(linewd[i]) ;
				break ;

			case VERB:
				k = XVERB ;
				break ;

			default:
				k = 0 ;
				break ;
		}
		if ( linewd[i] < 0 )
			k = BADWORD ;
		setval (argwd[i],linewd[i]) ;
		setbit (argwd[i],k) ;
	}

	setval (status,linlen) ;
	if ( linlen <= 0 )
		return ;
	
	switch ( class(linewd[0]) )
	{
		case VERB:
			setbit(status,(bitval(status)|XVERB)) ;
			break ;

		case OBJECT:
			setbit(status,(bitval(status)|XOBJECT)) ;
			break ;

		case PLACE:
			setbit(status,(bitval(status)|XPLACE)) ;
			break ;
	}

	/* Track last verb and object for "it" substitution */
	if ( linlen > 0 && class(linewd[0]) == VERB )
	{
		static int it_obj = -1 ;
		if ( it_obj == -1 )
			it_obj = find("IT") ;
		
		lastverb = linewd[0] ;
		
		/* Check if we have an object in position 1 */
		if ( linlen > 1 && class(linewd[1]) == OBJECT && linewd[1] != it_obj )
		{
			lastobject = linewd[1] ;
		}
		/* For some verbs, object might be in position 2 (e.g., "pick up lamp") */
		else if ( linlen > 2 && class(linewd[2]) == OBJECT && linewd[2] != it_obj )
		{
			lastobject = linewd[2] ;
		}
	}

	return ;
}

/* Check if a verb typically takes an object */
int verb_takes_object (verb)
  int verb ;
{
	static int object_verbs[30] ;
	static int n_object_verbs = -1 ;
	register int i ;
	
	/* Initialize the list of object-taking verbs on first call */
	if ( n_object_verbs == -1 )
	{
		n_object_verbs = 0 ;
		/* Add common object-taking verbs */
		object_verbs[n_object_verbs++] = find("GET") ;
		object_verbs[n_object_verbs++] = find("DROP") ;
		object_verbs[n_object_verbs++] = find("TAKE") ;
		object_verbs[n_object_verbs++] = find("EXAMINE") ;
		object_verbs[n_object_verbs++] = find("LOOK") ;
		object_verbs[n_object_verbs++] = find("OPEN") ;
		object_verbs[n_object_verbs++] = find("CLOSE") ;
		object_verbs[n_object_verbs++] = find("READ") ;
		object_verbs[n_object_verbs++] = find("EAT") ;
		object_verbs[n_object_verbs++] = find("DRINK") ;
		object_verbs[n_object_verbs++] = find("THROW") ;
		object_verbs[n_object_verbs++] = find("WAVE") ;
		object_verbs[n_object_verbs++] = find("RUB") ;
		object_verbs[n_object_verbs++] = find("FILL") ;
		object_verbs[n_object_verbs++] = find("FEED") ;
		object_verbs[n_object_verbs++] = find("BREAK") ;
		object_verbs[n_object_verbs++] = find("KILL") ;
		object_verbs[n_object_verbs++] = find("ATTACK") ;
		object_verbs[n_object_verbs++] = find("LIGHT") ;
		object_verbs[n_object_verbs++] = find("EXTINGUISH") ;
		object_verbs[n_object_verbs++] = find("ON") ;
		object_verbs[n_object_verbs++] = find("OFF") ;
		object_verbs[n_object_verbs++] = find("POUR") ;
		object_verbs[n_object_verbs++] = find("RIDE") ;
	}
	
	/* Check if verb is in the object-taking list */
	for ( i = 0 ; i < n_object_verbs ; i++ )
		if ( object_verbs[i] == verb )
			return 1 ;
	
	return 0 ;
}

void low2up (word)
  register char *word ;
{
	for ( ; *word ; word++ )
		if ( (*word) >= 'a' && (*word) <= 'z' )
			*word += ( 'A' - 'a' ) ;
	return ;
}

#undef EOF
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LINESIZE	134

int readln (words)
  char words[LEXLEN][LEXSIZ] ;
{
	register int cnt, i ;
	register char *b ;
	char buffer[LINESIZE] ;

	printf ("? ") ;

	if ( fgets (buffer,LINESIZE,stdin) == NULL )
		exit (1) ;

#ifdef STATUS
	if ( strcmp (buffer,"~status\n") == 0 )
	{
		pcstat () ;
		(void) strcpy (buffer,"LOOK\n") ;
	}
#endif /* STATUS */

	cnt = 0 ;
	for ( cnt = 0, b = buffer ; *b != '\n' && *b != '\0' ; )
	{
		while ( *b == ' ' || *b == '\t' )
			b++ ;
	
		for ( i = 0 ; *b != ' ' && *b != '\t' && *b != '\n' && *b != '\0' ; b++ )
			if ( i < (LEXSIZ-1) )
				words[cnt][i++] = *b ;
	
		words[cnt][i] = EOS ;
		if ( i > 0 )
			if ( ++cnt >= LEXLEN )
				break ;
	}
	return (cnt) ;
}
