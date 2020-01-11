@echo off
clang -g -gcodeview -Wall -std=c18 -D=DEBUG -march=native -O0 -Iinclude src/typed_win32.c -o typed.exe -luser32 -lgdi32 -lwinmm