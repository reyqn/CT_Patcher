#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

int staging(std::fstream &file, std::string (&lineContent)[3], std::vector<std::pair<int,char>> &patch)
{
    std::pair<int,char> linePatch;
    char check;
    int position = std::stoi(lineContent[0], nullptr, 16);
    char oldChar = std::stoi(lineContent[1], nullptr, 16);
    char newChar = std::stoi(lineContent[2], nullptr, 16);
    file.seekg(position);
    file.get(check);
    if (check != oldChar)
    {
        return 1;
    }
    else
    {
        patch.push_back(std::make_pair(position, newChar));
    }
    return 0;
}

int backup(std::fstream &file, std::string filePath)
{
    std::ofstream backup(filePath + ".old", std::ios::binary);
    file.seekg(0);
    backup << file.rdbuf();
    backup.close();
    return 0;
}

int main(int argc, char** argv)
{
    std::string filePath, diffPath, line, log;
    ((argc == 3) ? (filePath = argv[1] , diffPath = argv[2]) : (filePath = "libcocos2d.dll" , diffPath = "patch.dif"));
    std::fstream file(filePath, std::ios::binary | std::ios::in | std::ios::out);
    std::ifstream diff(diffPath);
    std::string lineContent[3];
    std::vector<std::pair<int,char>> patch;

    if (!file.is_open() || !diff.is_open())
    {
        log = "Please place CT_Patcher.exe and " + diffPath + " in your game directory (with " + filePath + ").";
    }
    else
    {
        while (getline(diff, line))
        {
            if (!line.empty() && line.at(0) != '#')
            {
                std::istringstream iss(line);
                if (!(iss >> lineContent[0] >> lineContent[1] >> lineContent[2]) || line.at(0) != '0')
                {
                    std::cout << line << std::endl;
                }
                else
                {
                    if (staging(file, lineContent, patch) != 0)
                    {
                        log = "Unexpected value at offset 0x" + lineContent[0] + ". Has file already been patched?";
                    }
                }
            }
        }
    }

    if (log.empty())
    {
        backup(file, filePath);
        for (auto &it : patch)
        {
            int position = it.first;
            char newChar = it.second;
            file.seekg(position);
            file.write(&newChar, sizeof(newChar));
        }
        log = "Patch applied successfully! Backup of original created (" + filePath + ".old).";
    }

    file.close();
    diff.close();

    std::cout << log;
    std::cin.get();

    return 0;
}


