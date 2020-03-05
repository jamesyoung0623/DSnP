/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include <algorithm>
#include <vector>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
    if(_isclean){return;}
    vector<int> print;
    for(int i=0;i<_and;i++)
    {
        if(!isdfs(_andList[i]))
        {
            if(_andList[i]->getfin()[0] != NULL)
            {
                for(size_t j=0;j<_andList[i]->getfin()[0]->getfout().size();j++)
                {
                    if(_andList[i]->getfin()[0]->getfout()[j] == _andList[i])
                    {
                        _andList[i]->getfin()[0]->removefout(j);
                        break;
                    }
                }
            }
            if(_andList[i]->getfin()[1] != NULL)
            {
                for(size_t j=0;j<_andList[i]->getfin()[1]->getfout().size();j++)
                {
                    if(_andList[i]->getfin()[1]->getfout()[j] == _andList[i])
                    {
                        _andList[i]->getfin()[1]->removefout(j);
                        break;
                    }
                }
            }
            for(size_t j=0;j<_andList[i]->getfout().size();j++)
            {
                if(_andList[i]->getfout()[j] != NULL)
                {
                    for(size_t k=0;k<_andList[i]->getfout()[j]->getfin().size();k++)
                    {
                        if(_andList[i]->getfout()[j]->getfin()[k] == _andList[i])
                        {
                            _andList[i]->getfout()[j]->removefin(k);
                        }
                    }
                }
            }
            print.push_back(_andList[i]->getid());
            _in[_andList[i]->getid()] = NULL;
            _andList.erase(_andList.begin()+i);
            _and--;
            i--;
        }
    }
    sort(print.begin(),print.end());
    for(size_t i=0;i<print.size();i++)
    {
        cout << "Sweeping: AIG(" << print[i] << ") removed..." << endl;
    }
    _isclean = true;
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
    if(_isopt){return;}
    for(size_t i=0;i<_dfsList.size();i++)
    {
        if(isand(_dfsList[i]))
        {
            if(_dfsList[i]->getfin()[0]->getid() == 0)
            {
                if(_dfsList[i]->getphase()[0] == false)
                {
                    remove2(_dfsList[i]);
                }
                else
                {
                    remove1(_dfsList[i]);
                }
            }
            else if(_dfsList[i]->getfin()[1]->getid() == 0)
            {
                if(_dfsList[i]->getphase()[1] == false)
                {
                    remove2(_dfsList[i]);
                }
                else
                {
                    remove1(_dfsList[i]);
                }
            }
            else if(_dfsList[i]->getfin()[0]->getid() == _dfsList[i]->getfin()[1]->getid())
            {
                remove3(_dfsList[i]);
            }
        }
    }
    for(size_t i=0;i<_andList.size();i++)
    {
        if(!isand(_andList[i]))
        {
            _isclean = false;
            _andList.erase(_andList.begin()+i);
            _and--;
            i--;
        }
    }
    if(!_isclean)
    {
        _isstrash = false;
        _dfsList.clear();
        genDFSList();
    }
    _isopt = true;
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/

void
CirMgr::remove2(CirGate* g)
{
    if(!isand(g) || !isdfs(g)){return;}
    cout << "Simplifying: 0 merging " << g->getid() << "..." << endl;
    if(g->getfin()[0] != NULL)
    {
        for(size_t j=0;j<g->getfin()[0]->getfout().size();j++)
        {
            if(g->getfin()[0]->getfout()[j] == g)
            {
                g->getfin()[0]->removefout(j);
                break;
            }
        }
    }
    if(g->getfin()[1] != NULL)
    {
        for(size_t j=0;j<g->getfin()[1]->getfout().size();j++)
        {
            if(g->getfin()[1]->getfout()[j] == g)
            {
                g->getfin()[1]->removefout(j);
                break;
            }
        }
    }
    for(size_t j=0;j<g->getfout().size();j++)
    {
        for(size_t k=0;k<g->getfout()[j]->getfin().size();k++)
        {
            if(g->getfout()[j]->getfin()[k] == g)
            {
                g->getfout()[j]->changefin(k,const0,false);
                const0->addfout(g->getfout()[j]);
            }
        }
    }
    _in[g->getid()] = NULL;
    g->settype("");
}

void
CirMgr::remove1(CirGate* g)
{
    if(!isand(g) || !isdfs(g)){return;}
    if(g->getfin()[0] != NULL)
    {
        for(size_t j=0;j<g->getfin()[0]->getfout().size();j++)
        {
            if(g->getfin()[0]->getfout()[j] == g)
            {
                g->getfin()[0]->removefout(j);
                break;
            }
        }
    }
    if(g->getfin()[1] != NULL)
    {
        for(size_t j=0;j<g->getfin()[1]->getfout().size();j++)
        {
            if(g->getfin()[1]->getfout()[j] == g)
            {
                g->getfin()[1]->removefout(j);
                break;
            }
        }
    }
    if(g->getfin()[0]->getid() == 0)
    {
        if(g->getphase()[1] == false)
        {cout << "Simplifying: " << g->getfin()[1]->getid() << " merging " << g->getid() << "..." << endl;}
        else
        {cout << "Simplifying: " << g->getfin()[1]->getid() << " merging !" << g->getid() << "..." << endl;}
        for(size_t j=0;j<g->getfout().size();j++)
        {
            for(size_t k=0;k<g->getfout()[j]->getfin().size();k++)
            {
                if(g->getfout()[j]->getfin()[k] == g)
                {
                    g->getfout()[j]->changefin(k,g->getfin()[1],g->getphase()[1]);
                    g->getfin()[1]->addfout(g->getfout()[j]);
                }
            }
        }
    }
    else
    {
        if(g->getphase()[0] == false)
        {cout << "Simplifying: " << g->getfin()[0]->getid() << " merging " << g->getid() << "..." << endl;}
        else
        {cout << "Simplifying: " << g->getfin()[0]->getid() << " merging !" << g->getid() << "..." << endl;}
        for(size_t j=0;j<g->getfout().size();j++)
        {
            for(size_t k=0;k<g->getfout()[j]->getfin().size();k++)
            {
                if(g->getfout()[j]->getfin()[k] == g)
                {
                    g->getfout()[j]->changefin(k,g->getfin()[0],g->getphase()[0]);
                    g->getfin()[0]->addfout(g->getfout()[j]);
                }
            }
        }
    }
    _in[g->getid()] = NULL;
    g->settype("");
}

void
CirMgr::remove3(CirGate* g)
{
    if(!isand(g) || !isdfs(g)){return;}
    if(g->getfin()[0] != NULL)
    {
        for(size_t j=0;j<g->getfin()[0]->getfout().size();j++)
        {
            if(g->getfin()[0]->getfout()[j] == g)
            {
                g->getfin()[0]->removefout(j);
                break;
            }
        }
    }
    if(g->getfin()[1] != NULL)
    {
        for(size_t j=0;j<g->getfin()[1]->getfout().size();j++)
        {
            if(g->getfin()[1]->getfout()[j] == g)
            {
                g->getfin()[1]->removefout(j);
                break;
            }
        }
    }
    if(g->getphase()[0] == g->getphase()[1])
    {
        if(g->getphase()[0] == false)
        {cout << "Simplifying: " << g->getfin()[0]->getid() << " merging " << g->getid() << "..." << endl;}
        else
        {cout << "Simplifying: " << g->getfin()[0]->getid() << " merging !" << g->getid() << "..." << endl;}
        for(size_t j=0;j<g->getfout().size();j++)
        {
            for(size_t k=0;k<g->getfout()[j]->getfin().size();k++)
            {
                if(g->getfout()[j]->getfin()[k] == g)
                {
                    g->getfout()[j]->changefin(k,g->getfin()[0],g->getphase()[0]);
                    g->getfin()[0]->addfout(g->getfout()[j]);
                }
            }
        }
    }
    else
    {
        cout << "Simplifying: 0 merging " << g->getid() << "..." << endl;
        for(size_t j=0;j<g->getfout().size();j++)
        {
            for(size_t k=0;k<g->getfout()[j]->getfin().size();k++)
            {
                if(g->getfout()[j]->getfin()[k] == g)
                {
                    g->getfout()[j]->changefin(k,const0,false);
                    const0->addfout(g->getfout()[j]);
                }
            }
        }
    }
    _in[g->getid()] = NULL;
    g->settype("");
}