/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
    CirGate () {}
    virtual ~CirGate() {}

    // Basic access methods
    string getTypeStr() const {return _type;}
    size_t getid() const {return _id;}
    size_t getLineNo() const {return _lineNo;}
    vector<int> getin() const {return _input;}
    vector<bool> getphase() const {return _phase;}
    vector<CirGate*> getfin() const {return _finList;}
    vector<CirGate*> getfout() const {return _foutList;}
    string getname() const {return _name;}
    
    void setid(size_t id){_id = id;}
    void setline(size_t n){_lineNo = n;}
    void settype(string s){_type = s;}
    void setin(int i){_input.push_back(i);} 
    void setphase(bool i){_phase.push_back(i);}
    void setfin(CirGate* g){_finList.push_back(g);}
    void setfout(CirGate* g)
    {
        _foutList.push_back(g);
        int temp = _foutList.size()-1;
        for(int i = _foutList.size()-2;i>=0;i--)
        {
            if(_foutList[i]->getid() > _foutList[temp]->getid())
            {
                CirGate* t = _foutList[i];
                _foutList[i] = _foutList[temp];
                _foutList[temp] = t;
                temp = i;
            }
        }
    }
    void setname(string s){_name = s;}
    bool _isdfs = false;
    
    // Printing functions
    void reportGate() const;
    void reportFanin(int level) const;
    void reportFanout(int level) const;
    void printinput() const
    {
        for(unsigned int i = 0;i<_input.size();i++)
        {
            cout << _input[i] << endl;
        }
    }
    
    void printFanin(int level,bool inv) const;
    void printFanout(int level,bool inv) const;
    
protected:
    size_t _id;
    size_t _lineNo;
    vector<CirGate*> _finList;
    vector<CirGate*> _foutList;
    vector<int> _input;
    vector<bool> _phase;
    string _name = "";
    string _type;
};

#define CirGateClass(T)                         \
class T: public CirGate                         \
{                                               \
public:                                         \
    T() {}                                      \
    ~T() {}                                     \
}   

CirGateClass(CirAigGate);
CirGateClass(CirPiGate);
CirGateClass(CirPoGate);

#endif // CIR_GATE_H
