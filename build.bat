@echo off
g++ src\main.cc -o Dictionary
del src\main.exe
del src\main.o
rd /s /q src\Dictionaries

echo.
pause