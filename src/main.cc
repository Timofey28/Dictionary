#include <iostream>
#include <algorithm>
#include <ctime>
#include <fcntl.h>
#include <fstream>
#include <map>
#include <random>
#include <stack>
#include <set>
#include <thread>
#include <vector>
#include "dictionary.cc"
#include "folder_info.cc"
using namespace std;

FolderInfo folderInfo;

void dirCommand(map<string, vector<string>>& films, string& film_name);
void renew(map<string, vector<string>>& films);

int main()
{
    setlocale(0, "");
    const Colors HIGHLIGHTING = LIGHT_RED; // цветовое изменение строки на которой курсор при выборе фильма из списка
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO structCursorInfo;
    map<string, vector<string>> films;

    another_movie:
    GetConsoleCursorInfo(hConsole, &structCursorInfo);
    structCursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &structCursorInfo);
    vector<string> vars;
    films = folderInfo.GetAllFilms();

    string s, film_name, just_a_name = "";
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
        if(c[0] == 27) return 0;
        if(c[0] == ' ' && !film_name.size() || c[0] == ' ' && film_name.back() == ' ') continue;
        if(c[0] == 13 || c[0] == 9) { // enter || tab
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
                    if(vars.size()) {
                        film_name = vars[0];
                        if(films[vars[0]].size()) film_name += "/all";
                    }
                    if(film_name.back() == ' ') film_name.erase(film_name.size() - 1);
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
        else if(c[0] == 'k') {  // up (для ввода букв k, j нужна русская раскладка)
            if(!vars.size()) continue;
            goto arrowUp;
        }
        else if(c[0] == 'j') {  // down
            if(!vars.size()) continue;
            goto arrowDown;
        }
        else if(c[0] == 11) {  // ctrl + k
            if(!vars.size()) continue;
            goto ctrlArrowUp;
        }
        else if(c[0] == 10) {  // ctrl + j
            if(!vars.size()) continue;
            goto ctrlArrowDown;
        }
        else if(c[0] == -32 && kbhit()) { // стрелки
            changer = _getch();
            if(!vars.size()) continue;
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
            static map<char, char> ru2en = {{'й', 'q'}, {'ц', 'w'}, {'у', 'e'}, {'к', 'r'}, {'е', 't'}, {'н', 'y'}, {'г', 'u'},
            {'ш', 'i'}, {'щ', 'o'}, {'з', 'p'}, {'ф', 'a'}, {'ы', 's'}, {'в', 'd'}, {'а', 'f'}, {'п', 'g'}, {'р', 'h'}, {'о', 'j'},
            {'л', 'k'}, {'д', 'l'}, {'я', 'z'}, {'ч', 'x'}, {'с', 'c'}, {'м', 'v'}, {'и', 'b'}, {'т', 'n'}, {'ь', 'm'}, {'Й', 'Q'},
            {'Ц', 'W'}, {'У', 'E'}, {'К', 'R'}, {'Е', 'T'}, {'Н', 'Y'}, {'Г', 'U'}, {'Ш', 'I'}, {'Щ', 'O'}, {'З', 'P'}, {'Ф', 'A'},
            {'Ы', 'S'}, {'В', 'D'}, {'А', 'F'}, {'П', 'G'}, {'Р', 'H'}, {'О', 'J'}, {'Л', 'K'}, {'Д', 'L'}, {'Я', 'Z'}, {'Ч', 'X'},
            {'С', 'C'}, {'М', 'V'}, {'И', 'B'}, {'Т', 'N'}, {'Ь', 'M'}};
            if(ru2en.find(c[0]) != ru2en.end())
                c[0] = ru2en[c[0]];

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
        if(film_name == "exit" || film_name == "EXIT") return 0;
        if(film_name == "renew" || film_name == "RENEW") {
            setPosition(33, 1);
            setColor(LIGHT_RED);
            cout << film_name;
            renew(films);
            setColor(WHITE);
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
        bool notSuits;
        for(auto it = films.begin(); it != films.end(); ++it) {
            notSuits = 0;
            string film = it->first, input = film_name;
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
                    if(realCurrentLine == i_lineCounter++) setColor(HIGHLIGHTING);
                    cout << " - " << vars[i] << ' ';
                    cout << "\n\t                         ";
                    setColor(WHITE);
                    if(realCurrentLine == i_lineCounter++) setColor(HIGHLIGHTING);
                    _setmode(_fileno(stdout), _O_U16TEXT);
                    wcout << (wchar_t) 0x203E << (wchar_t) 0x203E << (wchar_t) 0x203E;
                    _setmode(_fileno(stdout), _O_TEXT);
                    cout << "|- " << films[vars[i]][0];
                    setColor(WHITE);
                    for(int j = 1; j < films[vars[i]].size() - 1; ++j) {
                        cout << "\n\t                            ";
                        if(realCurrentLine == i_lineCounter++) setColor(HIGHLIGHTING);
                        cout << "|- ";
                        cout << films[vars[i]][j];
                        setColor(WHITE);
                    }
                }
                else {
                    if(realCurrentLine == i_lineCounter++) setColor(HIGHLIGHTING);
                    cout << " ";
                    _setmode(_fileno(stdout), _O_U16TEXT);
                    wcout << (wchar_t) 0x2261;
                    _setmode(_fileno(stdout), _O_TEXT);
                    cout << ' ' << vars[i] << ' ';
                    setColor(WHITE);
                }
            }
            else {
                if(realCurrentLine == i_lineCounter++) setColor(HIGHLIGHTING);
                cout << ' ' << vars[i] << ' ';
                setColor(WHITE);
            }
        }
        setPosition(33 + film_name.size(), 1);
    }
    while(film_name[0] == ' ') film_name.erase(0, 1);
    while(film_name.back() == ' ') film_name.pop_back();
    if(film_name.size() && film_name[0] == '\\') film_name.erase(0, 1);
    if(film_name == "") goto another_movie;
    demonstrationToPath(film_name);
    if(film_name.find(">") != string::npos) film_name[film_name.find(">")] = '/';
    if(film_name.find("/") != string::npos && film_name.substr(0, 7) != "papka__") {
        if(film_name.find("_/_") != string::npos) film_name.replace(film_name.find("/") - 1, 3, "/");
        else if(film_name.find("_/") != string::npos) film_name.replace(film_name.find("/") - 1, 2, "/");
        else if(film_name.find("/_") != string::npos) film_name.replace(film_name.find("/"), 2, "/");
        string folder = "papka__" + film_name.substr(0, film_name.find("/"));
        string file = film_name.substr(film_name.find("/") + 1);
        if(!folderInfo.FolderExists(folder)) {
            mkdir(("Dictionaries/" + folder).c_str());
            ofstream f("Dictionaries/" + folder + "/all.txt");
            f.close();
        }
        folderInfo.AddFile(file, folder);
        film_name = "papka__" + film_name;
    }
    else {
        string file = film_name, folder = "";
        if(file.find("/") != string::npos) {
            file = film_name.substr(film_name.find("/") + 1);
            folder = film_name.substr(0, film_name.find("/"));
        }
        folderInfo.AddFile(file, folder);
    }
    Dictionary d(film_name);
    char choice;
    string folderWriting, fileWriting, path;
    do {
        system("cls");
        path = d.GetFileName();
        if(folderInfo.MovieIsExcluded(path)) setColor(SEA_WAVE);
        else setColor(BEIGE);
        fileWriting = d.GetFileName();
        fileWriting = fileWriting.substr(fileWriting.rfind("/") + 1);
        fileWriting.erase(fileWriting.size() - 4);
        folderWriting = d.GetFolder();
        pathToDemonstration(fileWriting);
        if(folderWriting.size()) {
            folderWriting.erase(0, 7);
            folderWriting.pop_back();
            pathToDemonstration(folderWriting);
            folderWriting[0] = toupper(folderWriting[0]);
            cout << "\n\t<<< " << folderWriting << " -> " << fileWriting << " >>>";
        }
        else {
            fileWriting[0] = toupper(fileWriting[0]);
            cout << "\n\t<<< " << fileWriting << " >>>";
        }

        cout << "\n\tCurrent dictionary size: " << d.GetSize() << "\n\n\t";                        setColor(GREEN);

        if(d.GetFileName() != "Dictionaries/" + d.GetFolder() + "all.txt") {
            cout << "a"; setColor(WHITE); cout << " - Add a new word\n\t";                         setColor(GREEN);
            cout << "m"; setColor(WHITE); cout << " - add a new Meaning to some word\n\t";         setColor(GREEN);
        }
        cout << "s"; setColor(WHITE); cout << " - Show all words with meanings\n\t";               setColor(GREEN);
        cout << "w"; setColor(WHITE); cout << " - show Without meanings\n\t";                      setColor(GREEN);
        if(d.GetFileName() != "Dictionaries/" + d.GetFolder() + "all.txt")
        {cout << "d"; setColor(WHITE); cout << " - Delete some word from dictionary\n\t";          setColor(GREEN);}
        cout << "f"; setColor(WHITE); cout << " - Find a word\n\t";                                setColor(GREEN);
        if(d.GetFileName() != "Dictionaries/all.txt")
        {cout << "v"; setColor(WHITE); cout << " - find among all\n\t";                            setColor(GREEN);}
        cout << "t"; setColor(WHITE); cout << " - deTermine which dictionary the word is in\n\t";  setColor(GREEN);
        cout << "x"; setColor(WHITE); cout << " - pass an eXam\n\t";                               setColor(GREEN);
        if(d.GetFileName() == "Dictionaries/all.txt")
        {cout << "i"; setColor(WHITE); cout << " - Interesting Info\n";                            setColor(GREEN);}
        cout << "\n\t";

        if(d.GetFileName() != "Dictionaries/" + d.GetFolder() + "all.txt")
        {cout << "z"; setColor(WHITE); cout << " - delete this dictionary\n\t";                    setColor(GREEN);}
        cout << "c"; setColor(WHITE); cout << " - Choose another movie\n\t";                       setColor(GREEN);
        cout << "r"; setColor(WHITE); cout << " - foldeRs\n\t";                                    setColor(GREEN);
        if(d.GetFileName() != "Dictionaries/" + d.GetFolder() + "all.txt")
        {cout << "g"; setColor(WHITE); cout << " - change dictionary name\n\t";                    setColor(GREEN);}
        cout << "h"; setColor(WHITE); cout << " - help\n\t";                                       setColor(GREEN);
        cout << "e"; setColor(WHITE); cout << " - Exit\n\n\t";
        cin.sync();
        cin >> choice;
        choice = tolower(choice);
        switch(choice)
        {
            case -28:
            case -108:
            case 'a': if(d.GetFileName() != "Dictionaries/" + d.GetFolder() + "all.txt") d.Add();
                      break;
            case -20:
            case 'm': if(d.GetFileName() != "Dictionaries/" + d.GetFolder() + "all.txt") d.AddSomeMeanings("");
                      break;
            case -21:
            case -101:
            case 's': d.Print();
                      break;
            case -26:
            case -106:
            case 'w': d.PrintOnlyEnglish();
                      break;
            case -94:
            case -126:
            case 'd': if(d.GetFileName() != "Dictionaries/" + d.GetFolder() + "all.txt") d.GetRidOf();
                      break;
            case -96:
            case -112:
            case 'f': d.FindAWord(0);
                      break;
            case -84:
            case 'v': d.FindAWord(1);
                      break;
            case -25:
            case -105:
            case 'x': d.Exam();
                      break;
            case -17:
            case 'z': if(d.GetFileName() != "Dictionaries/" + d.GetFolder() + "all.txt") {
                          if(d.Remov()) goto another_movie;
                      }
                      break;
            case -87:
            case -119:
            case 'q': if(path.substr(path.rfind("/") + 1) == "all.txt") continue;
                      if(folderInfo.MovieIsExcluded(path)) folderInfo.RemoveFromExcluded(path);
                      else folderInfo.AddToExcluded(path);
                      break;
            case -31:
            case -111:
            case 'c': if(folderInfo.MovieIsExcluded(path)) addToFile(d, 0);
                      else addToFile(d, 1);
                      goto another_movie;
                      break;
            case -76:
            case -123:
            case 't': d.Determine();
                      break;
            case -70:
            case 'r': d.FoldersHandler();
                      break;
            case -125:
            case '<': d.FileUpDown('<');
                     break;
            case -98:
            case '>': d.FileUpDown('>');
                     break;
            case -65:
            case 'g': d.ChangeDictionaryName();
                      break;
            case -92:
            case -124:
            case 'l': if(d.GetFileName() != "Dictionaries/" + d.GetFolder() + "all.txt") d.ShowLastAddedWord();
                      break;
            case -82:
            case 'j': if(structCursorInfo.bVisible == TRUE) structCursorInfo.bVisible = FALSE;
                      else structCursorInfo.bVisible = TRUE;
                      SetConsoleCursorInfo(hConsole, &structCursorInfo);
                      break;
            case -24:
            case -104:
            case 'i': if(d.GetFileName() == "Dictionaries/all.txt") d.InterestingInfo();
                      break;
            case -89:
            case -121:
            case 'p': d.CheckDictionariesExclusion();
                      break;
            default:  break;
        }
        if(d.GetFileName() == "") goto another_movie;
    }while(choice != 'e' && choice != -29 && choice != -109);
    if(folderInfo.MovieIsExcluded(d.GetFileName())) addToFile(d, 0);
    else addToFile(d, 1);
    system("cls");

    return 0;
}

void dirCommand(map<string, vector<string>>& films, string& film_name)
{
    if(!films.size()) return;
    const int HIGHLIGHTING = 12;
    int screenHeight = getConsoleWH().second;
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
        screenHeight = getConsoleWH().second;
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
            setColor(HIGHLIGHTING);
            _setmode(_fileno(stdout), _O_U16TEXT);
            wcout << wstrOptions[changeCursor_to];
            _setmode(_fileno(stdout), _O_TEXT);
            cout << options[changeCursor_to];
            setColor(WHITE);
        }
        else {
            system("cls");
            if(firstRowNumber == -1) screenHeight--;
            int startWith = max(0, firstRowNumber);
            for(int i = startWith; i < options.size() && i < screenHeight + startWith; ++i) {
                if(i == startWith && firstRowNumber != -1) cout << "\t";
                else cout << "\n\t";
                if(i == cursorPos) setColor(HIGHLIGHTING);
                _setmode(_fileno(stdout), _O_U16TEXT);
                wcout << wstrOptions[i];
                _setmode(_fileno(stdout), _O_TEXT);
                cout << options[i];
                setColor(WHITE);
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
                demonstrationToPath(path);
                if(!folderInfo.MovieIsExcluded(path)) {
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
            demonstrationToPath(path);
            if(!folderInfo.MovieIsExcluded(path)) {
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
