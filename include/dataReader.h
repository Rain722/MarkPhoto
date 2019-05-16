
#ifndef DATAREADER_H
#define DATAREADER_H

#include <string.h>
#include <cstring>
#include <iostream>
#include <vector>

struct BoxSize
{
    std::string name;
    int xMin, yMin;
    int xMax, yMax;
    double Score;
    bool friend operator < (BoxSize a, BoxSize b){
        return a.Score > b.Score;
    }
};

//XML读取
bool ReadParaXml(std::string m_strXmlPath, std::vector<BoxSize>& vecNode);

//txt读取
bool ReadFromTxt(const char* m_strXmlPath, std::vector<BoxSize>& vecNode);

#endif
