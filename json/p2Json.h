/****************************************************************************
  FileName     [ p2Json.h]
  PackageName  [ p2 ]
  Synopsis     [ Header file for class Json JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef P2_JSON_H
#define P2_JSON_H

#include <vector>
#include <string>
#include <unordered_set>

using namespace std;

class JsonElem
{
public:
   JsonElem() {}
   JsonElem(const string& k, int v): _key(k), _value(v) {}
   string key(){return _key;}
   int value(){return _value;}
   friend ostream& operator << (ostream&, const JsonElem&);

private:
   string  _key;   // DO NOT change this definition. Use it to store key.
   int     _value; // DO NOT change this definition. Use it to store value.
};

class Json
{
public:
   Json() {}
   bool read(const string&);
   void printobj()
   {
        cout << "{" << endl;
        for(unsigned int i=0;i<_obj.size();i++)
        {
            cout << "  " << _obj[i].key() << " : " <<  _obj[i].value();
            if(i != _obj.size()-1)
            {
                cout << ",";
            }
            cout << endl;
        }
        cout << "}" << endl; 
   }
   int get(){return _obj.size();}
   int sum(){
        int sum = 0; 
        for(unsigned int i=0 ;i<_obj.size() ;i++)
        {
            sum = sum + _obj[i].value();
        }
        return sum;
   }
   int max(){
        int m = _obj[0].value(); 
        int r = 0;
        for(unsigned int i=0 ;i<_obj.size() ;i++)
        {
            if(_obj[i].value()>m)
            {
                m = _obj[i].value();
                r = i;
            }
        }
        return r;
   }
   int min(){
        int m = _obj[0].value();
        int r = 0;
        for(unsigned int i=0 ;i<_obj.size() ;i++)
        {
            if(_obj[i].value()<m)
            {m = _obj[i].value();
            r = i;}
        }
        return r;
   }
   float ave(){
        float sum = 0; 
        for(unsigned int i=0 ;i<_obj.size() ;i++)
        {
            sum = sum + _obj[i].value();
        }
        return (sum/_obj.size());
   }
   void add(JsonElem j){
            _obj.push_back(j);
        }
   string k(int i){
        return _obj[i].key();
   }
   int v(int i){
        return _obj[i].value();
   }
private:
   vector<JsonElem>       _obj;  // DO NOT change this definition.
                                 // Use it to store JSON elements.
};



#endif // P2_TABLE_H
