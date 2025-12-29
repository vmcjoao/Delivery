# Compiladores
CC = gcc
CXX = g++

# Flags
CFLAGS = -c      # Flags para C
CXXFLAGS = -Isqlite # Flags para C++
LDFLAGS = 

# Arquivos
TARGET = delivery
OBJS = sqlite3.o src/main.o

# Regra principal
all: $(TARGET)

# Como criar o executável
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Como compilar o SQLite (usando gcc)
sqlite3.o: sqlite/sqlite3.c
	$(CC) $(CFLAGS) sqlite/sqlite3.c -o sqlite3.o

# Como compilar o main (usando g++)
src/main.o: src/main.cpp
	$(CXX) -c src/main.cpp -o src/main.o $(CXXFLAGS)

# Limpar arquivos compilados
clean:
	rm -f *.o src/*.o $(TARGET)