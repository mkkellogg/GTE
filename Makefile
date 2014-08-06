LIBS=-lm -lGL -lglut -lGLU -lGLEW
CFLAGS=-I src -std=c++11 -Wall
CC=g++
OBJECTFILES= obj/shadermanager.o obj/shadersource.o obj/shader.o obj/debug.o obj/gte.o

SHADERSRC= src/graphics/shader

all: gtemain shadermanager ui
	$(CC) -o bin/gte $(OBJECTFILES) $(LIBS) 
	rm -rf bin/resources
	cp -r resources bin/

gtemain: src/gte.cpp src/gte.h
	$(CC) $(CFLAGS) -o obj/gte.o -c src/gte.cpp

shadermanager: $(SHADERSRC)/shadermanager.cpp $(SHADERSRC)/shadermanager.h $(SHADERSRC)/shadersource.cpp  $(SHADERSRC)/shadersource.h $(SHADERSRC)/shader.cpp $(SHADERSRC)/shader.h
	$(CC) $(CFLAGS) -o obj/shadermanager.o -c $(SHADERSRC)/shadermanager.cpp 
	$(CC) $(CFLAGS) -o obj/shadersource.o -c $(SHADERSRC)/shadersource.cpp
	$(CC) $(CFLAGS) -o obj/shader.o -c $(SHADERSRC)/shader.cpp

ui: src/ui/debug.cpp src/ui/debug.h
	$(CC) $(CFLAGS) -o obj/debug.o -c src/ui/debug.cpp 

clean:
	rm -f obj/*   
	rm -f bin/gte
	rm -rf bin/resources
