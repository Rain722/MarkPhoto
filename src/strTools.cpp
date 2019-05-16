#include "include/strTools.h"
#include <io.h>
#include <algorithm>

bool startWith(std::string s, std::string sub) {
    transform(s.begin(),s.end(),s.begin(),::tolower);
    transform(sub.begin(),sub.end(),sub.begin(),::tolower);
    return s.find(sub)==0?1:0;
}

bool endWith(std::string s,std::string sub) {
    transform(s.begin(),s.end(),s.begin(),::tolower);
    transform(sub.begin(),sub.end(),sub.begin(),::tolower);
    return s.rfind(sub)==(s.length()-sub.length())?1:0;
}

//字符串分隔
std::vector<std::string> split(const std::string &s, const std::string &sep)
{
    std::vector<std::string> v;
    std::string::size_type pos1 = 0, pos2 = s.find(sep);
    while(std::string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));
        pos1 = pos2 + sep.size();
        pos2 = s.find(sep, pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
    return v;
}

//获取当前文件夹下指定后缀的文件列表
std::vector<std::string> listFiles(const std::string &string_dir, const std::string &end_str)
{
    char dirNew[200];
    strcpy(dirNew, string_dir.c_str());
    strcat(dirNew, "\\*.*");    // 在目录后面加上"\\*.*"进行第一次搜索
    std::vector<std::string> res;
    intptr_t handle;
    _finddata_t findData;

    handle = _findfirst(dirNew, &findData);
    if (handle == -1)        // 检查是否成功
        return res;
    auto end_str_split = split(end_str, ";");
    do {
        std::string dName = std::string(findData.name);
        if (findData.attrib & _A_SUBDIR)
            continue;
        else {
            for(auto &i : end_str_split) {
                if(endWith(dName, i)) {
                    res.push_back(string_dir + std::string("/")+ dName);
                    break;
                }
            }
        }
    } while (_findnext(handle, &findData) == 0);
    _findclose(handle);    // 关闭搜索句柄
    return res;
}

//获取前缀相同的文件对
std::vector<std::pair<std::string, std::string>>
            getCommonStrPair(std::vector<std::string> &pic_names, std::vector<std::string> &data_names) {
    std::vector<std::pair<std::string, std::string> > pair_arr;
    for(auto &i : pic_names){
        for(auto &j : data_names) {
//            std::cout<<i<<" "<<j<<std::endl;
            auto istart = i.rfind("/"), iend = i.rfind(".");
            auto jstart = j.rfind("/"), jend = j.rfind(".");
            if(iend-istart != jend-jstart)
                continue;
            std::string one = i.substr(istart+1, iend-istart-1);
            std::string two = j.substr(jstart+1, jend-jstart-1);
            if(one == two)
                pair_arr.push_back(std::make_pair(i, j));
        }
    }
    return pair_arr;
}