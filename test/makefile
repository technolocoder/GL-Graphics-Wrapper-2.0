.PHONY : build_all
build_all : build_directories exec/test-texture exec/test-rect exec/test-camera

APP=test-texture

.PHONY : run
run : build_all
	./exec/$(APP)

exec/test-rect : bin/test-rect.o
	g++ bin/test-rect.o -O3 -lGL -lGLU -lGLEW -lSDL2 -L../lib -l gl-wrapper -o exec/test-rect

bin/test-rect.o : src/test-rect.cpp
	g++ src/test-rect.cpp -O3 -c -o bin/test-rect.o -I../include

exec/test-texture : bin/test-texture.o
	g++ bin/test-texture.o -O3 -lGL -lGLU -lGLEW -lSDL2 -L../lib -l gl-wrapper -o exec/test-texture

bin/test-texture.o : src/test-texture.cpp
	g++ src/test-texture.cpp -O3 -c -o bin/test-texture.o  -I../include 

exec/test-camera : bin/test-camera.o
	g++ bin/test-camera.o -O3 -lGL -lGLU -lGLEW -lSDL2 -L../lib -l gl-wrapper -o exec/test-camera

bin/test-camera.o : src/test-camera.cpp
	g++ src/test-camera.cpp -O3 -c -o bin/test-camera.o -I../include

.PHONY : build_directories
build_directories : 
	mkdir -p bin exec

.PHONY : clean
clean :
	rm -rf exec bin