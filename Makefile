CC = clang

CFLAGS = -Wall -Wextra -Wpedantic -std=c17
CPPFLAGS = -Iinclude

TARGET = build/cgit

SRC = src/main.c \
      src/cli.c \
      src/repository.c \
      src/hash.c \
      src/object.c \
      src/index.c \
      src/tree.c \
      src/commit.c \
      src/diff.c

OBJ = $(SRC:src/%.c=build/%.o)

TEST_TARGET = build/cgit_tests
TEST_SRC = tests/test_main.c
TEST_OBJ = $(TEST_SRC:tests/%.c=build/test_%.o)

ASAN_CFLAGS = $(CFLAGS) -fsanitize=address -fno-omit-frame-pointer
ASAN_TARGET = build/cgit_tests_asan

ASAN_OBJ = $(SRC:src/%.c=build/asan_%.o)
ASAN_TEST_OBJ = $(TEST_SRC:tests/%.c=build/asan_test_%.o)


# Normal build

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

build/%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@


# Normal tests

$(TEST_TARGET): $(TEST_OBJ) $(filter-out build/main.o,$(OBJ))
	$(CC) $(CFLAGS) $(TEST_OBJ) $(filter-out build/main.o,$(OBJ)) -o $(TEST_TARGET)

build/test_%.o: tests/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -Isrc -Itests -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)


# AddressSanitizer tests

$(ASAN_TARGET): $(ASAN_TEST_OBJ) $(filter-out build/asan_main.o,$(ASAN_OBJ))
	$(CC) $(ASAN_CFLAGS) $(ASAN_TEST_OBJ) $(filter-out build/asan_main.o,$(ASAN_OBJ)) -o $(ASAN_TARGET)

build/asan_%.o: src/%.c
	$(CC) $(CPPFLAGS) $(ASAN_CFLAGS) -c $< -o $@

build/asan_test_%.o: tests/%.c
	$(CC) $(CPPFLAGS) $(ASAN_CFLAGS) -Isrc -Itests -c $< -o $@

asan: $(ASAN_TARGET)
	./$(ASAN_TARGET)


# Cleanup

clean:
	rm -f build/*.o $(TARGET) $(TEST_TARGET) $(ASAN_TARGET)


# Installation

INSTALL_DIR = $(HOME)/.local/bin

install: $(TARGET)
	mkdir -p $(INSTALL_DIR)
	cp $(TARGET) $(INSTALL_DIR)/cgit

uninstall:
	rm -f $(INSTALL_DIR)/cgit
