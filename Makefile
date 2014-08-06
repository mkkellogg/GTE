LIBS=-lm -lGL -lglut -lGLU -lGLEW
CFLAGS=-I src -std=c++11 -Wall
CC=g++
OBJECTFILES= obj/shadermanager.o obj/shadersource.o obj/shader.o obj/graphics.o obj/debug.o obj/gte.o
PLATFORMOBJECTFILES= obj/graphicsGL.o

GRAPHICSSRC= src/graphics
SHADERSRC= $(GRAPHICSSRC)/shader


all: gtemain graphics ui
	$(CC) -o bin/gte $(OBJECTFILES) $(PLATFORMOBJECTFILES) $(LIBS) 
	rm -rf bin/resources
	cp -r resources bin/

gtemain: src/gte.cpp src/gte.h
	$(CC) $(CFLAGS) -o obj/gte.o -c src/gte.cpp

graphics: shader $(GRAPHICSSRC)/graphics.cpp  $(GRAPHICSSRC)/graphics.h $(GRAPHICSSRC)/graphicsGL.cpp  $(GRAPHICSSRC)/graphicsGL.h
	$(CC) $(CFLAGS) -o obj/graphics.o -c $(GRAPHICSSRC)/graphics.cpp
	$(CC) $(CFLAGS) -o obj/graphicsGL.o -c $(GRAPHICSSRC)/graphicsGL.cpp

shader: $(SHADERSRC)/shadermanager.cpp $(SHADERSRC)/shadermanager.h $(SHADERSRC)/shadersource.cpp  $(SHADERSRC)/shadersource.h $(SHADERSRC)/shader.cpp $(SHADERSRC)/shader.h
	$(CC) $(CFLAGS) -o obj/shadermanager.o -c $(SHADERSRC)/shadermanager.cpp 
	$(CC) $(CFLAGS) -o obj/shadersource.o -c $(SHADERSRC)/shadersource.cpp
	$(CC) $(CFLAGS) -o obj/shader.o -c $(SHADERSRC)/shader.cpp

ui: src/ui/debug.cpp src/ui/debug.h
	$(CC) $(CFLAGS) -o obj/debug.o -c src/ui/debug.cpp 

clean:
	rm -f obj/*   
	rm -f bin/gte
	rm -rf bin/resources
