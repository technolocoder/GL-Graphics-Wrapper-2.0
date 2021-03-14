.PHONY : build_all
build_all : build_directories exec/test-rect exec/test-texture

exec/test-rect : bin/test-rect.o
	g++ bin/test-rect.o -O3 -lGL -lGLU -lGLEW -lSDL2 -lSOIL -o exec/test-rect 

bin/test-rect.o : src/test-rect.cpp
	g++ src/test-rect.cpp -O3 -c -o bin/test-rect.o -I../include

exec/test-texture : bin/test-texture.o
	g++ bin/test-texture.o -O3 -lGL -lGLU -lGLEW -lSDL2 -lSOIL -o exec/test-texture

bin/test-texture.o : src/test-texture.cpp
	g++ src/test-texture.cpp -O3 -c -o bin/test-texture.o  -I../include

.PHONY : build_directories
build_directories : 
	mkdir -p bin exec

clean :
	rm -rf exec bin