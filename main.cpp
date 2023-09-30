#include <bits/stdc++.h>
#include "dictionary.h"
#include "dictionary.cpp"
#include <dirent.h>
#include <fcntl.h>
using namespace std;
void setPosition(int, int);
void dirCommand(map<string, vector<string>>& films, string& film_name);
void renew(map<string, vector<string>>& films);
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
    while((entry = readdir(dir)) != nullptr) {
        innerFolderFilms.clear();
        s = entry->d_name;
        if(s == "." || s == ".." || s == "__excluded__.txt") continue;
        if(s.substr(0, 7) == "papka__") {
            if(s.substr(s.size() - 4) == ".txt") {
                cout << "\nСоздался файл \"" << s << "\"\n";
                exit(1);
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
        films[s] = innerFolderFilms;
    }
    closedir(dir);
    vector<bool> kIsInFolder;
    system("cls");
    char c[2];
    bool ok = 0;
    int k = -1, k_withFolderOpen = -1, realCurrentLine = -1;
    cout << "\n\tВведите название фильма: ";
    cin.sync();
    while(1) {
        c[0] = _getch();
        if(c[0] == ' ' && !film_name.size() || c[0] == ' ' && film_name.back() == ' ') continue;
        if(c[0] == 13) { // enter
            if(~k) {
                if(!films[vars[k]].size()) {
                    film_name = vars[k];
                    break;
                }
                else if(k_withFolderOpen == -1) {
                    if(films[vars[k]].back() == "DOBAVLEN") {
                        films[vars[k]].pop_back();
                        kIsInFolder.erase(kIsInFolder.begin() + realCurrentLine+1, kIsInFolder.begin() + realCurrentLine+1 + films[vars[k]].size());
                    }
                    else {
                        kIsInFolder.insert(kIsInFolder.begin() + realCurrentLine+1, films[vars[k]].size(), 1);
                        films[vars[k]].push_back("DOBAVLEN");
                    }
                    system("cls");
                    cout << "\n\tВведите название фильма: " << film_name;
                    goto folderAddedToVars;
                }
                else {
                    film_name = "papka__" + vars[k] + "/";
                    int index = -1;
                    while(kIsInFolder[realCurrentLine]) {
                        realCurrentLine--;
                        index++;
                    }
                    film_name += films[vars[k]][index];
                    break;
                }
            }
            else if(film_name.size()) break;
            continue;
        }
        sameUpOrDown2: // это для кнопок j, k, ctrl-j и ctrl-k
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
                if(it->second.size() && it->second.back() == "DOBAVLEN")
                    it->second.pop_back();
            }
        }
        else if(c[0] == 9) { // tab
            if(film_name == "") {
                film_name = "all";
                break;
            }
            if(k != -1 && films[vars[k]].size()) {
                if(films[vars[k]].back() == "DOBAVLEN") {
                    films[vars[k]].pop_back();
                    kIsInFolder.erase(kIsInFolder.begin() + realCurrentLine+1, kIsInFolder.begin() + realCurrentLine+1 + films[vars[k]].size());
                }
                else {
                    kIsInFolder.insert(kIsInFolder.begin() + realCurrentLine+1, films[vars[k]].size(), 1);
                    films[vars[k]].push_back("DOBAVLEN");
                }
                system("cls");
                cout << "\n\tВведите название фильма: " << film_name;
                goto folderAddedToVars;
            }
            if(~k) film_name = vars[k];
            else if(vars.size()) {
                film_name = vars[0];
                if(films[vars[0]].size()) film_name += "/all";
            }
            break;
        }

        // абсолютно такие же 4 куска кода что и снизу со стрелками вверх/вниз и ctrl-вверх/ctrl-вниз
        // условия не объеденены, потому что клавиши j, k, ctrl-j и ctrl-k занимают 1 байт, а стрелки 2 байта, вот такая вот хуйня
        else if(c[0] == 'k' || c[0] == -85) { // up
            if(k == -1 || k == 0 && k_withFolderOpen == -1) {
                if(kIsInFolder.size() && kIsInFolder.back())
                    k_withFolderOpen = kIsInFolder.size() - 1;
                k = vars.size() - 1;
                realCurrentLine = kIsInFolder.size() - 1;
            }
            else {  // если курсор еще не достиг первой строки
                if(k_withFolderOpen == -1) {
                    if(films[vars[k - 1]].size() && films[vars[k - 1]].back() == "DOBAVLEN")
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
            goto justArrows;
        }
        else if(c[0] == 'j' || c[0] == -82) { // down
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
            goto justArrows;
        }
        else if(c[0] == 11) { // ctrl + k
            if(realCurrentLine == -1 || !realCurrentLine || !kIsInFolder[realCurrentLine - 1] ||
               kIsInFolder[realCurrentLine - 1] && !kIsInFolder[realCurrentLine] ||
               !kIsInFolder[realCurrentLine - 1] && kIsInFolder[realCurrentLine]) {
                c[0] = 'k';
                goto sameUpOrDown2;
            }
            else do {
                k_withFolderOpen--;
                realCurrentLine--;
            }while(kIsInFolder[realCurrentLine - 1]);
            goto justArrows;
        }
        else if(c[0] == 10) { // ctrl + j
            if(!kIsInFolder[realCurrentLine + 1]) { // курсор уже в конце папки ИЛИ вне папки и не попадает в нее
                c[0] = 'j';
                goto sameUpOrDown2;
            }
            else { // курсор не в конце папки ИЛИ курсор на названии открытой папки и сейчас попадет туда
                do {
                    if(k_withFolderOpen == -1) k_withFolderOpen = k + 1;
                    else k_withFolderOpen++;
                    realCurrentLine++;
                }while(kIsInFolder[realCurrentLine + 1]);
            }
            goto justArrows;
        }
        /////////////////////////////////////////////////////////////////////////////////////////
        else if(c[0] == -32 && kbhit()) {
            char changer = _getch();
            sameUpOrDown:
            if(changer == 72) { // up
                if(k == -1 || k == 0 && k_withFolderOpen == -1) {
                    if(kIsInFolder.size() && kIsInFolder.back())
                        k_withFolderOpen = kIsInFolder.size() - 1;
                    k = vars.size() - 1;
                    realCurrentLine = kIsInFolder.size() - 1;
                }
                else {  // если курсор еще не достиг первой строки
                    if(k_withFolderOpen == -1) {
                        if(films[vars[k - 1]].size() && films[vars[k - 1]].back() == "DOBAVLEN")
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
                if(it->second.size() && it->second.back() == "DOBAVLEN")
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
//            cout << "\n";
//            for(int i = 0; i < films.size(); ++i)
//                cout << "\n\t" << i + 1 << ") " << films[i];
//            cout << "\n\t";
//            do {
//                foo:
//                getline(cin, s);
//                while(s[0] == ' ') s.erase(0, 1);
//                while(s.back() == ' ') s.erase(s.size()-1);
//                if(s == "") goto another_movie;
//                if(s == "exit" || s == "гзиҐ" || s == "EXIT") return 1;
//                for(int i = 0; i < s.size(); ++i) {
//                    if(!isdigit(s[i])) {
//                        cout << "\tНеверный формат, повторите попытку => ";
//                        goto foo;
//                    }
//                }
//                k = atoi(s.c_str());
//                if(k < 1 || k > films.size()) cout << "\tТакого номера нет, повторите попытку => ";
//                else {
//                    film_name = films[k-1];
//                    ok = 1;
//                    break;
//                }
//            }while(1);
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
                if(films[vars[i]].back() == "DOBAVLEN") {
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
        GetConsoleCursorInfo(hConsole, &structCursorInfo);
        structCursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(hConsole, &structCursorInfo);
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
    const int HIGHLIGHTING = 12;
    int k = -1,
        k_withFolderOpen = -1,
        realCurrentLine = -1;
    char choice;
    vector<bool> kIsInFolder(films.size());
    vector<string> vars;
    for(auto it = films.begin(); it != films.end(); ++it)
        if(it->second.size()) vars.push_back(it->first);
    for(auto it = films.begin(); it != films.end(); ++it)
        if(!it->second.size()) vars.push_back(it->first);
    int height;
    HANDLE hWndConsole;
    bool firstTime = 1;
    float whatsUp; // первая строка консоли в текущий момент (-1: пустая строка; 0: 1-ый элемент vars; 3.004: 4-ый элемент, 4-ый файл в папке)
    int realWhatsUp; // номер строки, находящейся сверху
    int total, decimal; // целая часть, дробная часть
    while(1) {
        if(firstTime) {
            firstTime = 0;
            whatsUp = -1;
            realWhatsUp = -1;
            if(hWndConsole = GetStdHandle(-12))
            {
                CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
                if(GetConsoleScreenBufferInfo(hWndConsole, &consoleInfo))
                    height = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;
                else exit(-2);
            }
            else exit(-2);
            goto firstTimee;
        }
        choice = _getch();
        if(hWndConsole = GetStdHandle(-12))
        {
            CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
            if(GetConsoleScreenBufferInfo(hWndConsole, &consoleInfo))
                height = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;
            else exit(-2);
        }
        else exit(-2);
        if(choice == 9) { // tab
            film_name = "all";
            return;
        }
        else if(choice == 13) { // enter
            if(~k) {
                if(!films[vars[k]].size()) {
                    film_name = vars[k];
                    return;
                }
                else if(k_withFolderOpen == -1) {
                    if(films[vars[k]].back() == "DOBAVLEN") {
                        films[vars[k]].pop_back();
                        kIsInFolder.erase(kIsInFolder.begin() + realCurrentLine+1, kIsInFolder.begin() + realCurrentLine+1 + films[vars[k]].size());
                    }
                    else {
                        kIsInFolder.insert(kIsInFolder.begin() + realCurrentLine+1, films[vars[k]].size(), 1);
                        films[vars[k]].push_back("DOBAVLEN");
                    }
                }
                else {
                    film_name = "papka__" + vars[k] + "/";
                    int index = -1;
                    while(kIsInFolder[realCurrentLine]) {
                        realCurrentLine--;
                        index++;
                    }
                    film_name += films[vars[k]][index];
                    return;
                }
            }
            else continue;
        }
        else if(choice == -32 && kbhit()) { // стрелки
            char changer = _getch();
            sameUpOrDown:
            total = floor(whatsUp);
            decimal = (whatsUp - total) * 1000;
            if(total == -1) total++;
            if(changer == 72) { // up
//                system("cls"); cout << "\n\tk: " << k << "\n\tk_withFolderOpen: " << k_withFolderOpen; cin.sync(); cin.get();
                if(k == -1 || k == 0 && k_withFolderOpen == -1) {
                    if(kIsInFolder.size() && kIsInFolder.back())
                        k_withFolderOpen = kIsInFolder.size() - 1;
                    k = vars.size() - 1;
//                cout << "\n\n\tk: " << k << "\n\tk_withFolderOpen: " << k_withFolderOpen; cin.sync(); cin.get();
                    realCurrentLine = kIsInFolder.size() - 1;
                    if(realCurrentLine + 1 < height - 1) {
                        whatsUp = -1;
                        realWhatsUp = -1;
                    }
                    else {
//                        system("cls"); cout << "here"; cin.sync(); cin.get();
                        int ones = -1,
                            h = height - 1,
                            realCL = realCurrentLine;
                        whatsUp = count(kIsInFolder.begin(), kIsInFolder.end(), 0);
                        realWhatsUp = kIsInFolder.size() - h;
                        while(h--) {
                            if(kIsInFolder[realCL]) {
                                if(ones == -1) {
                                    ones = 1;
                                    while(kIsInFolder[kIsInFolder.size()-1 - (realCurrentLine - realCL) - ones]) ones++;
                                    whatsUp = floor(whatsUp) - 1 + float(ones) / 1000;
                                }
                                whatsUp -= 0.001;
                            }
                            else {
                                whatsUp = round(whatsUp) - 1;
                                ones = -1;
                            }
                            realCL--;
                        }
                    }
//                    cout << "\n\trealCurrentLine: " << realCurrentLine << "\n\twhatsUp: " << whatsUp << "\n"; cin.sync(); cin.get();
                }
                else {  // если курсор еще не достиг первой строки
                    if(realCurrentLine == realWhatsUp) {
                        if(kIsInFolder[realWhatsUp]) {
                            whatsUp -= 0.001;
                            if(whatsUp < 0) whatsUp = 0;
                        }
                        else {
                            if(kIsInFolder[realWhatsUp - 1]) { // из "непапки" в папку
                                int ones = 1;
                                while(kIsInFolder[realWhatsUp-1 - ones]) ones++;
                                whatsUp = round(whatsUp) - 1 + float(ones) / 1000;
                            }
                            else whatsUp--; // из "непапки" в "непапку"
                        }
                        realWhatsUp--;
                    }

                    if(k_withFolderOpen == -1) {
                        if(films[vars[k - 1]].size() && films[vars[k - 1]].back() == "DOBAVLEN")
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
//                cout << "\n\n\tk: " << k << "\n\tk_withFolderOpen: " << k_withFolderOpen; cin.sync(); cin.get();
            }
            else if(changer == 80) { // down
                if(k == -1 || realCurrentLine + 1 == kIsInFolder.size()) {
                    k = 0;
                    realCurrentLine = 0;
                    k_withFolderOpen = -1;
                    whatsUp = -1;
                    realWhatsUp = -1;
                }
                else { // если курсор еще не достиг последней строки

                    if(realCurrentLine - realWhatsUp + 1 == height) {
                        if(kIsInFolder[realWhatsUp + 1]) whatsUp += 0.001;
                        else whatsUp = floor(whatsUp) + 1;
                        realWhatsUp++;
                    }

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
        }
        else if(choice == 8 || choice == 27) { // backspace || escape
            film_name = "";
            return;
        }
        else continue;

        firstTimee:
        system("cls");
        int i_lineCounter = 0; // для подкрашивания строки, на которой курсор

//        cout << "\n\twhatsUp: " << whatsUp << "\n\trealWhatsUp: " << realWhatsUp<<"\n\tdecimal: " << decimal; cin.sync(); cin.get();
        bool firstLine = 1;
        total = floor(whatsUp);
        decimal = (whatsUp - total) * 1000;
        int h = 0;
        if(whatsUp == -1) {
            cout << "\n";
            h++;
        }
        else {
//            int zeros = 0;
//            while(i_lineCounter < kIsInFolder.size()) {
//                if(!kIsInFolder[i_lineCounter]) zeros++;
//                if(zeros > total) break;
//                i_lineCounter++;
//            }
//            i_lineCounter += decimal;
            i_lineCounter = realWhatsUp;
        }
        int printFrom;
        if(total == -1) total++;
        for(int i = total; i < kIsInFolder.size(); ++i) {
            if(_kbhit()) continue;
            if(firstLine) {
                firstLine = 0;
                cout << "\t";
            }
            else cout << "\n\t";

            if(films[vars[i]].size()) {
                if(films[vars[i]].back() == "DOBAVLEN") {
                    if(decimal > 0) {
                        printFrom = decimal - 1;
                        decimal = 0;
                    }
                    else {
                        printFrom = 0;
                        if(realCurrentLine == i_lineCounter++) Dictionary::setColor(HIGHLIGHTING);
                        cout << " - " << vars[i] << ' ';
                        Dictionary::setColor(15);
                        if(++h == height) break;
                    }
                    for(int j = printFrom; j < films[vars[i]].size() - 1; ++j) {
                        if(!h) cout << " ";
                        else cout << "\n\t ";
                        if(!j) {
                            if(realCurrentLine == i_lineCounter++) Dictionary::setColor(HIGHLIGHTING);
                            _setmode(_fileno(stdout), _O_U16TEXT);
                            wcout << (wchar_t) 0x203E << (wchar_t) 0x203E << (wchar_t) 0x203E;
                            _setmode(_fileno(stdout), _O_TEXT);
                            cout << "|- " << films[vars[i]][0];
                            Dictionary::setColor(15);
                        }
                        else {
                            if(realCurrentLine == i_lineCounter++) Dictionary::setColor(HIGHLIGHTING);
                            cout << "   |- " << films[vars[i]][j];
                            Dictionary::setColor(15);
                        }
                        if(++h == height) break;
                    }
                }
                else {
                    if(realCurrentLine == i_lineCounter++) Dictionary::setColor(HIGHLIGHTING);
                    _setmode(_fileno(stdout), _O_U16TEXT);
                    wcout << " " << (wchar_t) 0x2261;
                    _setmode(_fileno(stdout), _O_TEXT);
                    cout << ' ' << vars[i] << ' ';
                    Dictionary::setColor(15);
                    if(++h == height) break;
                }
            }
            else {
                if(realCurrentLine == i_lineCounter++) Dictionary::setColor(HIGHLIGHTING);
                cout << ' ' << vars[i] << ' ';
                Dictionary::setColor(15);
                if(++h == height) break;
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
            folder = "papka__" + it->first + "/";
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
