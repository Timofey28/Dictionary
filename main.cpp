#include <bits/stdc++.h>
#include "dictionary.h"
#include "dictionary.cpp"
#include <dirent.h>
#include <fcntl.h>
using namespace std;
void setPosition(int, int);
void dirCommand(map<string, vector<string>>& films, string& film_name);
void renew(map<string, vector<string>>& films);
pair<int, int> getScreenWH();
set<string> excluded;

int main()
{
    setlocale(0, "");
    const int HIGHLIGHTING = 12; // цветовое изменение строки на которой курсор при выборе фильма из списка
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO structCursorInfo;
    DIR *dir;
    dirent *entry;
    dir = opendir("Dictionaries");
    if(!dir) {
        mkdir("Dictionaries");
        ofstream f("Dictionaries/all.txt");
        f.close();
        f.open("Dictionaries/__excluded__.txt");
        f.close();
        dir = opendir("Dictionaries");
        if(!dir) {
            perror("\nНе удалось создать папку \"Dictionaries\"");
            exit(-1);
        }
    }
    string path;
    ifstream fin("Dictionaries/__excluded__.txt");
    if(!fin.is_open()) {
        ofstream f("Dictionaries/__excluded__.txt");
        f.close();
    }
    else {
        while(fin >> path) excluded.insert(path);
        fin.close();
    }
    another_movie:
    GetConsoleCursorInfo(hConsole, &structCursorInfo);
    structCursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &structCursorInfo);
    vector<string> vars, innerFolderFilms;
    map<string, vector<string>> films;
    string s, film_name, just_a_name = "", afterSlash;
    dir = opendir("Dictionaries");
    bool isFolder;
    while((entry = readdir(dir)) != nullptr) {
        innerFolderFilms.clear();
        s = entry->d_name;
        if(s == "." || s == ".." || s == "__excluded__.txt") continue;
        isFolder = false;
        if(s.substr(0, 7) == "papka__") {
            isFolder = true;
            if(s.substr(s.size() - 4) == ".txt") {
                cout << "\nСоздался файл \"" << s << "\"\n";
                exit(-1);
            }
            DIR *innerDir = opendir(("Dictionaries/" + s).c_str());
            if(!innerDir) {
                string endProgram = "\nmain.cpp -> строка " + to_string(__LINE__);
                perror(endProgram.c_str());
                exit(-1);
            }
            dirent *innerEntry;
            while((innerEntry = readdir(innerDir)) != nullptr) {
                string innerFilm = innerEntry->d_name;
                if(innerFilm == "." || innerFilm == "..") continue;
                innerFilm.erase(innerFilm.size() - 4);
                Dictionary::pathToDemonstration(innerFilm);
                innerFolderFilms.push_back(innerFilm);
                if(innerFilm == "all") rotate(innerFolderFilms.begin(), innerFolderFilms.end() - 1, innerFolderFilms.end());
            }
            s.erase(0, 7);
        }
        else s.erase(s.size() - 4);
        Dictionary::pathToDemonstration(s);
        if(isFolder) s += ' ';
        films[s] = innerFolderFilms;
    }
    closedir(dir);
    vector<bool> kIsInFolder;
    system("cls");
    char c[2];
    bool ok = 0;
    int k = -1, k_withFolderOpen = -1, realCurrentLine = -1;
    char changer;
    cout << "\n\tВведите название фильма: ";
    cin.sync();
    while(1) {
        c[0] = _getch();
        if(c[0] == ' ' && !film_name.size() || c[0] == ' ' && film_name.back() == ' ') continue;
        if(c[0] == 13 || c[0] == 9) { // enter
            if(film_name == "" && c[0] == 9) {
                film_name = "all";
                break;
            }
            if(k != -1) {
                if(!films[vars[k]].size()) {
                    film_name = vars[k];
                    break;
                }
                else if(k_withFolderOpen == -1) {
                    if(films[vars[k]].back() == "PAPKA_OTKRITA") {
                        films[vars[k]].pop_back();
                        kIsInFolder.erase(kIsInFolder.begin() + realCurrentLine+1, kIsInFolder.begin() + realCurrentLine+1 + films[vars[k]].size());
                    }
                    else {
                        kIsInFolder.insert(kIsInFolder.begin() + realCurrentLine+1, films[vars[k]].size(), 1);
                        films[vars[k]].push_back("PAPKA_OTKRITA");
                    }
                    system("cls");
                    cout << "\n\tВведите название фильма: " << film_name;
                    goto folderAddedToVars;
                }
                else {
                    film_name = "papka__" + vars[k].substr(0, vars[k].size() - 1) + "/";
                    int index = -1;
                    while(kIsInFolder[realCurrentLine]) {
                        realCurrentLine--;
                        index++;
                    }
                    film_name += films[vars[k]][index];
                    break;
                }
            }
            else {
                if(film_name.size() && c[0] == 13) break;
                else if(c[0] == 9) {
                    film_name = vars[0];
                    if(film_name.back() == ' ') film_name.erase(film_name.size() - 1);
                    if(films[vars[0]].size()) film_name += "/all";
                    break;
                }
            }
            continue;
        }
        if((c[0] == 8 || c[0] == 127) && film_name.size()) { // backspace || ctrl + backspace
            if(c[0] == 8) film_name.pop_back();
            else {
                if(film_name.back() == ' ') film_name.pop_back();
                if(film_name.find(' ') == string::npos) film_name = "";
                else film_name.erase(film_name.rfind(' '));
            }
            k = -1;
            k_withFolderOpen = -1;
            realCurrentLine = -1;
            for(auto it = films.begin(); it != films.end(); ++it) {
                if(it->second.size() && it->second.back() == "PAPKA_OTKRITA")
                    it->second.pop_back();
            }
        }
        else if(c[0] == 'k' || c[0] == -85) goto arrowUp;       // up
        else if(c[0] == 'j' || c[0] == -82) goto arrowDown;     // down;
        else if(c[0] == 11)                 goto ctrlArrowUp;   // ctrl + k
        else if(c[0] == 10)                 goto ctrlArrowDown; // ctrl + j
        else if(c[0] == -32 && kbhit()) { // стрелки
            changer = _getch();
            sameUpOrDown:
            if(changer == 72) { // up
                arrowUp:
                if(k == -1 || k == 0 && k_withFolderOpen == -1) {
                    if(kIsInFolder.size() && kIsInFolder.back())
                        k_withFolderOpen = kIsInFolder.size() - 1;
                    k = vars.size() - 1;
                    realCurrentLine = kIsInFolder.size() - 1;
                }
                else {  // если курсор еще не достиг первой строки
                    if(k_withFolderOpen == -1) {
                        if(films[vars[k - 1]].size() && films[vars[k - 1]].back() == "PAPKA_OTKRITA")
                            k_withFolderOpen = k - 2 + films[vars[k - 1]].size();
                        k--;
                    }
                    else { // если курсор в папке в наст.вр.
                        if(kIsInFolder[realCurrentLine - 1])
                            k_withFolderOpen--;
                        else k_withFolderOpen = -1;
                    }
                    realCurrentLine--;
                }
            }
            else if(changer == 80) { // down
                arrowDown:
                if(k == -1 || realCurrentLine + 1 == kIsInFolder.size()) {
                    k = 0;
                    realCurrentLine = 0;
                    k_withFolderOpen = -1;
                }
                else { // если курсор еще не достиг последней строки
                    if(k_withFolderOpen == -1) {
                        if(kIsInFolder[realCurrentLine + 1]) k_withFolderOpen = k + 1;
                        else k++;
                    }
                    else { // если курсор в папке в наст.вр.
                        if(kIsInFolder[realCurrentLine + 1])
                            k_withFolderOpen++;
                        else {
                            k++;
                            k_withFolderOpen = -1;
                        }
                    }
                    realCurrentLine++;
                }
            }
            else if(changer == -115) { // ctrl + up
                ctrlArrowUp:
                if(realCurrentLine == -1 || !realCurrentLine || !kIsInFolder[realCurrentLine - 1] ||
                   kIsInFolder[realCurrentLine - 1] && !kIsInFolder[realCurrentLine] ||
                   !kIsInFolder[realCurrentLine - 1] && kIsInFolder[realCurrentLine]) {
                    changer = 72;
                    goto sameUpOrDown;
                }
                else do {
                    k_withFolderOpen--;
                    realCurrentLine--;
                }while(kIsInFolder[realCurrentLine - 1]);
            }
            else if(changer == -111) { // ctrl + down
                ctrlArrowDown:
                if(!kIsInFolder[realCurrentLine + 1]) { // курсор уже в конце папки ИЛИ вне папки и не попадает в нее
                    changer = 80;
                    goto sameUpOrDown;
                }
                else { // курсор не в конце папки ИЛИ курсор на названии открытой папки и сейчас попадет туда
                    do {
                        if(k_withFolderOpen == -1) k_withFolderOpen = k + 1;
                        else k_withFolderOpen++;
                        realCurrentLine++;
                    }while(kIsInFolder[realCurrentLine + 1]);
                }
            }
            else continue;
            goto justArrows;
        }
        if(c[0] != 8 && (c[0] != -32 || !kbhit())) { // 8: backspace; -32 - первый байт
            OemToCharA(c, c);
            static map<char, char> ru_to_en = {{'й', 'q'}, {'ц', 'w'}, {'у', 'e'}, {'к', 'r'}, {'е', 't'}, {'н', 'y'}, {'г', 'u'},
            {'ш', 'i'}, {'щ', 'o'}, {'з', 'p'}, {'ф', 'a'}, {'ы', 's'}, {'в', 'd'}, {'а', 'f'}, {'п', 'g'}, {'р', 'h'}, {'о', 'j'},
            {'л', 'k'}, {'д', 'l'}, {'я', 'z'}, {'ч', 'x'}, {'с', 'c'}, {'м', 'v'}, {'и', 'b'}, {'т', 'n'}, {'ь', 'm'}, {'Й', 'Q'},
            {'Ц', 'W'}, {'У', 'E'}, {'К', 'R'}, {'Е', 'T'}, {'Н', 'Y'}, {'Г', 'U'}, {'Ш', 'I'}, {'Щ', 'O'}, {'З', 'P'}, {'Ф', 'A'},
            {'Ы', 'S'}, {'В', 'D'}, {'А', 'F'}, {'П', 'G'}, {'Р', 'H'}, {'О', 'J'}, {'Л', 'K'}, {'Д', 'L'}, {'Я', 'Z'}, {'Ч', 'X'},
            {'С', 'C'}, {'М', 'V'}, {'И', 'B'}, {'Т', 'N'}, {'Ь', 'M'}};
            if(ru_to_en.find(c[0]) != ru_to_en.end())
                c[0] = ru_to_en[c[0]];

            k = -1;
            k_withFolderOpen = -1;
            realCurrentLine = -1;
            if(!film_name.size()) {
                string temp(1, c[0]);
                film_name = temp;
            }
            else if(c[0] != 8) film_name += c[0];

            for(auto it = films.begin(); it != films.end(); ++it) {
                if(it->second.size() && it->second.back() == "PAPKA_OTKRITA")
                    it->second.pop_back();
            }
        }
        system("cls");
        cout << "\n\tВведите название фильма: " << film_name;
        if(!film_name.size()) continue;
        if(film_name == "exit" || film_name == "EXIT") {
            ofstream fout("Dictionaries/__excluded__.txt");
            for(auto it = excluded.begin(); it != excluded.end(); ++it)
                fout << *it << "\n";
            fout.close();
            return 0;
        }
        if(film_name == "renew" || film_name == "RENEW") {
            setPosition(33, 1);
            SetConsoleTextAttribute(hConsole, (WORD) 12);
            cout << film_name;
            renew(films);
            SetConsoleTextAttribute(hConsole, (WORD) 15);
            goto another_movie;
        }
        if(film_name == "dir" || film_name == "DIR") {
            dirCommand(films, film_name);
            if(film_name != "") break;
            system("cls");
            cout << "\n\tВведите название фильма: ";
            vars.clear();
            goto folderAddedToVars;
        }
        if(ok) break;
        vars.clear();
        bool notSuits, hasSlash;
        for(auto it = films.begin(); it != films.end(); ++it) {
            notSuits = 0;
            string film = it->first, input = film_name;
            if(input.find("/") != string::npos) {
                hasSlash = 1;
                afterSlash = input.substr(input.find("/") + 1);
            }
            else hasSlash = 0;
            for(int i = 0; i < film.size(); ++i)
                film[i] = tolower(film[i]);
            for(int i = 0; i < input.size(); ++i)
                input[i] = tolower(input[i]);
            if(film.find(input) == 0) vars.push_back(it->first);
            else if(input.size() <= count(film.begin(), film.end(), ' ') + 1 -
                    count(film.begin(), film.end(), '-')) { // если длина введенного слова <= кол-во слов в film
                for(int j = 0; j < input.size(); ++j) {
                    if(input[j] != film[0]) {
                        notSuits = 1;
                        break;
                    }
                    do film.erase(0, film.find(" ") + 1);
                    while(film[0] == '-');
                }
                if(notSuits) continue;
                vars.push_back(it->first);
            }
        }
        kIsInFolder.resize(vars.size());
        for(int i = 0; i < vars.size(); ++i)
            kIsInFolder[i] = 0;
        folderAddedToVars:
        justArrows:
        int i_lineCounter = 0;
        for(int i = 0; i < vars.size(); ++i) {
            if(_kbhit()) continue;
            cout << "\n\t                        ";
            if(films[vars[i]].size()) {
                if(films[vars[i]].back() == "PAPKA_OTKRITA") {
                    if(realCurrentLine == i_lineCounter++) Dictionary::setColor(HIGHLIGHTING);
                    cout << " - " << vars[i] << ' ';
                    cout << "\n\t                         ";
                    Dictionary::setColor(15);
                    if(realCurrentLine == i_lineCounter++) Dictionary::setColor(HIGHLIGHTING);
                    _setmode(_fileno(stdout), _O_U16TEXT);
                    wcout << (wchar_t) 0x203E << (wchar_t) 0x203E << (wchar_t) 0x203E;
                    _setmode(_fileno(stdout), _O_TEXT);
                    cout << "|- " << films[vars[i]][0];
                    Dictionary::setColor(15);
                    for(int j = 1; j < films[vars[i]].size() - 1; ++j) {
                        cout << "\n\t                            ";
                        if(realCurrentLine == i_lineCounter++) Dictionary::setColor(HIGHLIGHTING);
                        cout << "|- ";
                        cout << films[vars[i]][j];
                        Dictionary::setColor(15);
                    }
                }
                else {
                    if(realCurrentLine == i_lineCounter++) Dictionary::setColor(HIGHLIGHTING);
                    cout << " ";
                    _setmode(_fileno(stdout), _O_U16TEXT);
                    wcout << (wchar_t) 0x2261;
                    _setmode(_fileno(stdout), _O_TEXT);
                    cout << ' ' << vars[i] << ' ';
                    Dictionary::setColor(15);
                }
            }
            else {
                if(realCurrentLine == i_lineCounter++) Dictionary::setColor(HIGHLIGHTING);
                cout << ' ' << vars[i] << ' ';
                Dictionary::setColor(15);
            }
        }
        setPosition(33 + film_name.size(), 1);
    }
    while(film_name[0] == ' ') film_name.erase(0, 1);
    while(film_name.back() == ' ') film_name.pop_back();
    if(film_name == "") goto another_movie;
    if(film_name[0] == '\\') film_name.erase(0, 1);
    Dictionary::demonstrationToPath(film_name);
    if((film_name.find("/") != string::npos || film_name.find(">") != string::npos) && film_name.substr(0, 7) != "papka__") {
        if(film_name.find(">") != string::npos) film_name[film_name.find(">")] = '/';
        if(film_name.find("_/_") != string::npos) film_name.replace(film_name.find("/") - 1, 3, "/");
        else if(film_name.find("_/") != string::npos) film_name.replace(film_name.find("/") - 1, 2, "/");
        else if(film_name.find("/_") != string::npos) film_name.replace(film_name.find("/"), 2, "/");
        string papka = "papka__" + film_name.substr(0, film_name.find("/"));
        dir = opendir(("Dictionaries/" + papka).c_str());
        if(!dir) {
            mkdir(("Dictionaries/" + papka).c_str());
            ofstream f("Dictionaries/" + papka + "/all.txt");
            f.close();
        }
        closedir(dir);
        film_name = "papka__" + film_name;
    }
    Dictionary d(film_name);
    char ch;
    string folderWriting, fileWriting;
    do {
        system("cls");
        path = d.getFileName().substr(13, d.getFileName().size() - 17);
        if(excluded.find(path) == excluded.end()) d.setColor(14);
        else d.setColor(11);
        fileWriting = d.getFileName();
        fileWriting = fileWriting.substr(fileWriting.rfind("/") + 1);
        fileWriting.erase(fileWriting.size() - 4);
        folderWriting = d.getFolder();
        Dictionary::pathToDemonstration(fileWriting);
        if(folderWriting.size()) {
            folderWriting.erase(0, 7);
            folderWriting.pop_back();
            Dictionary::pathToDemonstration(folderWriting);
            folderWriting[0] = toupper(folderWriting[0]);
            cout << "\n\t<<< " << folderWriting << " -> " << fileWriting << " >>>";
        }
        else {
            fileWriting[0] = toupper(fileWriting[0]);
            cout << "\n\t<<< " << fileWriting << " >>>";
        }

        cout << "\n\tCurrent dictionary size: " << d.getSize() << "\n\n\t";                       d.setColor(10);

        if(d.getFileName() != "Dictionaries/" + d.getFolder() + "all.txt") {
            cout << "a"; d.setColor(15); cout << " - Add a new word\n\t";                         d.setColor(10);
            cout << "m"; d.setColor(15); cout << " - add a new Meaning to some word\n\t";         d.setColor(10);
        }
        cout << "s"; d.setColor(15); cout << " - Show all words with meanings\n\t";               d.setColor(10);
        cout << "w"; d.setColor(15); cout << " - show Without meanings\n\t";                      d.setColor(10);
        if(d.getFileName() != "Dictionaries/" + d.getFolder() + "all.txt")
        {cout << "d"; d.setColor(15); cout << " - Delete some word from dictionary\n\t";          d.setColor(10);}
        cout << "f"; d.setColor(15); cout << " - Find a word\n\t";                                d.setColor(10);
        if(d.getFileName() != "Dictionaries/all.txt")
        {cout << "v"; d.setColor(15); cout << " - find among all\n\t";                            d.setColor(10);}
        cout << "t"; d.setColor(15); cout << " - deTermine which dictionary the word is in\n\t";  d.setColor(10);
        cout << "x"; d.setColor(15); cout << " - pass an eXam\n\n\t";                             d.setColor(10);

        if(d.getFileName() != "Dictionaries/" + d.getFolder() + "all.txt")
        {cout << "z"; d.setColor(15); cout << " - delete this dictionary\n\t";                    d.setColor(10);}
        cout << "c"; d.setColor(15); cout << " - Choose another movie\n\t";                       d.setColor(10);
        cout << "r"; d.setColor(15); cout << " - foldeRs\n\t";                                    d.setColor(10);
        if(d.getFileName() != "Dictionaries/" + d.getFolder() + "all.txt")
        {cout << "g"; d.setColor(15); cout << " - change dictionary name\n\t";                    d.setColor(10);}
        cout << "e"; d.setColor(15); cout << " - Exit\n\n\t";
        cin.sync();
        cin >> c[0];
        OemToCharA(c, c);
        ch = tolower(c[0]);
        switch(ch)
        {
            case -12:
            case 'a': if(d.getFileName() != "Dictionaries/" + d.getFolder() + "all.txt") d.add();
                      break;
            case -4:
            case 'm': if(d.getFileName() != "Dictionaries/" + d.getFolder() + "all.txt") d.addSomeMeanings("");
                      break;
            case -5:
            case 's': d.print();
                      break;
            case -10:
            case 'w': d.printOnlyEnglish();
                      break;
            case -30:
            case 'd': if(d.getFileName() != "Dictionaries/" + d.getFolder() + "all.txt") d.getRidOf();
                      break;
            case -32:
            case 'f': d.findAWord(0);
                      break;
            case -20:
            case 'v': d.findAWord(1);
                      break;
            case -9:
            case 'x': d.exam();
                      break;
            case -1:
            case 'z': if(d.getFileName() != "Dictionaries/" + d.getFolder() + "all.txt")
                          if(d.remov()) {
                              if(excluded.find(path) != excluded.end()) excluded.erase(path);
                              goto another_movie;
                          }
                      break;
            case -23:
            case 'q': if(excluded.find(path) == excluded.end() && path != d.getFolder() + "all") excluded.insert(path);
                      addToFile(d, 0);
                      goto another_movie;
                      break;
            case -15:
            case 'c': if(excluded.find(path) != excluded.end()) excluded.erase(path);
                      addToFile(d, 1);
                      goto another_movie;
                      break;
            case -27:
            case 't': d.determine();
                      break;
            case -22:
            case 'r': d.foldersHandler();
                      break;
            case -63:
            case -31:
            case 60: d.fileUpDown('<');
                     break;
            case -34:
            case -2:
            case 62: d.fileUpDown('>');
                     break;
            case -17:
            case 'g': d.changeDictionaryName();
                      break;
            case -28:
            case 'l': if(d.getFileName() != "Dictionaries/" + d.getFolder() + "all.txt") d.showLastAddedWord();
                      break;
            case -18:
            case 'j': if(structCursorInfo.bVisible == TRUE) structCursorInfo.bVisible = FALSE;
                      else structCursorInfo.bVisible = TRUE;
                      SetConsoleCursorInfo(hConsole, &structCursorInfo);
                      break;
            default:  break;
        }
        if(d.getFileName() == "") goto another_movie;
    }while(ch != 'e' && ch != -13);
    if(excluded.find(d.getFileName().substr(13, d.getFileName().size() - 17)) == excluded.end())
        addToFile(d, 1);
    else addToFile(d, 0);
    ofstream fout("Dictionaries/__excluded__.txt");
    for(auto it = excluded.begin(); it != excluded.end(); ++it)
        fout << *it << "\n";
    fout.close();
    system("cls");

    return 0;
}

void dirCommand(map<string, vector<string>>& films, string& film_name)
{
    if(!films.size()) return;
    const int HIGHLIGHTING = 12;
    int screenHeight = getScreenWH().second;
    vector<string> options, fullPaths;
    vector<wstring> wstrOptions;
    for(auto it = films.begin(); it != films.end(); ++it) {
        if(it->second.size()) {
            options.push_back(it->first);
            fullPaths.push_back("");
            wstring ws = wstring(1, 0x2261) + L" "; // 3 горизонтальные палки перед именем папки
            wstrOptions.push_back(ws);
        }
    }
    for(auto it = films.begin(); it != films.end(); ++it) {
        if(!it->second.size()) {
            options.push_back(it->first);
            fullPaths.push_back(it->first);
            wstrOptions.push_back(L"");
        }
    }
    system("cls");
    for(int i = 0; i < options.size() && i < screenHeight - 1; ++i) {
        cout << "\n\t";
        _setmode(_fileno(stdout), _O_U16TEXT);
        wcout << wstrOptions[i];
        _setmode(_fileno(stdout), _O_TEXT);
        cout << options[i];
    }

    int cursorPos = -1,
        firstRowNumber = -1;
    char choice;
    bool changeCursorOnly;
    int changeCursor_from, changeCursor_to;
    while(1) {
        changeCursorOnly = 0;
        screenHeight = getScreenWH().second;
        choice = _getch();
        if(choice == 'k' || choice == -85)      goto arrowUp_dirCommand;       // up
        else if(choice == 'j' || choice == -82) goto arrowDown_dirCommand;     // down;
        else if(choice == 11)                   goto ctrlArrowUp_dirCommand;   // ctrl + k == ctrl + up
        else if(choice == 10)                   goto ctrlArrowDown_dirCommand; // ctrl + j == ctrl + down
        else if(choice == 13 || choice == 9) { // enter || tab
            if(cursorPos == -1) {
                if(choice == 9) {
                    film_name = "all";
                    return;
                }
                else continue;
            }
            if(options[cursorPos].back() != ' ') {
                film_name = fullPaths[cursorPos];
                return;
            }
            else {
                if(films[options[cursorPos]].back() == "PAPKA_OTKRITA") {
                    films[options[cursorPos]].pop_back();
                    int filesAmount = films[options[cursorPos]].size();
                    wstrOptions[cursorPos] = wstring(1, 0x2261) + L" ";
                    options.erase(options.begin() + cursorPos + 1, options.begin() + cursorPos + 1 + filesAmount);
                    wstrOptions.erase(wstrOptions.begin() + cursorPos + 1, wstrOptions.begin() + cursorPos + 1 + filesAmount);
                    fullPaths.erase(fullPaths.begin() + cursorPos + 1, fullPaths.begin() + cursorPos + 1 + filesAmount);
                }
                else {
                    wstrOptions[cursorPos] = L"- ";
                    vector<string> content(films[options[cursorPos]]);
                    for(int i = 0; i < content.size(); ++i)
                        content[i] = "papka__" + options[cursorPos].substr(0, options[cursorPos].size() - 1) + "/" + content[i];
                    vector<wstring> wstr_content(content.size());
                    wstr_content[0] = wstring(3, 0x203E) + L"|- "; // 3 верхних подчеркивания
                    for(int i = 1; i < content.size(); ++i)
                        wstr_content[i] = L"   |- ";
                    options.insert(options.begin() + cursorPos + 1, films[options[cursorPos]].begin(), films[options[cursorPos]].end());
                    wstrOptions.insert(wstrOptions.begin() + cursorPos + 1, wstr_content.begin(), wstr_content.end());
                    fullPaths.insert(fullPaths.begin() + cursorPos + 1, content.begin(), content.end());
                    films[options[cursorPos]].push_back("PAPKA_OTKRITA");
                }
            }
        }
        else if(choice == 8 || choice == 27) { // backspace || escape
            film_name = "";
            return;
        }
        else if(choice == -32 && _kbhit()) { // стрелки
            choice = _getch();
            sameUpOrDown:
            if(choice == 72) { // up
                arrowUp_dirCommand:
                if(cursorPos == -1 || cursorPos == 0) {
                    cursorPos = options.size() - 1;
                    firstRowNumber = max(-1, cursorPos - screenHeight + 2);
                }
                else {
                    if(firstRowNumber == cursorPos) firstRowNumber--;
                    else {
                        changeCursorOnly = 1;
                        changeCursor_from = cursorPos;
                        changeCursor_to = cursorPos - 1;
                    }
                    cursorPos--;
                }
            }
            else if(choice == 80) { // down
                arrowDown_dirCommand:
                if(cursorPos == -1 || cursorPos == options.size() - 1) {
                    cursorPos = 0;
                    firstRowNumber = -1;
                }
                else {
                    if(cursorPos - firstRowNumber + 1 == screenHeight) firstRowNumber++;
                    else {
                        changeCursorOnly = 1;
                        changeCursor_from = cursorPos;
                        changeCursor_to = cursorPos + 1;
                    }
                    cursorPos++;
                }
            }
            else if(choice == -115) { // ctrl + up
                ctrlArrowUp_dirCommand:
                if(!cursorPos) {
                    if(!firstRowNumber) firstRowNumber = -1;
                    else continue;
                }
                else if(fullPaths[cursorPos].substr(0, 7) != "papka__" ||
                        fullPaths[cursorPos].substr(0, 7) == "papka__" && fullPaths[cursorPos - 1] == "") {
                    choice = 72;
                    goto sameUpOrDown;
                }
                else {
                    changeCursor_from = cursorPos;
                    do cursorPos--;
                    while(fullPaths[cursorPos] != "");
                    cursorPos++;
                    if(cursorPos < firstRowNumber) firstRowNumber = cursorPos;
                    else {
                        changeCursorOnly = 1;
                        changeCursor_to = cursorPos;
                    }
                }
            }
            else if(choice == -111) { // ctrl + down
                ctrlArrowDown_dirCommand:
                if(cursorPos == options.size() - 1) {
                    if(cursorPos - firstRowNumber + 1 == screenHeight) firstRowNumber++;
                    else continue;
                }
                else if(!(fullPaths[cursorPos] == "" && films[options[cursorPos]].back() == "PAPKA_OTKRITA" || fullPaths[cursorPos].substr(0, 7) == "papka__") ||
                        fullPaths[cursorPos].substr(0, 7) == "papka__" && (cursorPos + 1 == fullPaths.size() || fullPaths[cursorPos + 1].substr(0, 7) != "papka__")) {
                    choice = 80;
                    goto sameUpOrDown;
                }
                else {
                    changeCursor_from = cursorPos;
                    do cursorPos++;
                    while(cursorPos < fullPaths.size() && fullPaths[cursorPos].substr(0, 7) == "papka__");
                    cursorPos--;
                    if(cursorPos - screenHeight + 1 > firstRowNumber) firstRowNumber = cursorPos - screenHeight + 1;
                    else {
                        changeCursorOnly = 1;
                        changeCursor_to = cursorPos;
                    }
                }
            }
        }
        else continue;

        if(changeCursorOnly) {
            int y = changeCursor_from - firstRowNumber;
            setPosition(8, y);
            _setmode(_fileno(stdout), _O_U16TEXT);
            wcout << wstrOptions[changeCursor_from];
            _setmode(_fileno(stdout), _O_TEXT);
            cout << options[changeCursor_from];
            y = changeCursor_to - firstRowNumber;
            setPosition(8, y);
            Dictionary::setColor(HIGHLIGHTING);
            _setmode(_fileno(stdout), _O_U16TEXT);
            wcout << wstrOptions[changeCursor_to];
            _setmode(_fileno(stdout), _O_TEXT);
            cout << options[changeCursor_to];
            Dictionary::setColor(15);
        }
        else {
            system("cls");
            if(firstRowNumber == -1) screenHeight--;
            int startWith = max(0, firstRowNumber);
            for(int i = startWith; i < options.size() && i < screenHeight + startWith; ++i) {
                if(i == startWith && firstRowNumber != -1) cout << "\t";
                else cout << "\n\t";
                if(i == cursorPos) Dictionary::setColor(HIGHLIGHTING);
                _setmode(_fileno(stdout), _O_U16TEXT);
                wcout << wstrOptions[i];
                _setmode(_fileno(stdout), _O_TEXT);
                cout << options[i];
                Dictionary::setColor(15);
            }
        }
    }
}

void renew(map<string, vector<string>>& films)
{
    string path, folder;
    ofstream f("Dictionaries/all.txt");
    f.close();
    bool R = 1;
    for(auto it = films.begin(); it != films.end(); ++it) {
        if(it->first == "all") continue;
        if(it->second.size()) {
            folder = "papka__" + it->first.substr(0, it->first.size() - 1) + "/";
            for(int i = 0; i < it->second.size(); ++i) {
                path = folder + it->second[i];
                Dictionary::demonstrationToPath(path);
                if(excluded.find(path) == excluded.end()) {
                    setPosition(33, 1);
                    if(R) cout << "RENEW";
                    else cout << "renew";
                    R ^= 1;
                    Dictionary *temp = new Dictionary(path);
                    addToFile(*temp, 1);
                    delete temp;
                }
            }
        }
        else {
            path = it->first;
            Dictionary::demonstrationToPath(path);
            if(excluded.find(path) == excluded.end()) {
                setPosition(33, 1);
                if(R) cout << "RENEW";
                else cout << "renew";
                R ^= 1;
                Dictionary *temp = new Dictionary(path);
                addToFile(*temp, 1);
                delete temp;
            }
        }
    }
}

pair<int, int> getScreenWH()
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

void setPosition(int x, int y)
{
    static COORD coord;
    coord.X = x;
    coord.Y = y;
    static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if(!SetConsoleCursorPosition(hConsole, coord)) {
        cout << "\nError: " << GetLastError() << "\n";
        exit(1);
    }
}
