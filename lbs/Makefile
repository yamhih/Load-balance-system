# This is a makefile file, it contains the compile arguments

BIN = ./bin
LIB = ./lib
INCLUDE = ./include
SRC = ./src

$(BIN)/lb: $(BIN)/server
	gcc  ./src/lb.c -I$(INCLUDE) -o $(BIN)/lb -lpthread
$(BIN)/server: $(BIN)/client
	gcc  ./src/server.c -I$(INCLUDE) -o $(BIN)/server
$(BIN)/client:
	gcc  ./src/client.c -I$(INCLUDE) -o $(BIN)/client


run:
	./bin/lb &
	./bin/server &
	./client &

clean:
	rm $(BIN)/lb
	rm $(BIN)/server
	rm $(BIN)/client
