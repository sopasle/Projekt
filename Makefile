prog: construct_frlzsi.o	find_frlzsi.o
	gcc -o prog main.o foo.o bar.o

construct_frlzsi.o: FRLZSI.cpp	construct_FRLZSI.cpp
	g++ -std=c++11 -O3 -DNDEBUG -I ../sdsl_installed/include -L ../sdsl_installed/lib FRLZSI.cpp construct_FRLZSI.cpp -lsdsl -ldivsufsort -ldivsufsort64

find_frlzsi.o: FRLZSI.cpp	find_FRLZSI.cpp
	g++ -std=c++11 -O3 -DNDEBUG -I ../sdsl_installed/include -L ../sdsl_installed/lib FRLZSI.cpp find_FRLZSI.cpp -lsdsl -ldivsufsort -ldivsufsort64
