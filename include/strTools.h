
#ifndef STRTOOLS_H
#define STRTOOLS_H

#include <iostream>
#include <cstring>
#include <vector>

bool startWith(std::string s, std::string sub);

bool endWith(std::string s,std::string sub);

//字符串分隔
std::vector<std::string> split(const std::string &s, const std::string &sep);

//获取当前文件夹下指定后缀的文件列表
std::vector<std::string> listFiles(const std::string &string_dir, const std::string &end_str);

//获取前缀相同的文件对
std::vector<std::pair<std::string, std::string>>
            getCommonStrPair(std::vector<std::string> &pic_names, std::vector<std::string> &data_names);

#endif
