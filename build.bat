@echo off
clang -g -gcodeview -Wall -std=c18 -D=DEBUG -march=native -mtune=native -mllvm -force-vector-width=8 -O2 -Iinclude src/typed_win32.c -o typed.exe -luser32 -lgdi32 -lwinmm