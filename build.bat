@echo off
clang -g -gcodeview -Wall -std=c11 -march=native -O2 src/typed_win32.c -o typed.exe -luser32 -lgdi32 -lwinmm