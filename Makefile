CC = clang
CFLAGS = -Wall -Wextra -Wpedantic -std=c17
CPPFLAGS = -Iinclude

TARGET = build/cgit

# SRC = src/main.c src/cli.c
SRC = src/main.c src/cli.c src/repository.c src/hash.c src/object.c src/index.c src/tree.c
OBJ = $(SRC:src/%.c=build/%.o)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

build/%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f build/*.o $(TARGET)