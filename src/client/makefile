FLAGS = -std=c++17 -masm=intel -fconcepts -mlong-double-128 -ggdb3 -Wpedantic -Wall -Wextra -Wconversion -Wsign-conversion -Weffc++ -Wstrict-null-sentinel -Wold-style-cast -Wnoexcept -Wctor-dtor-privacy -Woverloaded-virtual -Wsign-promo -Wzero-as-null-pointer-constant -Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override -lquadmath

main: user.pb.o
	g++ $(FLAGS) client.hpp client.cpp user.pb.o `pkg-config --cflags --libs protobuf`

user.pb.o:
	protoc -I=../proto --cpp_out=../proto/src ../proto/user.proto
	g++ -c $(FLAGS) ../proto/src/user.pb.h ../proto/src/user.pb.cc `pkg-config --cflags --libs protobuf`
