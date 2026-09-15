# cgit

A simplified Git-like version control system written in C.

## Purpose

cgit is a learning project designed to understand:

- C programming
- File systems
- Data structures
- Hashing
- File I/O
- Compilation and linking
- Build systems
- Version control internals
- Memory management
- Debugging and testing

## Build

Clone the repository:

```bash
git clone git@github.com:Abinsanth/cgit.git
```

Go into the cgit directory:

```bash
cd cgit
```

### Requirements

You need:

- C compiler (Clang)
- Make
- macOS or Linux

Build cgit:

```bash
make
```

All commands below should be run **inside the cgit project directory** unless stated otherwise.

## Install

Install cgit:

```bash
make install
```

Check the installation:

```bash
cgit version
```

## Test

Run the test suite:

```bash
make test
```

Run tests with AddressSanitizer:

```bash
make asan
```

## Try cgit

After installation, you can use `cgit` from any directory.

Create a test directory:

```bash
mkdir my-test-repo
cd my-test-repo
```

Initialize a repository:

```bash
cgit init
```

Create a file:

```bash
echo "Hello, cgit!" > hello.txt
```

Add and commit it:

```bash
cgit add hello.txt
cgit commit -m "Add hello.txt"
```

Check the status:

```bash
cgit status
```

View the commit history:

```bash
cgit log
```

## Uninstall

Go back to the cgit project directory:

```bash
cd cgit
```

Remove the installed cgit:

```bash
make uninstall
```
