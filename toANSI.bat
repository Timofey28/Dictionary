@echo off

set FILES= main.cpp Dictionary.h Dictionary.cpp


for %%a in (%FILES%) do (
	call :convert2ansi %%a
)
echo:
echo Press any key to continue
pause > NUL
goto :eof


:convert2ansi <FILE>
set FILE=%~1
echo | set /p dummyName="%FILE%... "
IF NOT EXIST %FILE% (
	echo no such file
	goto :eof
)
iconv -f utf-8 -t windows-1251 %FILE% > temp.cpp 2> NUL
IF ERRORLEVEL 1 (
	del temp.cpp
	echo already in ANSI
) ELSE (
	copy temp.cpp %FILE% > NUL
	del temp.cpp
	echo converted to ANSI
)
goto :eof


