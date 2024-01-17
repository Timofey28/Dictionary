#pragma once

std::map<char, char> path2title, title2path = {
    {' ',   '_'},
    {':',   ';'},
    {'*',   '#'},
};



class FolderInfo
{
    // в конце названия каждой папки пробел, чтобы можно было иметь папки и файлы с одинаковыми названиями
    std::map<std::string, std::vector<std::string>> elements;
    std::map<std::string, std::set<std::string>> ordinaryFilesOrder;  // только в папках
    std::map<std::string, std::array<std::multimap<int, std::string, std::greater<int>>, 2>> dateFilesOrder;  // тоже для папок

    void CheckFolderAndTechFilesExistence();
    void ArrangeTitlesInOrder(const std::string& folderName, std::vector<std::string>& titles);
    std::string TransformPathIntoMovieTitle(std::string& path, bool isSurelyAFolder = 0);
    bool IsDateWithYear(const std::string& title, int& correspondingValue);
    bool IsDateWithoutYear(const std::string& title, int& correspondingValue);
    void SortSameValues(std::multimap<int, std::string, std::greater<int>>& mm, int value);
public:
    FolderInfo();
    std::map<std::string, std::vector<std::string>> GetAllFilms() { return elements; }
    std::vector<std::string> GetAllFilmsInSingleContainer();
    std::vector<std::string> GetAllPathsInSingleContainer();
    std::vector<std::string> GetFilesFromFolder(std::string folder = "");
    std::vector<std::string> GetAllFolders();
    std::string TransformMovieTitleIntoPath(std::string movieTitle, bool includeMainFolder = 1);
    void AddFile(std::string file, std::string folder = "");
    void AddFolder(std::string folder);
    void DeleteFile(std::string file);
    void DeleteFolder(std::string folder);
    bool FolderExists(std::string folder);
};
