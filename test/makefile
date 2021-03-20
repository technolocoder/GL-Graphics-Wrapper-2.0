.PHONY : build_all
build_all : build_directories exec/test-texture exec/test-rect exec/test-camera exec/test-model exec/test-lighting exec/test-adv-lighting

APP=test-rect
OPTIMIZED= 

.PHONY : run
run : build_all
	./exec/$(APP)

exec/test-rect : bin/test-rect.o
	g++ bin/test-rect.o -l gl-wrapper -lGL -lGLU -lGLEW -lSDL2 -lassimp -L../lib -o exec/test-rect

bin/test-rect.o : src/test-rect.cpp
ifeq ($(OPTIMIZED),TRUE)
	g++ src/test-rect.cpp -O3 -c -o bin/test-rect.o -I../include -I ../dependencies/stb
else
	g++ src/test-rect.cpp -c -o bin/test-rect.o -I../include -I ../dependencies/stb
endif
exec/test-texture : bin/test-texture.o
	g++ bin/test-texture.o -l gl-wrapper -lGL -lGLU -lGLEW -lSDL2 -lassimp -L../lib -o exec/test-texture

bin/test-texture.o : src/test-texture.cpp
ifeq ($(OPTIMIZED),TRUE)
	g++ src/test-texture.cpp -O3 -c -o bin/test-texture.o  -I../include -I ../dependencies/stb
else
	g++ src/test-texture.cpp -c -o bin/test-texture.o  -I../include -I ../dependencies/stb
endif
exec/test-camera : bin/test-camera.o
	g++ bin/test-camera.o -l gl-wrapper -lGL -lGLU -lGLEW -lSDL2 -lassimp -L../lib -o exec/test-camera

bin/test-camera.o : src/test-camera.cpp
ifeq ($(OPTIMIZED),TRUE)
	g++ src/test-camera.cpp -O3 -c -o bin/test-camera.o -I../include -I ../dependencies/stb
else
	g++ src/test-camera.cpp -c -o bin/test-camera.o -I../include -I ../dependencies/stb
endif

exec/test-model : bin/test-model.o
	g++ bin/test-model.o -l gl-wrapper -lGL -lGLU -lGLEW -lSDL2 -lassimp -L../lib -o exec/test-model

bin/test-model.o : src/test-model.cpp
ifeq ($(OPTIMIZED),TRUE)
	g++ src/test-model.cpp -O3 -c -o bin/test-model.o -I../include -I ../dependencies/stb
else
	g++ src/test-model.cpp -c -o bin/test-model.o -I../include -I ../dependencies/stb
endif

exec/test-lighting : bin/test-lighting.o
	g++ bin/test-lighting.o -l gl-wrapper -lGL -lGLU -lGLEW -lSDL2 -lassimp -L../lib -o exec/test-lighting

bin/test-lighting.o : src/test-lighting.cpp
ifeq ($(OPTIMIZED),TRUE)
	g++ src/test-lighting.cpp -O3 -c -o bin/test-lighting.o -I../include -I ../dependencies/stb
else
	g++ src/test-lighting.cpp -c -o bin/test-lighting.o -I../include -I ../dependencies/stb
endif

exec/test-adv-lighting : bin/test-adv-lighting.o
	g++ bin/test-adv-lighting.o -l gl-wrapper -lGL -lGLU -lGLEW -lSDL2 -lassimp -L../lib -o exec/test-adv-lighting

bin/test-adv-lighting.o : src/test-adv-lighting.cpp
ifeq ($(OPTIMIZED),TRUE)
	g++ src/test-adv-lighting.cpp -O3 -c -o bin/test-adv-lighting.o -I../include -I ../dependencies/stb
else
	g++ src/test-adv-lighting.cpp -c -o bin/test-adv-lighting.o -I../include -I ../dependencies/stb
endif

.PHONY : build_directories
build_directories : 
	mkdir -p bin exec

.PHONY : clean
clean :
	rm -rf exec bin