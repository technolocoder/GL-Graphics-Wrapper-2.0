.PHONY : build_all
build_all : build_directories lib/camera-wrapper.a
	$(MAKE) -C test 

lib/camera-wrapper.a : bin/camera-wrapper.o
	ar -rvs $@ $^

bin/camera-wrapper.o : src/camera-wrapper.cpp  include/camera-wrapper.hpp
	g++ $< -c -o $@ -O3 -I include

.PHONY : build_directories
build_directories :
	mkdir -p lib bin

.PHONY : clean
clean : 
	rm -rf bin lib
	$(MAKE) -C test clean