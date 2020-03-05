/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include <limits>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;
// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
    if(_isstrash){return;}
    HashMap<HashKey, CirGate*> hash(4*_max);
    for(size_t i=0;i<_dfsList.size();i++)
    {
        if(isand(_dfsList[i]))
        {
            HashKey k(_dfsList[i]->getin()[0],_dfsList[i]->getin()[1]);
            CirGate* mergeGate;
            if(hash.query(k, mergeGate) == true)
            {
                merge(mergeGate,_dfsList[i]);
            }
            else
            {
                hash.insert(k, _dfsList[i]); 
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
        _isopt = false;
        _dfsList.clear();
        genDFSList();
    }
    _isstrash = true;
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
void
CirMgr::merge(CirGate* f,CirGate* g)
{
    for(size_t i=0;i<g->getfout().size();i++)
    {
        for(size_t j=0;j<g->getfout()[i]->getfin().size();j++)
        {
            if(g->getfout()[i]->getfin()[j] == g)
            {
                g->getfout()[i]->changefin(j,f,false);
                f->addfout(g->getfout()[i]);
            }
        }
    }
    for(size_t i=0;i<g->getfin().size();i++)
    {
        if(g->getfin()[i] != NULL)
        {
            for(size_t j=0;j<g->getfin()[i]->getfout().size();j++)
            {
                if(g->getfin()[i]->getfout()[j] == g)
                {
                    g->getfin()[i]->removefout(j);
                }
            }
        }
    }
    _in[g->getid()] = NULL;
    g->settype("");
    cout << "Strashing: " << f->getid() << " merging " << g->getid() << "..." << endl;
}