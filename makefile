.PHONY : build_all
build_all : build_directories lib/libgl-wrapper.a
	$(MAKE) -C test 

APP=test-rect
OPTIMIZED= 

export APP
.PHONY : run
run : lib/libgl-wrapper.a build_directories
	$(MAKE) -C test run 

lib/libgl-wrapper.a : lib/libcamera-wrapper.a lib/libtexture-wrapper.a lib/libshader-wrapper.a lib/libprogram-wrapper.a lib/libmesh-wrapper.a lib/libmodel-wrapper.a 
	ar -rvs $@ bin/camera-wrapper.o bin/texture-wrapper.o bin/shader-wrapper.o bin/program-wrapper.o bin/mesh-wrapper.o bin/model-wrapper.o
	$(MAKE) -C test clean
	
lib/libcamera-wrapper.a : bin/camera-wrapper.o
	ar -rvs $@ $^

bin/camera-wrapper.o : src/camera-wrapper.cpp  include/camera-wrapper.hpp
	g++ $< -c -o $@ -O3 -I include -I dependencies/stb

lib/libtexture-wrapper.a : bin/texture-wrapper.o
	ar -rvs $@ $^

bin/texture-wrapper.o : src/texture-wrapper.cpp include/texture-wrapper.hpp
	g++ $< -c -o $@ -O3 -I include -I dependencies/stb

lib/libprogram-wrapper.a : bin/program-wrapper.o
	ar -rvs $@ $^

bin/program-wrapper.o : src/program-wrapper.cpp include/program-wrapper.hpp
	g++ $< -c -o $@ -O3 -I include -I dependencies/stb

lib/libshader-wrapper.a : bin/shader-wrapper.o
	ar -rvs $@ $^

bin/shader-wrapper.o : src/shader-wrapper.cpp include/shader-wrapper.hpp
	g++ $< -c -o $@ -O3 -I include -I dependencies/stb

lib/libmesh-wrapper.a : bin/mesh-wrapper.o
	ar -rvs $@ $^

bin/mesh-wrapper.o : src/mesh-wrapper.cpp include/mesh-wrapper.hpp
	g++ $< -c -o $@ -O3 -I include -I dependencies/stb

lib/libmodel-wrapper.a : bin/model-wrapper.o
	ar -rvs $@ $^

bin/model-wrapper.o : src/model-wrapper.cpp include/model-wrapper.hpp
	g++ $< -c -o $@ -O3 -I include -I dependencies/stb

.PHONY : build_directories
build_directories :
	mkdir -p lib bin

.PHONY : clean
clean : 
	rm -rf bin lib
	$(MAKE) -C test clean

.PHONY : install_dependencies
install_dependencies : 
	@echo -n "This will install SDL2 , GLEW , GLM ,SOIL Image loader and Assimp Model Loader. Confirm [y/N] ? " && read ans && [ $${ans:-N} = y ]
	@echo "Installing SDL"
	sudo apt install libsdl2-dev
	@echo "Installing GLEW"
	sudo apt install -y libglew-dev
	sudo apt install -y glew-utils
	@echo "Installing GLM"
	sudo apt install libglm-dev
	@echo "Installing SOIL Image Loader"
	sudo apt install libsoil-dev
	@echo "Initializing STB Submodule"
	git submodule init
	git submodule update
	@echo "Installing Assimp Model Loader"
	sudo apt install assimp-utils
	sudo apt install libassimp-dev
	sudo apt install libassimp-doc
	@echo "Done Installing Dependencies"
