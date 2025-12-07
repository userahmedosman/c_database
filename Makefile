TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))
DB_FILE = bin/mydb.db

run: clean default
	@echo "=== Step 1: Creating database ==="
	./$(TARGET) -f bin/mydb.db -n
	@echo "=== Checking what files exist ==="
	@echo "Current directory:"
	pwd
	@echo "Files in current directory:"
	ls -la *.db 2>/dev/null || echo "No .db files in current dir"
	@echo "Files in bin/:"
	ls -la bin/*.db 2>/dev/null || echo "No .db files in bin/"
	@echo "=== Step 2: Adding employee ==="
	./$(TARGET) -f bin/mydb.db -a "Alice Smith,456 Oak Ave,35"

default: dirs $(TARGET)

dirs:
	mkdir -p obj bin

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $^

obj/%.o: src/%.c
	gcc -c $< -o $@ -Iinclude