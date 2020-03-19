#Copie heuteuse du travail qui n'est pas le mien mais c'est très joli
#Mais l'auteur original a modifié ce magnifique makefile et t'excuse d'utiliser son travail
COMPILER_FLAGS= -std=c++17 -fconcepts  -mlong-double-128 -ggdb3 -Wpedantic -Wall -Wextra -Wstrict-null-sentinel -Wold-style-cast -Wnoexcept -Wctor-dtor-privacy -Woverloaded-virtual -Wsign-promo -Wzero-as-null-pointer-constant -Wsuggest-final-types -Wsuggest-final-methods `pkg-config --cflags --libs protobuf`
LINKER_FLAGS=-pthread -ldl -lpthread -lncurses `pkg-config --cflags --libs protobuf`

QT_COMPILER_FLAGS = -O2 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_MULTIMEDIA_LIB -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -I. -I. -isystem /usr/include/x86_64-linux-gnu/qt5 -isystem /usr/include/x86_64-linux-gnu/qt5/QtWidgets -isystem /usr/include/x86_64-linux-gnu/qt5/QtMultimedia -isystem /usr/include/x86_64-linux-gnu/qt5/QtGui -isystem /usr/include/x86_64-linux-gnu/qt5/QtNetwork -isystem /usr/include/x86_64-linux-gnu/qt5/QtCore -I. -isystem /usr/include/libdrm -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++
QT_LINKER_FLAGS = -Wl,-O1 -lQt5Widgets -lQt5Gui -lQt5Core -lGL -lpthread

EXECUTABLE=lombricABrac
CXX=g++ -pipe -ggdb -ldl
HPP_FLAGS=moc -DQT_DEPRECATED_WARNINGS -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB --include ./moc_predefs.h -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++ -I/home/simon/Documents/qt -I/home/simon/Documents/qt -I/usr/include/x86_64-linux-gnu/qt5 -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets -I/usr/include/x86_64-linux-gnu/qt5/QtGui -I/usr/include/x86_64-linux-gnu/qt5/QtCore -I/usr/include/c++/8 -I/usr/include/x86_64-linux-gnu/c++/8 -I/usr/include/c++/8/backward -I/usr/lib/gcc/x86_64-linux-gnu/8/include -I/usr/local/include -I/usr/lib/gcc/x86_64-linux-gnu/8/include-fixed -I/usr/include/x86_64-linux-gnu -I/usr/include
QRC = rcc -name ressourceWindow

SOURCES=$(wildcard src/*.cpp)
SHARED_SOURCES=$(wildcard ../sharedFiles/src/*.cpp)
SOURCES_MOC=$(wildcard includes/*.hpp)
SOURCES_QRC = $(wildcard ./*.qrc)
SOURCES_UI=$(wildcard UI/*.ui)

TMP_OBJECTS=$(SOURCES:.cpp=.o)
TMP_SHARED_OBJECTS = $(SHARED_SOURCES:.cpp=.o)
TMP_SOURCES_MOC=$(SOURCES_MOC:.hpp=_moc.o)
TMP_QRC=$(SOURCES_QRC:.qrc=_qrc.o)
TMP_UI=$(SOURCES_UI:.ui=_ui.hpp)

OBJECTS=$(TMP_OBJECTS:src/%=build/%)
SHARED_OBJECTS=$(TMP_SHARED_OBJECTS:../sharedFiles/src/%=../sharedFiles/build/%)
OBJECTS_MOC=$(TMP_SOURCES_MOC:includes/%=build/%)
OBJECTS_QRC=$(TMP_QRC:./%=build/%)
OBJECTS_UI=$(TMP_UI:UI/%=UI/src/%)

all: pre $(EXECUTABLE)

pre:
	@echo "\e[0;32m============== Compiling  =============\e[0m"

$(EXECUTABLE): build/user.pb.o build/main.o $(OBJECTS_UI) $(OBJECTS_QRC) $(OBJECTS_MOC) $(OBJECTS) $(SHARED_OBJECTS) $(LIBS)
	@echo "\e[1;32mcompiling : $^ -> $@\e[0m"
	@$(CXX) $^ $(LINKER_FLAGS) $(QT_COMPILER_FLAGS) $(QT_LINKER_FLAGS) -o $@

build/main.o: main.cpp
	@echo "\e[1;32mcompiling : $^ -> $@\e[0m"
	@mkdir -p build
	@$(CXX) $(COMPILER_FLAGS) $(QT_COMPILER_FLAGS) $(QT_LINKER_FLAGS) -c $^ -o $@

build/user.pb.o:
	@echo "\e[1;33m >> Compiling user.proto\e[0m"
	@mkdir -p build
	@mkdir -p proto/src/
	@protoc -I=../sharedFiles --cpp_out=proto/src ../sharedFiles/user.proto
	@$(CXX) $(COMPILER_FLAGS) -c proto/src/*
	@mv user.pb.o build/user.pb.o

UI/src/%_ui.hpp: UI/%.ui
	@echo "\e[1;33m >> $^ -> $@\e[0m"
	@mkdir -p UI/src
	@uic $< -o $@

build/%_qrc.o: temp/%_qrc.cpp
	@echo "\e[1;33m >> $^ -> $@\e[0m"
	@mkdir -p build
	@$(CXX) $(COMPILER_FLAGS) $(QT_COMPILER_FLAGS) $(QT_LINKER_FLAGS) -c $< -o $@

temp/%_qrc.cpp: ./%.qrc
	@mkdir -p temp
	@echo "\e[1;33m >> $^ -> $@\e[0m"
	@$(QRC) $< -o $@
build/%.o: src/%.cpp
	@mkdir -p build
	@echo "\e[1;33m >> $^ -> $@\e[0m"
	@$(CXX) $(COMPILER_FLAGS) $(QT_COMPILER_FLAGS) $(QT_LINKER_FLAGS) -c $^ -o $@

../sharedFiles/build/%.o: ../sharedFiles/src/%.cpp
	@mkdir -p ../sharedFiles/build
	@echo "\e[1;33m >> $^ -> $@\e[0m"
	@$(CXX) $(COMPILER_FLAGS) -c $^ -o $@

build/%_moc.o: temp/%_moc.cpp
	@mkdir -p build
	@echo "\e[1;33m >> $^ -> $@\e[0m"
	@$(CXX) $(COMPILER_FLAGS) $(QT_COMPILER_FLAGS) $(QT_LINKER_FLAGS) -c $^ -o $@

temp/%_moc.cpp: includes/%.hpp includes/moc_predefs.h
	@mkdir -p temp
	@echo "\e[1;33m >> $< -> $@\e[0m"
	@$(HPP_FLAGS) $< -o $@

includes/moc_predefs.h:
	@mkdir -p temp
	@echo "\e[1;33m >> Compiling .h for QT\e[0m"
	@g++ -pipe -O2 -Wall -W -dM -E -o temp/moc_predefs.h /usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/data/dummy.cpp

clean:
	clear
	@echo "\e[0;33m============== Cleaning  ==============\e[0m"
	@rm -f build/*.o
	@rm -f ../sharedFiles/build/*.o
	@rm -f temp/*
	@rm -f UI/src/*

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
