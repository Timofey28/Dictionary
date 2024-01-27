#include "folder_info.h"
#include <dirent.h>
#include <cassert>
using namespace std;

FolderInfo::FolderInfo()
{
    CheckFolderAndTechFilesExistence();
    for(auto it = title2path.begin(); it != title2path.end(); ++it) {
        path2title[it->second] = it->first;
    }

    ReadExcluded();
    WriteExcluded();

    DIR *dir, *innerDir;
    dirent *entry;
    dir = opendir("Dictionaries");
    string currentFile, currentFolder;
    int correspondingValue;
    while((entry = readdir(dir)) != nullptr) {
        currentFile = entry->d_name;
        if(currentFile == "." || currentFile == ".." || currentFile == "__excluded__.txt") continue;

        if(currentFile.substr(0, 7) == "papka__") {
            assert(currentFile.substr(currentFile.size() - 4) != ".txt");
            currentFolder = currentFile;
            innerDir = opendir(("Dictionaries/" + currentFolder).c_str());
            if(!innerDir) {
                perror("Dictionaries");
                exit(-1);
            }
            currentFolder = TransformPathIntoMovieTitle(currentFolder);
            dateFilesOrder[currentFolder];
            while((entry = readdir(innerDir)) != nullptr) {
                currentFile = entry->d_name;
                if(currentFile == "." || currentFile == "..") continue;
                assert(currentFile.substr(currentFile.size() - 4) == ".txt");
                currentFile = TransformPathIntoMovieTitle(currentFile);
                if(IsDateWithYear(currentFile, correspondingValue)) {
                    bool moreThanOne = dateFilesOrder[currentFolder][0].find(correspondingValue) != dateFilesOrder[currentFolder][0].end();
                    dateFilesOrder[currentFolder][0].insert(make_pair(correspondingValue, currentFile));
                    if(moreThanOne) SortSameValues(dateFilesOrder[currentFolder][0], correspondingValue);
                }
                else if(IsDateWithoutYear(currentFile, correspondingValue)) {
                    bool moreThanOne = dateFilesOrder[currentFolder][1].find(correspondingValue) != dateFilesOrder[currentFolder][1].end();
                    dateFilesOrder[currentFolder][1].insert(make_pair(correspondingValue, currentFile));
                    if(moreThanOne) SortSameValues(dateFilesOrder[currentFolder][1], correspondingValue);
                }
                else {
                    elements[currentFolder].push_back(currentFile);
                    if(currentFile != "all") ordinaryFilesOrder[currentFolder].insert(currentFile);
                }
            }
            ArrangeTitlesInOrder(currentFolder, elements[currentFolder]);
        }
        else elements[TransformPathIntoMovieTitle(currentFile)];
    }
    closedir(dir);
}

void FolderInfo::ArrangeTitlesInOrder(const string& folderName, vector<string>& titles)
{
    vector<string> dateFiles;
    for(auto it = dateFilesOrder[folderName][0].begin(); it != dateFilesOrder[folderName][0].end(); ++it) {
        dateFiles.push_back(it->second);
    }
    titles.insert(titles.begin(), dateFiles.begin(), dateFiles.end());

    dateFiles.clear();
    for(auto it = dateFilesOrder[folderName][1].begin(); it != dateFilesOrder[folderName][1].end(); ++it) {
        dateFiles.push_back(it->second);
    }
    titles.insert(titles.begin(), dateFiles.begin(), dateFiles.end());

    for(auto it = titles.begin(); it != titles.end(); ++it) {
        if(*it == "all") {
            rotate(titles.begin(), it, it + 1);
            break;
        }
    }
}

void FolderInfo::ReadExcluded()
{
    ifstream fin("Dictionaries/__excluded__.txt");
    assert(fin.is_open());
    string line, path;
    while(getline(fin, line)) {
        getRidOfSpaces(line);
        if(line.find(" -> ") != string::npos) line.replace(line.find(" -> "), 4, "/");
        else if(line.find(" ->") != string::npos) line.replace(line.find(" ->"), 3, "/");
        else if(line.find("-> ") != string::npos) line.replace(line.find("-> "), 3, "/");
        else if(line.find(" / ") != string::npos) line.replace(line.find(" / "), 3, "/");
        else if(line.find(" /") != string::npos) line.replace(line.find(" /"), 2, "/");
        else if(line.find("/ ") != string::npos) line.replace(line.find("/ "), 2, "/");
        path = TransformMovieTitleIntoPath(line);
        excluded.insert(path);
    }
    fin.close();
}

void FolderInfo::WriteExcluded()
{
    ofstream fout("Dictionaries/__excluded__.txt");
    string movie;
    for(auto path : excluded) {
        movie = TransformPathIntoMovieTitle(path);
        if(movie.find("/") != string::npos) movie.replace(movie.find("/"), 1, " -> ");
        fout << movie << '\n';
    }
    fout.close();
}

bool FolderInfo::MovieIsExcluded(string pathOrTitle)
{
    string path = TransformMovieTitleIntoPath(pathOrTitle);
    return excluded.find(path) != excluded.end();
}

void FolderInfo::AddToExcluded(string pathOrTitle)
{
    string path = TransformMovieTitleIntoPath(pathOrTitle);
    if(excluded.find(path) == excluded.end()) {
        excluded.insert(path);
        WriteExcluded();
    }
}

void FolderInfo::RemoveFromExcluded(string pathOrTitle)
{
    string path = TransformMovieTitleIntoPath(pathOrTitle);
    if(excluded.find(path) != excluded.end()) {
        excluded.erase(path);
        WriteExcluded();
    }
}

void FolderInfo::AddFile(string file, string folder)
{
    assert(folder.substr(0, 7) == "papka__" || folder == "");
    int correspondingValue;
    file = TransformPathIntoMovieTitle(file);
    folder = TransformPathIntoMovieTitle(folder, 1);

    if(folder == "") {
        if(elements.find(file) == elements.end()) elements[file];
        return;
    }

    if(elements.find(folder) != elements.end() &&
       find(elements[folder].begin(), elements[folder].end(), file) != elements[folder].end()) return;

    if(IsDateWithYear(file, correspondingValue)) {
        if(elements.find(folder) == elements.end()) {
            elements[folder].push_back("all");
            ordinaryFilesOrder[folder];
        }
        bool moreThanOne = dateFilesOrder[folder][0].find(correspondingValue) != dateFilesOrder[folder][0].end();
        dateFilesOrder[folder][0].insert(make_pair(correspondingValue, file));
        if(moreThanOne) SortSameValues(dateFilesOrder[folder][0], correspondingValue);
        assert(find(elements[folder].begin(), elements[folder].end(), "all") != elements[folder].end());

        int place = 0;
        for(auto it = dateFilesOrder[folder][0].begin(); it != dateFilesOrder[folder][0].end(); ++it, ++place) {
            if(it->first == correspondingValue && it->second == file) break;
        }
        place += 1 + dateFilesOrder[folder][1].size();
        elements[folder].insert(elements[folder].begin() + place, file);
    }
    else if(IsDateWithoutYear(file, correspondingValue)) {
        if(elements.find(folder) == elements.end()) {
            elements[folder].push_back("all");
            ordinaryFilesOrder[folder];
        }
        bool moreThanOne = dateFilesOrder[folder][1].find(correspondingValue) != dateFilesOrder[folder][1].end();
        dateFilesOrder[folder][1].insert(make_pair(correspondingValue, file));
        if(moreThanOne) SortSameValues(dateFilesOrder[folder][1], correspondingValue);
        assert(find(elements[folder].begin(), elements[folder].end(), "all") != elements[folder].end());

        int place = 0;
        for(auto it = dateFilesOrder[folder][1].begin(); it != dateFilesOrder[folder][1].end(); ++it, ++place) {
            if(it->first == correspondingValue && it->second == file) break;
        }
        place += 1;
        elements[folder].insert(elements[folder].begin() + place, file);
    }
    else if(file == "all") {
        if(elements.find(folder) == elements.end()) {
            elements[folder].push_back("all");
        }
    }
    else {
        if(elements.find(folder) == elements.end()) {
            elements[folder].push_back("all");
            dateFilesOrder[folder];
        }
        ordinaryFilesOrder[folder].insert(file);
        assert(find(elements[folder].begin(), elements[folder].end(), "all") != elements[folder].end());

        int place = 0;
        for(auto it = ordinaryFilesOrder[folder].begin(); it != ordinaryFilesOrder[folder].end(); ++it, ++place) {
            if(*it == file) break;
        }
        place += 1 + dateFilesOrder[folder][1].size() + dateFilesOrder[folder][0].size();
        elements[folder].insert(elements[folder].begin() + place, file);
    }
}

void FolderInfo::DeleteFile(string file)
{
    if(file.substr(0, 13) == "Dictionaries/") file.erase(0, 13);
    string folder = "";
    if(file.find("/") != string::npos) {
        folder = file.substr(0, file.find("/"));
        file = file.substr(file.find("/") + 1);
        folder = TransformPathIntoMovieTitle(folder, 1);
        file = TransformPathIntoMovieTitle(file);
    }
    else file = TransformPathIntoMovieTitle(file);
    if(folder == "") {
        auto it = elements.find(file);
        if(it != elements.end()) elements.erase(it);
    }
    else {
        assert(elements.find(folder) != elements.end());
        for(auto it = elements[folder].begin(); it != elements[folder].end(); ++it) {
            if(*it == file) {
                elements[folder].erase(it);
                break;
            }
        }
        int value;
        if(IsDateWithYear(file, value)) {
            for(auto it = dateFilesOrder[folder][0].begin(); it != dateFilesOrder[folder][0].end(); ++it) {
                if(it->first == value && it->second == file) {
                    dateFilesOrder[folder][0].erase(it);
                    break;
                }
            }
        }
        else if(IsDateWithoutYear(file, value)) {
            for(auto it = dateFilesOrder[folder][1].begin(); it != dateFilesOrder[folder][1].end(); ++it) {
                if(it->first == value && it->second == file) {
                    dateFilesOrder[folder][1].erase(it);
                    break;
                }
            }
        }
        else ordinaryFilesOrder[folder].erase(file);
    }
}

void FolderInfo::AddFolder(string folder)
{
    folder = TransformPathIntoMovieTitle(folder, 1);
    if(elements.find(folder) == elements.end() ||
       find(elements[folder].begin(), elements[folder].end(), "all") == elements[folder].end())
    {
        // вторая часть условия на случай если папка уже существует, но файла "all.txt" по каким-то причинам нет
        elements[folder].push_back("all");
    }
}

void FolderInfo::DeleteFolder(string folder)
{
    folder = TransformPathIntoMovieTitle(folder, 1);
    auto it = elements.find(folder);
    if(it != elements.end()) elements.erase(it);
}

bool FolderInfo::IsDateWithYear(const string& title, int& correspondingValue)
{
    set<int> digitIndexes = {0, 1,  3, 4,  6, 7};
    bool hasFullYear = 0;
    if(title.size() < 8) return 0;
    for(int i = 0; i < 8; ++i) {
        if(digitIndexes.find(i) != digitIndexes.end() && !isdigit(title[i])) return 0;
        if(digitIndexes.find(i) == digitIndexes.end() && title[i] != '.') return 0;
    }
    if(title.size() >= 10) {
        if(isdigit(title[8]) && isdigit(title[9])) hasFullYear = 1;
    }
    int year, month, day;
    if(hasFullYear) year = atoi(title.substr(title.rfind(".") + 1, 4).c_str());
    else year = atoi(title.substr(title.rfind(".") + 1, 2).c_str());
    month = atoi(title.substr(title.find(".") + 1, 2).c_str());
    day = atoi(title.substr(0, 2).c_str());
    if(!hasFullYear) year += 2000;
    correspondingValue = year * 366 + month * 31 + day;
    return 1;
}

bool FolderInfo::IsDateWithoutYear(const string& title, int& correspondingValue)
{
    if(title.size() < 5) return 0;
    for(int i = 0; i < 5; ++i) {
        if(i != 2 && !isdigit(title[i])) return 0;
        if(i == 2 && title[i] != '.') return 0;
    }
    if(title.size() >= 8) {
        if(title[5] == '.' && isdigit(title[6]) && isdigit(title[7])) return 0;
    }
    int month = atoi(title.substr(3, 2).c_str()),
        day = atoi(title.substr(0, 2).c_str());
    correspondingValue = month * 31 + day;
    return 1;
}

string FolderInfo::TransformMovieTitleIntoPath(string movieTitle, bool includeMainFolder)
{
    assert(movieTitle != "");
    for(int i = 0; i < movieTitle.size(); ++i) {
        if(title2path.find(movieTitle[i]) != title2path.end())
            movieTitle[i] = title2path[movieTitle[i]];
    }
    if(movieTitle.substr(0, 13) != "Dictionaries/" && movieTitle.find("/") != string::npos && movieTitle.substr(0, 7) != "papka__")
        movieTitle = "papka__" + movieTitle;
    if(includeMainFolder && movieTitle.substr(0, 13) != "Dictionaries/") movieTitle = "Dictionaries/" + movieTitle;
    if(movieTitle.size() < 4 || movieTitle.substr(movieTitle.size() - 4) != ".txt") movieTitle += ".txt";
    return movieTitle;
}

string FolderInfo::TransformPathIntoMovieTitle(string path, bool isSurelyAFolder)
{
    if(path == "") return path;
    if(path.substr(0, 13) == "Dictionaries/") path.erase(0, 13);
    if(path.back() == '/') path.pop_back();
    if(path.find("/") == string::npos) {
        if(path.substr(0, 7) == "papka__") {
            path.erase(0, 7);
            path += " ";
        }
        if(path.size() >= 4 && path.substr(path.size() - 4) == ".txt") path.erase(path.size() - 4);
        if(isSurelyAFolder && path.back() != ' ') path += " ";
    }
    else {
        assert(path.substr(0, 7) == "papka__");
        path.erase(0, 7);
        if(path.size() >= 4 && path.substr(path.size() - 4) == ".txt") path.erase(path.size() - 4);
    }
    for(int i = 0; i < path.size(); ++i) {
        if(path2title.find(path[i]) != path2title.end())
            path[i] = path2title[path[i]];
    }
    return path;
}

void FolderInfo::SortSameValues(multimap<int, string, greater<int>>& mm, int value)
{
    set<string> dateTitles;
    while(1) {
        auto it = mm.find(value);
        if(it == mm.end()) break;
        dateTitles.insert(it->second);
        mm.erase(it);
    }
    for(auto it = dateTitles.begin(); it != dateTitles.end(); ++it) {
        mm.insert(make_pair(value, *it));
    }
}

bool FolderInfo::FolderExists(string folder)
{
    folder = TransformPathIntoMovieTitle(folder, 1);
    return elements.find(folder) != elements.end();
}

vector<string> FolderInfo::GetAllFilmsInSingleContainer()
{
    vector<string> result;
    for(auto it = elements.begin(); it != elements.end(); ++it) {
        if(it->second.size()) {
            for(const auto& film : it->second)
                result.push_back(it->first.substr(0, it->first.size() - 1) + "/" + film);
        }
        else result.push_back(it->first);
    }
    return result;
}

vector<string> FolderInfo::GetAllPathsInSingleContainer()
{
    vector<string> result;
    string path;
    for(auto it = elements.begin(); it != elements.end(); ++it) {
        if(it->second.size()) {
            for(auto film : it->second) {
                path = it->first.substr(0, it->first.size() - 1) + "/" + film;
                result.push_back(TransformMovieTitleIntoPath(path));
            }
        }
        else {
            path = it->first;
            result.push_back(TransformMovieTitleIntoPath(path));
        }
    }
    return result;
}

vector<string> FolderInfo::GetFilesFromFolder(string folder)
{
    if(folder == "") {
        vector<string> files;
        for(auto it = elements.begin(); it != elements.end(); ++it) {
            if(!it->second.size()) files.push_back(it->first);
        }
        return files;
    }
    else {
        folder = TransformPathIntoMovieTitle(folder, 1);
        assert(elements.find(folder) != elements.end());
        return elements[folder];
    }
}

vector<string> FolderInfo::GetAllFolders()
{
    vector<string> folders;
    for(auto it = elements.begin(); it != elements.end(); ++it) {
        if(it->second.size()) folders.push_back(it->first.substr(0, it->first.size() - 1));
    }
    return folders;
}

void FolderInfo::CheckFolderAndTechFilesExistence()
{
    DIR *dir = opendir("Dictionaries");
    if(!dir) mkdir("Dictionaries");
    else closedir(dir);
    ofstream fout;
    fout.open("Dictionaries/all.txt", ios::app);
    fout.close();
    fout.open("Dictionaries/__excluded__.txt", ios::app);
    fout.close();
}
