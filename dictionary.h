#ifndef DICTIONARY_H
#define DICTIONARY_H
#include <Windows.h>

class Dictionary
{
    std::map<std::string, std::vector<std::string>> dict, all_dict, innerAll_dict;
    std::string file, innerAll_file, folder;
    std::stack<std::string> last_word;
    void printAWord(std::string& word, int spaces_amount = 0, std::string toUpper = "", int color = 15, bool from_all_dict = 0);
    bool examRightAnswer(std::vector<std::string>& answers, std::string answer);
    std::pair<char, char> enterWord(std::string& word, bool isAdding = 0);
    COORD getPos();
    char getChar(short, short);
    void readDic(std::string path, std::map<std::string, std::vector<std::string>>& dic);
    void writeInDic(std::string path, std::map<std::string, std::vector<std::string>>& dic);
    std::string modifyString(std::string s, int width, int indent);
    void getRidOfSpaces(std::string& word, char sym = ' ');
    int numLength(int num);
    void foldersHandlerInstructions(std::string command);
public:
    bool isInAll;
    Dictionary(std::string file_name);
    void print();
    void printOnlyEnglish();
    void add();
    void addSomeMeanings(std::string s);
    void findAWord(bool findAmongAll = 0);
    void getRidOf();
    void exam();
    int getSize() {return dict.size();}
    std::string getFolder() {return folder;}
    std::string getFileName() {return file;}
    bool remov();
    friend void addToFile(Dictionary& x, bool saveInAll);
    static void setColor(int color);
    int determine(std::string word);
    void foldersHandler();
    void fileUpDown(char);
    void changeDictionaryName();
    void showLastAddedWord();
    static void pathToDemonstration(std::string& s);
    static void demonstrationToPath(std::string& s);
};

#endif // DICTIONARY_H
