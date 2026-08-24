CC = clang
CFLAGS = -Wall -Wextra -Wpedantic -std=c17

TARGET = build/cgit
SRC = src/main.c
OBJ = build/main.o

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

build/main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o build/main.o

clean:
	rm -f build/*.o $(TARGET)