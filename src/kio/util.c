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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include "kio.h"

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#define MAXPATH 256

static char *getExecutableDir() {
    static char exedir[MAXPATH];
    static int initialized = 0;
    
    if (initialized) {
        return exedir;
    }
    
    char exepath[MAXPATH];
    
#ifdef __APPLE__
    uint32_t size = sizeof(exepath);
    if (_NSGetExecutablePath(exepath, &size) == 0) {
        char *dir = dirname(exepath);
        strncpy(exedir, dir, MAXPATH - 1);
        exedir[MAXPATH - 1] = '\0';
        initialized = 1;
        return exedir;
    }
#elif defined(__linux__)
    ssize_t len = readlink("/proc/self/exe", exepath, sizeof(exepath) - 1);
    if (len != -1) {
        exepath[len] = '\0';
        char *dir = dirname(exepath);
        strncpy(exedir, dir, MAXPATH - 1);
        exedir[MAXPATH - 1] = '\0';
        initialized = 1;
        return exedir;
    }
#endif
    
    return NULL;
}

int findDatabaseFiles(const char* basename, char* keypath, char* recpath) {
    char testkeypath[MAXPATH];
    char testrecpath[MAXPATH];
    
    /* 1. Check ADVPATH environment variable (highest priority) */
    char* advpath = getenv("ADVPATH");
    if (advpath) {
        snprintf(testkeypath, MAXPATH, "%s/%s.key", advpath, basename);
        snprintf(testrecpath, MAXPATH, "%s/%s.rec", advpath, basename);
        if (access(testkeypath, R_OK) == 0 && access(testrecpath, R_OK) == 0) {
            snprintf(keypath, MAXPATH, "%s/%s.key", advpath, basename);
            snprintf(recpath, MAXPATH, "%s/%s.rec", advpath, basename);
            return 0;
        }
    }
    
    /* 2. Check current directory */
    snprintf(testkeypath, MAXPATH, "%s.key", basename);
    snprintf(testrecpath, MAXPATH, "%s.rec", basename);
    if (access(testkeypath, R_OK) == 0 && access(testrecpath, R_OK) == 0) {
        snprintf(keypath, MAXPATH, "%s.key", basename);
        snprintf(recpath, MAXPATH, "%s.rec", basename);
        return 0;
    }
    
    /* 3. Check executable directory (fallback) */
    char* exedir = getExecutableDir();
    if (exedir) {
        snprintf(testkeypath, MAXPATH, "%s/%s.key", exedir, basename);
        snprintf(testrecpath, MAXPATH, "%s/%s.rec", exedir, basename);
        if (access(testkeypath, R_OK) == 0 && access(testrecpath, R_OK) == 0) {
            snprintf(keypath, MAXPATH, "%s/%s.key", exedir, basename);
            snprintf(recpath, MAXPATH, "%s/%s.rec", exedir, basename);
            return 0;
        }
    }
    
    return -1; /* not found */
}