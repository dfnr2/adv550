# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is the classic Adventure game (550-point version), originally written by David Platt in Fortran-77 and later ported to C by Ken Wellsch in 1985. The codebase consists of three main components:

- **kio/**: Keyed I/O library for database operations
- **munge/**: Adventure compiler that processes game database files
- **adv/**: Adventure game interpreter

## Build System

Build the entire project:
```bash
make
```

This will:
1. Build the keyed I/O library (`klib.a`)
2. Build the munge compiler 
3. Build the adventure interpreter
4. Compile the game database using `munge < comcave`

Individual component builds:
```bash
cd src/kio && make     # Build keyed I/O library
cd src/munge && make   # Build database compiler
cd src/adv && make     # Build game interpreter
```

## Development Commands

Lint all components:
```bash
make lint
```

Clean build artifacts:
```bash
make clean
```

Print source code (legacy command):
```bash
make print
```

## Architecture

The system uses a three-tier architecture:

1. **Database Layer (adv_db/)**: Game data files defining places, objects, verbs, actions, and game logic
2. **Compiler Layer (munge/)**: Compiles the human-readable database files into binary format for the interpreter
3. **Runtime Layer (adv/)**: Game interpreter that loads the compiled database and runs the game

### Key Files

- `comcave`: Master database file that includes all game data files in correct order
- `src/adv/adefs.h`: Main constants and definitions for the game engine
- `src/kio/kio.h`: Keyed I/O system interface
- `src/munge/mdefs.h`: Compiler definitions

### Database Compilation

The game database must be compiled before running:
```bash
src/munge/munge < comcave
```

This creates `adv.key` and `adv.rec` files that the interpreter reads.

## Game Constants

The game is configured for:
- 120 objects
- 307 places  
- 50 variables
- 58 operation types
- Various buffer and cache sizes defined in `adefs.h`

## Development Status

- **KIO module**: ✅ Builds successfully with gcc14, tests pass
- **MUNGE module**: ✅ Builds successfully with gcc14, creates working database files, tests pass  
- **ADV module**: ✅ Builds successfully with gcc14, game runs and is playable

## Modernization Completed

All three modules have been successfully modernized for gcc14 compatibility while maintaining K&R C style. Key improvements:

### Code Fixes
- Fixed NULL symbol conflicts (`#define NULL n` → `#define NULL_CMD n`, `#define NULL_TYPE n`)
- Added missing headers (`stdio.h`, `stdlib.h`, `string.h`)
- Fixed function prototypes and return types
- Resolved system function name conflicts (`getline` → `getln`)
- Fixed #endif directive warnings (`#endif XXX` → `#endif /* XXX */`)
- Corrected function return types (`void findnam()` → `int findnam()`)

### Build System Improvements
- **Fixed all three makefiles** - replaced fragile library extraction pattern with proper dependency-based building
- Original pattern: `ar x $(LIB); cc -c $?; ar ru $(LIB) $(OBJ)` (failed on clean builds)
- New pattern: `.c.o: $(HEADER); $(LIB): $(OBJ)` (reliable, standard makefile practice)
- Added robust clean targets (`rm -f *.o` instead of `rm *.o`)

### Testing and Verification
- Created comprehensive test suites for KIO and MUNGE modules
- Verified database compilation pipeline works correctly
- **Game functionality confirmed**: Adventure builds, runs, and is playable
- All modules build cleanly from scratch

## Architecture Summary

The complete adventure game pipeline now works:

1. **KIO Library**: Provides keyed I/O database operations
2. **MUNGE Compiler**: Processes game data files (`munge < comcave`) to create binary database
3. **ADV Interpreter**: Loads compiled database and runs the interactive game

The modernization preserves the original 1985 K&R C coding style while ensuring compatibility with modern gcc14 compiler.

## Running Adventure

### Requirements
To run the Adventure game, you need:
1. The `adv` executable
2. Database files: `adv.key` and `adv.rec` 
3. The database files must be accessible from where you run `adv`

### Database File Path Prioritization (Implemented)
The game now uses path prioritization to find `adv.key` and `adv.rec` files. The search order is:

1. **`$ADVPATH` environment variable** - user-specified database location (highest priority)
2. **Current working directory** - for local development work
3. **Same directory as the `adv` executable** - system default (fallback)

This follows standard Unix conventions for data file location.

#### Usage Examples

**Use custom database location** (highest priority):
```bash
export ADVPATH=/home/user/adventure-data
./src/adv/adv  # uses files from ADVPATH directory
```

**Traditional usage** (current directory):
```bash
./src/adv/adv  # finds adv.key and adv.rec in current dir
```

**Run from anywhere** (executable directory fallback):
```bash
cd /tmp
/path/to/adventure/src/adv/adv  # uses files next to adv executable
```

#### Implementation Details
- Both `.key` and `.rec` files must exist in the same location
- If only partial files exist in a location, search continues to next location
- The `findDatabaseFiles()` function in `src/kio/util.c` handles the path search
- Test suite in `src/kio/test_path.c` available via `make test-path`

## Testing

### Path Detection Tests
A test suite is available to verify the path prioritization functionality:

```bash
make test-path
```

The test suite covers:
- Basic functionality with real adventure files
- Partial file scenarios (only `.key` or only `.rec` exists)
- ADVPATH environment variable behavior
- Fallback chain validation
- Mixed scenarios with files in different locations
- Edge cases and error conditions

All tests should pass, demonstrating that the path prioritization works correctly across various scenarios.

## Implementing the "AGAIN" Verb

This section provides detailed instructions for implementing an "again" verb that repeats the last command.

### Overview

The "again" command will allow players to repeat their last non-"again" command. This is useful for repetitive actions like fighting monsters, taking multiple objects, or navigating through similar passages.

### Implementation Steps

#### Step 1: Add Storage for Last Command

First, we need to add storage for the last command in the global state. Edit `src/adv/advglob.c`:

```c
// Add after line 82 (after the existing global variables)
char lastlex[LEXLEN][LEXSIZ];  // Storage for last command words
int lastlexcnt = 0;             // Number of words in last command
```

Also declare these as extern in `src/adv/adefs.h`:

```c
// Add after line 134 (after other extern declarations)
extern char lastlex[LEXLEN][LEXSIZ];
extern int lastlexcnt;
```

#### Step 2: Add AGAIN Verb to Database

Edit `adv_db/verbs.d` to add the AGAIN verb. Insert this line after line 100 (before the terminating 101):

```
   101→VERB	AGAIN,G,REPEAT
   102→
```

Note: The verb number will be 101, and we add synonyms "G" (short form) and "REPEAT".

#### Step 3: Modify Command Processing

Edit `src/adv/command.c` to save commands and handle "again". Replace the entire `readln` function (lines 94-130) with:

```c
int readln (words)
  char words[LEXLEN][LEXSIZ] ;
{
	register int cnt, i ;
	register char *b ;
	char buffer[LINESIZE] ;
	int is_again = 0 ;

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

	/* Check if this is an "again" command */
	if ( cnt > 0 )
	{
		char temp[LEXSIZ] ;
		(void) strcpy (temp, words[0]) ;
		low2up (temp) ;
		if ( strcmp(temp, "AGAIN") == 0 || strcmp(temp, "G") == 0 || 
		     strcmp(temp, "REPEAT") == 0 )
		{
			is_again = 1 ;
		}
	}

	/* If "again", restore last command */
	if ( is_again && lastlexcnt > 0 )
	{
		cnt = lastlexcnt ;
		for ( i = 0 ; i < cnt ; i++ )
			(void) strcpy (words[i], lastlex[i]) ;
		
		printf (">> ") ;
		for ( i = 0 ; i < cnt ; i++ )
		{
			if ( i > 0 ) printf (" ") ;
			printf ("%s", words[i]) ;
		}
		printf ("\n") ;
	}
	/* Otherwise, save this command for future "again" */
	else if ( cnt > 0 && !is_again )
	{
		lastlexcnt = cnt ;
		for ( i = 0 ; i < cnt ; i++ )
			(void) strcpy (lastlex[i], words[i]) ;
	}

	return (cnt) ;
}
```

Also add the string.h include at the top of command.c if not already present:

```c
// Add after line 12
#include <string.h>
```

#### Step 4: Rebuild Everything

After making all the code changes:

```bash
# Clean everything first
make clean

# Rebuild all components
make

# The make command will automatically:
# 1. Build kio library
# 2. Build munge compiler
# 3. Build adventure interpreter
# 4. Recompile the database with the new AGAIN verb
```

### Testing the Implementation

#### Basic Test Cases

1. **Simple Command Repeat**:
   ```
   ? take lamp
   [game response]
   ? again
   >> take lamp
   [same action repeated]
   ```

2. **Short Form**:
   ```
   ? north
   [moves north]
   ? g
   >> north
   [moves north again]
   ```

3. **Multiple Word Commands**:
   ```
   ? take all
   [takes multiple items]
   ? again
   >> take all
   [repeats the take all command]
   ```

4. **First Command Edge Case**:
   ```
   ? again
   [should do nothing or give error]
   ```

5. **Chain of Different Commands**:
   ```
   ? look
   [description]
   ? inventory
   [shows inventory]
   ? again
   >> inventory
   [shows inventory again, not look]
   ```

### Troubleshooting

#### If AGAIN verb is not recognized:
1. Check that `adv_db/verbs.d` was edited correctly
2. Ensure database was recompiled: `src/munge/munge < comcave`
3. Verify new `adv.key` and `adv.rec` files were created

#### If commands are not being saved:
1. Check that global variables were added to both `advglob.c` and `adefs.h`
2. Verify the modified `readln` function is saving non-again commands

#### If compilation fails:
1. Ensure `string.h` is included in `command.c`
2. Check for typos in variable names
3. Make sure array declarations match between files

### Implementation Notes

- The implementation stores the raw lexical tokens before parsing, allowing exact replay
- The ">> " prefix shows the repeated command for clarity
- Only non-"again" commands are saved to prevent recursion
- The verb ID 101 is used (next available after existing verbs)
- Case-insensitive matching works for "AGAIN", "again", "G", "g", etc.