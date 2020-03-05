/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"
#include <vector>
#include <stack>
#include <string>

using namespace std;

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
    cout << "==================================================" << endl;
    cout << "= ";
    const CirGate* g = &*this;
    int c = 0;
    if(cirMgr->ispi(g))
    {
        if(g->getid() == 0){cout << "CONST(0)";c += 8;}
        else{cout << "PI(" << g->getid() << ")"; c = c+4+to_string(g->getid()).size();}
        if(g->getname() != "")
        {
            cout << "\"" << g->getname() << "\"";
            c = c+2+g->getname().size();
        }
        cout << ", line " << g->getLineNo();
        c = c+7+to_string(g->getLineNo()).size();
    }
    else if(cirMgr->ispo(g))
    {
        cout << "PO(" << g->getid() << ")" ;
        c = c+4+to_string(g->getid()).size();
        if(g->getname() != "")
        {
            cout << "\"" << g->getname() << "\"";
            c = c+2+g->getname().size();
        }
        cout << ", line " << g->getLineNo();
        c = c+7+to_string(g->getLineNo()).size();
    }
    else if(cirMgr->isand(g))
    {
        cout << "AIG(" << g->getid() << ")" ;
        c = c+5+to_string(g->getid()).size();
        cout << ", line " << g->getLineNo();
        c = c+7+to_string(g->getLineNo()).size();
    }
    cout << setw(48-c) << "=" << endl;
    cout << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
    if(level < 0){return;}
    cirMgr->_level = level;
    printFanin(0,false);
    cirMgr->_isprinted.clear();
}

void
CirGate::printFanin(int level,bool inv) const
{
    if(cirMgr->_level < level){return;}
    for(int i = 0;i<level;i++)
    {
        cout << "  ";
    }
    const CirGate* g = &*this;
    if(cirMgr->ispi(g))
    {
        if(inv == true){cout << "!";}
        if(g->getid() == 0){cout << "CONST 0";}
        else{cout << "PI " << g->getid();}
        cout << endl;
    }
    else if(cirMgr->ispo(g))
    {
        if(inv == true){cout << "!";}
        cout << "PO " << g->getid() << endl;
        if(g->getfin()[0] != NULL)
        {
            if(g->getphase()[0] == 1){g->getfin()[0]->printFanin(level+1,true);} 
            else{g->getfin()[0]->printFanin(level+1,false);}
        }
        else
        {
            for(int i = 0;i<level+1;i++)
            {
                cout << "  ";
            }
            if(g->getphase()[0] == 1){cout << "!";} 
            cout << "UNDEF " << g->getin()[0]/2 << endl;
        }
    }
    else if(cirMgr->isand(g))
    {
        if(inv == true){cout << "!";}
        cout << "AIG " << g->getid();
        for(unsigned int i = 0;i<cirMgr->_isprinted.size();i++)
        {
            if(cirMgr->_isprinted[i] == g->getid())
            {
                if(cirMgr->_level != level){cout << " (*)" << endl;return;}
            }
        }
        cout << endl;
        if(cirMgr->_level != level)
        {
            cirMgr->_isprinted.push_back(g->getid());
        }
        
        if(g->getfin()[0] != NULL)
        {
            if(g->getphase()[0] == 1){g->getfin()[0]->printFanin(level+1,true);} 
            else{g->getfin()[0]->printFanin(level+1,false);}
        }
        else
        {
            for(int i = 0;i<level+1;i++)
            {
                cout << "  ";
            }
            if(g->getphase()[0] == 1){cout << "!";} 
            cout << "UNDEF " << g->getin()[0]/2 << endl;
        }
        if(g->getfin()[1] != NULL)
        {
            if(g->getphase()[1] == 1){g->getfin()[1]->printFanin(level+1,true);} 
            else{g->getfin()[1]->printFanin(level+1,false);}
        }
        else
        {
            for(int i = 0;i<level+1;i++)
            {
                cout << "  ";
            }
            if(g->getphase()[1] == 1){cout << "!";} 
            cout << "UNDEF " << g->getin()[1]/2 << endl;
        }
    }
}

void
CirGate::reportFanout(int level) const
{
    if(level < 0){return;}
    cirMgr->_level = level;
    printFanout(0,false);
    cirMgr->_isprinted.clear();
}

void
CirGate::printFanout(int level,bool inv) const
{
    if(cirMgr->_level < level){return;}
    const CirGate* g = &*this;
    for(int i = 0;i<level;i++)
    {
        cout << "  ";
    }
    if(cirMgr->ispi(g))
    {
        if(inv == true){cout << "!";}
        cout << "PI " << g->getid() << endl;
        for(unsigned int i = 0;i<g->getfout().size();i++)
        {
            for(unsigned int j = 0;j<g->getfout()[i]->getfin().size();j++)
            {
                if(g->getfout()[i]->getfin()[j] == g)
                {
                    if(g->getfout()[i]->getphase()[j] == 1){g->getfout()[i]->printFanout(level+1,true);}
                    else{g->getfout()[i]->printFanout(level+1,false);}
                    break;
                }
            }
        }
    }
    else if(cirMgr->ispo(g))
    {
        if(inv == true){cout << "!";}
        cout << "PO " << g->getid() << endl;
    }
    else if(cirMgr->isand(g))
    {
        if(inv == true){cout << "!";}
        cout << "AIG " << g->getid();
        for(unsigned int i = 0;i<cirMgr->_isprinted.size();i++)
        {
            if(cirMgr->_isprinted[i] == g->getid())
            {
                if(cirMgr->_level != level){cout << " (*)" << endl;return;}
            }
        }
        cout << endl;
        if(cirMgr->_level != level && g->getfout().size() != 0)
        {
            cirMgr->_isprinted.push_back(g->getid());
        }
        for(unsigned int i = 0;i<g->getfout().size();i++)
        {
            for(unsigned int j = 0;j<g->getfout()[i]->getfin().size();j++)
            {
                if(g->getfout()[i]->getfin()[j] == g)
                {
                    if(g->getfout()[i]->getphase()[j] == 1){g->getfout()[i]->printFanout(level+1,true);}
                    else{g->getfout()[i]->printFanout(level+1,false);}
                    break;
                }
            }
        }
    }
}
