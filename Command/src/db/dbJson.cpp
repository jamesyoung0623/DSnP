/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "{ " << j._key << " : " << j._value << " }";
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
    // TODO: to read in data from Json file and store them in a DB 
    // - You can assume the input file is with correct JSON file format
    // - NO NEED to handle error file format
    string temp;
    string key,value;
    int ad = key[0];
    vector<string> line;
    unsigned int e = 0;
    while(getline(is,temp))
    {
        line.push_back(temp);
    }
    for(unsigned int i=1;i<(line.size()-1);i++)
    {
        for(unsigned int a=0;a<line[i].length();a++)
        {
            string str = line[i];
            if(int(str[a]) == 125)
            {
                break;
            }
            if(int(str[a]) != 32 && int(str[a]) != 9)
            {
                if(int(str[a]) == 58){e++;}
                if(e == i-1)
                {
                    key.push_back(str[a]);
                    //cout << int(str[a]) << endl;
                }
                else
                {
                    if(int(str[a]) != 58 && int(str[a]) != 44)
                    {
                      value.push_back(str[a]); 
                      //cout << int(str[a]) << endl;
                    }
                    else if(int(str[a]) == 44)
                    {
                        int v;
                        myStr2Int(value,v);
                        DBJsonElem elem(key,v);
                        j._obj.push_back(elem);
                        //cout << key << v;                        
                        key.erase(0,key.size());
                        value.erase(0,value.size());
                    }
                }
            }
        }
    }
    int v;
    myStr2Int(value,v);
    DBJsonElem elem(key,v);
    j._obj.push_back(elem);
    //cout << key << v;
    if(ad==int(key[0]))
    {
        j._obj.pop_back();
    }
    return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
    os << "{" << endl;
    for(size_t i = 0;i<j.size();i++)
    {
        os << "  " << j[i].key() << " : " << j[i].value() << endl;
    }
    os << "}" << endl;
    return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
   _obj.clear();
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
    for(unsigned int i=0;i<_obj.size();i++)
    {
        if(_obj[i].key() == elm.key())
            return false;
    }
    _obj.push_back(elm);
    return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
    float count;
    float ave;
    if(_obj.empty()){return NAN;}
    for(unsigned int i = 0;i<_obj.size();i++)
    {
        count += _obj[i].value();
    }
    ave = count/_obj.size();
    return ave;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
    int maxN = INT_MIN;
    idx = 0;
    if(_obj.size() == 0){idx = _obj.size(); return  maxN;}
    for(unsigned int r = 0;r<_obj.size();r++)
    {
        if(_obj[r].value() > maxN)
        {
            maxN = _obj[r].value();
            idx = r;
        }
    }
    return  maxN;
}

// If DBJson is empty, set idx to size() and return INT_MAX
int
DBJson::min(size_t& idx) const
{
    int minN = INT_MAX;
    idx = 0;
    if(_obj.size() == 0){idx = _obj.size(); return  minN;}
    for(unsigned int r = 0;r<_obj.size();r++)
    {
        if(_obj[r].value() < minN)
        {
            minN = _obj[r].value();
            idx = r;
        }
    }
    return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
    int s = 0;
    if(_obj.size() == 0){return  s;}
    for(unsigned int i=0;i<_obj.size();i++)
    {
        s += _obj[i].value();
    }
    return s;
}
