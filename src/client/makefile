FLAGS = -std=c++17 -masm=intel -fconcepts -mlong-double-128 -ggdb3 -Wpedantic -Wall -Wextra -Wconversion -Wsign-conversion -Weffc++ -Wstrict-null-sentinel -Wold-style-cast -Wnoexcept -Wctor-dtor-privacy -Woverloaded-virtual -Wsign-promo -Wzero-as-null-pointer-constant -Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override -lquadmath

main: structMessages.pb.o
	g++ $(FLAGS) client.hpp client.cpp structMessages.pb.o `pkg-config --cflags --libs protobuf`

structMessages.pb.o:
	g++ -c $(FLAGS) ../proto/src/structMessages.pb.h ../proto/src/structMessages.pb.cc `pkg-config --cflags --libs protobuf`