COMPILER_FLAGS= -std=c++17 -masm=intel -fconcepts -mlong-double-128 -ggdb3 -Wpedantic -Wall -Wextra -Wconversion -Wsign-conversion -Wstrict-null-sentinel -Wold-style-cast -Wnoexcept -Wctor-dtor-privacy -Woverloaded-virtual -Wsign-promo -Wzero-as-null-pointer-constant -Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override `pkg-config --cflags --libs protobuf`
LINKER_FLAGS=-pthread
EXECUTABLE=server
CXX=g++ -ggdb

SOURCES=$(wildcard src/*.cpp)
PROTO_SOURCES=$(wildcard *.proto)

TMP_OBJECTS=$(SOURCES:.cpp=.o)
OBJECTS=$(TMP_OBJECTS:src/%=build/%)

PROTO_TMP_OBJECTS=$(PROTO_SOURCES:.proto=.pb.cpp)
PROTO_OBJECTS=$(PROTO_TMP_OBJECTS:%=cpl_proto/%)
PROTO_TMP_BUILDS=$(PROTO_TMP_OBJECTS:%=build/%)
PROTO_BUILDS=$(PROTO_TMP_BUILDS:.cpp=.o)

all: pre $(PROTO_OBJECTS) $(EXECUTABLE)

cpl_proto/%.pb.cpp: %.proto
	@mkdir -p cpl_proto
	@echo "\e[1;33mProtobuf >> $^ -> $@\e[0m"
	@protoc -I=./ --cpp_out=./cpl_proto/ $^
	@mv $(@:.cpp=.cc) $@

client: client_test.cpp
	@$(CXX) $^ $(COMPILER_FLAGS) -o $@

pre:
	@echo "\e[0;32m============== Compiling  =============\e[0m"

$(EXECUTABLE): main.cpp $(OBJECTS) $(PROTO_BUILDS)
	@echo "\e[1;32mcompiling : $^ -> $@\e[0m"
	@$(CXX) $^ $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $@

build/%.pb.o: cpl_proto/%.pb.cpp
	@mkdir -p build
	@echo "\e[1;33m >> $^ -> $@\e[0m"
	@$(CXX) $(COMPILER_FLAGS) -c $^ -o $@ `pkg-config --cflags --libs protobuf`

build/%.o: src/%.cpp
	@mkdir -p build
	@echo "\e[1;33m >> $^ -> $@\e[0m"
	@$(CXX) $(COMPILER_FLAGS) -c $^ -o $@

clean :
	clear
	@echo "\e[0;33m============== Cleaning  ==============\e[0m"
	rm -f build/*.o
	rm -f cpl_proto/*

run: all
	@echo "\e[0;31m================= Run ================\e[0m"
	@echo "\e[0;31m"
	@./$(EXECUTABLE) 1880
	@echo "\e[0m"

valrun: all
	@echo "\e[0;31m============ Valgrind Run ============\e[0m"
	valgrind --leak-check=full --show-reachable=yes --show-leak-kinds=all --error-limit=no --gen-suppressions=all --log-file=supdata.log ./$(EXECUTABLE) 8080

mrproper: clean
	rm -f $(EXECUTABLE)

.PHONY: clean mrproper
