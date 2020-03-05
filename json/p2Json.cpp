/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "p2Json.h"

using namespace std;
bool
Json::read(const string& jsonFile)
{
    string temp;
    string key,value;
    int ad = key[0];
    vector<string> line;
    int e = 0;
    fstream file;
    file.open(jsonFile);
    if(file.is_open())
    {
        while(getline(file,temp))
        {
            line.push_back(temp);
        }
        file.close();
    }
    for(int i=1;i<(line.size()-1);i++)
    {
        for(int j=0;j<line[i].length();j++)
        {
            string str = line[i];
            if(int(str[j]) == 125)
            {
                break;
            }
            if(int(str[j]) != 32 && int(str[j]) != 9)
            {
                if(int(str[j]) == 58){e++;}
                if(e == i-1)
                {
                    key.push_back(str[j]);
                    //cout << int(str[j]) << endl;
                }
                else
                {
                    if(int(str[j]) != 58 && int(str[j]) != 44)
                    {
                      value.push_back(str[j]); 
                      //cout << int(str[j]) << endl;
                    }
                    else if(int(str[j]) == 44)
                    {
                        int v = atoi(value.c_str());
                        JsonElem j(key,v);
                        _obj.push_back(j);
                        //cout << key << endl;
                        //cout << v << endl;
                        key.erase(0,key.size());
                        value.erase(0,value.size());
                    }
                }
            }
        }
    }
    int v = atoi(value.c_str());
    JsonElem j(key,v);
    _obj.push_back(j);
    if(ad==int(key[0]))
    {
        _obj.pop_back();
    }
    return true;
}

ostream&
operator << (ostream& os, const JsonElem& j)
{
   return (os << "\"" << j._key << "\" : " << j._value);
}

