CXX = g++ -std=c++11
cc = gcc

LIB = -lpthread -ldl
BIN = sqlite pfox

all : $(BIN)
sqlite : sqlite3.c shell.c
	$(cc) -o $@ $^ $(LIB)
pfox : main.cpp sqlite3.o
	$(CXX) -o $@ $^ $(LIB)
sqlite3.o : sqlite3.c
	$(cc) -o $@ -c $^

clean :
	rm -f $(BIN)

.PHONY: all, clean
