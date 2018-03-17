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

int backup(std::fstream &file)
{
    std::ofstream backup("libcocos2d.dll.old", std::ios::binary);
    file.seekg(0);
    backup << file.rdbuf();
    backup.close();
    return 0;
}

int main()
{
    std::fstream file("libcocos2d.dll", std::ios::binary | std::ios::in | std::ios::out);
    std::ifstream diff("patch.dif");
    std::string line, log;
    std::string lineContent[3];
    std::vector<std::pair<int,char>> patch;

    if (!file.is_open() || !diff.is_open())
    {
        log = "Please place CT_Patcher.exe and patch.dif in your game directory (with libcocosd2.dll).";
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
                    std::cout << line + "\n" << std::endl;
                }
                else
                {
                    if (staging(file, lineContent, patch) != 0)
                    {
                        log = "File has already been patched at this offset.";
                    }
                }
            }
        }
    }

    if (log.empty())
    {
        backup(file);
        for (auto &it : patch)
        {
            int position = it.first;
            char newChar = it.second;
            file.seekg(position);
            file.write(&newChar, sizeof(newChar));
        }
        log = "Patch applied succesfully! Backup of original created (libcocos2d.dll.old).";
    }

    file.close();
    diff.close();

    std::cout << log;
    std::cin.get();

    return 0;
}


