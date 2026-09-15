# Status and Diff Model

## File States

cgit compares three versions of a tracked file:

- HEAD: last committed version
- Index: staged version
- Working tree: current file

## Comparisons

HEAD vs Index determines staged changes.

Index vs Working Tree determines unstaged changes.

## Status States

### Untracked

The file exists in the working tree but is not present in the index.

### Modified

The file exists in the index but its working-tree content differs from the indexed content.

### Staged

The index content differs from the HEAD content.

### Staged and Modified

The index differs from HEAD and the working tree differs from the index.

### Deleted

A tracked file is missing from the working tree.

### Unchanged

HEAD, index, and working tree contain equivalent content.

## Diff

The basic diff compares two text versions line by line.

The diff identifies:

- added lines
- removed lines
- unchanged lines

The diff implementation is intentionally simplified and is not Git-compatible.
