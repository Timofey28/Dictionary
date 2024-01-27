#include "dictionary.h"
#include "folder_info.h"
#include <iomanip>
#include <conio.h>
#include <dirent.h>
#include <cassert>
using namespace std;

extern FolderInfo folderInfo;

Dictionary::Dictionary(string file_name)
{
    file = "Dictionaries/" + file_name + ".txt";
    folder = (file_name.find("/") == string::npos) ? "" : file_name.substr(0, file_name.rfind("/") + 1);
    innerAll_file = (folder == "") ? "" : "Dictionaries/" + folder + "all.txt";
    ofstream f(file, ios::app);
    f.close();

    ReadDic(file, dict);
    if(file == "Dictionaries/all.txt") return;
    ReadDic("Dictionaries/all.txt", all_dict);
    for(auto it = dict.begin(); it != dict.end(); ++it) {
        auto del = all_dict.find(it->first);
        if(del != all_dict.end()) all_dict.erase(del);
    }

    if(folder == "" || file == innerAll_file) return;
    ReadDic(innerAll_file, innerAll_dict);
    for(auto it = dict.begin(); it != dict.end(); ++it) {
        auto del = innerAll_dict.find(it->first);
        if(del != innerAll_dict.end()) innerAll_dict.erase(del);
    }
}

void Dictionary::InterestingInfo()
{
    int minimumWidth = 86;
    int nConsoleWidth, nConsoleHeight;
    getConsoleWH(nConsoleWidth, nConsoleHeight);
    if(nConsoleWidth < minimumWidth) {
        system("cls");
        string answer = "Console is not wide enough, should be at least " + to_string(minimumWidth) +
                        " characters, you need " + to_string(minimumWidth - nConsoleWidth) + " more";
        cout << "\n\t" << ModifyString(answer, nConsoleWidth - 16, 0);;
        cin.sync();
        _getch();
        return;
    }

    int highestPossibleColumn = nConsoleHeight - 4;
    map<char, int> frequency;
    for(char c = 'a'; c <= 'z'; c++) frequency[c] = 0;
    for(auto it = dict.begin(); it != dict.end(); ++it) {
        if(it->first[0] < 'a' || it->first[0] > 'z') continue;
        frequency[it->first[0]]++;
//        for(int i = 1; i < it->first.size(); ++i) {
//            if(!ispunct(it->first[i]) && !isspace(it->first[i])) frequency[it->first[i]]++;
//        }
    }
    int greatestFreq = 0;
    for(auto it = frequency.begin(); it != frequency.end(); ++it) greatestFreq = max(greatestFreq, it->second);
    map<char, int> barHeight;
    for(auto it = frequency.begin(); it != frequency.end(); ++it) {
        int currentHeight = round((double) it->second * highestPossibleColumn / greatestFreq);
        barHeight[it->first] = currentHeight;
    }

//    system("cls");
//    setPosition(7, nConsoleHeight - 2);
//    for(char c = 'a'; c < 'z'; c++) cout << c << "  ";
//    cout << 'z';
//    int x = 1,
//        y = nConsoleHeight - 2;
//    for(int i = 1; i < highestPossibleColumn; ++i) {
//        setPosition(x, y - i);
//        cout << setw(4);
//        if(i % 3 == 0) {
//            cout << round((double) i * greatestFreq / highestPossibleColumn);
//            _setmode(_fileno(stdout), _O_U16TEXT);
//            wcout << " " << wstring(78, (wchar_t) 0x02D9);
//            _setmode(_fileno(stdout), _O_TEXT);
//        }
//        else cout << "|";
//    }
//    setPosition(x, y - highestPossibleColumn);
//    cout << setw(4) << greatestFreq;
//    _setmode(_fileno(stdout), _O_U16TEXT);
//    wcout << " " << wstring(78, (wchar_t) 0x02D9);
//    _setmode(_fileno(stdout), _O_TEXT);
//    setPosition(x, y - highestPossibleColumn - 1);
//    cout << setw(4) << (char) 24;  // стрелка вверх
//
//    setColor(255);
//    for(auto it = barHeight.begin(); it != barHeight.end(); ++it) {
//        x = 7 + 3 * (it->first - 'a');
//        y = nConsoleHeight - 3;
//        for(int i = 0; i < it->second; ++i) {
//            setPosition(x, y - i);
//            cout << "ww";
//        }
//
//        if(!it->second && frequency[it->first]) {
//            setPosition(x, y);
//            setColor(WHITE);
//            cout << "__";
//            setColor(255);
//        }
//    }
//    setColor(WHITE);
//    _getch();

    system("cls");
    setPosition(8, nConsoleHeight - 2);
    for(char c = 'a'; c < 'z'; c++) cout << c << "  ";
    cout << 'z';
    int x = 0,
        y = nConsoleHeight - 2;
    for(int i = 1; i < highestPossibleColumn; ++i) {
        setPosition(x, y - i);
        if(i % 3 == 0) {
            cout << setw(5) << round((double) i * greatestFreq / highestPossibleColumn);
            _setmode(_fileno(stdout), _O_U16TEXT);
            wcout << "|" << wstring(79, (wchar_t) 0x02D9);
            _setmode(_fileno(stdout), _O_TEXT);
        }
        else cout << "     |";
    }
    setPosition(x, y - highestPossibleColumn);
    cout << setw(5) << greatestFreq;
    _setmode(_fileno(stdout), _O_U16TEXT);
    wcout << "|" << wstring(79, (wchar_t) 0x02D9);
    _setmode(_fileno(stdout), _O_TEXT);
    setPosition(x + 5, y - highestPossibleColumn - 1);
    cout << (char) 24;  // стрелка вверх

    setColor(255);
    for(auto it = barHeight.begin(); it != barHeight.end(); ++it) {
        x = 8 + 3 * (it->first - 'a');
        y = nConsoleHeight - 3;
        for(int i = 0; i < it->second; ++i) {
            setPosition(x, y - i);
            cout << "ww";
        }

        if(!it->second && frequency[it->first]) {
            setPosition(x, y);
            setColor(WHITE);
            cout << "__";
            setColor(255);
        }
    }
    setColor(WHITE);
    _getch();
}

void Dictionary::CheckDictionariesExclusion()
{
    if(folder == "") return;
    system("cls");
    vector<string> dictionaries = folderInfo.GetFilesFromFolder(folder);
    int width, height;
    getConsoleWH(width, height);
    if(height - 2 < dictionaries.size()) {
        int rowsNeeded = dictionaries.size() - (height-2);
        cout << "\n\tNot enough space, " << rowsNeeded << " more row";
        cout << (rowsNeeded == 1 ? " is needed" : "s are needed");
        cin.sync();
        _getch();
        return;
    }

    int cursor = -1;
    for(int i = 0; i < dictionaries.size(); ++i) {
        if(folderInfo.MovieIsExcluded(folder + dictionaries[i])) setColor(SEA_WAVE);
        else setColor(BEIGE);
        cout << "\n\t" << dictionaries[i];
        if(cursor == -1 && folderInfo.TransformMovieTitleIntoPath(folder + dictionaries[i]) == file) cursor = i;
    }
    const int ARROW_COLOR = GOLD;
    const char arrow = 26;
    setPosition(6, cursor + 1);
    setColor(ARROW_COLOR);
    cout << arrow;

    char choice;
    while(1) {
        choice = _getch();
        if(choice == -32 && _kbhit()) choice = _getch();
        else choice = tolower(choice);
        if(choice == 80 || choice == 'j' || choice == -82 || choice == -98) {
            setPosition(6, cursor + 1);
            setColor(0);
            cout << " ";
            if(cursor == dictionaries.size() - 1) cursor = 0;
            else cursor++;
            setPosition(6, cursor + 1);
            setColor(ARROW_COLOR);
            cout << arrow;
        }
        else if(choice == 72 || choice == 'k' || choice == -85 || choice == -117) {
            setPosition(6, cursor + 1);
            setColor(0);
            cout << " ";
            if(cursor == 0) cursor = dictionaries.size() - 1;
            else cursor--;
            setPosition(6, cursor + 1);
            setColor(ARROW_COLOR);
            cout << arrow;
        }
        else if(choice == 'q' || choice == -87 || choice == -119) {
            if(dictionaries[cursor] == "all") continue;
            if(folderInfo.MovieIsExcluded(folder + dictionaries[cursor])) {
                folderInfo.RemoveFromExcluded(folder + dictionaries[cursor]);
                setPosition(8, cursor + 1);
                setColor(BEIGE);
                cout << dictionaries[cursor];
            }
            else {
                folderInfo.AddToExcluded(folder + dictionaries[cursor]);
                setPosition(8, cursor + 1);
                setColor(SEA_WAVE);
                cout << dictionaries[cursor];
            }
        }
        else if(choice == 13 || choice == 9) {  // enter || tab
            string newFile = folderInfo.TransformMovieTitleIntoPath(folder + dictionaries[cursor]);
            if(newFile != file) {
                if(folderInfo.MovieIsExcluded(file)) addToFile(*this, 0);
                else addToFile(*this, 1);
                Dictionary *temp = new Dictionary(newFile.substr(13, newFile.size() - 17));
                file = newFile;
                dict = temp->dict;
                all_dict = temp->all_dict;
                innerAll_dict = temp->innerAll_dict;
                delete temp;
            }
            setColor(WHITE);
            return;
        }
    }
    _getch();
}

pair<char, char> Dictionary::EnterWord(string& meaning, bool isAdding)
{
    meaning = "";
    char input[100];
    bool ignoreSpecialCharacters = 0;
    cin >> input;
    while(1) {
        OemToCharA(input, input);
        string word(input);
        if(ignoreSpecialCharacters) {
            if(word.back() == '*') {
                word.pop_back();
                if(!word.size() && meaning.back() == ' ') {
                    while(meaning.back() == ' ') meaning.pop_back();
                }
                ignoreSpecialCharacters = 0;
            }
            if(word.size()) meaning += " " + word;
            cin >> input;
            continue;
        }
        if(!ignoreSpecialCharacters && word[0] == '*') {
            word.erase(0, 1);
            if(word.back() == '*') word.pop_back();
            else ignoreSpecialCharacters = 1;
            if(word.size()) meaning += " " + word;
            cin >> input;
            continue;
        }

        if(strlen(input) < 3) {
            if((input[0] == 'r' || input[0] == 'к' || input[0] == 'R' || input[0] == 'К') && input[1] == '\0') {
                if(meaning == "") meaning = " ";
                return {'r', '\0'};
            }
            if((input[0] == 'f' || input[0] == 'а' || input[0] == 'F' || input[0] == 'А') && input[1] == '\0')
                return {'f', '\0'};
            if(tolower(input[0]) == 'b' && tolower(input[1]) == 'b' || tolower(input[0]) == 'и' && tolower(input[1]) == 'и') {
                if(isAdding) last_word.pop();
                return {'b', 'b'};
            }
            if(isAdding) goto off;
            if((input[0] == 'j' || input[0] == 'о') && (input[1] == '\0' || isdigit(input[1])))
                return {'j', input[1]};
            if((input[0] == 'k' || input[0] == 'л') && (input[1] == '\0' || isdigit(input[1])))
                return {'k', input[1]};
            if((input[0] == 'u' || input[0] == 'г') && (input[1] == '\0' || isdigit(input[1])))
                return {'u', input[1]};
            if((input[0] == 'z' || input[0] == 'я') && (input[1] == '\0' || isdigit(input[1])))
                return {'z', input[1]};
            if((input[0] == 'd' || input[0] == 'в') && (input[1] == '\0' || isdigit(input[1])))
                return {'d', input[1]};
            if((input[0] == 'f' || input[0] == 'а' || input[0] == 'F' || input[0] == 'А') && isdigit(input[1]))
                return {'f', input[1]};
            if((input[0] == 'r' || input[0] == 'к' || input[0] == 'R' || input[0] == 'К') && isdigit(input[1]))
                return {'r', input[1]};
            if(strlen(input) == 1 && input[0] == 'g' || input[0] == -17)
                return {'g', '\0'};
        }
        off:
        string cppInput(input);
        if(cppInput[0] == '\\') cppInput.erase(0, 1);
        meaning += " " + cppInput;
        cin >> input;
    }
}

void Dictionary::Print()
{
    system("cls");
    char choice;
    if(!dict.size()) {
        cout << "\n\tNo words yet";
        choice = _getche();
        return;
    }
    bool lineBreak = 0;
    for(auto it = dict.begin(); it != dict.end(); ++it) {
        if(_kbhit() && _getch() == 27) return;
        string word = it->first;
        if(dict[word].size() == 1 && dict[word][0] == " ") {
            if(lineBreak) cout << "\n";
            else lineBreak = 1;
            PrintAWord(word, 0, "", SEA_WAVE);
        }
        else {
            PrintAWord(word, 0, "", GREEN);
            lineBreak = 0;
        }
    }
    cout << "\n\t";
    int h = 0;
    HANDLE hWndConsole;
    if(hWndConsole = GetStdHandle(-12))
    {
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if(GetConsoleScreenBufferInfo(hWndConsole, &consoleInfo))
            h = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;
        else exit(2);
    }
    else exit(2);
    bool up = 0;
    COORD coord = getPos();
    int downY = coord.Y, leftX = coord.X - 1;
    setColor(BLACK);
    while((choice = _getch()) != 13) {
        coord.X = leftX;
        if(!SetConsoleCursorPosition(hWndConsole, coord))
            cout << "\nError: " << GetLastError() << "\n";
        if(choice == 'k' || choice == -85)      goto arrowUp_print;
        else if(choice == 'j' || choice == -82) goto arrowDown_print;
        else if(choice == 11)                   goto screenUp_print;  // ctrl+k
        else if(choice == 10)                   goto screenDown_print; // ctlr+j
        else if(choice == -32) {
            choice = _getch();
            if(choice == 72) { // стрелка вверх
                arrowUp_print:
                if(!up) {
                    up = 1;
                    coord.Y = max(0, coord.Y - h + 1);
                }
                do coord.Y = max(0, coord.Y - 1);
                while(getChar(leftX + 1, coord.Y) == ' ' && coord.Y);
                if(!SetConsoleCursorPosition(hWndConsole, coord))
                    cout << "\nError: " << GetLastError() << "\n";
            }
            else if(choice == 80) { // стрелка вниз
                arrowDown_print:
                if(up) {
                    up = 0;
                    coord.Y = min(downY, coord.Y + h - 1);
                }
                do coord.Y = min(downY, coord.Y + 1);
                while(getChar(leftX + 1, coord.Y + 1) == ' ' && coord.Y != downY);
                if(!SetConsoleCursorPosition(hWndConsole, coord))
                    cout << "\nError: " << GetLastError() << "\n";
            }
            else if(choice == -115) { // screen up
                screenUp_print:
                if(!up) {
                    up = 1;
                    coord.Y = max(0, coord.Y - h + 1);
                }
                coord.Y = max(0, coord.Y - h);
                if(coord.Y) while(getChar(leftX + 1, coord.Y) == ' ') coord.Y += 1;
                if(!SetConsoleCursorPosition(hWndConsole, coord))
                    cout << "\nError: " << GetLastError() << "\n";
            }
            else if(choice == -111) { // screen down
                screenDown_print:
                if(up) {
                    up = 0;
                    coord.Y = min(downY, coord.Y + h - 1);
                }
                coord.Y = min(downY, coord.Y + h);
                if(coord.Y < downY) while(getChar(leftX + 1, coord.Y + 1) == ' ') coord.Y -= 1;
                if(!SetConsoleCursorPosition(hWndConsole, coord))
                    cout << "\nError: " << GetLastError() << "\n";
            }
        }
    }
    setColor(WHITE);
}

void Dictionary::PrintOnlyEnglish()
{
    char filler = ' ';
    int space_between_columns = 4;

    system("cls");
    if(!dict.size()) {
        cout << "\n\tNo words yet";
        filler = _getche();
        return;
    }
    int w = 0, h = 0;
    HANDLE hWndConsole;
    if(hWndConsole = GetStdHandle(-12))
    {
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if(GetConsoleScreenBufferInfo(hWndConsole, &consoleInfo))
        {
            w = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
            h = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;
        }
        else exit(-1);
    }
    else exit(-1);
    if(w && h && dict.size() > h - 2) {
        int columns = 1, max_len = dict.begin()->first.size();
        for(auto it = dict.begin(); it != dict.end(); ++it)
            if(it->first.size() > max_len) max_len = it->first.size();
        w -= 16;
        while(w - space_between_columns * (columns-1) - columns * max_len >= 0) columns++;
        columns--;
        vector<int> lens(columns);
        int columns_plus1 = dict.size() % columns, k;
        auto it = dict.begin();
        for(int i = 0; i < columns; ++i) {
            k = dict.size() / columns;
            if(columns_plus1) {
                columns_plus1--;
                k++;
            }
            max_len = 0;
            while(k--) {
                if(it->first.size() > max_len) max_len = it->first.size();
                it++;
            }
            lens[i] = max_len;
        }
        while(1) {
            columns++;
            vector<int> lens_temp(columns);
            it = dict.begin();
            columns_plus1 = dict.size() % columns;
            for(int i = 0; i < columns; ++i) {
                k = dict.size() / columns;
                if(columns_plus1) {
                    columns_plus1--;
                    k++;
                }
                max_len = 0;
                while(k--) {
                    if(it->first.size() > max_len) max_len = it->first.size();
                    it++;
                }
                lens_temp[i] = max_len;
            }
            max_len = space_between_columns * (lens_temp.size() - 1);
            for(int i = 0; i < lens_temp.size(); ++i) max_len += lens_temp[i];
            if(max_len <= w) lens = lens_temp;
            else {
                columns--;
                break;
            }
        }
        columns_plus1 = dict.size() % columns;
        k = dict.size() / columns;
        if(columns_plus1) k++;
        int col_number, bar = 0;
        for(int i = 0; i < k; ++i) {
            if(_kbhit() && _getch() == 27) return;
            cout << "\n\t";
            if(columns_plus1 && i == k - 1)
                columns = columns_plus1;
            for(col_number = 0; col_number < columns; ++col_number) {
                it = dict.begin();
                bar = i;
                if(col_number <= columns_plus1 || !columns_plus1) bar += col_number * k;
                else bar += columns_plus1 * k + (col_number - columns_plus1) * (k - 1);
                for(int j = 0; j < bar; ++j, ++it);
                string word = it->first;
                for(int i = 0; i < word.size(); ++i)
                    if(word[i] == '_') word[i] = ' ';
                string spaces(lens[col_number] + space_between_columns - word.size(), filler);
                cout << word;
                if(col_number != columns - 1) cout << spaces;
            }
        }
        cout << "\n\t";
        COORD coord = getPos();
        int downY = coord.Y, leftX = coord.X - 1;
        coord.X = 0;
        coord.Y = 0;
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if(!SetConsoleCursorPosition(hConsole, coord))
            cout << "Error: " << GetLastError() << "\n";
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO structCursorInfo;
        GetConsoleCursorInfo(hConsole, &structCursorInfo);
        structCursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(hConsole, &structCursorInfo);
        bool up = 1;
        setColor(BLACK);
        char choice;
        while((choice = _getch()) != 13) {
            if(choice == 'k' || choice == -85)      goto arrowUp_printOnlyEnglish;
            else if(choice == 'j' || choice == -82) goto arrowDown_printOnlyEnglish;
            else if(choice == 11)                   goto screenUp_printOnlyEnglish; // ctrl+k
            else if(choice == 10)                   goto screenDown_printOnlyEnglish; // ctrl+j
            else if(choice == -32 && _kbhit()) {
                choice = _getch();
                if(choice == 72) { // стрелка вверх
                    arrowUp_printOnlyEnglish:
                    if(!up) {
                        up = 1;
                        coord.Y = max(0, coord.Y - h + 1);
                    }
                    coord.Y = max(0, coord.Y - 1);
                    if(!SetConsoleCursorPosition(hConsole, coord))
                        cout << "\nError: " << GetLastError() << "\n";
                }
                else if(choice == 80) { // стрелка вниз
                    arrowDown_printOnlyEnglish:
                    if(up) {
                        up = 0;
                        coord.Y = min(downY, coord.Y + h - 1);
                    }
                    coord.Y = min(downY, coord.Y + 1);
                    if(!SetConsoleCursorPosition(hConsole, coord))
                        cout << "\nError: " << GetLastError() << "\n";
                }
                else if(choice == -115) { // screen up
                    screenUp_printOnlyEnglish:
                    if(!up) {
                        up = 1;
                        coord.Y = max(0, coord.Y - h + 1);
                    }
                    coord.Y = max(0, coord.Y - h + 1);
                    if(!SetConsoleCursorPosition(hConsole, coord))
                        cout << "\nError: " << GetLastError() << "\n";
                }
                else if(choice == 10 || choice == -111) { // screen down
                    screenDown_printOnlyEnglish:
                    if(up) {
                        up = 0;
                        coord.Y = min(downY, coord.Y + h - 1);
                    }
                    coord.Y = min(downY, coord.Y + h - 1);
                    if(!SetConsoleCursorPosition(hConsole, coord))
                        cout << "\nError: " << GetLastError() << "\n";
                }
            }
        }
        setColor(WHITE);
        return;
    }
    for(auto it = dict.begin(); it != dict.end(); ++it) {
        string s = it->first;
        for(int i = 0; i < s.size(); ++i)
            if(s[i] == '_') s[i] = ' ';
        cout << "\n\t" << s;
    }
    cout << "\n\t";
    cin.sync();
    cin.get();
}

string Dictionary::ModifyString(string s, int width, int indent) // indent - расстояние от начала слова до
{                                                                // начала значения
    width -= indent;
    if(s.size() <= width) return s;
    int longest_word_size = 0;
    string word;
    for(int i = 0; i < s.size(); ++i) {
        if(s[i] == ' ') {
            longest_word_size = max(longest_word_size, (int) word.size());
            word = "";
        }
        else word += s[i];
    }
    longest_word_size = max(longest_word_size, (int) word.size());
    if(width < longest_word_size) return s;
    int i = width;
    while(i < s.size()) {
        while(s[i] != ' ') i--;
        s[i] = '\n';
        s.insert(++i, indent + 8, ' ');
        i += width + indent + 8;
    }
    return s;
}

void Dictionary::PrintAWord(string &word, int spaces_amount, string toUpper, Colors color, bool from_all_dict)
{ // само слово, длина отступа не считая первых 8 пробелов (используется только в тесте)
// string toUpper - какую часть слова надо подкрасить (используется только в поиске)
// color - цвет слова; from_all_dict - глобальный/локальный поиск
    int width;
    HANDLE hConsole;
    if(hConsole = GetStdHandle(-12)) {
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if(GetConsoleScreenBufferInfo(hConsole, &consoleInfo))
            width = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
        else exit(-1);
    }
    else exit(-1);
    width -= 16;

    map<string, vector<string>> temp(dict);
    if(from_all_dict) temp.insert(all_dict.begin(), all_dict.end());
    if(temp.find(word) == temp.end()) {
        cout << "\n\tNo such word\n\t";
        return;
    }
    string s = word;
    for(int i = 0; i < s.size(); ++i)
        if(s[i] == '_') s[i] = ' ';
    string spaces(spaces_amount, ' ');
    if(temp[word].size() == 1 && temp[word][0] == " ") s = ModifyString(s, width, spaces_amount);
    if(toUpper.size()) {
        setColor(color);
        string normal_color = s.substr(0, s.find(toUpper));
        s.erase(0, s.find(toUpper));
        cout << "\n\t" << spaces << normal_color;
        setColor(GOLD);
        string highlight = s.substr(0, toUpper.size());
        s.erase(0, toUpper.size());
        cout << highlight;
        setColor(color);
        cout << s;
        if(!(temp[word].size() == 1 && temp[word][0] == " ")) cout << " --";
        setColor(WHITE);
    }
    else {
        setColor(color);
        cout << "\n\t" << spaces << s;
        setColor(WHITE);
        if(!(temp[word].size() == 1 && temp[word][0] == " ")) cout << " --";
    }
    string whitespaces(word.size() + 4 + spaces_amount, ' ');
    int num = 1;
    if(temp[word].size() == 1 && temp[word][0] != " ") {
        cout << ModifyString(temp[word][0], width, word.size() + 4 + spaces_amount);
    }
    else if(temp[word].size() > 1) {
        cout << " 1)" << ModifyString(temp[word][0], width, word.size() + 7 + spaces_amount);
        while(num < temp[word].size()) {
            cout << "\n\t" << whitespaces << num+1 << ')';
            cout << ModifyString(temp[word][num++], width, word.size() + 7 + spaces_amount);
        }
    }
}

void Dictionary::Add()
{
    system("cls");
    string word, meaning, temp;
    char c[200];
    cout << "\n\tWrite a new word: ";
    cin >> word;
    if(word[0] == '\"' && word.back() != '\"') {
        string fff;
        do {
            cin >> fff;
            word += "_" + fff;
        }while(fff.back() != '\"');
        word.erase(0, 1);
        word.pop_back();
    }
    strcpy(c, word.c_str());
    OemToCharA(c, c);
    word = c;
    getRidOfSpaces(word);
    if(word == "") return;
    if(dict.find(word) != dict.end()) {
        system("cls");
        cout << "\n\tДанное слово уже есть в этом словаре:\n";
        PrintAWord(word, 0, "", GREEN);
        cout << "\n\n\n\tДобавить новые значения?";
        setColor(SEA_WAVE);
        cout << "  [Y/n]";
        setColor(WHITE);
        vector<string> startingMeaning;
        while(cin.peek() == ' ') cin.ignore();
        if(cin.peek() != '\n') {
            cin.ignore();
            if(cin.peek() != 'r' && cin.peek() != 170 && cin.peek() != 'R' && cin.peek() != 138) {
                do {
                    pair<char, char> p = EnterWord(meaning, 1);
                    startingMeaning.push_back(meaning);
                    if(p.first == 'r') break;
                }while(1);
            }
        }
        if(!YesOrNo()) return;
        if(startingMeaning.size()) dict[word] = startingMeaning;
        AddSomeMeanings(word);
        return;
    }
    else if(all_dict.find(word) != all_dict.end()) {
        vector<string> startingMeaning;
        while(cin.peek() == ' ') cin.ignore();
        if(cin.peek() == '\n') {
            again1:
            startingMeaning = all_dict[word];
            system("cls");
            cout << "\n\tДанное слово уже есть в общем словаре:\n";
            PrintAWord(word, 0, "", GREEN, 1);
            cout << "\n\n\n\tДобавить его сюда тоже                     "; setColor(SEA_WAVE); cout << "[y]"; setColor(WHITE);
            cout <<     "\n\tПосмотреть в каких словарях оно находится  "; setColor(SEA_WAVE); cout << "[t]"; setColor(WHITE);
            cout <<     "\n\tОставить как есть                          "; setColor(SEA_WAVE); cout << "[n]"; setColor(WHITE);
            only_yes_or_no3:
            c[0] = _getch();
            OemToCharA(c, c);
            c[0] = tolower(c[0]);
            if(c[0] == 110 || c[0] == -14) return; // n || т
            if(c[0] == 116 || c[0] == -27) { // t || е
                int res = Determine(word);
                if(!res) return;
                else if(res == 1) goto again1;
            }
            else if(c[0] != 121 && c[0] != -19) goto only_yes_or_no3; // y && н
            else goto addHereAlso;
        }
        cin >> c;
        OemToCharA(c, c);
        if((c[0] == 170 || c[0] == 114 || c[0] == 82 || c[0] == 138) && c[1] == '\0') // к, r, R, К соотв.
            startingMeaning = all_dict[word];
        else {
            string mnRightAfterWord = c;
            do {
                pair<char, char> p = EnterWord(meaning, 1);
                if(mnRightAfterWord.size()) {
                    meaning = " " + mnRightAfterWord + meaning;
                    mnRightAfterWord = "";
                }
                startingMeaning.push_back(meaning);
                if(p.first == 'r') {
                    if(!startingMeaning.size()) startingMeaning.push_back(" " + mnRightAfterWord);
                    break;
                }
            }while(1);
        }
        again2:
        system("cls");
        cout << "\n\tДанное слово уже есть в общем словаре:\n";
        PrintAWord(word, 0, "", GREEN, 1);
        cout << "\n\n\n\tДобавить с только что введенными значениями     "; setColor(SEA_WAVE); cout << "[y]"; setColor(WHITE);
        if(startingMeaning != all_dict[word]) {
            cout << "\n\tДобавить с этими значениями                     "; setColor(SEA_WAVE); cout << "[s]"; setColor(WHITE);
        }
        cout <<     "\n\tПосмотреть в каких словарях оно находится       "; setColor(SEA_WAVE); cout << "[t]"; setColor(WHITE);
        cout <<     "\n\tОставить как есть                               "; setColor(SEA_WAVE); cout << "[n]"; setColor(WHITE);
        only_yes_or_no2:
        cin.sync();
        c[0] = _getch();
        OemToCharA(c, c);
        c[0] = tolower(c[0]);
        if(c[0] == 110 || c[0] == -14) return; // (n || т)
        else if((c[0] == 115 || c[0] == -5) && startingMeaning != all_dict[word]) { // (s || ы)
            dict[word] = all_dict[word];
            last_word.push(word);
            return;
        }
        else if(c[0] == 116 || c[0] == -27) { // (t || е)
            int res = Determine(word);
            if(!res) return;
            else if(res == 1) goto again2;
        }
        else if(c[0] != 121 && c[0] != -19) goto only_yes_or_no2; // (!y && !н)

        addHereAlso:
        vector<string> oldMeaning = dict[word];
        dict[word] = startingMeaning;
        AddSomeMeanings(word);
        if(startingMeaning != oldMeaning) {
            // изменяем значения этого слова во всех словарях
            DIR *dir;
            dirent *entry;
            dir = opendir("Dictionaries");
            if(!dir) {
                string endProgram = "\ndictionary.cc -> строка " + to_string(__LINE__);
                perror(endProgram.c_str());
                exit(-1);
            }
            string path;
            while((entry = readdir(dir)) != nullptr) {
                path = entry->d_name;
                if(path == "." || path == ".." || path == "all.txt" || path == "__excluded__.txt" ||
                   "Dictionaries/" + path == file || path.substr(path.size() - 4) != ".txt") continue;
                path.erase(path.size() - 4);
                Dictionary *temp = new Dictionary(path);
                if(temp->dict.find(word) != temp->dict.end()) {
                    temp->dict[word] = dict[word];
                    addToFile(*temp, 0);
                }
                delete temp;
            }
        }
        last_word.push(word);
        return;
    }
    last_word.push(word);
    while(1) {
        system("cls");
        cout << "\n\tWrite a meaning: ";
        pair<char, char> p = EnterWord(meaning, 1);
        dict[word].push_back(meaning);
        if(p.first == 'r') return;
        if(p.first == 'b') {
            auto del = dict.find(word);
            if(del != dict.end()) dict.erase(del);
            return;
        }
    }
}

void Dictionary::AddSomeMeanings(string word)
{
    system("cls");
    string temp, meaning;
    char choice, c[200];
    bool wordIsLocal = 1, calledFromMenu = 0;
    if(!dict.size()) {
        cout << "\n\tNo words yet";
        _getch();
        return;
    }
    if(word == "") {  // если функция вызвана из главного меню
        calledFromMenu = 1;
        cin.getline(c, 200);
        OemToCharA(c, c);
        word = c;
        getRidOfSpaces(word);
        if(word == "") {
            cout << "\n\tWhich word to change?\n\n\t=>";
            cin.sync();
            cin.getline(c, 200);
            OemToCharA(c, c);
            word = c;
            getRidOfSpaces(word);
            system("cls");
        }
    }
    if(word == "") return;
    if(word == "." || word == "ю") {
        if(last_word.size()) word = last_word.top();
        else if(dict.size() == 1) word = dict.begin()->first;
        else {
            if(!dict.size()) cout << "\n\tNo words added yet";
            else cout << "\n\tNo words added recently";
            _getch();
            return;
        }
    }
    for(int i = 0; i < word.size(); ++i) {
        if(word[i] == ' ') word[i] = '_';
    }
    if(!calledFromMenu && dict.find(word) == dict.end()) { // если ты сюда пришел из determine() -> Wanna change? - yes -> и слова нет в текущем словаре
        wordIsLocal = 0;
        dict[word] = all_dict[word];
    }

    if(dict.find(word) == dict.end()) {
        cout << "\n\tNo such word. Wanna add?";
        setColor(SEA_WAVE);
        cout << "  [Y/n]";
        setColor(WHITE);
        if(YesOrNo()) {
            dict[word].push_back(" ");
            last_word.push(word);
            AddSomeMeanings(word);
        }
    }
    else {
        bool cursorIsVisible = 1;
        CONSOLE_CURSOR_INFO structCursorInfo;
        GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &structCursorInfo);
        if(structCursorInfo.bVisible != TRUE) {
            cursorIsVisible = 0;
            structCursorInfo.bVisible = TRUE;
            SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &structCursorInfo);
        }
        vector<string> oldMeaning = dict[word];
        string oldWord = word;
        while(1) {
            system("cls");
            PrintAWord(word);
            cout << "\n\n\tWrite a meaning: ";
            pair<char, char> p = EnterWord(meaning);
            if(p.first == 'r') { // выход/добавление нового значения и затем сразу выход
                if(isdigit(p.second)) {
                    int k = min((int) dict[word].size(), p.second - 49);
                    if(k == dict[word].size()) {
                        if(dict[word].size() == 1 && dict[word][0] == " ") dict[word][0] = meaning;
                        else dict[word].push_back(meaning);
                    }
                    else {
                        vector<string> new_meaning;
                        new_meaning.assign(dict[word].begin(), dict[word].begin() + k);
                        new_meaning.push_back(meaning);
                        new_meaning.insert(new_meaning.end(), dict[word].begin() + k, dict[word].end());
                        dict[word] = new_meaning;
                    }
                }
                else {
                    if(dict[word][0] == " " && meaning != " ") dict[word][0] = meaning;
                    else if(meaning != " ") dict[word].push_back(meaning);
                }
                break;
            }
            if(p.first == 'd') { // удаление значения
                if(p.second == '\0') dict[word].pop_back();
                else dict[word].erase(dict[word].begin() + p.second - 49);
                if(!dict[word].size()) dict[word].push_back(" ");
            }
            else if(p.first == 'z') { // замена значения
                int index = (p.second == '\0') ? dict[word].size() - 1 : p.second - 49;
                if(count(meaning.begin(), meaning.end(), 'Э') >= 4) {
                    for(int i = 0; i < meaning.size(); ++i)
                        if(meaning[i] == 'Э') meaning[i] = '\"';
                }
                if(count(meaning.begin(), meaning.end(), '\"') == 4) {
                    if(meaning.size()) meaning.erase(0, 1);
                    string from = meaning.substr(1, meaning.find("\"", 1) - 1);
                    int thirdQuoteMark = meaning.rfind("\"", meaning.size() - 2);
                    string to = meaning.substr(thirdQuoteMark + 1, meaning.rfind("\"") - thirdQuoteMark - 1);
                    int k = 0;
                    vector<int> indexesWithWordFrom;
                    while(k < dict[word][index].size()) {
                        k = dict[word][index].find(from, k);
                        if(k != string::npos) {
                            indexesWithWordFrom.push_back(k);
                            k += from.size();
                        }
                        else break;
                    }
                    for(int i = indexesWithWordFrom.size() - 1; ~i; --i)
                        dict[word][index].replace(indexesWithWordFrom[i], from.size(), to);
                    while(dict[word][index][0] == ' ') dict[word][index].erase(0, 1);
                    if(dict[word][index][0] != ' ') dict[word][index].insert(0, 1, ' ');
                }
                else dict[word][index] = meaning;
            }
            else if(p.first == 'j') { // добавление в уже имеющееся значение после запятой
                if(p.second == '\0') {
                    if(dict[word].size() == 1 && dict[word][0] == " ") dict[word][0] = meaning;
                    else dict[word].back() += "," + meaning;
                }
                else dict[word][max(0, p.second - 49)] += "," + meaning;
            }
            else if(p.first == 'k') { // добавление в уже имеющееся значение после пробела
                if(p.second == '\0') {
                    if(dict[word].size() == 1 && dict[word][0] == " ") dict[word][0] = meaning;
                    else dict[word].back() += meaning;
                }
                else dict[word][max(0, p.second - 49)] += meaning;
            }
            else if(p.first == 'u') { // удаление последнего слова/выражения после последней запятой у какого-то значения
                int k;
                if(p.second == '\0') k = dict[word].size() - 1;
                else k = p.second - 49;
                k = min(k, (int) dict[word].size() - 1);
                string s = dict[word][k];
                if(s.find(",") != string::npos) {
                    s.erase(s.rfind(","));
                    dict[word][k] = s;
                }
            }
            else if((p.first == 'f') && isdigit(p.second)) { // добавление нового значения
                int k = max(min((int) dict[word].size(), p.second - 49), 0);
                if(k == dict[word].size()) {
                    if(dict[word].size() == 1 && dict[word][0] == " ") dict[word][0] = meaning;
                    else dict[word].push_back(meaning);
                }
                else {
                    vector<string> new_meaning;
                    new_meaning.assign(dict[word].begin(), dict[word].begin() + k);
                    new_meaning.push_back(meaning);
                    new_meaning.insert(new_meaning.end(), dict[word].begin() + k, dict[word].end());
                    dict[word] = new_meaning;
                }
            }
            else if(p.first == 'g') { // замена слова
                meaning.erase(0, 1);
                for(int i = 0; i < meaning.size(); ++i)
                    if(meaning[i] == ' ') meaning[i] = '_';
                if(dict.find(meaning) == dict.end() && all_dict.find(meaning) == all_dict.end()) {
                    map<string, vector<string>> excludedDictionaries;
                    for(const auto& excludedPath : folderInfo.GetExcludedPaths())
                        ReadDic(excludedPath, excludedDictionaries, 0);
                    if(excludedDictionaries.find(meaning) == excludedDictionaries.end()) {
                        dict[meaning] = dict[word];
                        dict.erase(dict.find(word));
                        word = meaning;
                        if(last_word.size()) last_word.pop();
                        last_word.push(word);
                    }
                }
            }
            else if(p.first != 'b') {
                if(dict[word].size() == 1 && dict[word][0] == " ") dict[word][0] = meaning;
                else dict[word].push_back(meaning);
            }
        }
        if(word != oldWord || dict[word] != oldMeaning) {
            vector<string> dictionaries = folderInfo.GetAllPathsInSingleContainer();
            map<string, vector<string>> workbook;
            for(const auto& path : dictionaries) {
                ReadDic(path, workbook);
                auto it = workbook.find(oldWord);
                if(it != workbook.end()) {
                    if(word != oldWord) workbook.erase(it);
                    workbook[word] = dict[word];
                    WriteInDic(path, workbook);
                }
            }
        }
        if(!cursorIsVisible) {
            structCursorInfo.bVisible = FALSE;
            SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &structCursorInfo);
        }
    }
}

void Dictionary::FindAWord(bool findAmongAll)
{
    vector<bool> lettersEntered; // 1 - english, 0 - russian
    char c[2];
    while(cin.peek() == ' ') cin.ignore();
    string word = "";
    if(cin.peek() != '\n') {
        getline(cin, word);
        getRidOfSpaces(word);
        for(int i = 0; i < word.size(); ++i) {
            c[0] = word[i];
            OemToCharA(c, c);
            if(!isalpha(c[0])) {
                if(lettersEntered.size()) {
                    lettersEntered.push_back(lettersEntered.back());
                    if(lettersEntered.back() && word[i] == ' ') word[i] = '_';
                    else word[i] = c[0];
                }
                else {
                    if(word[i] == ' ') word[i] = '_';
                    else word[i] = c[0];
                    lettersEntered.push_back(1);
                }
            }
            if(word[i] == c[0]) {
                lettersEntered.push_back(1);
            }
            else {
                lettersEntered.push_back(0);
                word[i] = c[0];
            }
        }
    }
    map<string, vector<string>> dictToSearchIn(dict);
    if(findAmongAll) dictToSearchIn.insert(all_dict.begin(), all_dict.end());
    if(!dictToSearchIn.size()) {
        cout << "\n\tNo words yet";
        _getch();
        return;
    }
    int width, height;
    getConsoleWH(width, height);
    width -= 16;
    bool up = 1;
    COORD coord;
    int downY, leftX;
    char inputBuffer = '.';  // сначала присваиваем любой символ не являющийся цифрой
    vector<string> foundWords;
    if(word.size()) goto firstTime;

    if(findAmongAll || file == "Dictionaries/all.txt") setColor(PURPLE);
    else setColor(GRAY);
    system("cls");
    cout << "\n\tInput: ";
    char sym;
    cin.sync();
    while(1) {
        sym = _getch();
        if(sym == 13 || sym == 27) return; // enter || escape
        if(sym == 9) { // tab
            if(foundWords.size()) {
                int wordNo = 0;
                if(isdigit(inputBuffer) && inputBuffer != '0') {
                    wordNo = min((int) inputBuffer - 49, (int) foundWords.size() - 1);
                    inputBuffer = '.';
                }
                assert(wordNo >= 0 && wordNo < foundWords.size());
                int res = Determine(foundWords[wordNo]);
                assert(res == 0 || res == 1);
                if(res == 1) goto firstTime;
                else if(res == 0) return;
            }
            continue;
        }

        inputBuffer = sym;
        if(isdigit(inputBuffer)) continue;

        if(sym == 8) { // backspace
            if(word.size()) {
                word.pop_back();
                lettersEntered.pop_back();
            }
        }
        else if(sym == 127) { // ctrl + backspace
            if(word == "") continue;
            while(word.back() == '_' || word.back() == ' ') {
                word.pop_back();
                lettersEntered.pop_back();
            }
            if(word.find('_') != string::npos) {
                while(word.back() != '_') {
                    word.pop_back();
                    lettersEntered.pop_back();
                }
            }
            else if(word.find(' ') != string::npos) {
                while(word.back() != ' ') {
                    word.pop_back();
                    lettersEntered.pop_back();
                }
            }
            else {
                word = "";
                lettersEntered.clear();
            }
        }
        else if(sym == 32 && word.size() && (word.back() == ' ' || word.back() == '_')) continue; // второй пробел подряд не печатаем
        else if((sym == -32 || sym == -16) && _kbhit()) {
            sym = _getch();
            if(!word.size()) continue;
            if(sym == 72) { // стрелка вверх
                if(!up) {
                    up = 1;
                    coord.Y = max(0, coord.Y - height + 1);
                }
                do coord.Y = max(0, coord.Y - 1);
                while(getChar(leftX + 1, coord.Y) == ' ' && coord.Y);
                setPosition(coord);
            }
            else if(sym == 80) { // стрелка вниз
                if(up) {
                    up = 0;
                    coord.Y = min(downY, coord.Y + height - 1);
                }
                do coord.Y = min(downY, coord.Y + 1);
                while(getChar(leftX + 1, coord.Y + 1) == ' ' && coord.Y != downY);
                setPosition(coord);
            }
            else if(sym == 73 || sym == -115) { // PgUp || Ctrl + Up
                if(!up) {
                    up = 1;
                    coord.Y = max(0, coord.Y - height + 1);
                }
                coord.Y = max(0, coord.Y - height + 1);
                if(coord.Y) while(getChar(leftX + 1, coord.Y) == ' ') coord.Y += 1;
                setPosition(coord);
            }
            else if(sym == 81 || sym == -111) { // PgDn || Ctrl + down
                if(up) {
                    up = 0;
                    coord.Y = min(downY, coord.Y + height - 1);
                }
                coord.Y = min(downY, coord.Y + height - 1);
                if(coord.Y < downY) while(getChar(leftX + 1, coord.Y + 1) == ' ') coord.Y -= 1;
                setPosition(coord);
            }
            continue;
        }
        else {
            c[0] = sym;
            OemToCharA(c, c);
            if(!isalpha(c[0])) {
                if(lettersEntered.size()) {
                    lettersEntered.push_back(lettersEntered.back());
                    if(lettersEntered.back() && sym == ' ') word += '_';
                    else word += c[0];
                }
                else {
                    if(sym == ' ') word = "_";
                    else word = c[0];
                    lettersEntered.push_back(1);
                }
            }
            else {
                if(sym == c[0]) lettersEntered.push_back(1);
                else lettersEntered.push_back(0);
                word += c[0];
            }
            up = 1;
        }
        firstTime:
        system("cls");
        if(findAmongAll || file == "Dictionaries/all.txt") setColor(PURPLE);
        else setColor(GRAY);
        cout << "\n\tInput: ";
        if(!word.size()) continue;
        string wordToShow = word;
        for(int i = 0; i < wordToShow.size(); ++i)
            if(wordToShow[i] == '_') wordToShow[i] = ' ';
        setColor(GOLD);
        cout << wordToShow;
        setColor(WHITE);
        bool no_words = 1;
        cout << "\n";

        foundWords.clear();
        if(count(lettersEntered.begin(), lettersEntered.end(), 1) == lettersEntered.size()) { // english
            for(auto it = dictToSearchIn.begin(); it != dictToSearchIn.end(); ++it) {
                if(_kbhit()) continue;
                string temp = it->first;
                if(temp.find(word) == 0) {
                    PrintAWord(temp, 0, "", SEA_WAVE, findAmongAll);
                    no_words = 0;
                    foundWords.push_back(temp);
                }
                else if(temp.find(word) < (1<<30)) no_words = 0;
            }
            if(!no_words) cout << "\n\n\t------------------------------------------\n";
            for(auto it = dictToSearchIn.begin(); it != dictToSearchIn.end(); ++it) {
                if(_kbhit()) continue;
                string temp = it->first;
                if(temp.find(word) != string::npos && temp.find(word)) {
                    PrintAWord(temp, 0, word, GRAY, findAmongAll);
                    no_words = 0;
                    foundWords.push_back(temp);
                }
            }
        }
        else if(count(lettersEntered.begin(), lettersEntered.end(), 0) == lettersEntered.size()) { // russian
            for(auto it = dictToSearchIn.begin(); it != dictToSearchIn.end(); ++it) {
                if(_kbhit()) continue;
                vector<string> meaning = it->second;
                if(meaning.size() == 1 && meaning[0] != " ")
                    meaning[0] = ModifyString(meaning[0], width, it->first.size() + 4);
                else if(meaning.size() > 1)
                    for(int i = 0; i < meaning.size(); ++i)
                        meaning[i] = ModifyString(meaning[i], width, it->first.size() + 7);
                for(int i = 0; i < meaning.size(); ++i) {
                    int pos = meaning[i].find(word);
                    if(pos != string::npos && pos != 0) {
                        no_words = 0;
                        string key = it->first;
                        foundWords.push_back(key);
                        for(int j = 0; j < key.size(); ++j)
                            if(key[j] == '_') key[j] = ' ';
                        setColor(SEA_WAVE);
                        cout << "\n\t" << key;
                        setColor(WHITE);
                        cout << " --";
                        setColor(GRAY);
                        string whitespaces(it->first.size() + 12, ' ');
                        if(meaning.size() == 1 && meaning[0] != " ") {
                            pos = meaning[0].find(word);
                            if(pos == string::npos) cout << meaning[0];
                            else {
                                cout << meaning[0].substr(0, pos);
                                setColor(GOLD);
                                cout << meaning[0].substr(pos, word.size());
                                setColor(GRAY);
                                cout << meaning[0].substr(pos + word.size());
                            }
                        }
                        else if(meaning.size() > 1) {
                            pos = meaning[0].find(word);
                            if(pos == string::npos) cout << " 1)" << meaning[0];
                            else {
                                cout << " 1)" << meaning[0].substr(0, pos);
                                setColor(GOLD);
                                cout << meaning[0].substr(pos, word.size());
                                setColor(GRAY);
                                cout << meaning[0].substr(pos + word.size());
                            }
                            for(int j = 1; j < meaning.size(); ++j) {
                                pos = meaning[j].find(word);
                                cout << "\n" << whitespaces << j + 1 << ")";
                                if(pos == string::npos) cout << meaning[j];
                                else {
                                    cout << meaning[j].substr(0, pos);
                                    setColor(GOLD);
                                    cout << meaning[j].substr(pos, word.size());
                                    setColor(GRAY);
                                    cout << meaning[j].substr(pos + word.size());
                                }
                            }
                        }
                        else cout << " ...";
                        break;
                    }
                }
            }
            setColor(WHITE);
        }
        else {
            cout << "\n\tMust be only one language";
            continue;
        }

        if(no_words) cout << "\n\tNo words found\n\t";
        else cout << "\n\t";
        coord = getPos();
        leftX = coord.X - 1;
        downY = coord.Y;
        coord.X = 15 + word.size();
        coord.Y = 0;
        setPosition(coord);
        coord.Y = 1;
        setPosition(coord);
        coord.Y = 0;
    }
}

void Dictionary::GetRidOf()
{
    system("cls");
    string word;
    char c[200];
    if(!dict.size()) {
        cout << "\n\tNo words yet";
        c[0] = _getche();
        return;
    }
    cin.getline(c, 200);
    OemToCharA(c, c);
    word = c;
    bool digit = 0;
    for(int i = 0; i < word.size(); ++i) {
        if(!isspace(word[i])) {
            digit = 1;
            break;
        }
    }
    if(!digit) {
        cout << "\n\tWhich word to delete?\n\n\t";
        cin.sync();
        cin.getline(c, 200);
        OemToCharA(c, c);
        word = c;
    }
    getRidOfSpaces(word);
    for(int i = 0; i < word.size(); ++i)
        if(word[i] == ' ') word[i] = '_';
    bool leaveWordInAll = 0;
    vector<string> value;
    if(word.find('*') != string::npos) {
        while(word.find('*') != string::npos)
            word.replace(word.find('*'), 1, "");
        leaveWordInAll = 1;
        getRidOfSpaces(word, '_');
    }
    if(word.size() > 1 && word[0] == '/') word.erase(0, 1);
    else if(word == "ю" || word == ".") {
        if(last_word.size()) {
            word = last_word.top();
            last_word.pop();
        }
        else if(dict.size() == 1) word = dict.begin()->first;
        else {
            if(!dict.size()) cout << "\n\tNo words added yet";
            else cout << "\n\tNo words added recently";
            c[0] = _getch();
            return;
        }
    }
    auto it = dict.find(word);
    if(it == dict.end()) return;
    if(leaveWordInAll) all_dict[word] = dict[word];
    dict.erase(it);
}

bool Dictionary::ExamRightAnswer(vector<string> &answers, string answer)
{
    set<string> ans;
    while(answer.find(" ") != string::npos) {
        ans.insert(answer.substr(0, answer.find(" ")));
        answer.erase(0, answer.find(" ") + 1);
    }
    ans.insert(answer);
    bool right;
    for(const auto &x : answers) {
        right = 1;
        for(auto it = ans.begin(); it != ans.end(); ++it) {
            if(x.find(*it) == string::npos) right = 0;
        }
        if(right) return 1;
    }
    return 0;
}

void Dictionary::Exam()
{
    mt19937 mersenne(static_cast<unsigned int>(time(0)));
    system("cls");
    map<string, vector<string>> wrong;
    bool switch_to_all = 1;
    int amount_of_words = 0;
    char choice;
    if(!dict.size()) {
        cout << "\n\tNo words yet";
        choice = _getch();
        return;
    }
    vector<int> rest(dict.size()), order(20);
    for(int i = 0; i < dict.size(); ++i) rest[i] = i;
    if(dict.size() <= 20) {
        order = rest;
        rest.clear();
    }
    else {
        for(int i = 0; i < 20; ++i) {
            int random = mersenne() % rest.size();
            order[i] = rest[random];
            rest.erase(rest.begin() + random);
        }
    }
    shuffle(rest.begin(), rest.end(), mersenne);
    string answer;
    int right_ans = 0, question_number = 1;
    cin.sync();
    for(int i = 0; i < order.size(); ++i) {
        auto it = dict.begin();
        int te = order[i];
        for(; te--; ++it);
        system("cls");
        cout << "\n\t(" << right_ans << "/" << order.size() << " правильных ответов)\n";
        cout << "\n\t" << question_number++ << ") ";
        string t = it->first;
        for(int i = 0; i < t.size(); ++i)
            if(t[i] == '_') t[i] = ' ';
        string s = t + " -- ";
        for(auto x : s) {
            cout << x;
            this_thread::sleep_for(35ms);
        }

        char a[255];
        cin.sync();
        cin.getline(a, 255);
        OemToCharA(a, a);
        string temp(a);
        getRidOfSpaces(temp);
        answer = temp;

        if(answer == "all" || answer == "фдд") {
            if(switch_to_all) {
                order.insert(order.end(), rest.begin(), rest.end());
                switch_to_all = 0;
            }
            i--;
            question_number--;
            continue;
        }
        if(answer == "end" || answer == "утв") {
            amount_of_words = question_number - 2;
            break;
        }
        int spaces_amount;
        if(answer == "") {
            system("cls");
            wrong[it->first] = it->second;
            cout << "\n\t(" << right_ans << "/" << order.size() << " правильных ответов)\n";
            temp = it->first;
            if(question_number <= 10) spaces_amount = 3;
            else if(question_number <= 100) spaces_amount = 4;
            else spaces_amount = 5;
            PrintAWord(temp, spaces_amount);
            cout << "\n\n\t";
            choice = _getche();
            if(choice == 'f' || choice == ' ') right_ans++;
            continue;
        }
        else if(answer == "exit" || answer == "учше") return;
        if(ExamRightAnswer(it->second, answer)) {
            system("cls");
            right_ans++;
            cout << "\n\t(" << right_ans << "/" << order.size() << " правильных ответов)\n";
        }
        temp = it->first;
        if(question_number <= 10) spaces_amount = 3;
        else if(question_number <= 100) spaces_amount = 4;
        else spaces_amount = 5;
        PrintAWord(temp, spaces_amount);
        if(ExamRightAnswer(it->second, answer)) {
            cout << "\n\n\tВерно!\n\n\t";
            cin.sync();
            cin.get();
        }
        else {
            cout << "\n\n\tНеверно\n";
            cout << "\tДобавить это значение? ";
            char choice[10];
            cin.sync();
            cin.getline(choice, 10);
            OemToCharA(choice, choice);
            if(choice[0] == 'в' || choice[0] == 'd') {
                answer = " " + answer;
                it->second.push_back(answer);
                right_ans++;
            }
            else if(choice[0] == 'о' || choice[0] == 'j') {
                int l = it->second.size()-1, k;
                if(choice[1] == '\0') k = it->second.size()-1;
                else k = min(l, abs(choice[1] - 49));
                it->second[k] += ", " + answer;
                right_ans++;
            }
            else if(choice[0] == 'а' || choice[0] == 'f') right_ans++;
            else wrong[it->first] = it->second;
        }
    }
    system("cls");
    cout << "\n\t--------------------------------------------";
    if(!amount_of_words) amount_of_words = order.size();
    double percent = (right_ans * 100 / (double) amount_of_words);
    percent = round(percent);
    cout << "\n\tПравильных ответов: " << right_ans << " из " << amount_of_words << "\t( " << percent << "% )";
    cout << "\n\t--------------------------------------------";
    if(wrong.size()) cout << "\n\n\tНеверные ответы:\n";
    for(auto it = wrong.begin(); it != wrong.end(); ++it) {
        string s = it->first;
        PrintAWord(s);
    }
    cout << "\n\t";
    cin.get();
}

bool Dictionary::Remov()
{
    string just_a_name = file;
    just_a_name.erase(0, just_a_name.find("/") + 1);
    just_a_name.erase(just_a_name.size() - 4);
    if(just_a_name.substr(0, 7) == "papka__") {
        just_a_name.erase(0, 7);
        just_a_name.replace(just_a_name.find("/"), 1, " -> ");
    }
    just_a_name[0] = toupper(just_a_name[0]);
    pathToDemonstration(just_a_name);
    char c[2];
    system("cls");
    cout << "\n\tДействительно удалить словарь \"" << just_a_name << "\"?";
    setColor(SEA_WAVE);
    cout << "\n\n\t[Y/n]";
    setColor(WHITE);
    if(!YesOrNo()) return 0;
    WriteInDic("Dictionaries/all.txt", all_dict);
    if(innerAll_file != "") WriteInDic(innerAll_file, innerAll_dict);
    remove(file.c_str());
    folderInfo.DeleteFile(file);
    folderInfo.RemoveFromExcluded(file);
    file = innerAll_file;
    innerAll_dict.clear();
    if(file != "") {
        ReadDic(innerAll_file, dict);
        return 0;
    }
    return 1;
}

void addToFile(Dictionary &x, bool saveInAll)
{
    if(x.file == "") return;
    x.WriteInDic(x.file, x.dict);
    string xAllFile = "Dictionaries/all.txt";
    if(x.file == "Dictionaries/" + x.folder + "all.txt") return;
    if(saveInAll) {
        map<string, vector<string>> intermediate(x.all_dict);
        for(auto it = x.dict.begin(); it != x.dict.end(); ++it)
            intermediate[it->first] = it->second;
        x.WriteInDic(xAllFile, intermediate);
    }
    else x.WriteInDic(xAllFile, x.all_dict);
    if(x.folder != "") {
        map<string, vector<string>> intermediate(x.innerAll_dict);
        for(auto it = x.dict.begin(); it != x.dict.end(); ++it)
            intermediate[it->first] = it->second;
        x.WriteInDic(x.innerAll_file, intermediate);
    }
}

int Dictionary::Determine(string word)
{
    if(file != "Dictionary/all.txt") {
        if(folderInfo.MovieIsExcluded(file)) addToFile(*this, 0);
        else addToFile(*this, 1);
    }
    system("cls");
    char c[200];
    bool calledFromAdding = 1;
    if(word == "") {  // если функция вызвана из главного меню
        calledFromAdding = 0;
        cin.getline(c, 200);
        OemToCharA(c, c);
        word = c;
        getRidOfSpaces(word);
        if(word == "") {
            cout << "\n\tWhich word to find?\n\n\t=> ";
            cin.sync();
            cin.getline(c, 200);
            OemToCharA(c, c);
            word = c;
            getRidOfSpaces(word);
            system("cls");
        }
    }
    if(word == "") return 0;
    for(int i = 0; i < word.size(); ++i)
        if(word[i] == ' ') word[i] = '_';
    if(word == "." || word == "ю") {
        if(!last_word.size()) {
            cout << "\n\tNo words added recently";
            cin.sync();
            cin.get();
            return 0;
        }
        else word = last_word.top();
    }
    if(dict.find(word) == dict.end() && all_dict.find(word) == all_dict.end()) {
        cout << "\n\tNo such word";
        cin.sync();
        cin.get();
        return 0;
    }

    clock_t startTime = clock();
    vector<string> films = folderInfo.GetAllFilmsInSingleContainer();
    map<string, vector<string>> intermediateDictionary;
    map<string, int> found;
    string path;
    for(auto film : films) {
        path = folderInfo.TransformMovieTitleIntoPath(film);
        if(path.find("/all.txt") != string::npos) continue;
        ReadDic(path, intermediateDictionary);
        if(intermediateDictionary.find(word) != intermediateDictionary.end()) {
            film[0] = toupper(film[0]);
            if(film.find("/") != string::npos) film.replace(film.find("/"), 1, " -> ");
            found[film] = intermediateDictionary.size();
        }
    }
    clock_t finishTime = clock();

    if(!found.size()) {  // слово по каким-то причинам есть в общем all, но его нет в других словарях
        auto it = all_dict.find(word);
        assert(it != all_dict.end());
        all_dict.erase(it);
        cout << "\n\tNo such word";
        cin.sync();
        cin.get();
        return 0;
    }
    vector<string> dots;
    if(found.size() == 1) dots.push_back(string(5, '.'));
    else {
        int maxSize = 0;
        for(auto it = found.begin(); it != found.end(); ++it) maxSize = max(maxSize, int(it->first.size()));
        maxSize += 5;
        for(auto it = found.begin(); it != found.end(); ++it) dots.push_back(string(maxSize - it->first.size(), '.'));
    }
    setColor(GREEN);
    cout << "\n\tFound in: (for " << (double) (finishTime - startTime) / CLOCKS_PER_SEC << "s)";
    setColor(WHITE);
    int index = 0, longestDictSize = 0; // длина числа количества слов в словаре (5 -> 1, 12 -> 2, 143 -> 3)
    for(auto it = found.begin(); it != found.end(); ++it) longestDictSize = max(longestDictSize, numLength(it->second));
    for(auto it = found.begin(); it != found.end(); ++it) {
        cout << "\n\t\"" << it->first << "\" ";
        setColor(GRAY);
        cout << dots[index++] << string(longestDictSize - numLength(it->second), '.') << " " << it->second;
        setColor(WHITE);
    }
    cout << "\n";
    PrintAWord(word, 0, "", GREEN, 1);
    if(calledFromAdding) {
        cout << "\n\n\n\tНазад           "; setColor(SEA_WAVE); cout << "[b]"; setColor(WHITE);
        cout << "\n\tВ главное меню  "; setColor(SEA_WAVE); cout << "[n]"; setColor(WHITE);
        while(1) {
            c[0] = _getch();
            OemToCharA(c, c);
            if(c[0] == 'n' || c[0] == 'т') return 0;
            if(c[0] == 'b' || c[0] == 'и') return 1;
        }
    }
    cout << "\n\n\n\tWanna change anything?";
    setColor(SEA_WAVE);
    cout << "  [Y/n]";
    setColor(WHITE);
    if(YesOrNo()) AddSomeMeanings(word);
    return 0;
}

int getDictionarySize(const string& path)
{
    ifstream fin(path.c_str());
    if(!fin.is_open()) {
        perror(("\ndictionary.cc -> строка " + to_string(__LINE__) + "\n" + path).c_str());
        exit(-1);
    }
    int ans = 0;
    string word;
    while(!fin.eof()) {
        fin >> word;
        if(word[0] == '!') ans++;
    }
    fin.close();
    return ans;
}

void Dictionary::FoldersHandler()
{
    system("cls");
    cout << "\n";
    string choice;
    getline(cin, choice);
    getRidOfSpaces(choice);
    bool out = 0;
    if(choice != "") out = 1;
    for(int i = 0; i < choice.size(); ++i)
        choice[i] = tolower(choice[i]);
    string output_before_user_request = file.substr(file.rfind("/") + 1);
    output_before_user_request.erase(output_before_user_request.size() - 4);
    output_before_user_request += "/> ";
    if(folder != "") {
        output_before_user_request = folder.substr(7, folder.size() - 8) + " -> " + output_before_user_request;
        output_before_user_request[0] = toupper(output_before_user_request[0]);
    }
    pathToDemonstration(output_before_user_request);
    if(output_before_user_request == "all/> ") output_before_user_request = "ALL/> ";
    bool needInstructions = 1;
    string command = "";
    while(1) {
        if(choice.substr(0, 6) == "create") { // создает папку
            if(choice == "create") {
                command = choice;
                goto thatsall;
            }
            needInstructions = 0;
            string newFolderName = choice.substr(6);
            getRidOfSpaces(newFolderName);
            if(newFolderName == "") goto thatsall;
            string folderName = newFolderName;
            demonstrationToPath(newFolderName);
            newFolderName = "Dictionaries/papka__" + newFolderName;
            if(folderInfo.FolderExists(newFolderName)) {
                if(!out) {
                    cout << '\a';
                    setColor(RED);
                    cout << "This folder already exists\n\n";
                    setColor(WHITE);
                }
                goto thatsall;
            }
            mkdir(newFolderName.c_str());
            newFolderName += "/all.txt";
            ofstream f(newFolderName.c_str());
            f.close();
            folderInfo.AddFolder(folderName);
            if(!out) {
                cout << "Folder ";
                setColor(GOLD);
                cout << folderName;
                setColor(WHITE);
                cout << " was created\n\n";
            }
        }
        else if(choice.substr(0, 6) == "delete") { // удаляет папку (словари в ней перемещаются в общую область видимости)
            if(choice == "delete") {
                command = choice;
                goto thatsall;
            }
            needInstructions = 0;
            bool deleteFolderIAmInRightNow;
            string folderToDelete = choice.substr(6);
            getRidOfSpaces(folderToDelete);
            if(folderToDelete == "") goto thatsall;
            if(folderToDelete == "this") {
                if(folder == "") goto thatsall;
                folderToDelete = folder.substr(7, folder.size() - 8);
                pathToDemonstration(folderToDelete);
            }
            string folderName = folderToDelete;
            demonstrationToPath(folderToDelete);
            if(folder != "" && folder.substr(7, folder.size() - 8) == folderToDelete) deleteFolderIAmInRightNow = 1;
            else deleteFolderIAmInRightNow = 0;
            folderToDelete = "Dictionaries/papka__" + folderToDelete;
            if(!folderInfo.FolderExists(folderToDelete)) {
                if(!out) {
                    setColor(RED);
                    cout << "No such folder\n\n";
                    setColor(WHITE);
                }
                goto thatsall;
            }
            vector<string> filesLeft = folderInfo.GetFilesFromFolder(folderToDelete);
            folderName[0] = toupper(folderName[0]);
            if(out) {
                system("cls");
                cout << "\n\t";
            }
            cout << "Действительно удалить папку ";
            setColor(GOLD);
            cout << folderName;
            setColor(WHITE);
            cout << " ?";
            setColor(SEA_WAVE);
            cout << "  [Y/n]\n";
            setColor(WHITE);
            if(filesLeft.size() > 1) {
                if(out) cout << "\n";
                cout << "\n";
                if(out) cout << "\t";
                else cout << "  ";
                cout << "В общую область видимости попад";
                if(filesLeft.size() == 2) cout << "ет словарь:";
                else cout << "ут словари:";
                if(out) cout << "\n";
                for(int i = 0; i < filesLeft.size(); ++i) {
                    if(filesLeft[i] != "all") {
                        setColor(RED);
                        cout << "\n";
                        if(out) cout << "\t";
                        else cout << "    ";
                        cout << (char) 26 << ' ';
                        setColor(GRAY);
                        cout << filesLeft[i];
                    }
                }
                setColor(WHITE);
                cout << "\n";
            }
            if(!YesOrNo()) {
                if(!out) cout << "\n";
                goto thatsall;
            }
            if(folderInfo.MovieIsExcluded(file)) addToFile(*this, 0);
            else addToFile(*this, 1);
            for(int i = 0; i < filesLeft.size(); ++i) {
                demonstrationToPath(filesLeft[i]);
                string source = folderToDelete + "/" + filesLeft[i] + ".txt";
                if(filesLeft[i] == "all") {
                    remove(source.c_str());
                    continue;
                }
                string destination = "Dictionaries/" + filesLeft[i] + ".txt";
                if(folderInfo.MovieIsExcluded(source)) {
                    folderInfo.RemoveFromExcluded(source);
                    folderInfo.AddToExcluded(destination);
                }
                rename(source.c_str(), destination.c_str());
                folderInfo.AddFile(filesLeft[i]);
            }
            RemoveDirectory(folderToDelete.c_str());
            folderInfo.DeleteFolder(folderToDelete);
            if(deleteFolderIAmInRightNow) {
                if(file.substr(file.rfind("/") + 1) == "all.txt") {
                    file = "";
                    out = 1;
                }
                else {
                    file.erase(13, folder.size());
                    folder = "";
                    output_before_user_request.erase(0, output_before_user_request.find(" -> ") + 4);
                }
            }
            if(!out) {
                cout << "\nFolder ";
                setColor(GOLD);
                cout << folderName;
                setColor(WHITE);
                cout << " was successfully deleted\n\n";
            }
        }
        else if(choice.substr(0, 6) == "add to") { // добавляет текущий словарь во введенную папку
            if(choice == "add to") {
                command = choice;
                goto thatsall;
            }
            needInstructions = 0;
            if(folder != "" || file == "Dictionaries/all.txt") {
                if(!out) {
                    cout << '\a';
                    setColor(RED);
                    if(folder != "") cout << "This dictionary is already in some folder\n\n";
                    else cout << "All dictionary can't be moved anywhere\n\n";
                    setColor(WHITE);
                }
                goto thatsall;
            }
            string folderName = choice.substr(6);
            getRidOfSpaces(folderName);
            if(folderName == "") goto thatsall;
            demonstrationToPath(folderName);
            folderName = "Dictionaries/papka__" + folderName;
            if(!folderInfo.FolderExists(folderName)) {
                if(!out) {
                    setColor(RED);
                    cout << "No such folder\n\n";
                    setColor(WHITE);
                }
                goto thatsall;
            }
            folderInfo.DeleteFile(file);
            folderName += file.substr(file.rfind("/")); // "Dictionaries/papka__test/film.txt"
            rename(file.c_str(), folderName.c_str());
            if(folderInfo.MovieIsExcluded(file)) {
                folderInfo.RemoveFromExcluded(file);
                folderInfo.AddToExcluded(folderName);
            }
            file = folderName;
            folderName = folderName.substr(13); // "papka__test/<file>.txt"
            folderName.erase(folderName.size() - 4); // "papka__test/<file>"
            Dictionary *temp = new Dictionary(folderName);
            folder = temp->folder;
            innerAll_file = temp->innerAll_file;
            innerAll_dict = temp->innerAll_dict;
            if(folderInfo.MovieIsExcluded(file)) addToFile(*temp, 0);
            else addToFile(*temp, 1);
            delete temp;
            folderInfo.AddFile(file.substr(file.rfind("/") + 1), folder);
            if(!out) {
                output_before_user_request = folder.substr(7, folder.size() - 8) + " -> " + output_before_user_request;
                output_before_user_request[0] = toupper(output_before_user_request[0]);
                pathToDemonstration(output_before_user_request);
                cout << "Dictionary ";
                setColor(GOLD);
                string file_demo = folderName.substr(folderName.rfind("/") + 1);
                pathToDemonstration(file_demo);
                cout << file_demo;
                setColor(WHITE);
                cout << " was added to ";
                setColor(GOLD);
                folderName = folderName.substr(7, folderName.find("/") - 7);
                folderName[0] = toupper(folderName[0]);
                pathToDemonstration(folderName);
                cout << folderName;
                setColor(WHITE);
                cout << " folder\n\n";
            }
        }
        else if(choice.substr(0, 6) == "remove") { // перемещаем текущий словарь из папки в общую область видимости
            needInstructions = 0;
            if(folder == "" || file.substr(13) == folder + "all.txt") {
                if(!out) {
                    cout << '\a';
                    setColor(RED);
                    if(folder == "") cout << "This dictionary is not in any folder\n\n";
                    else cout << "Local \"all\" dictionary can't be removed\n\n";
                    setColor(WHITE);
                }
                goto thatsall;
            }
            folderInfo.DeleteFile(file);
            if(folderInfo.MovieIsExcluded(file)) addToFile(*this, 0);
            else addToFile(*this, 1);
            WriteInDic(innerAll_file, innerAll_dict);
            innerAll_dict.clear();
            innerAll_file = "";
            folder = "";
            string newDest = "Dictionaries/" + file.substr(file.rfind("/") + 1);
            rename(file.c_str(), newDest.c_str());
            if(folderInfo.MovieIsExcluded(file)) {
                folderInfo.RemoveFromExcluded(file);
                folderInfo.AddToExcluded(newDest);
            }
            file = newDest;
            folderInfo.AddFile(file.substr(file.rfind("/") + 1));
            output_before_user_request.erase(0, output_before_user_request.find(" -> ") + 4);
        }
        else if(choice.substr(0, 4) == "show") {
            needInstructions = 0;
            string folderName = choice.substr(4);
            getRidOfSpaces(folderName);
            if(folder != "") {
                if(folderInfo.MovieIsExcluded(file)) addToFile(*this, 0);
                else addToFile(*this, 1);
            }
            if(folderName == "") {
                vector<string> existingFolders = folderInfo.GetAllFolders();
                if(!existingFolders.size()) {
                    if(!out) {
                        setColor(RED);
                        cout << "There are no folders yet\n\n";
                        setColor(WHITE);
                    }
                    goto thatsall;
                }
                int star = -1; // индекс папки, которая открыта в данный момент
                pathToDemonstration(folder);
                for(int i = 0; i < existingFolders.size(); ++i) {
                    if("papka  " + existingFolders[i] + "/" == folder) {
                        star = i;
                        break;
                    }
                }
                demonstrationToPath(folder);
                if(out) cout << "\t";
                else cout << "\n    ";
                cout << "Existing folders:";
                if(out) cout << "\n";
                setColor(GOLD);
                for(int i = 0; i < existingFolders.size(); ++i) {
                    if(out) cout << "\n\t";
                    else cout << "\n    ";
                    if(i == star) {
                        setColor(LIGHT_RED);
                        cout << (char) 16 << " ";
                        setColor(GOLD);
                    }
                    else cout << "  ";
                    existingFolders[i][0] = toupper(existingFolders[i][0]);
                    cout << existingFolders[i];
                }
                cout << "\n\n";
                setColor(WHITE);
                if(out) _getch();
                goto thatsall;
            }
            demonstrationToPath(folderName);
            string title = folderName;
            if(title == "this" && folder != "") title = folder.substr(7, folder.size() - 8);
            title[0] = toupper(title[0]);
            pathToDemonstration(title);
            if(folderName == "this" && folder != "") folderName = "Dictionaries/" + folder.substr(0, folder.size() - 1);
            else folderName = "Dictionaries/papka__" + folderName;
            if(!folderInfo.FolderExists(folderName)) {
                if(!out) {
                    setColor(RED);
                    cout << "No such folder\n\n";
                    setColor(WHITE);
                }
                goto thatsall;
            }
            if(out) system("cls");
            setColor(GOLD);
            cout << "\n";
            if(out) cout << "\t";
            else cout << "    ";
            cout << title << "\n";
            vector<string> filesInFolder = folderInfo.GetFilesFromFolder(folderName);
            vector<int>    theirNameSizes;
            vector<int>    dictSizes;
            vector<int>    dictSizesNumLength; // количества цифр в числе размера словаря (напр. 375 -> 3)
            for(const auto& fileName : filesInFolder) {
                string fileName_path = fileName + ".txt";
                demonstrationToPath(fileName_path);
                int ds = getDictionarySize(folderName + "/" + fileName_path);
                if(ds >= 10000) dictSizesNumLength.push_back(5);
                else if(ds >= 1000) dictSizesNumLength.push_back(4);
                else if(ds >= 100) dictSizesNumLength.push_back(3);
                else if(ds >= 10) dictSizesNumLength.push_back(2);
                else dictSizesNumLength.push_back(1);
                dictSizes.push_back(ds);
                theirNameSizes.push_back(fileName.size());
            }
            int distance = *max_element(theirNameSizes.begin(), theirNameSizes.end()) + 3;
            int maxNumberSize = *max_element(dictSizesNumLength.begin(), dictSizesNumLength.end());
            for(int i = 0; i < theirNameSizes.size(); ++i)
                theirNameSizes[i] -= maxNumberSize - dictSizesNumLength[i];
            for(int i = 0; i < filesInFolder.size(); ++i) {
                setColor(RED);
                cout << "\n";
                if(out) cout << "\t";
                else cout << "    ";
                cout << (char) -92 << " ";
                setColor(WHITE);
                cout << filesInFolder[i];
                setColor(GRAY);
                cout << " " << string(distance - theirNameSizes[i], '.') << " " << dictSizes[i];
            }
            setColor(WHITE);
            if(out) _getch();
            if(!out) cout << "\n\n";
        }
        else if(choice.substr(0, 5) == "renew") {
            needInstructions = 0;
            if(folder == "") {
                if(out) return;
                else {
                    cout << '\a';
                    setColor(RED);
                    cout << "This kind of operation must be performed from a folder\n\n";
                    setColor(WHITE);
                }
                goto thatsall;
            }
            if(folderInfo.MovieIsExcluded(file)) addToFile(*this, 0);
            else addToFile(*this, 1);
            map<string, vector<string>> newLocalAll;
            vector<string> filesInFolder = folderInfo.GetFilesFromFolder(folder);
            COORD currentPos = getPos();
            currentPos.Y--;
            currentPos.X = output_before_user_request.size();
            if(!out) setColor(LIGHT_RED);
            bool R = 1;
            for(auto fileName : filesInFolder) {
                if(fileName == "all") continue;
                fileName = folderInfo.TransformMovieTitleIntoPath(fileName, 0);
                ReadDic("Dictionaries/" + folder + fileName, newLocalAll, 0);
                if(!out) {
                    setPosition(currentPos);
                    if(R) cout << "RENEW";
                    else cout << "renew";
                    R ^= 1;
                }
            }
            if(!out && !R) {
                setPosition(currentPos);
                cout << "renew";
            }
            cout << "\n";
            WriteInDic(innerAll_file, newLocalAll);
            if(file.substr(file.rfind("/") + 1) == "all.txt") dict = newLocalAll;
            else {
                innerAll_dict = newLocalAll;
                for(auto it = dict.begin(); it != dict.end(); ++it) {
                    auto del = innerAll_dict.find(it->first);
                    if(del != innerAll_dict.end()) innerAll_dict.erase(del);
                }
            }
        }
        else if(choice == "cls") {
            needInstructions = 0;
            system("cls");
            cout << "\n";
        }
        else if(choice == "exit") out = 1;
        thatsall:
        if(out) break;
        if(needInstructions && choice != "") FoldersHandlerInstructions(command);
        setColor(GRAY);
        cout << output_before_user_request;
        setColor(WHITE);
        needInstructions = 1;
        command = "";
        getline(cin, choice);
        getRidOfSpaces(choice);
        for(int i = 0; i < choice.size(); ++i)
        choice[i] = tolower(choice[i]);
    }
}

void Dictionary::FoldersHandlerInstructions(string command)
{
    string output;
    int width;
    HANDLE hConsole;
    if(hConsole = GetStdHandle(-12)) {
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if(GetConsoleScreenBufferInfo(hConsole, &consoleInfo))
            width = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
        else exit(-1);
    }
    else exit(-1);
    width -= 12;
    Colors instructionColor = GOLD,
           descriptionColor = GRAY,
           folderNameColor = CYAN;

    if(command == "create") {
        setColor(instructionColor);
        cout << "\n    create ";
        setColor(folderNameColor);
        cout << "<folder name>";
        setColor(descriptionColor);
        cout << ModifyString(" - создает новую папку", width, 19) << "\n\n";
    }
    else if(command == "delete") {
        setColor(instructionColor);
        cout << "\n    delete ";
        setColor(folderNameColor);
        cout << "<folder name>";
        setColor(descriptionColor);
        cout << ModifyString(" - удаляет существующую папку и перемещает все находящиеся в ней словари в общую область видимости", width, 19) << "\n\n";
    }
    else if(command == "add to") {
        setColor(instructionColor);
        cout << "\n    add to ";
        setColor(folderNameColor);
        cout << "<folder name>";
        setColor(descriptionColor);
        cout << ModifyString(" - добавляет текущий словарь во введенную папку", width, 19) << "\n\n";
    }
    else {
        setColor(instructionColor);
        cout << "\n    create ";
        setColor(folderNameColor);
        cout << "<folder name>";
        setColor(descriptionColor);
        cout << ModifyString(" - создать новую папку", width, 19);

        setColor(instructionColor);
        cout << "\n    delete ";
        setColor(folderNameColor);
        cout << "<folder name>";
        setColor(descriptionColor);
        cout << ModifyString(" - удалить существующую папку с перемещением всех находящихся в ней словарей в общую область видимости", width, 19);

        setColor(instructionColor);
        cout << "\n    add to ";
        setColor(folderNameColor);
        cout << "<folder name>";
        setColor(descriptionColor);
        cout << ModifyString(" - добавить текущий словарь во введенную папку", width, 19);

        setColor(instructionColor);
        cout << "\n    remove";
        setColor(descriptionColor);
        cout << ModifyString(" - переместить текущий словарь из папки в общую область видимости", width, 5);

        setColor(instructionColor);
        cout << "\n    show";
        setColor(descriptionColor);
        cout << ModifyString(" - показать существующие папки с указанием в какой папке находится текущий словарь (если он не в общей области видимости)", width, 3);

        setColor(instructionColor);
        cout << "\n    show ";
        setColor(folderNameColor);
        cout << "<folder name>";
        setColor(descriptionColor);
        cout << ModifyString(" - показать все словари введенной папки с указанием размера каждого словаря", width, 17);

        setColor(instructionColor);
        cout << "\n    renew";
        setColor(descriptionColor);
        cout << ModifyString(" - обновить общий словарь текущей папки", width, 4);

        setColor(instructionColor);
        cout << "\n    cls";
        setColor(descriptionColor);
        cout << ModifyString(" - очистить экран", width, 2);

        setColor(instructionColor);
        cout << "\n    exit";
        setColor(descriptionColor);
        cout << ModifyString(" - вернуться в главное меню", width, 3);
        cout << "\n\n";

        setColor(WHITE);
    }
}

void Dictionary::ReadDic(string path, map<string, vector<string>>& dic, bool clearItFirst)
{
    if(clearItFirst) dic.clear();
    ifstream fin(path);
    string word, meaning, temp;
    if(!fin.is_open()) {
        perror(("\ndictionary.cc -> ReadDic() -> строка " + to_string(__LINE__)).c_str());
        cout << "Path: " << path << "\n";
        exit(-1);
    }
    while(1) {
        if(fin.eof()) {
            if(word.size() && dic[word].size() == 0 && meaning == "")
                dic[word].push_back(" ");
            else if(word.size() && meaning.size())
                dic[word].push_back(meaning);
            break;
        }
        else fin >> temp;

        if(temp[0] == '!') {
            if(word.size()) {
                vector<string> v = dic[word];
                if(!v.size() && meaning == "") dic[word].push_back(" ");
                else if(find(v.begin(), v.end(), meaning) == v.end())
                    dic[word].push_back(meaning);
                meaning = "";
            }
            dic[temp.erase(0, 1)];
            word = temp;
        }
        else if(temp[0] == '#') {
            vector<string> v = dic[word];
            if(find(v.begin(), v.end(), meaning) == v.end())
                dic[word].push_back(meaning);
            meaning = "";
        }
        else meaning += " " + temp;
    }
    fin.close();
    for(auto it = dic.begin(); it != dic.end(); ++it) {
        if(it->second.size() && it->second.back() == "") it->second.pop_back();
        if(!it->second.size()) it->second.push_back(" ");
    }
    if(dic.find("") != dic.end()) dic.erase("");
}

void Dictionary::WriteInDic(string path, const map<string, vector<string>>& dic)
{
    ofstream fout(path.c_str());
    if(!fout.is_open()) {
        perror(("\ndictionary.cc -> строка " + to_string(__LINE__)).c_str());
        cout << "Имя файла: \"" << path << "\"\n";
        exit(-1);
    }
    string temp = "";
    for(auto it = dic.begin(); it != dic.end(); ++it) {
        temp = "!" + it->first;
        fout << temp << " ";
        for(const auto &x : it->second) {
            temp = x;
            temp.erase(0, 1);
            fout << temp << " # ";
        }
    }
    fout.close();
}

void Dictionary::FileUpDown(char whereTo)
{
    if(folder == "") return;
    vector<string> filesInFolder = folderInfo.GetFilesFromFolder(folder);
    if(filesInFolder.size() == 1) return;
    int index = -1;
    string movieTitle = file.substr(file.rfind("/") + 1);
    movieTitle.erase(movieTitle.size() - 4);
    pathToDemonstration(movieTitle);
    for(int i = 0; i < filesInFolder.size(); ++i) {
        if(filesInFolder[i] == movieTitle) {
            index = i;
            break;
        }
    }
    if(whereTo == '<') {
        if(index) index--;
        else index = filesInFolder.size() - 1;
    }
    else {
        if(index == filesInFolder.size() - 1) index = 0;
        else index++;
    }
    if(folderInfo.MovieIsExcluded(file)) addToFile(*this, 0);
    else addToFile(*this, 1);
    string newFilePath = filesInFolder[index] + ".txt";
    demonstrationToPath(newFilePath);
    file = "Dictionaries/" + folder + newFilePath;
    ReadDic(file, dict);
    ReadDic("Dictionaries/all.txt", all_dict);
    ReadDic("Dictionaries/" + folder + "all.txt", innerAll_dict);
    for(auto it = dict.begin(); it != dict.end(); ++it) {
        auto del = all_dict.find(it->first);
        if(del != all_dict.end()) all_dict.erase(del);
        del = innerAll_dict.find(it->first);
        if(del != innerAll_dict.end()) innerAll_dict.erase(del);
    }
    while(last_word.size()) last_word.pop();
}

void Dictionary::ChangeDictionaryName()
{
    if(file.substr(file.size() - 7) == "all.txt") {
        Beep(80, 400);
        return;
    }
    string newName;
    if(cin.peek() != '\n') {
        getline(cin, newName);
        getRidOfSpaces(newName);
        if(newName == "") {
            system("cls");
            cout << "\n\tНовое имя словаря: ";
            cin.sync();
            getline(cin, newName);
            getRidOfSpaces(newName);
        }
    }
    else {
        system("cls");
        cout << "\n\tНовое имя словаря: ";
        cin.sync();
        getline(cin, newName);
        getRidOfSpaces(newName);
    }
    if(newName == "") return;
    vector<string> filesInCurrentFolder = folderInfo.GetFilesFromFolder(folder);
    if(find(filesInCurrentFolder.begin(), filesInCurrentFolder.end(), newName) != filesInCurrentFolder.end()) {
        cout << '\a';
        return;
    }
    folderInfo.DeleteFile(file);
    folderInfo.AddFile(newName, folder);
    demonstrationToPath(newName);
    newName = "Dictionaries/" + folder + newName + ".txt";
    if(newName != file) {
        if(folderInfo.MovieIsExcluded(file)) {
            folderInfo.RemoveFromExcluded(file);
            folderInfo.AddToExcluded(newName);
        }
        rename(file.c_str(), newName.c_str());
        file = newName;
    }
}

void Dictionary::ShowLastAddedWord()
{
    system("cls");
    string last = (last_word.empty()) ? "" : last_word.top();
    if(last == "") {
        if(!dict.size()) cout << "\n\tNo words added yet";
        else cout << "\n\tNo words added recently";
    }
    else {
        if(dict[last].size() == 1 && dict[last][0] == " ") PrintAWord(last, 0, "", SEA_WAVE);
        else PrintAWord(last, 0, "", GREEN);
    }
    _getch();
}

bool Dictionary::YesOrNo()
{
    cin.sync();
    char choice;
    while("decide yes or no") {
        choice = _getch();
        if(choice == 'y' || choice == 'Y' || choice == -83 || choice == -115) return 1;
        if(choice == 'n' || choice == 'N' || choice == -30 || choice == -110) return 0;
    }
}
