#include "dictionary.h"
#include <windows.h>
#include <conio.h>
#include <dirent.h>
using namespace std;
extern set<string> excluded;

Dictionary::Dictionary(string file_name)
{
    file = "Dictionaries/" + file_name + ".txt";
    folder = (file_name.find("/") == string::npos) ? "" : file_name.substr(0, file_name.rfind("/") + 1);
    innerAll_file = (folder == "") ? "" : "Dictionaries/" + folder + "all.txt";
    ofstream f(file, ios::app);
    f.close();

    readDic(file, dict);
    if(file == "Dictionaries/all.txt") return;
    readDic("Dictionaries/all.txt", all_dict);
    for(auto it = dict.begin(); it != dict.end(); ++it) {
        auto del = all_dict.find(it->first);
        if(del != all_dict.end()) all_dict.erase(del);
    }
    if(innerAll_file == "") return;
    readDic(innerAll_file, innerAll_dict);
    for(auto it = dict.begin(); it != dict.end(); ++it) {
        auto del = innerAll_dict.find(it->first);
        if(del != innerAll_dict.end()) innerAll_dict.erase(del);
    }
}

pair<char, char> Dictionary::enterWord(string& meaning, bool isAdding)
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

void Dictionary::print()
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
            printAWord(word, 0, "", 11);
        }
        else {
            printAWord(word, 0, "", 10);
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
    setColor(0);
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
    setColor(15);
}

void Dictionary::printOnlyEnglish()
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
        setColor(0);
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
        setColor(15);
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

string Dictionary::modifyString(string s, int width, int indent) // indent - расстояние от начала слова до
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

void Dictionary::printAWord(string &word, int spaces_amount, string toUpper, int color, bool from_all_dict)
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
    if(temp[word].size() == 1 && temp[word][0] == " ") s = modifyString(s, width, spaces_amount);
    if(toUpper.size()) {
        setColor(color);
        string normal_color = s.substr(0, s.find(toUpper));
        s.erase(0, s.find(toUpper));
        cout << "\n\t" << spaces << normal_color;
        setColor(6);
        string highlight = s.substr(0, toUpper.size());
        s.erase(0, toUpper.size());
        cout << highlight;
        setColor(color);
        cout << s;
        if(!(temp[word].size() == 1 && temp[word][0] == " ")) cout << " --";
        setColor(15);
    }
    else {
        setColor(color);
        cout << "\n\t" << spaces << s;
        setColor(15);
        if(!(temp[word].size() == 1 && temp[word][0] == " ")) cout << " --";
    }
    string whitespaces(word.size() + 4 + spaces_amount, ' ');
    int num = 1;
    if(temp[word].size() == 1 && temp[word][0] != " ") {
        cout << modifyString(temp[word][0], width, word.size() + 4 + spaces_amount);
    }
    else if(temp[word].size() > 1) {
        cout << " 1)" << modifyString(temp[word][0], width, word.size() + 7 + spaces_amount);
        while(num < temp[word].size()) {
            cout << "\n\t" << whitespaces << num+1 << ')';
            cout << modifyString(temp[word][num++], width, word.size() + 7 + spaces_amount);
        }
    }
}

void Dictionary::add()
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
    while(word[0] == ' ') word.erase(0, 1);
    while(word.back() == ' ') word.pop_back();
    if(word == "") return;
    if(dict.find(word) != dict.end()) {
        system("cls");
        cout << "\n\tДанное слово уже есть в этом словаре:\n";
        printAWord(word, 0, "", 10);
        cout << "\n\n\n\tДобавить новые значения?  ";
        setColor(11); cout << "[Y/n]"; setColor(15);
        vector<string> startingMeaning;
        if(cin.peek() == '\n') goto only_yes_or_no;
        cin.ignore();
        if(cin.peek() != 170 && cin.peek() != 114 && cin.peek() != 82 && cin.peek() != 138) // к, r, R, К соотв.
            do {
                pair<char, char> p = enterWord(meaning, 1);
                startingMeaning.push_back(meaning);
                if(p.first == 'r') break;
            }while(1);
        only_yes_or_no:
        cin.sync();
        c[0] = _getch();
        OemToCharA(c, c);
        c[0] = tolower(c[0]);
        if(c[0] == 110 || c[0] == -14) return; // (n || т)
        else if(c[0] != 121 && c[0] != -19) goto only_yes_or_no; // (!y && !н)
        if(startingMeaning.size()) dict[word] = startingMeaning;
        addSomeMeanings(word);
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
            printAWord(word, 0, "", 10, 1);
            cout << "\n\n\n\tДобавить его сюда тоже                     "; setColor(11); cout << "[y]"; setColor(15);
            cout <<     "\n\tПосмотреть в каких словарях оно находится  "; setColor(11); cout << "[t]"; setColor(15);
            cout <<     "\n\tОставить как есть                          "; setColor(11); cout << "[n]"; setColor(15);
            only_yes_or_no3:
            c[0] = _getch();
            OemToCharA(c, c);
            c[0] = tolower(c[0]);
            if(c[0] == 110 || c[0] == -14) return; // n || т
            if(c[0] == 116 || c[0] == -27) { // t || е
                int res = determine(word);
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
                pair<char, char> p = enterWord(meaning, 1);
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
        printAWord(word, 0, "", 10, 1);
        cout << "\n\n\n\tДобавить его сюда тоже                     "; setColor(11); cout << "[y]"; setColor(15);
        if(startingMeaning != all_dict[word]) {
            cout << "\n\tДобавить с текущими значениями             "; setColor(11); cout << "[s]"; setColor(15);
        }
        cout <<     "\n\tПосмотреть в каких словарях оно находится  "; setColor(11); cout << "[t]"; setColor(15);
        cout <<     "\n\tОставить как есть                          "; setColor(11); cout << "[n]"; setColor(15);
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
            int res = determine(word);
            if(!res) return;
            else if(res == 1) goto again2;
        }
        else if(c[0] != 121 && c[0] != -19) goto only_yes_or_no2; // (!y && !н)

        addHereAlso:
        vector<string> oldMeaning = dict[word];
        dict[word] = startingMeaning;
        addSomeMeanings(word);
        if(startingMeaning != oldMeaning) {
            // изменяем значения этого слова во всех словарях
            DIR *dir;
            dirent *entry;
            dir = opendir("Dictionaries");
            if(!dir) {
                string endProgram = "\ndictionary.cpp -> строка " + to_string(__LINE__);
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
        pair<char, char> p = enterWord(meaning, 1);
        dict[word].push_back(meaning);
        if(p.first == 'r') return;
        if(p.first == 'b') {
            auto del = dict.find(word);
            if(del != dict.end()) dict.erase(del);
            return;
        }
    }
}

void Dictionary::addSomeMeanings(string s = "")
{
    system("cls");
    string word, temp, meaning;
    char choice, c[200];
    bool wordIsLocal = 1;
    if(!dict.size()) {
        cout << "\n\tNo words yet";
        _getch();
        return;
    }
    if(s == "") {
        cin.getline(c, 200);
        OemToCharA(c, c);
        word = c;
        getRidOfSpaces(word);
        if(!word.size()) {
            cout << "\n\tWord to change: ";
            cin.sync();
            cin.getline(c, 200);
            OemToCharA(c, c);
            word = c;
            getRidOfSpaces(word);
        }
        if(word == "") return;
        if(word.size() > 1 && word[0] == '/') word.erase(0, 1);
        else if(word == "ю" || word == ".") {
            if(last_word.size()) word = last_word.top();
            else if(dict.size() == 1) word = dict.begin()->first;
            else {
                if(!dict.size()) cout << "\n\tNo words added yet";
                else cout << "\n\tNo words added recently";
                c[0] = _getch();
                return;
            }
        }
    }
    else word = s;
    for(int i = 0; i < word.size(); ++i)
        if(word[i] == ' ') word[i] = '_';
    if(s != "" && dict.find(word) == dict.end()) { // если ты сюда пришел из determine() -> Wanna change? - yes -> и слова нет в текущем словаре
        wordIsLocal = 0;
        dict[word] = all_dict[word];
    }
    system("cls");
    if(dict.find(word) == dict.end()) {
        if(file == "Dictionaries/all.txt") {
            string endProgram = "\ndictionary.cpp -> строка " + to_string(__LINE__) +
            "\nПоздравляю, этого не должно было произойти! Удачи в поиске ошибки :D";
            perror(endProgram.c_str());
            exit(-1);
        }
        cout << "\n\tNo such word. Wanna add?";
        string choice;
        cin.sync();
        getline(cin, choice);
        if(choice == "d" || choice == "ў") {
            dict[word].push_back(" ");
            addSomeMeanings(word);
        }
    }
    else {
        vector<string> oldMeaning = dict[word];
        while(1) {
            system("cls");
            printAWord(word);
            cout << "\n\n\tWrite a meaning: ";
            pair<char, char> p = enterWord(meaning);
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
                    dict[meaning] = dict[word];
                    dict.erase(dict.find(word));
                    word = meaning;
                    if(last_word.size()) last_word.pop();
                    last_word.push(word);
                }
            }
            else if(p.first != 'b') {
                if(dict[word].size() == 1 && dict[word][0] == " ") dict[word][0] = meaning;
                else dict[word].push_back(meaning);
            }
        }
        if(oldMeaning == dict[word]) return;
        DIR *dir;
        dirent *entry;
        dir = opendir("Dictionaries");
        if(!dir) {
            string endProgram = "\ndictionary.cpp -> строка " + to_string(__LINE__);
            perror(endProgram.c_str());
            exit(-1);
        }
        string path;
        while((entry = readdir(dir)) != nullptr) {
            path = entry->d_name;
            if(path == "." || path == ".." || path == "all.txt" ||
               "Dictionaries/" + path == file || path == "__excluded__.txt") continue;
            if(path.substr(path.size() - 4) != ".txt") {
                DIR *innerDir = opendir(("Dictionaries/" + path).c_str());
                if(!innerDir) {
                    string endProgram = "\ndictionary.cpp -> строка " + to_string(__LINE__);
                    perror(endProgram.c_str());
                    exit(-1);
                }
                dirent *innerEntry;
                string innerPath;
                while((innerEntry = readdir(innerDir)) != nullptr) {
                    innerPath = innerEntry->d_name;
                    if(innerPath == "." || innerPath == ".." || innerPath == "all.txt" || innerPath.substr(innerPath.size() - 4) != ".txt")
                        continue;
                    innerPath.erase(innerPath.size() - 4);
                    Dictionary *innerDic = new Dictionary(path + "/" + innerPath);
                    if(innerDic->dict.find(word) != innerDic->dict.end()) {
                        innerDic->dict[word] = dict[word];
                        addToFile(*innerDic, 0);
                    }
                    delete innerDic;
                }
                continue;
            }
            path.erase(path.size() - 4);
            Dictionary *temp = new Dictionary(path);
            if(temp->dict.find(word) != temp->dict.end()) {
                temp->dict[word] = dict[word];
                addToFile(*temp, 0);
            }
            delete temp;
        }
        if(!wordIsLocal) dict.erase(dict.find(word));
    }
}

void Dictionary::findAWord(bool findAmongAll)
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
    system("cls");
    int height, width;
    HANDLE hWndConsole;
    if(hWndConsole = GetStdHandle(-12)) {
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if(GetConsoleScreenBufferInfo(hWndConsole, &consoleInfo)) {
            height = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;
            width = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
        }
        else exit(-2);
    }
    else exit(-2);
    width -= 16;
    bool up = 1;
    COORD coord;
    int downY, leftX;
    if(!dictToSearchIn.size()) {
        cout << "\n\tNo words yet";
        c[0] = _getch();
        return;
    }
    int wordsAmountUpper, wordsAmountLower;
    string wordUpper, wordLower;
    if(word.size()) goto firstTime;
    if(findAmongAll || file == "Dictionaries/all.txt") setColor(5);
    else setColor(7);
    cout << "\n\tInput: ";
    char sym;
    cin.sync();
    while(1) {
        sym = _getch();
        if(sym == 13 || sym == 27) return; // enter || escape
        else if(sym == 9) { // tab
            if(wordsAmountUpper == 1 || wordsAmountUpper && wordsAmountLower != 1) {
                int res = determine(wordUpper);
                if(res == 1) {
                    wordsAmountUpper = 0;
                    wordsAmountLower = 0;
                    goto firstTime;
                }
                else if(!res) return;
            }
            else if(wordsAmountLower) {
                int res = determine(wordLower);
                if(res == 1) {
                    wordsAmountUpper = 0;
                    wordsAmountLower = 0;
                    goto firstTime;
                }
                else if(!res) return;
            }
            else continue;
        }
        else if(sym == 8) { // backspace
            wordsAmountUpper = 0;
            wordsAmountLower = 0;
            if(word.size()) {
                word.pop_back();
                lettersEntered.pop_back();
            }
        }
        else if(sym == 127) { // ctrl + backspace
            wordsAmountUpper = 0;
            wordsAmountLower = 0;
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
                if(!SetConsoleCursorPosition(hWndConsole, coord))
                    cout << "\nError: " << GetLastError() << "\n";
            }
            else if(sym == 80) { // стрелка вниз
                if(up) {
                    up = 0;
                    coord.Y = min(downY, coord.Y + height - 1);
                }
                do coord.Y = min(downY, coord.Y + 1);
                while(getChar(leftX + 1, coord.Y + 1) == ' ' && coord.Y != downY);
                if(!SetConsoleCursorPosition(hWndConsole, coord))
                    cout << "\nError: " << GetLastError() << "\n";
            }
            else if(sym == 73 || sym == -115) { // PgUp || Ctrl + Up
                if(!up) {
                    up = 1;
                    coord.Y = max(0, coord.Y - height + 1);
                }
                coord.Y = max(0, coord.Y - height + 1);
                if(coord.Y) while(getChar(leftX + 1, coord.Y) == ' ') coord.Y += 1;
                if(!SetConsoleCursorPosition(hWndConsole, coord))
                    cout << "\nError: " << GetLastError() << "\n";
            }
            else if(sym == 81 || sym == -111) { // PgDn || Ctrl + down
                if(up) {
                    up = 0;
                    coord.Y = min(downY, coord.Y + height - 1);
                }
                coord.Y = min(downY, coord.Y + height - 1);
                if(coord.Y < downY) while(getChar(leftX + 1, coord.Y + 1) == ' ') coord.Y -= 1;
                if(!SetConsoleCursorPosition(hWndConsole, coord))
                    cout << "\nError: " << GetLastError() << "\n";
            }
            continue;
        }
        else {
            wordsAmountUpper = 0;
            wordsAmountLower = 0;
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
        if(findAmongAll || file == "Dictionaries/all.txt") setColor(5);
        else setColor(7);
        cout << "\n\tInput: ";
        string wordToShow = word;
        for(int i = 0; i < wordToShow.size(); ++i)
            if(wordToShow[i] == '_') wordToShow[i] = ' ';
        setColor(6);
        cout << wordToShow;
        setColor(15);
        if(!word.size()) continue;
        bool no_words = 1;
        cout << "\n";

        if(count(lettersEntered.begin(), lettersEntered.end(), 1) == lettersEntered.size()) { // english
            wordUpper = "";
            for(auto it = dictToSearchIn.begin(); it != dictToSearchIn.end(); ++it) {
                if(_kbhit()) continue;
                string temp = it->first;
                if(temp.find(word) == 0) {
                    printAWord(temp, 0, "", 11, findAmongAll);
                    no_words = 0;
                    wordsAmountUpper++;
                    if(wordUpper == "") wordUpper = temp;
                }
                else if(temp.find(word) < (1<<30)) no_words = 0;
            }
            if(!no_words) cout << "\n\n\t------------------------------------------\n";
            wordLower = "";
            for(auto it = dictToSearchIn.begin(); it != dictToSearchIn.end(); ++it) {
                if(_kbhit()) continue;
                string temp = it->first;
                if(temp.find(word) != string::npos && temp.find(word)) {
                    printAWord(temp, 0, word, 7, findAmongAll);
                    no_words = 0;
                    wordsAmountLower++;
                    if(wordLower == "") wordLower = temp;
                }
            }
        }
        else if(count(lettersEntered.begin(), lettersEntered.end(), 0) == lettersEntered.size()) { // russian
            wordUpper = "";
            for(auto it = dictToSearchIn.begin(); it != dictToSearchIn.end(); ++it) {
                if(_kbhit()) continue;
                vector<string> meaning = it->second;
                if(meaning.size() == 1 && meaning[0] != " ")
                    meaning[0] = modifyString(meaning[0], width, it->first.size() + 4);
                else if(meaning.size() > 1)
                    for(int i = 0; i < meaning.size(); ++i)
                        meaning[i] = modifyString(meaning[i], width, it->first.size() + 7);
                for(int i = 0; i < meaning.size(); ++i) {
                    int pos = meaning[i].find(word);
                    if(pos != string::npos && pos != 0) {
                        no_words = 0;
                        string key = it->first;
                        wordsAmountUpper++;
                        if(wordUpper == "") wordUpper = key;
                        for(int j = 0; j < key.size(); ++j)
                            if(key[j] == '_') key[j] = ' ';
                        setColor(11);
                        cout << "\n\t" << key;
                        setColor(15);
                        cout << " --";
                        setColor(7);
                        string whitespaces(it->first.size() + 12, ' ');
                        int spacesOneMeaning = it->first.size() + 12,
                            spacesFewMeanings = it->first.size() + 15;
                        if(meaning.size() == 1 && meaning[0] != " ") {
                            pos = meaning[0].find(word);
                            if(pos == string::npos) cout << meaning[0];
                            else {
                                cout << meaning[0].substr(0, pos);
                                setColor(6);
                                cout << meaning[0].substr(pos, word.size());
                                setColor(7);
                                cout << meaning[0].substr(pos + word.size());
                            }
                        }
                        else if(meaning.size() > 1) {
                            pos = meaning[0].find(word);
                            if(pos == string::npos) cout << " 1)" << meaning[0];
                            else {
                                cout << " 1)" << meaning[0].substr(0, pos);
                                setColor(6);
                                cout << meaning[0].substr(pos, word.size());
                                setColor(7);
                                cout << meaning[0].substr(pos + word.size());
                            }
                            for(int j = 1; j < meaning.size(); ++j) {
                                pos = meaning[j].find(word);
                                cout << "\n" << whitespaces << j + 1 << ")";
                                if(pos == string::npos) cout << meaning[j];
                                else {
                                    cout << meaning[j].substr(0, pos);
                                    setColor(6);
                                    cout << meaning[j].substr(pos, word.size());
                                    setColor(7);
                                    cout << meaning[j].substr(pos + word.size());
                                }
                            }
                        }
                        else cout << " ...";
                        break;
                    }
                }
            }
            setColor(15);
        }
        else {
            cout << "\n\tShould be only one language";
            continue;
        }

        if(no_words) cout << "\n\tNo words found\n\t";
        else cout << "\n\t";
        coord = getPos();
        leftX = coord.X - 1;
        downY = coord.Y;
        coord.X = 15 + word.size();
        coord.Y = 0;
        hWndConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if(!SetConsoleCursorPosition(hWndConsole, coord))
            cout << "Error: " << GetLastError() << "\n";
        coord.Y = 1;
        hWndConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if(!SetConsoleCursorPosition(hWndConsole, coord))
            cout << "Error: " << GetLastError() << "\n";
        coord.Y = 0;
    }
}

void Dictionary::getRidOf()
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
        else return;
    }
    auto it = dict.find(word);
    if(it == dict.end()) return;
    if(leaveWordInAll) all_dict[word] = dict[word];
    dict.erase(it);
}

bool Dictionary::examRightAnswer(vector<string> &answers, string answer)
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

void Dictionary::exam()
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
            printAWord(temp, spaces_amount);
            cout << "\n\n\t";
            choice = _getche();
            if(choice == 'f' || choice == ' ') right_ans++;
            continue;
        }
        else if(answer == "exit" || answer == "учше") return;
        if(examRightAnswer(it->second, answer)) {
            system("cls");
            right_ans++;
            cout << "\n\t(" << right_ans << "/" << order.size() << " правильных ответов)\n";
        }
        temp = it->first;
        if(question_number <= 10) spaces_amount = 3;
        else if(question_number <= 100) spaces_amount = 4;
        else spaces_amount = 5;
        printAWord(temp, spaces_amount);
        if(examRightAnswer(it->second, answer)) {
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
        printAWord(s);
    }
    cout << "\n\t";
    cin.get();
}

bool Dictionary::remov()
{
    string just_a_name = file;
    just_a_name.erase(0, just_a_name.find('/') + 1);
    just_a_name.erase(just_a_name.size() - 4);
    if(just_a_name.substr(0, 7) == "papka__") {
        just_a_name.erase(0, 7);
        just_a_name.replace(just_a_name.find("/"), 1, " -> ");
    }
    else just_a_name[0] = toupper(just_a_name[0]);
    pathToDemonstration(just_a_name);
    char c[2];
    system("cls");
    cout << "\n\tДействительно удалить словарь \"" << just_a_name << "\"?";
    setColor(11); cout << "\n\n\t[Y/n]"; setColor(15);
    only_yes_or_no:
    cin.sync();
    c[0] = _getch();
    OemToCharA(c, c);
    c[0] = tolower(c[0]);
    if(c[0] == 110 || c[0] == -14) return 0;  // (n || т)
    else if(c[0] != 121 && c[0] != -19) goto only_yes_or_no;
    writeInDic("Dictionaries/all.txt", all_dict);
    if(innerAll_file.size()) writeInDic(innerAll_file, innerAll_dict);
    remove(file.c_str());
    file = "";
    return 1;
}

void addToFile(Dictionary &x, bool saveInAll)
{
    if(x.file == "") return;
    x.writeInDic(x.file, x.dict);
    string xAllFile = "Dictionaries/all.txt";
    if(x.file == "Dictionaries/" + x.folder + "all.txt") return;
    if(saveInAll) {
        map<string, vector<string>> intermediate(x.all_dict);
        for(auto it = x.dict.begin(); it != x.dict.end(); ++it)
            intermediate[it->first] = it->second;
        x.writeInDic(xAllFile, intermediate);
    }
    else x.writeInDic(xAllFile, x.all_dict);
    if(x.folder != "") {
        map<string, vector<string>> intermediate(x.innerAll_dict);
        for(auto it = x.dict.begin(); it != x.dict.end(); ++it)
            intermediate[it->first] = it->second;
        x.writeInDic(x.innerAll_file, intermediate);
    }
}

void Dictionary::setColor(int color)
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD) color);
}

int Dictionary::determine(string word = "")
{
    string path;
    if(file != "Dictionary/all.txt") {
        path = file.substr(13, file.size() - 17);
        if(excluded.find(path) == excluded.end()) addToFile(*this, 1);
        else addToFile(*this, 0);
    }
    char c[200];
    bool calledFromAdding = 1;
    if(word == "") {
        system("cls");
        calledFromAdding = 0;
        cin.getline(c, 200);
        OemToCharA(c, c);
        word = c;
    }
    bool digit = 0;
    for(int i = 0; i < word.size(); ++i) {
        if(!isspace(word[i])) {
            digit = 1;
            break;
        }
    }
    if(!digit) {
        cout << "\n\tWhich word to find?\n\n\t";
        cin.sync();
        cin.getline(c, 200);
        OemToCharA(c, c);
        word = c;
    }
    getRidOfSpaces(word);
    if(word == "") return 0;
    for(int i = 0; i < word.size(); ++i)
        if(word[i] == ' ') word[i] = '_';
    DIR *dir;
    dirent *entry;
    dir = opendir("Dictionaries");
    if(!dir) {
        string endProgram = "\ndictionary.cpp -> строка " + to_string(__LINE__);
        perror(endProgram.c_str());
        exit(-1);
    }
    clock_t start = clock();
    map<string, int> found;
    while((entry = readdir(dir)) != nullptr) {
        path = entry->d_name;
        if(path == "." || path == ".." || path == "all.txt" || path == "__excluded__.txt") continue;
        if(path.substr(path.size() - 4) != ".txt") {
            DIR *innerDir = opendir(("Dictionaries/" + path).c_str());
            if(!dir) {
                string endProgram = "\ndictionary.cpp -> строка " + to_string(__LINE__);
                perror(endProgram.c_str());
                exit(-1);
            }
            string rawPath = path;
            path.erase(0, 7);
            pathToDemonstration(path);
            path[0] = toupper(path[0]);
            dirent *innerEntry;
            string innerPath;
            while((innerEntry = readdir(innerDir)) != nullptr) {
                innerPath = innerEntry->d_name;
                if(innerPath == "." || innerPath == ".." || innerPath == "all.txt" || innerPath.substr(innerPath.size() - 4) != ".txt")
                    continue;
                innerPath.erase(innerPath.size() - 4);
                Dictionary *innerDic = new Dictionary(rawPath + "/" + innerPath);
                if(innerDic->dict.find(word) != innerDic->dict.end()) {
                    pathToDemonstration(innerPath);
                    found[path + " -> " + innerPath] = innerDic->getSize();
                }
                delete innerDic;
            }
            continue;
        }
        path.erase(path.size() - 4);
        Dictionary *temp = new Dictionary(path);
        if(temp->dict.find(word) != temp->dict.end()) {
            pathToDemonstration(path);
            path[0] = toupper(path[0]);
            found[path] = temp->getSize();
        }
        delete temp;
    }
    clock_t finish = clock();
    system("cls");
    if(found.size()) {
        vector<string> dots;
        if(found.size() == 1) dots.push_back(string(5, '.'));
        else {
            int maxSize = 0;
            for(auto it = found.begin(); it != found.end(); ++it)
                maxSize = max(maxSize, int(it->first.size()));
            maxSize += 5;
            for(auto it = found.begin(); it != found.end(); ++it)
                dots.push_back(string(maxSize - it->first.size(), '.'));
        }
        setColor(10);
        cout << "\n\tFound in: (for " << (double) (finish - start) / CLOCKS_PER_SEC << "s)";
        setColor(15);
        int index = 0, longestDictSize = 0; // длина числа количества слов в словаре (5 - 1, 12 - 2, 143 - 3)
        for(auto it = found.begin(); it != found.end(); ++it)
            longestDictSize = max(longestDictSize, numLength(it->second));
        for(auto it = found.begin(); it != found.end(); ++it) {
            cout << "\n\t\"" << it->first << "\" ";
            setColor(7);
            cout << dots[index++] << string(longestDictSize - numLength(it->second), '.') << " " << it->second;
            setColor(15);
        }
        cout << "\n";
        printAWord(word, 0, "", 10, 1);
        if(calledFromAdding) {
            cout << "\n\n\n\tНазад           "; setColor(11); cout << "[b]"; setColor(15);
            cout << "\n\tВ главное меню  "; setColor(11); cout << "[n]"; setColor(15);
            while(1) {
                c[0] = _getch();
                OemToCharA(c, c);
                if(c[0] == 'n' || c[0] == 'т') return 0;
                if(c[0] == 'b' || c[0] == 'и') return 1;
            }
        }
        cout << "\n\n\n\tWanna change something?  "; setColor(11); cout << "[Y/n]"; setColor(15);
        only_yes_or_no:
        cin.sync();
        c[0] = _getch();
        OemToCharA(c, c);
        c[0] = tolower(c[0]);
        if(c[0] == 110 || c[0] == -14) return 0; // (n || т)
        else if(c[0] != 121 && c[0] != -19) goto only_yes_or_no;
        addSomeMeanings(word);
    }
    else {
        cout << "\n\tNo words found\n\n\t";
        cin.sync();
        cin.get();
    }
    return 0;
}

int amountOfLines(const vector<pair<bool, int>>& opened)
{
    int res = opened.size();
    for(int i = 0; i < opened.size(); ++i) {
        if(opened[i].first)
            res += opened[i].second;
    }
    return res;
}

int cursorOnFolder(const vector<pair<bool, int>>& opened, int k)
{
    if(k == -1) return -1;
    if(!k) return 0;
    int numLine = 0;
    for(int i = 1; i < opened.size(); ++i) {
        if(opened[i - 1].first) numLine += opened[i - 1].second + 1;
        else numLine++;
        if(numLine == k) return i;
        else if(numLine > k) return -1;
    }
    return -1;
}

int getDictionarySize(const string& path)
{
    ifstream fin(path.c_str());
    if(!fin.is_open()) {
        perror(("\ndictionary.cpp -> строка " + to_string(__LINE__)).c_str());
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

void Dictionary::foldersHandler()
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
            DIR *dir = opendir(newFolderName.c_str());
            if(dir) {
                closedir(dir);
                cout << "The folder already exists\n\n";
                goto thatsall;
            }
            mkdir(newFolderName.c_str());
            newFolderName += "/all.txt";
            ofstream f(newFolderName.c_str());
            f.close();
            if(!out) {
                cout << "Folder ";
                setColor(6);
                cout << folderName;
                setColor(15);
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
            if(folder != "" && folder.substr(7, folder.size() - 8) == folderToDelete)
                deleteFolderIAmInRightNow = 1;
            else deleteFolderIAmInRightNow = 0;
            folderToDelete = "Dictionaries/papka__" + folderToDelete;
            DIR *dir = opendir(folderToDelete.c_str());
            if(!dir) {
                if(!out) {
                    setColor(4);
                    cout << "No such folder\n\n";
                    setColor(15);
                }
                goto thatsall;
            }
            dirent *entry;
            vector<string> filesLeft;
            while((entry = readdir(dir)) != nullptr) {
                string s = entry->d_name;
                if(s != "." && s != "..") filesLeft.push_back(s);
            }
            closedir(dir);
            folderName[0] = toupper(folderName[0]);
            if(out) {
                system("cls");
                cout << "\n\t";
            }
            cout << "Действительно удалить папку ";
            setColor(6); cout << folderName; setColor(15); cout << " ?";
            setColor(11); cout << "  [Y/n]\n"; setColor(15);
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
                    if(filesLeft[i] != "all.txt") {
                        string s = filesLeft[i];
                        s.erase(s.size() - 4);
                        pathToDemonstration(s);
                        setColor(4);
                        cout << "\n";
                        if(out) cout << "\t";
                        else cout << "    ";
                        cout << (char) 26 << ' ';
                        setColor(7);
                        cout << s;
                    }
                }
                setColor(15);
                cout << "\n";
            }
            only_yes_or_no:
            char c[2];
            cin.sync();
            c[0] = _getch();
            OemToCharA(c, c);
            c[0] = tolower(c[0]);
            if(c[0] == 110 || c[0] == -14) {
                if(!out) cout << "\n";
                goto thatsall;
            }
            else if(c[0] != 121 && c[0] != -19) goto only_yes_or_no;
            string path = file.substr(13, file.size() - 17);
            if(excluded.find(path) == excluded.end()) addToFile(*this, 1);
            else addToFile(*this, 0);
            for(int i = 0; i < filesLeft.size(); ++i) {
                string source = folderToDelete + "/" + filesLeft[i];
                if(filesLeft[i] == "all.txt") {
                    remove(source.c_str());
                    continue;
                }
                string destination = "Dictionaries/" + filesLeft[i];
                path = source.substr(13, source.size() - 17);
                if(excluded.find(path) != excluded.end()) {
                    excluded.erase(path);
                    excluded.insert(destination.substr(13, destination.size() - 17));
                }
                rename(source.c_str(), destination.c_str());
            }
            RemoveDirectory(folderToDelete.c_str());
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
                setColor(6);
                cout << folderName;
                setColor(15);
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
                cout << '\a';
                setColor(4);
                if(folder != "") cout << "This dictionary is already in some folder\n\n";
                else cout << "All dictionary can't be moved anywhere\n\n";
                goto thatsall;
            }
            string folderName = choice.substr(6);
            getRidOfSpaces(folderName);
            if(folderName == "") goto thatsall;
            demonstrationToPath(folderName);
            folderName = "Dictionaries/papka__" + folderName;
            DIR *dir = opendir(folderName.c_str());
            if(!dir) {
                if(!out) {
                    setColor(4);
                    cout << "No such folder\n\n";
                    setColor(15);
                }
                goto thatsall;
            }
            closedir(dir);
            folderName += file.substr(file.rfind("/")); // "Dictionaries/papka__test/film.txt"
            rename(file.c_str(), folderName.c_str());
            string path = file.substr(13, file.size() - 17);
            if(excluded.find(path) != excluded.end()) {
                excluded.erase(path);
                excluded.insert(folderName.substr(13, folderName.size() - 17));
            }
            file = folderName;
            folderName = folderName.substr(13); // "papka__test/<file>.txt"
            folderName.erase(folderName.size() - 4); // "papka__test/<file>"
            Dictionary *temp = new Dictionary(folderName);
            folder = temp->folder;
            innerAll_file = temp->innerAll_file;
            innerAll_dict = temp->innerAll_dict;
            path = file.substr(13, file.size() - 17);
            if(excluded.find(path) == excluded.end()) addToFile(*temp, 1);
            else addToFile(*temp, 0);
            delete temp;
            if(!out) {
                output_before_user_request = folder.substr(7, folder.size() - 8) + " -> " + output_before_user_request;
                output_before_user_request[0] = toupper(output_before_user_request[0]);
                pathToDemonstration(output_before_user_request);
                cout << "Dictionary ";
                setColor(6);
                string file_demo = folderName.substr(folderName.rfind("/") + 1);
                pathToDemonstration(file_demo);
                cout << file_demo;
                setColor(15);
                cout << " was added to ";
                setColor(6);
                folderName = folderName.substr(7, folderName.find("/") - 7);
                folderName[0] = toupper(folderName[0]);
                pathToDemonstration(folderName);
                cout << folderName;
                setColor(15);
                cout << " folder\n\n";
            }
        }
        else if(choice.substr(0, 6) == "remove") { // перемещаем текущий словарь из папки в общую область видимости
            needInstructions = 0;
            if(folder == "" || file.substr(13) == folder + "all.txt") {
                if(!out) {
                    setColor(4);
                    if(folder == "") cout << "This dictionary is not in any folder\n\n";
                    else cout << "Local \"all\" dictionary can't be removed\n\n";
                    setColor(15);
                }
                cout << '\a';
                goto thatsall;
            }
            string path = file.substr(13, file.size() - 17);
            if(excluded.find(path) == excluded.end()) addToFile(*this, 1);
            else addToFile(*this, 0);
            writeInDic(innerAll_file, innerAll_dict);
            innerAll_dict.clear();
            innerAll_file = "";
            folder = "";
            string newDest = "Dictionaries" + file.substr(file.rfind("/"));
            rename(file.c_str(), newDest.c_str());
            if(excluded.find(path) != excluded.end()) {
                excluded.erase(path);
                excluded.insert(newDest.substr(13, newDest.size() - 17));
            }
            file = newDest;
            output_before_user_request.erase(0, output_before_user_request.find(" -> ") + 4);
        }
        else if(choice.substr(0, 4) == "show") {
            needInstructions = 0;
            const int markeringColor = 6;
            string folderName = choice.substr(4);
            getRidOfSpaces(folderName);
            if(folder != "") {
                string path = file.substr(13, file.size() - 17);
                if(excluded.find(path) == excluded.end()) addToFile(*this, 1);
                else addToFile(*this, 0);
            }
            if(folderName == "") {
                DIR *dir = opendir("Dictionaries");
                dirent *entry;
                vector<string> folders;
                string title;
                int star = -1; // индекс папки, которая открыта в данный момент
                while((entry = readdir(dir)) != nullptr) {
                    title = entry->d_name;
                    if(title != "." && title != ".." && title.substr(title.size() - 4) != ".txt") {
                        if(folder == title + "/") star = folders.size();
                        title.erase(0, 7);
                        pathToDemonstration(title);
                        title[0] = toupper(title[0]);
                        folders.push_back(title);
                    }
                }
                if(!folders.size()) {
                    setColor(4);
                    cout << "There are no folders yet\n\n";
                    setColor(15);
                    goto thatsall;
                }
                else {
                    if(out) cout << "\t";
                    else cout << "\n    ";
                    cout << "Existing folders:";
                    if(out) cout << "\n";
                    setColor(6);
                    for(int i = 0; i < folders.size(); ++i) {
                        if(out) cout << "\n\t";
                        else cout << "\n    ";
                        if(i == star) {
                            setColor(12);
                            cout << (char) 16;
                            setColor(6);
                        }
                        else cout << ' ';
                        cout << ' ' << folders[i];
                    }
                    cout << "\n\n";
                    setColor(15);
                    if(out) _getch();
                    goto thatsall;
                }
            }
            demonstrationToPath(folderName);
            string title = folderName;
            if(title == "this" && folder != "") title = folder.substr(7, folder.size() - 8);
            title[0] = toupper(title[0]);
            pathToDemonstration(title);
            if(folderName == "this" && folder != "") folderName = "Dictionaries/" + folder;
            else folderName = "Dictionaries/papka__" + folderName;
            DIR *dir = opendir(folderName.c_str());
            dirent *entry;
            if(!dir) {
                if(!out) {
                    setColor(4);
                    cout << "No such folder\n\n";
                    setColor(15);
                }
                goto thatsall;
            }
            if(out) system("cls");
            setColor(6);
            cout << "\n";
            if(out) cout << "\t";
            else cout << "    ";
            cout << title << "\n";
            vector<string> filesInFolder;
            vector<int>    theirNameSizes;
            vector<int>    dictSizes;
            vector<int>    dictSizesNumLength; // количества цифр в числе размера словаря (напр. 375 -> 3)
            while((entry = readdir(dir)) != nullptr) {
                string fileName = entry->d_name;
                if(fileName == "." || fileName == "..")
                    continue;
                int ds = getDictionarySize(folderName + "/" + fileName);
                if(ds >= 10000) dictSizesNumLength.push_back(5);
                else if(ds >= 1000) dictSizesNumLength.push_back(4);
                else if(ds >= 100) dictSizesNumLength.push_back(3);
                else if(ds >= 10) dictSizesNumLength.push_back(2);
                else dictSizesNumLength.push_back(1);
                dictSizes.push_back(ds);
                fileName.erase(fileName.size() - 4);
                pathToDemonstration(fileName);
                filesInFolder.push_back(fileName);
                theirNameSizes.push_back(fileName.size());
                if(fileName == "all") {
                    rotate(filesInFolder.rbegin(), filesInFolder.rbegin() + 1, filesInFolder.rend());
                    rotate(theirNameSizes.rbegin(), theirNameSizes.rbegin() + 1, theirNameSizes.rend());
                    rotate(dictSizes.rbegin(), dictSizes.rbegin() + 1, dictSizes.rend());
                    rotate(dictSizesNumLength.rbegin(), dictSizesNumLength.rbegin() + 1, dictSizesNumLength.rend());
                }
            }
            closedir(dir);
            int distance = *max_element(theirNameSizes.begin(), theirNameSizes.end()) + 3;
            int maxNumberSize = *max_element(dictSizesNumLength.begin(), dictSizesNumLength.end());
            for(int i = 0; i < theirNameSizes.size(); ++i)
                theirNameSizes[i] -= maxNumberSize - dictSizesNumLength[i];
            for(int i = 0; i < filesInFolder.size(); ++i) {
                setColor(4);  cout << "\n";
                              if(out) cout << "\t";
                              else cout << "    ";
                              cout << (char) -92 << " ";
                setColor(15); cout << filesInFolder[i];
                setColor(7);  cout << " " << string(distance - theirNameSizes[i], '.') << " " << dictSizes[i];
            }
            setColor(15);
            if(out) _getch();
            if(!out) cout << "\n\n";
        }
        else if(choice == "cls") {
            needInstructions = 0;
            system("cls");
            cout << "\n";
        }
        else if(choice == "exit") out = 1;
        thatsall:
        if(out) break;
        if(needInstructions && choice != "") foldersHandlerInstructions(command);
        setColor(7);
        cout << output_before_user_request;
        setColor(15);
        needInstructions = 1;
        command = "";
        getline(cin, choice);
        getRidOfSpaces(choice);
        for(int i = 0; i < choice.size(); ++i)
        choice[i] = tolower(choice[i]);
    }
}

void Dictionary::foldersHandlerInstructions(string command)
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
    int instructionColor = 6,
        descriptionColor = 7,
        folderNameColor = 3;

    if(command == "create") {
        setColor(instructionColor);
        cout << "\n    create ";
        setColor(folderNameColor);
        cout << "<folder name>";
        setColor(descriptionColor);
        cout << modifyString(" - создает новую папку", width, 19) << "\n\n";
    }
    else if(command == "delete") {
        setColor(instructionColor);
        cout << "\n    delete ";
        setColor(folderNameColor);
        cout << "<folder name>";
        setColor(descriptionColor);
        cout << modifyString(" - удаляет существующую папку и перемещает все находящиеся в ней словари в общую область видимости", width, 19) << "\n\n";
    }
    else if(command == "add to") {
        setColor(instructionColor);
        cout << "\n    add to ";
        setColor(folderNameColor);
        cout << "<folder name>";
        setColor(descriptionColor);
        cout << modifyString(" - добавляет текущий словарь во введенную папку", width, 19) << "\n\n";
    }
    else {
        setColor(instructionColor);
        cout << "\n    create ";
        setColor(folderNameColor);
        cout << "<folder name>";
        setColor(descriptionColor);
        cout << modifyString(" - создать новую папку", width, 19);

        setColor(instructionColor);
        cout << "\n    delete ";
        setColor(folderNameColor);
        cout << "<folder name>";
        setColor(descriptionColor);
        cout << modifyString(" - удалить существующую папку с перемещением всех находящихся в ней словарей в общую область видимости", width, 19);

        setColor(instructionColor);
        cout << "\n    add to ";
        setColor(folderNameColor);
        cout << "<folder name>";
        setColor(descriptionColor);
        cout << modifyString(" - добавить текущий словарь во введенную папку", width, 19);

        setColor(instructionColor);
        cout << "\n    remove";
        setColor(descriptionColor);
        cout << modifyString(" - переместить текущий словарь из папки в общую область видимости", width, 5);

        setColor(instructionColor);
        cout << "\n    show";
        setColor(descriptionColor);
        cout << modifyString(" - показать существующие папки с указанием в какой папке находится текущий словарь (если он не в общей области видимости)", width, 3);

        setColor(instructionColor);
        cout << "\n    show ";
        setColor(folderNameColor);
        cout << "<folder name>";
        setColor(descriptionColor);
        cout << modifyString(" - показать все словари введенной папки с указанием размера каждого словаря", width, 17);

        setColor(instructionColor);
        cout << "\n    cls";
        setColor(descriptionColor);
        cout << " - очистить экран";

        setColor(instructionColor);
        cout << "\n    exit";
        setColor(descriptionColor);
        cout << modifyString(" - вернуться в главное меню", width, 3);
        cout << "\n\n";

        setColor(15);
    }
}

void Dictionary::readDic(string path, map<string, vector<string>>& dic)
{
    dic.clear();
    ifstream fin(path);
    string word, meaning, temp;
    if(!fin.is_open()) {
        perror(("\ndicionary.cpp -> строка " + to_string(__LINE__)).c_str());
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

void Dictionary::writeInDic(string path, map<string, vector<string>>& dic)
{
    ofstream fout(path.c_str());
    if(!fout.is_open()) {
        perror(("\ndictionary.cpp -> строка " + to_string(__LINE__)).c_str());
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

void Dictionary::fileUpDown(char whereTo)
{
    if(folder == "") return;
    string dirFolder = file;
    dirFolder.erase(dirFolder.rfind("/"));
    DIR *dir = opendir(dirFolder.c_str());
    dirent *entry;
    if(!dir) {
        perror(("\ndictionary.cpp -> строка " + to_string(__LINE__)).c_str());
        exit(-1);
    }
    vector<string> files;
    string reader;
    while((entry = readdir(dir)) != nullptr) {
        reader = entry->d_name;
        if(reader == "." || reader == "..") continue;
        files.push_back(reader);
        if(reader == "all.txt") rotate(files.begin(), files.end() - 1, files.end());
    }
    closedir(dir);
    if(files.size() == 1) return;
    int index;
    reader = file;
    reader.erase(0, reader.rfind("/") + 1);
    for(int i = 0; i < files.size(); ++i) {
        if(files[i] == reader) {
            index = i;
            break;
        }
    }
    if(whereTo == '<') {
        if(index) index--;
        else index = files.size() - 1;
    }
    else {
        if(index == files.size() - 1) index = 0;
        else index++;
    }
    if(excluded.find(file.substr(13, file.size() - 17)) != excluded.end()) addToFile(*this, 0);
    else addToFile(*this, 1);
    file = "Dictionaries/" + folder + files[index];
    readDic(file, dict);
    readDic("Dictionaries/all.txt", all_dict);
    int initialSize = all_dict.size();
    for(auto it = dict.begin(); it != dict.end(); ++it) {
        auto del = all_dict.find(it->first);
        if(del != all_dict.end()) all_dict.erase(del);
    }
    readDic("Dictionaries/" + folder + "all.txt", innerAll_dict);
    for(auto it = dict.begin(); it != dict.end(); ++it) {
        auto del = innerAll_dict.find(it->first);
        if(del != innerAll_dict.end()) innerAll_dict.erase(del);
    }
    while(last_word.size()) last_word.pop();
}

void Dictionary::changeDictionaryName()
{
    if(file.substr(file.size() - 7) == "all.txt") {
        Beep(80, 400);
        return;
    }
    system("cls");
    cout << "\n\tНовое имя словаря: ";
    string newName;
    if(cin.peek() != '\n') {
        getline(cin, newName);
        getRidOfSpaces(newName);
    }
    if(newName == "") {
        cin.sync();
        getline(cin, newName);
    }
    getRidOfSpaces(newName);
    if(newName == "") return;
    demonstrationToPath(newName);
    if(newName == "all") {
        cout << '\a';
        return;
    }
    newName = "Dictionaries/" + folder + newName + ".txt";
    rename(file.c_str(), newName.c_str());
    file = newName;
}

void Dictionary::showLastAddedWord()
{
    system("cls");
    string last = (last_word.empty()) ? "" : last_word.top();
    if(last == "") {
        if(!dict.size()) cout << "\n\tNo words added yet";
        else cout << "\n\tNo words added recently";
    }
    else printAWord(last, 0, "", 10);
    char ch = _getch();
}

COORD Dictionary::getPos()
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

char Dictionary::getChar(short x, short y)
{
    char buff[3];
    DWORD cbRead = 0;
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {x, y};
    ReadConsoleOutputCharacter(hCon, buff, 2, pos, &cbRead);
    return buff[0];
}

void Dictionary::getRidOfSpaces(string& word, char sym)
{
    while(word[0] == sym) word.erase(0, 1);
    while(word.back() == sym) word.pop_back();
    int l = 1;
    while(l < word.size()) {
        if(word[l] == sym && word[l-1] == sym) {
            word.erase(l--, 1);
        }
        l++;
    }
}

void Dictionary::pathToDemonstration(string& s)
{
    for(int i = 0; i < s.size(); ++i) {
        if(s[i] == '_') s[i] = ' ';
        else if(s[i] == ';') s[i] = ':';
        else if(s[i] == '#') s[i] = '*';
    }
}

void Dictionary::demonstrationToPath(string& s)
{
    for(int i = 0; i < s.size(); ++i) {
        if(s[i] == ' ') s[i] = '_';
        else if(s[i] == ':') s[i] = ';';
        else if(s[i] == '*') s[i] = '#';
    }
}

int Dictionary::numLength(int num)
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
