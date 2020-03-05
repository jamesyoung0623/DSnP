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
    CirMgr() {}
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

    // Member functions about circuit optimization
    void sweep();
    void optimize();
    void remove1(CirGate* g);
    void remove2(CirGate* g);
    void remove3(CirGate* g);

    // Member functions about simulation
    void randomSim();
    void fileSim(ifstream&);
    void setSimLog(ofstream *logFile) { _simLog = logFile; }

    // Member functions about fraig
    void strash();
    void merge(CirGate* f, CirGate* g);
    void printFEC() const;
    void fraig();
    
    // Member functions about circuit reporting
    void printSummary() const;
    void printNetlist() const;
    void printPIs() const;
    void printPOs() const;
    void printFloatGates() const;
    void writeAag(ostream&) const;
    void printFECPairs() const;
    void writeGate(ostream&, CirGate*) const;
    void printdfs(int i) const;
    int _level;
    vector<size_t> _isprinted;
    bool _isclean = false;
    bool _isopt = false;
    bool _isstrash = false;
        
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
    CirGate* const0;
    ofstream         *_simLog;

};

#endif // CIR_MGR_H
