#include "include/dataReader.h"
#include "include/tinyXML/tinystr.h"
#include "include/tinyXML/tinyxml.h"
#include <regex>
#include <fstream>

bool ReadParaXml(std::string m_strXmlPath, std::vector<BoxSize>& vecNode)
{
    BoxSize *pNode = new BoxSize;
    //读取xml文件中的参数值
    TiXmlDocument* Document = new TiXmlDocument();
    TiXmlElement* RootElement = Document->RootElement();		//根目录
    if(!Document->LoadFile(m_strXmlPath.c_str())) {
        return false;
    }
    TiXmlElement* NextElement = RootElement->FirstChildElement();		//根目录下的第一个节点层
    //for(NextElement;NextElement;NextElement = NextElement->NextSiblingElement())
    while(NextElement!=nullptr)		//判断有没有读完
    {
        if(NextElement->ValueTStr() == "object")		//读到object节点
        {
            //NextElement = NextElement->NextSiblingElement();
            TiXmlElement* BoxElement = NextElement->FirstChildElement();
            while(BoxElement->ValueTStr() != "bndbox")		//读到box节点
            {
                if (BoxElement->ValueTStr() == "name") {
                    pNode->name = BoxElement->GetText();
                }
                BoxElement = BoxElement->NextSiblingElement();
            }
            //索引到xmin节点
            TiXmlElement* xminElemeng = BoxElement->FirstChildElement();
            {
                //分别读取四个数值
                pNode->xMin = atoi(xminElemeng->GetText());
                TiXmlElement* yminElemeng = xminElemeng->NextSiblingElement();
                pNode->yMin = atoi(yminElemeng->GetText());
                TiXmlElement* xmaxElemeng = yminElemeng->NextSiblingElement();
                pNode->xMax = atoi(xmaxElemeng->GetText());
                TiXmlElement* ymaxElemeng = xmaxElemeng->NextSiblingElement();
                pNode->yMax = atoi(ymaxElemeng->GetText());

                //加入到向量中
                vecNode.push_back(*pNode);
            }
        }
        NextElement = NextElement->NextSiblingElement();
    }
    //释放内存
    delete pNode;
    delete Document;
    return true;
}
bool ReadFromTxt(const char* m_strXmlPath, std::vector<BoxSize>& vecNode) {
    if(m_strXmlPath == nullptr)
        return false;
    std::fstream f(m_strXmlPath);
    std::string line;
    while (getline(f, line))
    {
        std::regex rgx("\\s+");
        std::sregex_token_iterator iter(line.begin(), line.end(), rgx, -1);
        std::sregex_token_iterator end;
        std::vector<std::string> v;
        for ( ; iter != end; ++iter)
            v.push_back(*iter);
        BoxSize node;
        node.xMin = stoi(v[0]);
        node.yMin = stoi(v[1]);
        node.xMax = stoi(v[0]) + stoi(v[2]);
        node.yMax = stoi(v[2]) + stoi(v[3]);
        node.Score = stod(v[4]);
        vecNode.push_back(node);
    }
    return true;
}
