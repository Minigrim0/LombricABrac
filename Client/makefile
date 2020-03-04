#Copie heuteuse du travail qui n'est pas le mien mais c'est très joli
#Mais l'auteur original a modifié ce magnifique makefile et t'excuse d'utiliser son travail
COMPILER_FLAGS= -std=c++17 -masm=intel -fconcepts  -mlong-double-128 -ggdb3 -Wpedantic -Wall -Wextra -Wstrict-null-sentinel -Wold-style-cast -Wnoexcept -Wctor-dtor-privacy -Woverloaded-virtual -Wsign-promo -Wzero-as-null-pointer-constant -Wsuggest-final-types -Wsuggest-final-methods `pkg-config --cflags --libs protobuf`
LINKER_FLAGS=-pthread -ldl -lpthread -lncurses
EXECUTABLE=lombricABrac
CXX=g++ -ggdb -ldl

SOURCES=$(wildcard src/*.cpp)

TMP_OBJECTS=$(SOURCES:.cpp=.o)
OBJECTS=$(TMP_OBJECTS:src/%=build/%)

all: pre $(EXECUTABLE)

pre:
	@echo "\e[0;32m============== Compiling  =============\e[0m"

$(EXECUTABLE): build/user.pb.o main.cpp $(OBJECTS) $(LIBS)
	@echo "\e[1;32mcompiling : $^ -> $@\e[0m"
	@$(CXX) $^ $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $@

build/user.pb.o:
	protoc -I=proto --cpp_out=proto/src proto/user.proto
	$(CXX) $(COMPILER_FLAGS) -c proto/src/*
	mv user.pb.o build/user.pb.o

build/%.o: src/%.cpp
	@mkdir -p build
	@echo "\e[1;33m >> $^ -> $@\e[0m"
	@$(CXX) $(COMPILER_FLAGS) -c $^ -o $@

clean:
	clear
	@echo "\e[0;33m============== Cleaning  ==============\e[0m"
	rm -f build/*.o

run: all
	@echo "\e[0;31m================= Run ================\e[0m"
	@echo "\e[0;31m"
	@./$(EXECUTABLE)
	@echo "\e[0m"

valrun: all
	@echo "\e[0;31m============ Valgrind Run ============\e[0m"
	valgrind --leak-check=full --show-reachable=yes --show-leak-kinds=all --error-limit=no --gen-suppressions=all --log-file=supdata.log ./$(EXECUTABLE)

mrproper: clean
	rm -f $(EXECUTABLE)

.PHONY: clean mrproper
