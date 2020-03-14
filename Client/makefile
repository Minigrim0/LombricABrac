#Copie heuteuse du travail qui n'est pas le mien mais c'est très joli
#Mais l'auteur original a modifié ce magnifique makefile et t'excuse d'utiliser son travail
COMPILER_FLAGS= -std=c++17 -fconcepts  -mlong-double-128 -ggdb3 -Wpedantic -Wall -Wextra -Wstrict-null-sentinel -Wold-style-cast -Wnoexcept -Wctor-dtor-privacy -Woverloaded-virtual -Wsign-promo -Wzero-as-null-pointer-constant -Wsuggest-final-types -Wsuggest-final-methods `pkg-config --cflags --libs protobuf`
LINKER_FLAGS=-pthread -ldl -lpthread -lncurses `pkg-config --cflags --libs protobuf`

QT_COMPILER_FLAGS = -pipe -O2 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I. -I. -isystem /usr/include/x86_64-linux-gnu/qt5 -isystem /usr/include/x86_64-linux-gnu/qt5/QtWidgets -isystem /usr/include/x86_64-linux-gnu/qt5/QtGui -isystem /usr/include/x86_64-linux-gnu/qt5/QtCore -I. -isystem /usr/include/libdrm -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++
QT_LINKER_FLAGS = -Wl,-O1 -lQt5Widgets -lQt5Gui -lQt5Core -lGL -lpthread

EXECUTABLE=lombricABrac
CXX=g++ -ggdb -ldl
HPP_FLAGS=moc -DQT_DEPRECATED_WARNINGS -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB --include ./moc_predefs.h -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++ -I/home/simon/Documents/qt -I/home/simon/Documents/qt -I/usr/include/x86_64-linux-gnu/qt5 -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets -I/usr/include/x86_64-linux-gnu/qt5/QtGui -I/usr/include/x86_64-linux-gnu/qt5/QtCore -I/usr/include/c++/8 -I/usr/include/x86_64-linux-gnu/c++/8 -I/usr/include/c++/8/backward -I/usr/lib/gcc/x86_64-linux-gnu/8/include -I/usr/local/include -I/usr/lib/gcc/x86_64-linux-gnu/8/include-fixed -I/usr/include/x86_64-linux-gnu -I/usr/include

SOURCES=$(wildcard src/*.cpp)
COMMON_SOURCES=$(wildcard ../sharedFiles/src/*.cpp)
HEADERS=$(wildcard includes/*.hpp)
SOURCES_MOC=$(wildcard temp/*.cpp)

TMP_OBJECTS=$(SOURCES:.cpp=.o)
TMP_OBJECTS2 = $(COMMON_SOURCES:.cpp=.o)

TMP_HEADERS = $(HEADERS:.hpp=_moc.cpp)
TMP_SOURCES_MOC=$(SOURCES_MOC:_moc.cpp=_moc.o)

OBJECTS=$(TMP_OBJECTS:src/%=build/%)
OBJECTS2=$(TMP_OBJECTS2:../sharedFiles/src/%=../sharedFiles/build/%)
OBJECTS3=$(TMP_HEADERS:includes/%=temp/%)
OBJECTS4=$(TMP_SOURCES_MOC:temp/%=build/%)

all: pre $(EXECUTABLE)

pre:
	@echo "\e[0;32m============== Compiling  =============\e[0m"

$(EXECUTABLE): build/main.o build/user.pb.o $(OBJECTS3) $(OBJECTS) $(OBJECTS2) $(LIBS)
	@echo "\e[1;32mcompiling : $^ -> $@\e[0m"
	@$(CXX) $^ $(LINKER_FLAGS) $(QT_COMPILER_FLAGS) $(QT_LINKER_FLAGS) -o $@

build/main.o: main.cpp
	@echo "\e[1;32mcompiling : $^ -> $@\e[0m"
	@mkdir -p build
	@$(CXX) $(COMPILER_FLAGS) $(QT_COMPILER_FLAGS) $(QT_LINKER_FLAGS) -c $^ -o $@

build/user.pb.o:
	@mkdir -p build
	@mkdir -p proto/src/
	protoc -I=../sharedFiles --cpp_out=proto/src ../sharedFiles/user.proto
	$(CXX) $(COMPILER_FLAGS) -c proto/src/*
	mv user.pb.o build/user.pb.o

build/%.o: src/%.cpp
	@mkdir -p build
	@echo "\e[1;33m >> $^ -> $@\e[0m"
	@$(CXX) $(COMPILER_FLAGS) $(QT_COMPILER_FLAGS) $(QT_LINKER_FLAGS) -c $^ -o $@

../sharedFiles/build/%.o: ../sharedFiles/src/%.cpp
	@mkdir -p ../sharedFiles/build
	@echo "\e[1;33m >> $^ -> $@\e[0m"
	@$(CXX) $(COMPILER_FLAGS) -c $^ -o $@

temp/%_moc.cpp:includes/%.hpp
	@mkdir -p temp
	@echo "\e[1;33m >> $^ -> $@\e[0m"
	@g++ -pipe -O2 -Wall -W -dM -E -o temp/moc_predefs.h /usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/data/dummy.cpp
	@$(HPP_FLAGS) $^ -o $@

temp/moc_predefs.h:
	@mkdir -p temp
	@echo "\e[Compiling .h fro QT\e[0m"
	@g++ -pipe -O2 -Wall -W -dM -E -o temp/moc_predefs.h /usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/data/dummy.cpp

build/%_moc.o: temp/%_moc.cpp
	@mkdir -p build
	@echo "\e[1;33m >> $^ -> $@\e[0m"
	@$(CXX) $(COMPILER_FLAGS) $(QT_COMPILER_FLAGS) $(QT_LINKER_FLAGS) -c $^ -o $@

clean:
	clear
	@echo "\e[0;33m============== Cleaning  ==============\e[0m"
	@rm -f build/*.o
	@rm -f ../sharedFiles/build/*.o
	@rm -f temp/*

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
