# cgit Testing Strategy

## 1. Unit tests

Unit tests verify individual modules independently.

Main modules to test:

- hash
- object
- index
- tree
- commit
- repository
- diff

Unit tests should focus on:

- valid input
- invalid input
- boundary conditions
- expected return values
- memory ownership where applicable

## 2. Integration tests

Integration tests verify complete CLI workflows.

Examples:

- init → add → commit
- commit → log
- add → status
- modify → status
- modify → diff
- branch → checkout → commit
- multiple branches

## 3. Invalid input tests

Test:

- unknown commands
- missing arguments
- extra arguments
- invalid branch names
- nonexistent branches
- invalid file paths
- invalid object IDs

Expected behavior:

- return non-zero
- print a useful error
- do not crash
- do not corrupt repository state

## 4. Filesystem failure tests

Test behavior when:

- `.cgit` is missing
- `HEAD` is missing
- index is missing
- branch reference is missing
- object is missing
- file cannot be read
- directory cannot be opened
- filesystem path is invalid

## 5. Memory-safety tests

Use AddressSanitizer to detect:

- use-after-free
- buffer overflows
- invalid memory access
- double-free
- other allocation-related errors

Audit allocation and cleanup paths for leaks.

## 6. Debugging

Use LLDB to:

- set breakpoints
- step through execution
- inspect variables
- inspect call stacks
- investigate unexpected behavior

## 7. Corrupted repository tests

Test repositories containing:

- invalid HEAD
- invalid branch references
- malformed commit objects
- malformed tree objects
- corrupted index
- missing objects
- invalid object IDs

Expected behavior:

- detect the failure
- return non-zero
- print a useful error
- avoid crashing
- avoid further repository corruption

## 8. Edge cases

Test:

- empty files
- multiple files
- large files
- long filenames
- multiple commits
- multiple branches
- empty commit messages
- repeated operations
- boundary conditions

## Testing workflow

For each feature:

1. Test normal behavior.
2. Test invalid input.
3. Test failure paths.
4. Test edge cases.
5. Run the test suite.
6. Run AddressSanitizer tests when applicable.
7. Investigate failures with LLDB when needed.
8. Fix the implementation.
9. Re-run the affected tests.
