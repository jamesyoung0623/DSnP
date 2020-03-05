/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <fstream>
#include <stack>
#include <bits/stdc++.h> 

using namespace std;


/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
    _file.open(fileName);
    if(!_file.is_open()){cerr << "Cannot open design “" << fileName << "”!!" << endl;return false;}
    const0 = new CirPiGate();
    const0->setid(0);
    const0->settype("PI");
    const0->_isdfs = false;
    _piList.push_back(const0);
    _in.push_back(const0);
    if(!readheader()){return false;}
    if(!readpi()){return false;}
    if(!readpo()){return false;}
    if(!readand()){return false;}
    if(!readsymbol()){return false;}
    connect();
    genDFSList();
    return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
void
CirMgr::printSummary() const
{
    cout << endl;
    cout << "Circuit Statistics" << endl;
    cout << "==================" << endl;
    cout << "  PI" << setw(12) << _pi << endl;
    cout << "  PO" << setw(12) << _po << endl;
    cout << "  AIG" << setw(11) << _and << endl;
    cout << "------------------" << endl;
    cout << "  Total" << setw(9) << _pi+_po+_and << endl;
}

void
CirMgr::printNetlist() const
{
    cout << endl;
    for(unsigned int i = 0;i<_dfsList.size();i++)
    {
        cout << "[" << i << "] " ;
        printdfs(i);
    }
}

void
CirMgr::printPIs() const
{
    cout << "PIs of the circuit:";
    for(unsigned int i = 1;i<_piList.size();i++)
    {
        cout << " " << _piList[i]->getid();
    }
    cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
    for(unsigned int i = 0;i<_poList.size();i++)
    {
        cout << " " << _poList[i]->getid();
    }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
    vector<int> t;
    for(unsigned int i = 0;i<_andList.size();i++)
    {
        if(_andList[i]->getfin()[0] == NULL || _andList[i]->getfin()[1] == NULL){t.push_back(_andList[i]->getid());}
    }
    for(unsigned int i = 0;i<_poList.size();i++)
    {
        if(_poList[i]->getfin()[0] == NULL){t.push_back(_poList[i]->getid());}
    }
    sort(t.begin(), t.end());
    if(t.size() > 0)
    {
        cout << "Gates with floating fanin(s):";
        for(unsigned int i = 0;i<t.size();i++)
        {
            cout << " " << t[i];
        }
        cout << endl;
    }
    t.clear();
    
    for(unsigned int i = 1;i<_piList.size();i++)
    {
        if(_piList[i]->getfout().size() == 0){t.push_back(_piList[i]->getid());}
    }
    for(unsigned int i = 0;i<_andList.size();i++)
    {
        if(_andList[i]->getfout().size() == 0){t.push_back(_andList[i]->getid());}
    }
    sort(t.begin(), t.end());
    if(t.size() > 0)
    {
        cout << "Gates defined but not used  :";
        for(unsigned int i = 0;i<t.size();i++)
        {
            cout << " " << t[i];
        }
        cout << endl;
    }
    t.clear();
}

void
CirMgr::writeAag(ostream& outfile) const
{
    int a = 0;
    for(unsigned int i = 0;i<_dfsList.size();i++)
    {
        if(isand(_dfsList[i])){a++;}
    }
    outfile << "aag " << _max << " " << _pi << " 0 " << _po << " " << a << endl;
    for(unsigned int i = 1;i<_piList.size();i++)
    {
        outfile << _piList[i]->getid()*2 << endl;
    }
    for(unsigned int i = 0;i<_poList.size();i++)
    {
        outfile << _poList[i]->getin()[0]<< endl;
    }
    for(unsigned int i = 0;i<_dfsList.size();i++)
    {
        if(isand(_dfsList[i]))
        {
            outfile << _dfsList[i]->getid()*2 << " " << _dfsList[i]->getin()[0] << " " << _dfsList[i]->getin()[1] << endl;
        }
    }
    a = 0;
    for(unsigned int i = 1;i<_piList.size();i++)
    {
        if(_piList[i]->getname() != "")
        {
            outfile << "i" << a << " " << _piList[i]->getname() << endl;
            a++;
        }
    }
    a = 0;
    for(unsigned int i = 0;i<_poList.size();i++)
    {
        if(_poList[i]->getname() != "")
        {
            outfile << "o" << a << " " << _poList[i]->getname() << endl;
            a++;
        }
    }
    outfile << "c" << endl << "AAG output by Chin-Chia (James) Yang" << endl; 
}

void
CirMgr::printFECPairs() const
{
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
}

void 
CirMgr::printdfs(int i) const
{
    if(ispi(_dfsList[i]))
    {
        if(_dfsList[i]->getid() == 0){cout << "CONST0";}
        else{cout << "PI  " << _dfsList[i]->getid();}
        if(_dfsList[i]->getname() != ""){cout << " (" << _dfsList[i]->getname() << ")";}
    }
    else if(ispo(_dfsList[i]))
    {
        cout << "PO  " << _dfsList[i]->getid() << " ";
        if(_dfsList[i]->getfin()[0] == 0){}
        if(_dfsList[i]->getphase()[0] == 1)
        {
            if(_dfsList[i]->getfin()[0] == 0){cout << "*!" << _dfsList[i]->getin()[0]/2;}
            else{cout << "!" << _dfsList[i]->getfin()[0]->getid();}
        }
        else
        {
            if(_dfsList[i]->getfin()[0] == 0){cout << "*" << _dfsList[i]->getin()[0]/2;}
            else{cout << _dfsList[i]->getfin()[0]->getid();}
        }       
        if(_dfsList[i]->getname() != ""){cout << " (" << _dfsList[i]->getname() << ")";}
    }
    else if(isand(_dfsList[i]))
    {
        cout << "AIG " << _dfsList[i]->getid() << " ";
        if(_dfsList[i]->getphase()[0] == 1)
        {
            if(_dfsList[i]->getfin()[0] == 0){cout << "*!" << _dfsList[i]->getin()[0]/2 << " ";}
            else{cout << "!" << _dfsList[i]->getfin()[0]->getid() << " ";}
        }
        else
        {
            if(_dfsList[i]->getfin()[0] == 0){cout << "*" << _dfsList[i]->getin()[0]/2 << " ";}
            else{cout << _dfsList[i]->getfin()[0]->getid() << " ";}
        }  
        if(_dfsList[i]->getphase()[1] == 1)
        {
            if(_dfsList[i]->getfin()[1] == 0){cout << "*!" << _dfsList[i]->getin()[1]/2;}
            else{cout << "!" << _dfsList[i]->getfin()[1]->getid();}
        }
        else
        {
            if(_dfsList[i]->getfin()[1] == 0){cout << "*" << _dfsList[i]->getin()[1]/2;}
            else{cout << _dfsList[i]->getfin()[1]->getid();}
        } 
    }
    cout << endl;
}

bool 
CirMgr::readheader()
{
    string header;
    getline(_file,header);
    int t = 0;
    int l = 0;
    unsigned int n;
    unsigned int two,three,four,five;
    two = three = four = five = 0;
    int c = 0;
    if(header.size() == 0)
    {
        cerr << "[ERROR] Line 1: Missing \"aag\"!!" << endl;
        return false;
    }
    for(unsigned int i = 0;i<header.size();i++)
    {
        if(int(header[i]) == 9)
        {
            if(i <= 4)
            {
                cerr << "[ERROR] Line 1, Col " << i+1 << ": Illegal white space char(9) is detected!!" << endl;
                return false;
            }
        }
        else if(int(header[i]) == 32 && c == 0)
        {
            if(i == 0)
            {
                cerr << "[ERROR] Line 1, Col 1: Extra space character is detected!!" << endl;
                return false;
            }
            else if(i < 3)
            {
                cerr << "[ERROR] Line 1: Illegal identifier \"" << header.substr(0,i) << "\"!!" << endl;
                return false;
            }
            else if(i == 3)
            {
                if(header.substr(0,3) != "aag")
                {
                    cerr << "[ERROR] Line 1: Illegal identifier \"" << header.substr(0,i) << "\"!!" << endl;
                    return false;
                }
            }
            else if(i > 3)
            {
                if(int(header[3])>=48 && int(header[3])<=57)
                {
                    cerr << "[ERROR] Line 1, Col 4: Missing space character!!" << endl;
                    return false;
                }
                else
                {
                    cerr << "[ERROR] Line 1: Illegal identifier \"" << header.substr(0,i) << "\"!!" << endl;
                    return false;
                }
            }
            c = 1;
        }
        else if(int(header[i]) == 32 && c != 0)
        {
            if(int(header[i-1]) == 32)
            {
                cerr << "[ERROR] Line 1, Col " << i+1 << ": Extra space character is detected!!" << endl;
                return false;    
            }
        }
    }
    
    for(unsigned int j = 4;j<header.size();j++)
    {
        if(header[j] == ' ' || int(header[j]) == 9)
        {
            if(int(header[j]) == 9)
            {
                cerr << "[ERROR] Line 1, Col " << j+1 << ": Missing space character!!" << endl;
                return false;
            }
            if(t == 0)
            {
                if(j!=5)
                {
                    for(unsigned int k = 4;k<j;k++)
                    {
                        if(int(header[k]) < 48 || int(header[k]) > 57)
                        {
                            cerr << "[ERROR] Line 1: Illegal number of variables(" << header.substr(4,j-4) << ")!!" << endl;
                            return false;
                        }
                    }
                    _max = stoi(header.substr(4,j-4));
                }
                else
                {
                    if(int(header[4]) < 48 || int(header[4]) > 57)
                    {
                        cerr << "[ERROR] Line 1: Illegal number of variables(" << header[4] << ")!!" << endl;
                        return false;
                    }
                    _max = header[4]-'0';
                }
                t =1;n = j+1;two = j;
            }
            else if(t == 1)
            {
                if(j!=n+1)
                {
                    for(unsigned int k = n;k<j;k++)
                    {
                        if(int(header[k]) < 48 || int(header[k]) > 57)
                        {
                            cerr << "[ERROR] Line 1: Illegal number of PIs(" << header.substr(n,j-n) << ")!!" << endl;
                            return false;
                        }
                    }
                    _pi = stoi(header.substr(n,j-n));
                }
                else
                {
                    if(int(header[n]) < 48 || int(header[n]) > 57)
                    {
                        cerr << "[ERROR] Line 1: Illegal number of PIs(" << header[n] << ")!!" << endl;
                        return false;
                    }
                    _pi = header[n]-'0';
                }
                t =2;n = j+1;three = j;
            }
            else if(t == 2)
            {
                if(j!=n+1)
                {
                    for(unsigned int k = n;k<j;k++)
                    {
                        if(int(header[k]) < 48 || int(header[k]) > 57)
                        {
                            cerr << "[ERROR] Line 1: Illegal number of latches(" << header.substr(n,j-n) << ")!!" << endl;
                            return false;
                        }
                    }
                    l = stoi(header.substr(n,j-n));
                }
                else
                {
                    if(int(header[n]) < 48 || int(header[n]) > 57)
                    {
                        cerr << "[ERROR] Line 1: Illegal number of latches(" << header[n] << ")!!" << endl;
                        return false;
                    }
                    l = header[n]-'0';
                }
                t =3;n = j+1;four = j;
            }
            else if(t == 3)
            {
                if(j!=n+1)
                {
                    for(unsigned int k = n;k<j;k++)
                    {
                        if(int(header[k]) < 48 || int(header[k]) > 57)
                        {
                            cerr << "[ERROR] Line 1: Illegal number of POs(" << header.substr(n,j-n) << ")!!" << endl;
                            return false;
                        }
                    }
                    _po = stoi(header.substr(n,j-n));
                }
                else
                {
                    if(int(header[n]) < 48 || int(header[n]) > 57)
                    {
                        cerr << "[ERROR] Line 1: Illegal number of POs(" << header[n] << ")!!" << endl;
                        return false;
                    }
                    _po = header[n]-'0';
                }
                t =4;n = j+1;five = j;
            }
            else if(t == 4)
            {
                if(j!=n+1)
                {
                    for(unsigned int k = n;k<j;k++)
                    {
                        if(int(header[k]) < 48 || int(header[k]) > 57)
                        {
                            cerr << "[ERROR] Line 1: Illegal number of AIGs(" << header.substr(n,j-n) << ")!!" << endl;
                            return false;
                        }
                    }
                    _and = stoi(header.substr(n,j-n));
                }
                else
                {
                    if(int(header[n]) < 48 || int(header[n]) > 57)
                    {
                        cerr << "[ERROR] Line 1: Illegal number of AIGs(" << header[n] << ")!!" << endl;
                        return false;
                    }
                    _and = header[n]-'0';
                } 
                cerr << "[ERROR] Line 1, Col " << j+1 << ": A new line is expected here!!" << endl;
                return false;
            }
        }
        else if(j == header.size()-1)
        {
            if(two == 0)
            {
                if(j!=4)
                {
                    for(unsigned int k = 4;k<j;k++)
                    {
                        if(int(header[k]) < 48 || int(header[k]) > 57)
                        {
                            cerr << "[ERROR] Line 1: Illegal number of variables(" << header.substr(4,j-4) << ")!!" << endl;
                            return false;
                        }
                    }
                    _max = stoi(header.substr(4,j-4));
                }
                else
                {
                    if(int(header[n]) < 48 || int(header[n]) > 57)
                    {
                        cerr << "[ERROR] Line 1: Illegal number of variables(" << header[4] << ")!!" << endl;
                        return false;
                    }
                    _max = header[4]-'0';
                }
                cerr << "[ERROR] Line 1, Col " << j+2 << ": Missing number of PIs!!" << endl;
                return false;
            }
            if(three == 0)
            {
                if(j!=n)
                {
                    for(unsigned int k = n;k<j;k++)
                    {
                        if(int(header[k]) < 48 || int(header[k]) > 57)
                        {
                            cerr << "[ERROR] Line 1: Illegal number of PIs(" << header.substr(n,j-n) << ")!!" << endl;
                            return false;
                        }
                    }
                    _pi = stoi(header.substr(n,j-n));
                }
                else
                {
                    if(int(header[n]) < 48 || int(header[n]) > 57)
                    {
                        cerr << "[ERROR] Line 1: Illegal number of PIs(" << header[n] << ")!!" << endl;
                        return false;
                    }
                    _pi = header[n]-'0';
                }
                cerr << "[ERROR] Line 1, Col " << j+2 << ": Missing number of latches!!" << endl;
                return false;
            }
            if(four == 0)
            {
                if(j!=n)
                {
                    for(unsigned int k = n;k<j;k++)
                    {
                        if(int(header[k]) < 48 || int(header[k]) > 57)
                        {
                            cerr << "[ERROR] Line 1: Illegal number of latches(" << header.substr(n,j-n) << ")!!" << endl;
                            return false;
                        }
                    }
                    l = stoi(header.substr(n,j-n));
                }
                else
                {
                    if(int(header[n]) < 48 || int(header[n]) > 57)
                    {
                        cerr << "[ERROR] Line 1: Illegal number of latches(" << header[n] << ")!!" << endl;
                        return false;
                    }
                    l = header[n]-'0';
                }
                cerr << "[ERROR] Line 1, Col " << j+2 << ": Missing number of POs!!" << endl;
                return false;
            }
            if(five == 0)
            {
                if(j!=n)
                {
                    for(unsigned int k = n;k<j;k++)
                    {
                        if(int(header[k]) < 48 || int(header[k]) > 57)
                        {
                            cerr << "[ERROR] Line 1: Illegal number of POs(" << header.substr(n,j-n) << ")!!" << endl;
                            return false;
                        }
                    }
                    _po = stoi(header.substr(n,j-n));
                }
                else
                {
                    if(int(header[n]) < 48 || int(header[n]) > 57)
                    {
                        cerr << "[ERROR] Line 1: Illegal number of POs(" << header[n] << ")!!" << endl;
                        return false;
                    }
                    _po = header[n]-'0';
                }
                cerr << "[ERROR] Line 1, Col " << j+2 << ": Missing number of AIGs!!" << endl;
                return false;
            }
            if(j == n)
            {
                if(int(header[n]) < 48 || int(header[n]) > 57)
                {
                    cerr << "[ERROR] Line 1: Illegal number of AIGs(" << header[n] << ")!!" << endl;
                    return false;
                }
                _and = header[n]-'0';
            }
            else
            {
                for(unsigned int k = n;k<j+1;k++)
                    {
                        if(int(header[k]) < 48 || int(header[k]) > 57)
                        {
                            cerr << "[ERROR] Line 1: Illegal number of AIGs(" << header.substr(n,j-n+1) << ")!!" << endl;
                            return false;
                        }
                    }
                _and = stoi(header.substr(n,j-n+1));
            }
        }
    }
    if(two == 0){cerr << "[ERROR] Line 1, Col " << header.size()+1 << ": Missing number of variables!!" << endl;return false;}
    if(three == 0){cerr << "[ERROR] Line 1, Col " << header.size()+1 << ": Missing number of PIs!!" << endl;return false;}
    if(four == 0){cerr << "[ERROR] Line 1, Col " << header.size()+1 << ": Missing number of latchs!!" << endl;return false;}
    if(five == 0){cerr << "[ERROR] Line 1, Col " << header.size()+1 << ": Missing number of POs!!" << endl;return false;}
    if(five == header.size()-1)
    {cerr << "[ERROR] Line 1, Col " << header.size()+1 << ": Missing number of AIGs!!" << endl;return false;}
    if(_max < _pi+_and){cerr << "[ERROR] Line 1: Number of variables is too small (" << _max << ")!!" << endl;return false;}
    if(l != 0){cerr << "[ERROR] Line 1: Illegal latches!!" << endl;return false;}
    for(int i = 0;i<_max;i++)
    {
        _in.push_back(NULL);
    }
    return true;
    
}

bool 
CirMgr::readpi()
{
    string pi; 
    for(int i = 0;i<_pi;i++)
    {
        int line = 2+i;
        if(getline(_file,pi))
        {
            if(pi.size() == 0)
            {
                cerr << "[ERROR] Line " << line << ", Col 1: Missing PI literal ID!!" << endl;
                return false;
            }
            for(unsigned int j = 0;j<pi.size();j++)
            {
                if(int(pi[j]) == 32)
                {
                    if(j == 0)
                    {
                        cerr << "[ERROR] Line " << line << ", Col 1: Extra space character is detected!!" << endl;
                        return false;
                    }
                    else
                    {
                        if(stoi(pi.substr(0,j)) == 0 || stoi(pi.substr(0,j)) == 1)
                        {
                            cerr << "[ERROR] Line "<< line << ", Col 1: Cannot redefine constant (" <<stoi(pi.substr(0,j)) << ")!!" << endl;
                            return false;
                        }
                        else
                        {
                            cerr << "[ERROR] Line " << line << ", Col " << j+1 << ": A new line is expected here!!" << endl;
                            return false;
                        }
                    }
                }
                else if(int(pi[j]) >= 48 && int(pi[j]) <= 57)
                {
                    
                }
                else
                {
                    for(unsigned int k = j;k<pi.size();k++)
                    {
                        if(pi[k] == ' ')
                        {
                            cerr << "[ERROR] Line " << line << ": Illegal PI literal ID(" << pi.substr(0,k) << ")!!" << endl;
                            return false;
                        }
                    }
                    cerr << "[ERROR] Line " << 2+i << ": Illegal PI literal ID(" << pi << ")!!" << endl;
                    return false;
                }
            }
            int b = stoi(pi)/2;
            if(stoi(pi) == 0 || stoi(pi) == 1)
            {
                cerr << "[ERROR] Line " << line << ", Col 1: Cannot redefine constant (" << stoi(pi) << ")!!" << endl;
                return false;
            }
            if(b > _max)
            {
                cerr << "[ERROR] Line " << line << ", Col 1: Literal \"" << stoi(pi) << "\" exceeds maximum valid ID!!" << endl;
                return false;
            }
            if(b*2 != stoi(pi))
            {
                cerr << "[ERROR] Line " << line << ", Col 1: PI " << stoi(pi) << "(" << b << ") cannot be inverted!!" << endl;
                return false;
            }
            if(_in[b] != NULL)
            {
                cerr << "[ERROR] Line " << line << ": Literal \"" << stoi(pi) << "\" is redefined, previously defined as PI in line " << findgate(b)->getLineNo() << "!!" << endl;
                return false;
            }
            CirGate* pi = new CirPiGate();
            pi->setid(b);
            pi->setline(line);
            pi->settype("PI");
            _piList.push_back(pi);
            _in[b] = pi;
        }
        else
        {
            cerr << "[ERROR] Line " << line << ": Missing PI definition!!" << endl;
            return false;
        }
    }
    return true;
}

bool
CirMgr::readpo()
{
    string po; 
    for(int i = 0;i<_po;i++)
    {
        int line = i+2+_pi;
        if(getline(_file,po))
        {
            if(po.size() == 0)
            {
               cerr << "[ERROR] Line " << line << ", Col 1: Missing PO literal ID!!" << endl;
                return false; 
            }
            for(unsigned int j = 0;j<po.size();j++)
            {
                if(po[j] == 32)
                {
                    if(j == 0)
                    {
                        cerr << "[ERROR] Line " << line << ", Col 1: Extra space character is detected!!" << endl;
                        return false;
                    }
                    else
                    {
                        cerr << "[ERROR] Line " << line << ", Col " << j+1 << ": A new line is expected here!!" << endl;
                        return false;
                    }
                }
                else if(int(po[j]) >= 48 && int(po[j]) <= 57)
                {
                    
                }
                else
                {
                    for(unsigned int k = j;k<po.size();k++)
                    {
                        if(po[k] == ' ')
                        {
                            cerr << "[ERROR] Line " << line << ": Illegal PO literal ID(" << po.substr(0,k) << ")!!" << endl;
                            return false;
                        }
                    }
                    cerr << "[ERROR] Line " << line << ": Illegal PO literal ID(" << po << ")!!" << endl;
                    return false;
                }
            }
            int b = stoi(po);
            if(b/2 > _max)
            {
                cerr << "[ERROR] Line " << line << ", Col 1: Literal \"" << stoi(po) << "\" exceeds maximum valid ID!!" << endl;
                return false;
            }
            CirGate* po = new CirPoGate();
            po->setin(b);
            if(b%2 == 0){po->setphase(0);}else{po->setphase(1);}
            po->setid(_max+1+i);
            po->setline(line);
            po->settype("PO");
            _poList.push_back(po);
        }
        else
        {
            cerr << "[ERROR] Line " << line << ": Missing PO definition!!" << endl;
            return false;
        }
    }
    return true;
}

bool 
CirMgr::readand()
{
    string a; 
    for(int i = 0;i<_and;i++)
    {
        int line = 2+i+_pi+_po;
        if(getline(_file,a))
        {
            if(a.size() == 0)
            {
                cerr << "[ERROR] Line " << line << ", Col 1: Missing AIG gate literal ID!!" << endl;
                return false;
            }   
            int c = 0;
            int next = 0;
            for(unsigned int j = 0;j<a.size();j++)
            {
                if(int(a[j]) == 32)
                {
                    c++;
                    if(j == 0)
                    {
                        cerr << "[ERROR] Line " << line << ", Col 1: Extra space character is detected!!" << endl;
                        return false;
                    }
                    else if(a[j-1] == 32)
                    {
                        cerr << "[ERROR] Line " << line << ", Col " << j+1 << ": Extra space character is detected!!"<< endl;
                        return false;
                    }
                    else if(c == 3)
                    {
                        cerr << "[ERROR] Line " << line << ", Col " << j+1 << ": A new line is expected here!!" << endl;
                        return false;
                    }
                    int b = stoi(a.substr(next,j-next))/2;
                    if(b > _max)
                    {
                        cerr<<"[ERROR] Line "<<line<<", Col " << next+1 << ": Literal \"" << stoi(a.substr(next,j-next)) << "\" exceeds maximum valid ID!!"<<endl;
                        return false;
                    } 
                    next = j+1;
                }
                else if(int(a[j]) >= 48 && int(a[j]) <= 57)
                {
                    
                }
                else
                {
                    for(unsigned int k = j;k<a.size();k++)
                    {
                        if(a[k] == ' ')
                        {
                            if(c == 0)
                            {
                                cerr <<"[ERROR] Line "<< line <<": Illegal AIG gate literal ID("<<a.substr(0,k)<<")!!"<<endl;
                                return false;
                            }
                            else
                            {
                                cerr<<"[ERROR] Line "<<line<<": Illegal AIG input literal ID("<<a.substr(next,k-next)<<")!!"<<endl;
                                return false;
                            }
                        }
                    }
                    if(c == 0)
                    {
                        cerr << "[ERROR] Line " << line << ": Illegal AIG gate literal ID(" << a << ")!!" << endl;
                        return false;
                    }
                    else
                    {
                        cerr<<"[ERROR] Line "<<line<<": Illegal AIG input literal ID("<<a.substr(next,a.size()-next)<<")!!"<<endl;
                        return false;
                    }
                }
            }
            if(c == 0)
            {
                cerr << "[ERROR] Line " << line << ", Col " << a.size()+1 <<": Missing space character!!" << endl;
                return false;
            }
            int b = stoi(a)/2;
            if(stoi(a) == 0 || stoi(a) == 1)
            {
                cerr << "[ERROR] Line " << line << ", Col 1: Cannot redefine constant (" << stoi(a) << ")!!" << endl;
                return false;
            }
            if(b > _max)
            {
                cerr<<"[ERROR] Line "<<line<<", Col 1: Literal \"" << stoi(a) << "\" exceeds maximum valid ID!!"<<endl;
                return false;
            }
            if(b*2 != stoi(a))
            {
                cerr<<"[ERROR] Line "<<line<<", Col 1: AIG gate "<<stoi(a)<<"(" << b << ") cannot be inverted!!"<<endl;
                return false;
            }
            if(_in[b] != NULL)
            {
                cerr << "[ERROR] Line " << line << ": Literal \"" << stoi(a) << "\" is redefined, previously defined as " << _in[b]->getTypeStr() <<" in line " << _in[b]->getLineNo() << "!!" << endl;
                return false;
            }
            CirGate* aig = new CirAigGate();
            aig->setid(b);
            aig->setline(line);
            aig->settype("AIG");
            _andList.push_back(aig);
            _in[b] = aig;
            int t = 0;
            unsigned int n; 
            for(unsigned int j = 0;j<a.size();j++)
            {
                if(a[j] == ' ')
                {
                    if(t == 0){t = 1;n = j+1;}
                    else if(t ==1)
                    {
                        int in = stoi(a.substr(n,j-n));
                        aig->setin(in);
                        if(in%2 == 0){aig->setphase(0);}else{aig->setphase(1);}
                        t = 2;
                        n = j+1;
                    }
                }
                else if(j == a.size()-1)
                {
                    int in = stoi(a.substr(n,j-n+1));
                    aig->setin(in);
                    if(in%2 == 0){aig->setphase(0);}else{aig->setphase(1);}
                }
            }
        }
        else
        {
            cerr << "[ERROR] Line " << line << ": Missing AIG definition!!" << endl;
            return false;
        }
    }
    return true;
}

bool
CirMgr::readsymbol()
{
    string symbol;
    unsigned int line = 2+_pi+_po+_and;
    while(getline(_file,symbol))
    {
        if(symbol.size() == 0)
        {
            cerr << "[ERROR] Line " << line << ", Col 1: Illegal symbol type ()!!" << endl;
            return false;
        }
        if(symbol.size() == 1 && symbol == "c"){return true;}
        if(symbol[0] == 'i')
        {
            int c = 0;
            for(unsigned int i = 1;i<symbol.size();i++)
            {
                if(symbol[i] == ' ')
                {
                    c = 1;
                    if(i == 1)
                    {
                        cerr << "[ERROR] Line " << line << ", Col 2: Extra space character is detected!!" << endl;
                        return false;
                    }
                    unsigned int j = stoi(symbol.substr(1,i-1));
                    for(unsigned int k = i;k<symbol.size();k++)
                    {
                        if(!isprint(symbol[k]))
                        {
                            cerr << "[ERROR] Line " << line << ", Col " << k+1 << ": Symbolic name contains un-printable char(" << int(symbol[k]) << ")!!" << endl;
                            return false;
                        }
                    }
                    if(j<_piList.size())
                    {
                        if(i<symbol.size()-1)
                        {
                            if(_piList[j+1]->getname() == "")
                            {
                                _piList[j+1]->setname(symbol.substr(i+1,symbol.size()-1-i));
                                break;
                            }
                            else
                            {
                                cerr << "[ERROR] Line " << line << ": Symbolic name for \"i" << j << "\" is redefined!!" << endl;
                                return false;
                            }
                        }
                        else
                        {
                            cerr << "[ERROR] Line " << line << ": Missing \"symbolic name\"!!" << endl;
                            return false;
                        }
                    }
                    else
                    {
                        cerr << "[ERROR] Line " << line << ": PI index is too big (" << j << ")!!" << endl;
                        return false;
                    }
                }
                else if(int(symbol[i]) == 9)
                {
                    if(i <= 1)
                    {
                        cerr << "[ERROR] Line " <<line<<", Col "<<i+1<<": Illegal white space char(9) is detected!!" << endl;
                        return false;
                    }
                    else
                    {
                        cerr << "[ERROR] Line " << line <<", Col " << i+1 << ": Missing space character!!" << endl;
                        return false;
                    }
                }
                else if(int(symbol[i]) >= 48 && int(symbol[i]) <= 57)
                {
                    
                }
                else
                {
                    for(unsigned int k = i;k<symbol.size();k++)
                    {
                        if(symbol[k] == ' ' || int(symbol[k]) == 9)
                        {
                            cerr << "[ERROR] Line " << line << ": Illegal symbol index(" << symbol.substr(1,k-1) << ")!!" << endl;
                            return false;
                        }
                    }
                    cerr << "[ERROR] Line " << line << ": Illegal symbol index(" << symbol.substr(1,symbol.size()-1) << ")!!" << endl;
                    return false;
                }
            }
            if(c == 0)
            {
                cerr << "[ERROR] Line " << line << ": Missing \"symbolic name\"!!" << endl;
                return false;
            }
        }
        else if(symbol[0] == 'o')
        {
            int c = 0;
            for(unsigned int i = 1;i<symbol.size();i++)
            {
                if(symbol[i] == ' ')
                {
                    c = 1;
                    if(i == 1)
                    {
                        cerr << "[ERROR] Line " << line << ", Col 2: Extra space character is detected!!" << endl;
                        return false;
                    }
                    unsigned int j = stoi(symbol.substr(1,i-1));
                    for(unsigned int k = i;k<symbol.size();k++)
                    {
                        if(!isprint(symbol[k]))
                        {
                            cerr << "[ERROR] Line " << line << ", Col " << k+1 << ": Symbolic name contains un-printable char(" << int(symbol[k]) << ")!!" << endl;
                            return false;
                        }
                    }
                    if(j<_poList.size())
                    {
                        if(i<symbol.size()-1)
                        {
                            if(_poList[j]->getname() == "")
                            {
                                _poList[j]->setname(symbol.substr(i+1,symbol.size()-1-i));
                                break;
                            }
                            else
                            {
                                cerr << "[ERROR] Line " << line << ": Symbolic name for \"o" << j << "\" is redefined!!" << endl;
                                return false;
                            }
                        }
                        else
                        {
                            cerr << "[ERROR] Line " << line << ": Missing \"symbolic name\"!!" << endl;
                            return false;
                        }
                    }
                    else
                    {
                        cerr << "[ERROR] Line " << line << ": PO index is too big (" << j << ")!!" << endl;
                        return false;
                    }
                }
                else if(int(symbol[i]) == 9)
                {
                    if(i <= 1)
                    {
                        cerr << "[ERROR] Line " <<line<<", Col "<<i+1<<": Illegal white space char(9) is detected!!" << endl;
                        return false;
                    }
                    else
                    {
                        cerr << "[ERROR] Line " << line <<", Col " << i+1 << ": Missing space character!!" << endl;
                        return false;
                    }
                }
                else if(int(symbol[i]) >= 48 && int(symbol[i]) <= 57)
                {
                    
                }
                else
                {
                    for(unsigned int k = i;k<symbol.size();k++)
                    {
                        if(symbol[k] == ' ' || int(symbol[k]) == 9)
                        {
                            cerr << "[ERROR] Line " << line << ": Illegal symbol index(" << symbol.substr(1,k-1) << ")!!" << endl;
                            return false;
                        }
                    }
                    cerr << "[ERROR] Line " << line << ": Illegal symbol index(" << symbol.substr(1,symbol.size()-1) << ")!!" << endl;
                    return false;
                }
            }
            if(c == 0)
            {
                cerr << "[ERROR] Line " << line << ": Missing \"symbolic name\"!!" << endl;
                return false;
            }
        }
        else if(int(symbol[0] == 32))
        {
            cerr << "[ERROR] Line " << line << ", Col 1: Extra space character is detected!!" << endl;
            return false;
        }
        else if(int(symbol[0] == 9))
        {
            cerr << "[ERROR] Line " << line << ", Col 1: Illegal white space char(9) is detected!!" << endl;
            return false;
        }
        else if(symbol[0] == 'c')
        {
            cerr << "[ERROR] Line " << line << ", Col 2: A new line is expected here!!" << endl;
            return false;
        }
        else
        {
            cerr << "[ERROR] Line " << line << ", Col 1: Illegal symbol type (" << symbol[0] << ")!!" << endl;
            return false;
        }
        line++;
    }
    return true;
}

CirGate*
CirMgr::findgate(size_t id) const
{
    for(unsigned int i = 0;i<_piList.size();i++)
    {
        if(_piList[i]->getid() == id){return _piList[i];}
    }
    for(unsigned int i = 0;i<_poList.size();i++)
    {
        if(_poList[i]->getid() == id){return _poList[i];}
    }
    for(unsigned int i = 0;i<_andList.size();i++)
    {
        if(_andList[i]->getid() == id){return _andList[i];}
    }
    return NULL;
}

bool 
CirMgr::ispi(const CirGate* g) const
{
    if(g->getTypeStr() == "PI"){return true;}
    return false;
}

bool 
CirMgr::ispo(const CirGate* g) const
{
    if(g->getTypeStr() == "PO"){return true;}
    return false;
}

bool 
CirMgr::isand(const CirGate* g) const
{
    if(g->getTypeStr() == "AIG"){return true;}
    return false;
}

bool 
CirMgr::isdfs(CirGate* &g) const
{
    if(g->_isdfs == true){return true;}
    return false;
}

void 
CirMgr::connect()
{
    for(unsigned int i = 0;i<_poList.size();i++)
    {
        CirGate* g = _in[(_poList[i]->getin()[0]/2)];
        _poList[i]->setfin(g);
        if(g != NULL){g->setfout(_poList[i]);}
    }
    for(unsigned int i = 0;i<_andList.size();i++)
    {
        CirGate* g = _in[(_andList[i]->getin()[0]/2)];
        _andList[i]->setfin(g);
        if(g != NULL){g->setfout(_andList[i]);}
        g = _in[(_andList[i]->getin()[1]/2)];
        _andList[i]->setfin(g);
        if(g != NULL){g->setfout(_andList[i]);}
    }
}

void
CirMgr::genDFSList() 
{
    _isprinted.clear();
    for(size_t i=0;i<_piList.size();i++)
    {
        _piList[i]->_isdfs = false;
    }
    for(int i=0;i<_po;i++)
    {
        _poList[i]->_isdfs = false;
    }
    for(int i=0;i<_and;i++)
    {
        _andList[i]->_isdfs = false;
    }
    stack<CirGate*> dfs;
    for(unsigned int i = 0;i<_poList.size();i++)
    {
        CirGate* g = _poList[i];
        dfs.push(g);
        while(!dfs.empty())
        {
            for(int j = g->getfin().size()-1;j>=0;j--)
            {
                if(g->getfin()[j] == NULL){}
                else if(!isdfs(g->getfin()[j])){dfs.push(g->getfin()[j]);}
            }
            g = dfs.top();
            while(isfrontier(g))
            {
                if(!isdfs(g)){_dfsList.push_back(g);g->_isdfs = true;}
                dfs.pop();
                if(dfs.empty()){break;}
                g = dfs.top();
            }
        }
    }
}

bool 
CirMgr::isfrontier(CirGate* g) const
{
    unsigned int count = 0;
    if(ispi(g)){return true;}
    if(g->getfin().size() == 0){return true;}
    for(unsigned int i = 0;i<g->getfin().size();i++)
    {
        if(g->getfin()[i] == NULL){count++;}
        else if(isdfs(g->getfin()[i])){count++;}
        if(count == g->getfin().size()){return true;}
    }
    return false;
}

void
CirMgr::reset()
{
    _max = _pi = _po = _and = 0;
    _piList.clear();
    _poList.clear();
    _andList.clear();
    _dfsList.clear();
    _isprinted.clear();
}
