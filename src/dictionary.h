#pragma once
#include "utils.h"
#include <Windows.h>

class Dictionary
{
    std::map<std::string, std::vector<std::string>> dict, all_dict, innerAll_dict;
    std::string file, innerAll_file, folder;
    std::stack<std::string> last_word;

    void PrintAWord(std::string& word, int spaces_amount = 0, std::string toUpper = "", Colors color = WHITE, bool from_all_dict = 0);
    bool ExamRightAnswer(std::vector<std::string>& answers, std::string answer);
    std::pair<char, char> EnterWord(std::string& meaning, bool isAdding = 0);
    void ReadDic(std::string path, std::map<std::string, std::vector<std::string>>& dic, bool clearItFirst = 1);
    void WriteInDic(std::string path, const std::map<std::string, std::vector<std::string>>& dic);
    std::string ModifyString(std::string s, int width, int indent);
    void FoldersHandlerInstructions(std::string command);
    bool YesOrNo();

public:
    bool isInAll;

    Dictionary(std::string file_name);
    void Print();
    void PrintOnlyEnglish();
    void Add();
    void AddSomeMeanings(std::string word = "");
    void FindAWord(bool findAmongAll = 0);
    void GetRidOf();
    void Exam();
    int GetSize() {return dict.size();}
    std::string GetFolder() { return folder; }
    std::string GetFileName() { return file; }
    bool Remov();
    friend void addToFile(Dictionary& x, bool saveInAll);
    int Determine(std::string word = "");
    void FoldersHandler();
    void FileUpDown(char);
    void ChangeDictionaryName();
    void ShowLastAddedWord();
    void InterestingInfo();
    void CheckDictionariesExclusion();
};
