#!/bin/bash

# MUNGE Test Suite
# Tests the MUNGE functionality (assumes binary already built)

echo "=== MUNGE Functionality Test Suite ==="
echo

# Test 1: Binary Existence
echo "Test 1: Binary Existence"
if [ -f "./munge" ]; then
    echo "✓ MUNGE binary exists"
    BINARY_OK=1
else
    echo "✗ MUNGE binary not found - run 'make' first"
    BINARY_OK=0
fi
echo

# Test 2: Simple Input Processing
echo "Test 2: Simple Input Processing"
if [ $BINARY_OK -eq 1 ]; then
    echo "Testing with simple input file..."
    
    # Clean up any existing database files
    rm -f adv.key adv.rec
    
    # Try to process test input (non-interactive)
    if echo "NULL_CMD test" | ./munge 2>error.log; then
        echo "✓ MUNGE processes simple input without crashing"
        
        # Check if database files were created (correct names)
        if [ -f "adv.key" ] && [ -f "adv.rec" ]; then
            echo "✓ MUNGE creates database files (adv.key, adv.rec)"
            
            # Check file sizes are reasonable
            key_size=$(stat -f%z adv.key 2>/dev/null || stat -c%s adv.key 2>/dev/null || echo 0)
            rec_size=$(stat -f%z adv.rec 2>/dev/null || stat -c%s adv.rec 2>/dev/null || echo 0)
            
            if [ "$key_size" -gt 100 ] && [ "$rec_size" -gt 0 ]; then
                echo "✓ Database files have reasonable sizes (key: ${key_size}, rec: ${rec_size})"
                
                # Check that key file is binary data
                if file adv.key | grep -q "data"; then
                    echo "✓ Key file is binary data format"
                else
                    echo "? Key file format unexpected"
                fi
                
                DB_CREATE_OK=1
            else
                echo "✗ Database files too small (key: ${key_size}, rec: ${rec_size})"
                DB_CREATE_OK=0
            fi
        else
            echo "✗ MUNGE doesn't create expected database files (adv.key, adv.rec)"
            DB_CREATE_OK=0
        fi
        
        SIMPLE_OK=1
    else
        echo "✗ MUNGE crashes or fails with simple input"
        echo "Error output:"
        cat error.log
        SIMPLE_OK=0
        DB_CREATE_OK=0
    fi
else
    echo "- Skipped (no binary)"
    SIMPLE_OK=0
    DB_CREATE_OK=0
fi
echo

# Test 3: File Input Processing  
echo "Test 3: File Input Processing"
if [ $BINARY_OK -eq 1 ]; then
    echo "Testing with test input file..."
    
    # Clean up any existing database files
    rm -f adv.key adv.rec
    
    # Try to process test file
    if ./munge test_input.d 2>file_error.log; then
        echo "✓ MUNGE processes file input without crashing"
        
        # Check if database files were created
        if [ -f "adv.key" ] && [ -f "adv.rec" ]; then
            echo "✓ MUNGE creates database files from file input"
            
            # Verify content from our test input is in the database
            if strings adv.rec | grep -q "This is a test text message"; then
                echo "✓ Database contains expected test content"
                CONTENT_OK=1
            else
                echo "✗ Database missing expected test content"
                echo "Database content:"
                strings adv.rec | head -5
                CONTENT_OK=0
            fi
            
            # Check for other expected content
            expected_strings=("test text message" "simple test place" "test object")
            missing_content=0
            
            for expected in "${expected_strings[@]}"; do
                if ! strings adv.rec | grep -q "$expected"; then
                    echo "✗ Missing expected content: $expected"
                    missing_content=1
                fi
            done
            
            if [ $missing_content -eq 0 ]; then
                echo "✓ All expected content found in database"
                FILE_OK=1
            else
                echo "? Some expected content missing from database"
                FILE_OK=0
            fi
        else
            echo "✗ MUNGE doesn't create database files from file input"
            FILE_OK=0
        fi
    else
        echo "✗ MUNGE fails with file input"
        echo "Error output:"
        cat file_error.log
        FILE_OK=0
    fi
else
    echo "- Skipped (no binary)"
    FILE_OK=0
fi
echo

# Test 4: Function Checks
echo "Test 4: Function Checks"
if [ $BINARY_OK -eq 1 ]; then
    echo "Checking which functions are available..."
    
    # Use nm to check symbols if available
    if command -v nm > /dev/null; then
        echo "Available functions in MUNGE:"
        nm ./munge | grep " T " | grep -v "^_" | head -10
        
        # Check for specific missing functions
        if nm ./munge | grep -q "wstab"; then
            echo "✓ wstab function present"
        else
            echo "✗ wstab function missing (expected)"
        fi
        
        if nm ./munge | grep -q "parse"; then
            echo "✓ parse function present"
        else
            echo "✗ parse function missing"
        fi
    else
        echo "? nm not available for symbol analysis"
    fi
else
    echo "- Skipped (no binary)"
fi
echo

# Summary
echo "=== Test Summary ==="
TOTAL_TESTS=3
PASSED_TESTS=0

if [ $BINARY_OK -eq 1 ]; then ((PASSED_TESTS++)); fi
if [ $SIMPLE_OK -eq 1 ]; then ((PASSED_TESTS++)); fi
if [ $FILE_OK -eq 1 ]; then ((PASSED_TESTS++)); fi

echo "Passed: $PASSED_TESTS/$TOTAL_TESTS tests"
echo

if [ $BINARY_OK -eq 1 ]; then
    echo "Status: MUNGE core functionality appears to be working"
    if [ $SIMPLE_OK -eq 1 ] || [ $FILE_OK -eq 1 ]; then
        echo "        Basic input processing is functional"
    else
        echo "        Input processing needs work"
    fi
else
    echo "Status: MUNGE binary not found - run 'make' first"
fi

echo
echo "Next steps:"
if [ $BINARY_OK -eq 0 ]; then
    echo "- Build MUNGE with 'make'"
elif [ $SIMPLE_OK -eq 0 ] && [ $FILE_OK -eq 0 ]; then
    echo "- Fix runtime errors to enable input processing"
    echo "- Complete missing functions"
else
    echo "- Test with more complex input files"
    echo "- Verify database output format"
    echo "- Complete remaining missing functions"
fi