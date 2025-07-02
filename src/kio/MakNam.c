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

#include <stdio.h>
#include <string.h>
#include "kio.h"

void MakNam (name)
  char *name ;
{
	static int initialized = 0;
	static char keypath[256];
	static char recpath[256];
	
	if (!initialized) {
		if (findDatabaseFiles(name, keypath, recpath) < 0) {
			/* Fall back to original behavior */
			snprintf(knam, 256, "%s.key", name);
			snprintf(rnam, 256, "%s.rec", name);
		} else {
			strncpy(knam, keypath, 255);
			strncpy(rnam, recpath, 255);
			knam[255] = '\0';
			rnam[255] = '\0';
		}
		initialized = 1;
	}
}
