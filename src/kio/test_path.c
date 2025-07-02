/*
 * Comprehensive test program for smart file location functionality
 * Tests the findDatabaseFiles function in various scenarios including corner cases
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "kio.h"

/* Function prototypes */
int findDatabaseFiles(const char* basename, char* keypath, char* recpath);

void test_scenario(const char* description, const char* basename,
                   const char* expected_result) {
    char keypath[256];
    char recpath[256];
    int result;

    printf("\n=== %s ===\n", description);
    printf("Testing basename: '%s'\n", basename);

    result = findDatabaseFiles(basename, keypath, recpath);

    if (result == 0) {
        printf("SUCCESS: Files found\n");
        printf("  Key file: %s\n", keypath);
        printf("  Rec file: %s\n", recpath);

        /* Verify files actually exist */
        if (access(keypath, R_OK) == 0 && access(recpath, R_OK) == 0) {
            printf("  Verification: Both files are readable\n");
        } else {
            printf("  WARNING: Files reported found but not readable\n");
            printf("    Key file readable: %s\n", access(keypath, R_OK) == 0 ? "YES" : "NO");
            printf("    Rec file readable: %s\n", access(recpath, R_OK) == 0 ? "YES" : "NO");
        }
    } else {
        printf("FAILED: Files not found (result=%d)\n", result);
    }

    printf("Expected: %s\n", expected_result);
}

void create_key_file(const char* dir, const char* basename) {
    char keypath[256];
    FILE *fp;

    snprintf(keypath, sizeof(keypath), "%s/%s.key", dir, basename);
    fp = fopen(keypath, "w");
    if (fp) {
        fprintf(fp, "test key file\n");
        fclose(fp);
        printf("Created: %s\n", keypath);
    }
}

void create_rec_file(const char* dir, const char* basename) {
    char recpath[256];
    FILE *fp;

    snprintf(recpath, sizeof(recpath), "%s/%s.rec", dir, basename);
    fp = fopen(recpath, "w");
    if (fp) {
        fprintf(fp, "test rec file\n");
        fclose(fp);
        printf("Created: %s\n", recpath);
    }
}

void create_both_files(const char* dir, const char* basename) {
    create_key_file(dir, basename);
    create_rec_file(dir, basename);
}

void cleanup_files(const char* dir, const char* basename) {
    char keypath[256];
    char recpath[256];

    snprintf(keypath, sizeof(keypath), "%s/%s.key", dir, basename);
    snprintf(recpath, sizeof(recpath), "%s/%s.rec", dir, basename);

    unlink(keypath);
    unlink(recpath);
}

void show_env_status() {
    char *advpath = getenv("ADVPATH");
    if (advpath) {
        printf("ADVPATH: %s\n", advpath);
    } else {
        printf("ADVPATH: not set\n");
    }
}

int main() {
    char cwd[256];

    printf("=== TESTING DATABASE SEARCH ===\n");

    if (getcwd(cwd, sizeof(cwd))) {
        printf("Current working directory: %s\n", cwd);
    }

    /* Setup test directories */
    mkdir("test_exe_dir", 0755);
    mkdir("test_advpath_dir", 0755);
    mkdir("test_cwd_dir", 0755);

    printf("\n" "=" "*70\n");
    printf("SECTION 1: BASIC FUNCTIONALITY TESTS\n");
    printf("=" "*70\n");

    /* Test 1: Real adventure files in current directory */
    unsetenv("ADVPATH");
    show_env_status();
    test_scenario("Real adventure files in current directory",
                  "adv", "Should find real adv.key and adv.rec");

    printf("\n" "=" "*70\n");
    printf("SECTION 2: CORNER CASE TESTS - PARTIAL FILES\n");
    printf("=" "*70\n");

    /* Test 2: Only .key file exists in current directory */
    cleanup_files(".", "partial");
    create_key_file(".", "partial");
    test_scenario("Only .key file in current directory",
                  "partial", "Should fail - missing .rec file");

    /* Test 3: Only .rec file exists in current directory */
    cleanup_files(".", "partial");
    create_rec_file(".", "partial");
    test_scenario("Only .rec file in current directory",
                  "partial", "Should fail - missing .key file");

    printf("\n" "=" "*70\n");
    printf("SECTION 3: ADVPATH TESTS - NORMAL CASES\n");
    printf("=" "*70\n");

    /* Test 4: ADVPATH set with both files present */
    cleanup_files(".", "advtest");
    create_both_files("test_advpath_dir", "advtest");
    setenv("ADVPATH", "test_advpath_dir", 1);
    show_env_status();
    test_scenario("Both files in ADVPATH directory",
                  "advtest", "Should find files via ADVPATH");

    printf("\n" "=" "*70\n");
    printf("SECTION 4: ADVPATH CORNER CASES - PARTIAL FILES\n");
    printf("=" "*70\n");

    /* Test 5: ADVPATH set but only .key file present */
    cleanup_files("test_advpath_dir", "advpartial");
    cleanup_files(".", "advpartial");
    create_key_file("test_advpath_dir", "advpartial");
    test_scenario("ADVPATH set, only .key file present",
                  "advpartial", "Should fail and not fallback to current dir");

    /* Test 6: ADVPATH set but only .rec file present */
    cleanup_files("test_advpath_dir", "advpartial");
    cleanup_files(".", "advpartial");
    create_rec_file("test_advpath_dir", "advpartial");
    test_scenario("ADVPATH set, only .rec file present",
                  "advpartial", "Should fail and not fallback to current dir");

    /* Test 7: ADVPATH set but no files present, fallback to current dir */
    cleanup_files("test_advpath_dir", "fallback");
    create_both_files(".", "fallback");
    test_scenario("ADVPATH set but empty, files in current dir",
                  "fallback", "Should fallback to current directory");

    printf("\n" "=" "*70\n");
    printf("SECTION 5: ADVPATH UNSET TESTS\n");
    printf("=" "*70\n");

    /* Test 8: ADVPATH unset, files in current directory */
    unsetenv("ADVPATH");
    show_env_status();
    cleanup_files(".", "unsettest");
    create_both_files(".", "unsettest");
    test_scenario("ADVPATH unset, files in current dir",
                  "unsettest", "Should find files in current directory");

    printf("\n" "=" "*70\n");
    printf("SECTION 6: FALLBACK CHAIN TESTS\n");
    printf("=" "*70\n");

    /* Test 9: No files anywhere */
    unsetenv("ADVPATH");
    cleanup_files("test_exe_dir", "nowhere");
    cleanup_files("test_advpath_dir", "nowhere");
    cleanup_files(".", "nowhere");
    test_scenario("No files in any location",
                  "nowhere", "Should fail completely");

    /* Test 10: Files only in current dir, ADVPATH set but empty */
    setenv("ADVPATH", "test_advpath_dir", 1);
    cleanup_files("test_advpath_dir", "currentonly");
    create_both_files(".", "currentonly");
    test_scenario("ADVPATH empty, files only in current dir",
                  "currentonly", "Should fallback to current directory");

    printf("\n" "=" "*70\n");
    printf("SECTION 7: PRIORITY ORDER TESTS\n");
    printf("=" "*70\n");

    /* Test 11: Files in both ADVPATH and current - ADVPATH should win */
    cleanup_files("test_advpath_dir", "priority1");
    cleanup_files(".", "priority1");
    create_both_files("test_advpath_dir", "priority1");  /* Files in ADVPATH */
    create_both_files(".", "priority1");                 /* Files in current */
    test_scenario("Files in both ADVPATH and current dir",
                  "priority1", "Should use ADVPATH files (higher priority)");

    /* Verify the returned path is from ADVPATH */
    char keypath[256], recpath[256];
    if (findDatabaseFiles("priority1", keypath, recpath) == 0) {
        if (strstr(keypath, "test_advpath_dir")) {
            printf("  PRIORITY VERIFIED: Using ADVPATH files\n");
        } else {
            printf("  PRIORITY ERROR: Not using ADVPATH files!\n");
        }
    }

    /* Test 12: Files only in current directory (ADVPATH unset) */
    unsetenv("ADVPATH");
    cleanup_files(".", "priority2");
    create_both_files(".", "priority2");
    test_scenario("ADVPATH unset, files in current dir",
                  "priority2", "Should use current directory files");

    /* Test 13: Files in ADVPATH and exe dir - ADVPATH should win */
    setenv("ADVPATH", "test_advpath_dir", 1);
    cleanup_files("test_advpath_dir", "priority3");
    cleanup_files("test_exe_dir", "priority3");
    create_both_files("test_advpath_dir", "priority3");  /* Files in ADVPATH */
    create_both_files("test_exe_dir", "priority3");      /* Files in exe dir */
    test_scenario("Files in both ADVPATH and exe dir",
                  "priority3", "Should use ADVPATH files (higher priority)");

    printf("\n" "=" "*70\n");
    printf("SECTION 8: MIXED SCENARIOS\n");
    printf("=" "*70\n");

    /* Test 14: Partial files in ADVPATH, complete files in current */
    cleanup_files("test_advpath_dir", "mixed");
    cleanup_files(".", "mixed");
    create_key_file("test_advpath_dir", "mixed");  /* Only .key in ADVPATH */
    create_both_files(".", "mixed");               /* Both in current */
    test_scenario("Partial in ADVPATH, complete in current",
                  "mixed", "Should fail at ADVPATH, then succeed at current");

    /* Test 15: Different partial files in each location */
    cleanup_files("test_advpath_dir", "scattered");
    cleanup_files(".", "scattered");
    create_key_file("test_advpath_dir", "scattered");  /* .key in ADVPATH */
    create_rec_file(".", "scattered");                 /* .rec in current */
    test_scenario("Different partial files in each location",
                  "scattered", "Should fail - files are scattered");

    printf("\n" "=" "*70\n");
    printf("CLEANUP\n");
    printf("=" "*70\n");

    /* Cleanup all test files and directories */
    cleanup_files("test_exe_dir", "partial");
    cleanup_files("test_exe_dir", "advtest");
    cleanup_files("test_exe_dir", "advpartial");
    cleanup_files("test_exe_dir", "fallback");
    cleanup_files("test_exe_dir", "unsettest");
    cleanup_files("test_exe_dir", "nowhere");
    cleanup_files("test_exe_dir", "currentonly");
    cleanup_files("test_exe_dir", "priority1");
    cleanup_files("test_exe_dir", "priority2");
    cleanup_files("test_exe_dir", "priority3");
    cleanup_files("test_exe_dir", "mixed");
    cleanup_files("test_exe_dir", "scattered");

    cleanup_files("test_advpath_dir", "partial");
    cleanup_files("test_advpath_dir", "advtest");
    cleanup_files("test_advpath_dir", "advpartial");
    cleanup_files("test_advpath_dir", "fallback");
    cleanup_files("test_advpath_dir", "unsettest");
    cleanup_files("test_advpath_dir", "nowhere");
    cleanup_files("test_advpath_dir", "currentonly");
    cleanup_files("test_advpath_dir", "priority1");
    cleanup_files("test_advpath_dir", "priority2");
    cleanup_files("test_advpath_dir", "priority3");
    cleanup_files("test_advpath_dir", "mixed");
    cleanup_files("test_advpath_dir", "scattered");

    cleanup_files(".", "partial");
    cleanup_files(".", "advtest");
    cleanup_files(".", "advpartial");
    cleanup_files(".", "fallback");
    cleanup_files(".", "unsettest");
    cleanup_files(".", "nowhere");
    cleanup_files(".", "currentonly");
    cleanup_files(".", "priority1");
    cleanup_files(".", "priority2");
    cleanup_files(".", "priority3");
    cleanup_files(".", "mixed");
    cleanup_files(".", "scattered");

    rmdir("test_exe_dir");
    rmdir("test_advpath_dir");
    rmdir("test_cwd_dir");

    unsetenv("ADVPATH");

    printf("\nTest suite completed.\n");
    printf("Review the results above to verify database finder.\n");
    return 0;
}
