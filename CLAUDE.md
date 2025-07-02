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

### Current File Location Behavior
The game currently looks for `adv.key` and `adv.rec` files in the current working directory only. The KIO library (`MakNam.c`) simply appends `.key` and `.rec` to the base name "adv" with no path prefix.

### Planned Enhancement: Smart File Location
A future improvement would be to search for database files in this order:
1. **Same directory as the `adv` executable** - allows running from anywhere
2. **`$ADVPATH` environment variable** - user-specified database location  
3. **Current working directory** - current behavior (fallback)

This would make the game more portable and user-friendly, similar to how many Unix utilities locate their data files.