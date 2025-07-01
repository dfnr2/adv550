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

#define NEXT		4
#define	OK		1
#define	MAJOR		0
#define	ERROR		(-1)
#define	EOF		(-2)
#define NIL		((struct symstr *)0)
#define	EOS		0
#define	YES		1
#define	NO		0
#define	KEEP		1
#define	RELEASE		0
#define	STDIN		0
#define	STDOUT		1

#define PERCENT		'%'
#define BLANK		' '
#define TAB		'\t'
#define COMMA		','
#define SLASH		'/'
#define NEWLINE		'\n'
#define ATSIGN		'@'
#define PLUS		'+'
#define MINUS		'-'

#define NULLOBJECT	">$<"
#define LINKPLACE	">*<"
#define SHORTDESC	1
#define LONGDESC	2

#define	MAXLINE		134
#define STATES		25
#define	MAXCOM		16
#define MATCHCOM	3
#define	MAXOPS		58
#define MATCHOPS	4

#define	ACTION		0
#define	AT		1
#define	DEFINE		2
#define	INCLUDE		3
#define	INITIAL		4
#define	LABEL		5
#define	LIST		6
#define	NOLIST		7
#define	NULL_CMD	8
#define	OBJECT		9
#define	PLACE		10
#define	REPEAT		11
#define	SYNON		12
#define	TEXT		13
#define	VARIABLE	14
#define	VERB		15

#define	MAXOBJECTS	120
#define MAXOTEXT	20
#define MAXOCODE	4
#define	MAXPLACES	350
#define MAXPCODE	8
#define	MAXVERBS	160
#define MAXVCODE	12

#define MAXKEY		14000
#define	NULLWORD	12000
#define	SYMTABREC	13000
#define	SYMPERREC	20

#define sep(c)		( (c) == BLANK || (c) == TAB || (c) == COMMA )
#define com(c)		( (c) == '*' || (c) == '{' )
#define status(key)	( state[(key)/1000] )

struct symstr
{
	char 		*s_nam ;
	struct symstr	*s_nxt ;
	short int   	 s_val ;
	short int   	 s_aux ;
	short int	 s_mod ;
} ;

extern	int inunit ;

extern	char list ;

extern	short int ninit	 ;
extern	short int nrep	 ;
extern	short int nvars	 ;
extern	short int nobj	 ;
extern	short int nplace ;

extern	char state [] ;

extern	short int class [] ;

extern	char token [] ;
extern	int dbunit ;

extern char clss[] ;

/* System function declarations for K&R compatibility */
extern void exit();
extern void perror();

/* KIO function prototypes */
extern int creatk();
extern int closek();
extern int writek();

/* MUNGE function prototypes */
extern int openf();
extern int makedb();
extern void iodone();
extern int parse();
extern void wstab();
extern void null();
extern void obj();
extern struct symstr *define();
extern void clrcode();
extern void appcode();
extern void outcode();
extern void clrtext();
extern void apptext();
extern void outtext();
extern int chkmaj();
extern int gettok();
extern int findop();
extern void flushline();
extern int type();
extern int push();
extern int pop();
extern int Getc();
extern void Ungetc();
extern void pbstr();
extern void prlist();
extern void closef();
extern int error();
extern void synerr();
extern void errout();
extern char *strsav();
extern int getln();
extern int exec();
extern int major();
extern int hash();
extern int skip();
extern void act();
extern void at();
extern void def();
extern void inc();
extern void init();
extern void lab();
extern void place();
extern void rep();
extern void syn();
extern void text();
extern void var();
extern void verb();
extern int compile();
extern struct symstr *lookup();
extern void plink();
extern int dupk();
extern int eval();
extern int seval();
extern int ctoi();
