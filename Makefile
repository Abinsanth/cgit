CC = clang
CFLAGS = -Wall -Wextra -Wpedantic -std=c17
CPPFLAGS = -Iinclude

TARGET = build/cgit

# SRC = src/main.c src/cli.c
SRC = src/main.c src/cli.c src/repository.c src/hash.c src/object.c src/index.c src/tree.c src/commit.c src/diff.c

OBJ = $(SRC:src/%.c=build/%.o)

$(TARGET): $(OBJ)

	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

build/%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f build/*.o $(TARGET)


TEST_TARGET = build/cgit_tests
TEST_SRC = tests/test_main.c
TEST_OBJ = $(TEST_SRC:tests/%.c=build/test_%.o)

$(TEST_TARGET): $(TEST_OBJ) $(filter-out build/main.o,$(OBJ))
	$(CC) $(CFLAGS) $(TEST_OBJ) $(filter-out build/main.o,$(OBJ)) -o $(TEST_TARGET)

build/test_%.o: tests/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -Itests -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)