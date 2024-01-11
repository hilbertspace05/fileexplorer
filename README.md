# fileexplorer
File descriptor using boost

Precisa das bibliotecas icu boost e GTK3

Built in MSYS2 using g++ -o fileexplorer fileexplorer.cpp functions.cpp convert.cpp `pkg-config --libs --cflags gtk+-3.0` -I /c/msys64/mingw64/include/boost/ -lboost_filesystem-mt -licu -licuuc

