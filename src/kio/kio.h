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

#include <sys/file.h>
#include <unistd.h>

/* KIO function prototypes */
extern int GetBlk();
extern int GetRec();
extern int PutBlk();
extern long PutRec();
extern int MakBlk();
extern long MapBlk();
extern void MapKey();
extern void MakNam();
extern int findDatabaseFiles();
extern int creatk();
extern int openk();
extern int closek();
extern int readk();
extern int writek();
extern int dupk();
extern void error();

#define MAXENTRIES	16
#define MAXIBLK		1024
#define EMPTY		-1
#define ERROR		-1

#define EXISTING	0
#define NEW		1

struct mkey
{
	int b ;
	int e ;
} ;

struct iblk
{
	long int i_loc[MAXENTRIES] ;
	short int i_siz[MAXENTRIES] ;
} ;

extern struct iblk Iblk ;
extern short int Sblk[MAXIBLK] ;
extern char knam[256], rnam[256] ;
extern int Modified ;
extern int CurBlk ;
extern long int RecLoc ;
extern short int IndLoc ;
extern int kfd ;
extern int rfd ;
