/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"
#include "cirGate.h"

extern CirMgr *cirMgr;

class CirMgr
{
public:
    CirMgr(){}
    ~CirMgr() {}

    // Access functions
    // return '0' if "gid" corresponds to an undefined gate.
    CirGate* getGate(unsigned gid) const {return findgate(gid);}
    CirGate* findgate(size_t id) const;
    bool ispi(const CirGate* g) const;
    bool ispo(const CirGate* g) const;
    bool isand(const CirGate* g) const;
    bool isdfs(CirGate* &g) const;
    
    // Member functions about circuit construction
    bool readCircuit(const string&);
    void connect();
    void genDFSList();
    bool isfrontier(CirGate* g) const;
    
    // Member functions about circuit reporting
    void printSummary() const;
    void printNetlist() const;
    void printPIs() const;
    void printPOs() const;
    void printFloatGates() const;
    void writeAag(ostream&) const;
    void printdfs(int i) const;
    int _level;
    vector<size_t> _isprinted;
        
    void reset();

private:
    bool readheader();
    bool readpi();
    bool readpo();
    bool readand();
    bool readsymbol();
    fstream _file;
    int _max = 0;
    int _pi = 0;
    int _po = 0;
    int _and = 0;
    vector<CirGate*> _piList;
    vector<CirGate*> _poList;
    vector<CirGate*> _andList;
    vector<CirGate*> _dfsList;
    vector<CirGate*> _in;
    static CirGate* const0;
};

#endif // CIR_MGR_H
