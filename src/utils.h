#pragma once
#include <Windows.h>

enum Colors {
    BLACK = 0,
    CYAN = 3,
    RED = 4,
    PURPLE = 5,
    GOLD = 6,
    GRAY = 7,
    GREEN = 10,
    SEA_WAVE = 11,
    LIGHT_RED = 12,
    BEIGE = 14,
    WHITE = 15,
};

int numLength(int num)
{
    if(num >= 1000000000) return 10;
    else if(num >= 100000000) return 9;
    else if(num >= 10000000) return 8;
    else if(num >= 1000000) return 7;
    else if(num >= 100000) return 6;
    else if(num >= 10000) return 5;
    else if(num >= 1000) return 4;
    else if(num >= 100) return 3;
    else if(num >= 10) return 2;
    else return 1;
}

void pathToDemonstration(std::string& path)
{
    for(int i = 0; i < path.size(); ++i) {
        if(path[i] == '_') path[i] = ' ';
        else if(path[i] == ';') path[i] = ':';
        else if(path[i] == '#') path[i] = '*';
    }
}

void demonstrationToPath(std::string& demo)
{
    for(int i = 0; i < demo.size(); ++i) {
        if(demo[i] == ' ') demo[i] = '_';
        else if(demo[i] == ':') demo[i] = ';';
        else if(demo[i] == '*') demo[i] = '#';
    }
}

void setColor(int color)
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD) color);
}

COORD getPos()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;
    if(GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        coord.X = csbi.dwCursorPosition.X;
        coord.Y = csbi.dwCursorPosition.Y;
        return coord;
    }
    coord.X = 0;
    coord.Y = 0;
    return coord;
}

char getChar(short x, short y)
{
    char buff[3];
    DWORD cbRead = 0;
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {x, y};
    ReadConsoleOutputCharacter(hCon, buff, 2, pos, &cbRead);
    return buff[0];
}

void setPosition(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if(!SetConsoleCursorPosition(hConsole, coord)) {
        std::cout << "\nError: " << GetLastError() << "\n";
        exit(1);
    }
}

void setPosition(COORD coord)
{
    static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if(!SetConsoleCursorPosition(hConsole, coord)) {
        std::cout << "\nError: " << GetLastError() << "\n";
        exit(1);
    }
}

std::pair<int, int> getConsoleWH()
{
    HANDLE hWndConsole;
    if(hWndConsole = GetStdHandle(-12))
    {
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if(GetConsoleScreenBufferInfo(hWndConsole, &consoleInfo)) {
            int width  = consoleInfo.srWindow.Right  - consoleInfo.srWindow.Left + 1;
            int height = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top  + 1;
            return {width, height};
        }
        else exit(-1);
    }
    else exit(-1);
}

void getConsoleWH(int& nConsoleWidth, int& nConsoleHeight)
{
    HANDLE hWndConsole;
    if(hWndConsole = GetStdHandle(-12))
    {
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if(GetConsoleScreenBufferInfo(hWndConsole, &consoleInfo)) {
            nConsoleWidth  = consoleInfo.srWindow.Right  - consoleInfo.srWindow.Left + 1;
            nConsoleHeight = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top  + 1;
        }
        else exit(-1);
    }
    else exit(-1);
}

void getRidOfSpaces(std::string& phrase, char sym = ' ')
{
    while(phrase[0] == sym) phrase.erase(0, 1);
    while(phrase.back() == sym) phrase.pop_back();
    int l = 1;
    while(l < phrase.size()) {
        if(phrase[l] == sym && phrase[l-1] == sym) {
            phrase.erase(l--, 1);
        }
        l++;
    }
}
