/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
    int c = 0;
    int t = 0;
    int first = 0,sec = 0;
    string s = "";
    vector<string> to;
    string token;
    for(unsigned int i = 0;i<option.size();i++)
    {
        if(option[i] != ' ')
        {            
            if(c == 0){c = 1; t = i;}
            if(c == 2){c = 3; t = i;}
            if(c == 4){c = 5; t = i;}
            if(c == 6){cerr << "Error: Extra option!! (" << option.substr(i,option.size() - i) << ")" << endl;return CMD_EXEC_ERROR;}
        }
        else if(option[i] == ' ')
        {
            if(c == 1)
            {
                c = 2; 
                if(option[0] == '-'){s = option.substr(1,i-1);}
                else
                {
                    if(!myStr2Int(option.substr(0,i),first))
                    {
                        cerr << option.substr(0,i) << " is not an integer!!";
                        return CMD_EXEC_ERROR;
                    }
                }
            }
            if(c == 3)
            {
                c = 4; 
                if(option[t] == '-'){s = option.substr(t+1,i-t-1);}
                else
                {
                    if(first != 0)
                    {cerr << "Error: Extra option!! (" << option.substr(t,option.size()-t) << ")" << endl;return CMD_EXEC_ERROR;}
                    else if(!myStr2Int(option.substr(t,i-t),first))
                    {
                        cerr << option.substr(t,i-t) << " is not an integer!!";
                        return CMD_EXEC_ERROR;
                    }
                }
            }
            if(c == 5)
            {
                c = 6; 
                if(option[t] == '-')
                {
                    cerr << "Error: Wrong arrangement!!" << endl;
                    return CMD_EXEC_ERROR;
                }
                else
                {
                    if(!myStr2Int(option.substr(t,i-t),sec))
                    {
                        cerr << option.substr(t,i-t) << " is not an integer!!";
                        return CMD_EXEC_ERROR;
                    }
                }
            }
        }
        if(i == option.size()-1)
        {
            if(c == 1)
            {
                if(option[0] == '-')
                {
                    cerr << "Error: Missing option!!" << endl;
                    return CMD_EXEC_ERROR;
                }
                else
                {
                    if(!myStr2Int(option.substr(0,i+1),first))
                    {
                        cerr << option.substr(0,i+1) << " is not an integer!!";
                        return CMD_EXEC_ERROR;
                    }
                }
            }
            if(c == 3)
            {
                if(option[t] == '-')
                {
                    lexOptions(option,to,1);
                    return CMD_EXEC_ERROR;
                }
                else
                {
                    if(first != 0)
                    {cerr << "Error: Extra option!! (" << option.substr(t,option.size()-t) << ")" << endl;return CMD_EXEC_ERROR;}
                    if(!myStr2Int(option.substr(t,i-t+1),first))
                    {
                        cerr << option.substr(t,i-t+1) << " is not an integer!!";
                        return CMD_EXEC_ERROR;
                    }
                }
            }
            if(c == 5)
            {
                if(option[t] == '-')
                {
                    cerr << "Error: Wrong arrangement!!";
                    return CMD_EXEC_ERROR;
                }
                else
                {
                    if(!myStr2Int(option.substr(t,i-t+1),sec))
                    {
                        cerr << option.substr(t,i-t+1) << " is not an integer!!";
                        return CMD_EXEC_ERROR;
                    }
                }
            }
        }
    }
    if(first <= 0){cerr << "Error: Illegal option!!" << endl;return CMD_EXEC_ERROR;}
    if(s == "")
    {
        if(sec == 0)
        {
            try {mtest.newObjs(first);} 
            catch (bad_alloc) 
            {  
                return CMD_EXEC_ERROR;
            }
            return CMD_EXEC_DONE;
        }
        cerr << "Error: Wrong arrangement!!" << endl;
        return CMD_EXEC_ERROR;
    }
    if(myStrNCmp("Array",s,1) == 0)
    {
        if(sec <= 0){cerr << "Error: Illegal option!!" << endl;return CMD_EXEC_ERROR;}
        if(option[0] == '-')
        {
            try {mtest.newArrs(sec,first);} 
            catch (bad_alloc) 
            {  
                return CMD_EXEC_ERROR;
            }
        }
        else
        {
            try {mtest.newArrs(first,sec);} 
            catch (bad_alloc) 
            {  
                return CMD_EXEC_ERROR;
            }
        }
    }
    else if(myStrNCmp("Array",s,1) != 0){cerr << "Error: Illegal command!!" << endl;return CMD_EXEC_ERROR;}
    
    // Use try-catch to catch the bad_alloc exception
    return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
    int c = 0;
    int t = 0;
    string first = "",sec = "";
    int x = -1;
    for(unsigned int i = 0;i<option.size();i++)
    {
        if(option[i] != ' ')
        {            
            if(c == 0){c = 1; t = i;}
            if(c == 2){c = 3; t = i;}
            if(c == 4){c = 5; t = i;}
            if(c == 6)
            {
                cerr << "Error: Extra option!! (" << option.substr(i,option.size() - i) << ")" << endl;
                return CMD_EXEC_ERROR;
            }
        }
        else if(option[i] == ' ')
        {
            if(c == 1)
            {
                c = 2; 
                if(option[0] == '-')
                {
                    first = option.substr(1,i-1);
                }
                else
                {
                    cerr << "Error: Wrong arrangement!!" << endl;
                    return CMD_EXEC_ERROR;
                }
            }
            if(c == 3)
            {
                c = 4; 
                if(option[t] == '-'){sec = option.substr(t+1,i-t-1);}
                else
                {
                    if(!myStr2Int(option.substr(t,i-t),x))
                    {
                        cerr << option.substr(t,i-t) << " is not an integer!!";
                        return CMD_EXEC_ERROR;
                    }
                }
            }
            if(c == 5)
            {
                c = 6; 
                if(option[t] == '-')
                {
                    sec = option.substr(t+1,i-t-1);
                }
                else
                {
                    if(x != -1)
                    {
                        cerr << "Error: Extra option!! (" << option.substr(t,i-t) << ")" << endl;
                        return CMD_EXEC_ERROR;
                    }
                    else if(!myStr2Int(option.substr(t,i-t),x))
                    {
                        cerr << option.substr(t,i-t) << " is not an integer!!";
                        return CMD_EXEC_ERROR;
                    }
                }
            }
        }
        if(i == option.size()-1)
        {
            if(c == 1)
            {
                cerr << "Error: Missing option!!" << endl;
                return CMD_EXEC_ERROR;
            }
            if(c == 3)
            {
                if(option[t] == '-')
                {
                    cerr << "Error: Missing option!!" << endl;
                    return CMD_EXEC_ERROR;
                }
                else
                {
                    if(!myStr2Int(option.substr(t,i-t+1),x))
                    {
                        cerr << option.substr(t,i-t+1) << " is not an integer!!";
                        return CMD_EXEC_ERROR;
                    }
                }
            }
            if(c == 5)
            {
                if(option[t] == '-')
                {
                    sec = option.substr(t+1,i-t);
                }
                else
                {
                    if(x != -1)
                    {
                        cerr << "Error: Extra option!! (" << option.substr(t,i-t+1) << ")" << endl;
                        return CMD_EXEC_ERROR;
                    }
                    else if(!myStr2Int(option.substr(t,i-t+1),x))
                    {
                        cerr << option.substr(t,i-t+1) << " is not an integer!!";
                        return CMD_EXEC_ERROR;
                    }
                }
            }
        }
    }
    if(x < 0){cerr << "Error: Illegal index!!" << endl;return CMD_EXEC_ERROR;}
    if(myStrNCmp("Index",first,1) == 0)
    {
        if(myStrNCmp("Array",sec,1) == 0)
        {
            if(x < 0){cerr << "Error: Illegal index!!" << endl;return CMD_EXEC_ERROR;}
            if(x>=mtest.getArrListSize())
            {
                cerr << "Size of array list (" << mtest.getArrListSize() << ") is <=" << x << "!!" << endl;
                cerr << "Error: Illegal option!!" << endl;
                return CMD_EXEC_ERROR;
            }
            mtest.deleteArr(x);
            return CMD_EXEC_DONE;
        }
        else if(sec == "")
        {
            if(x < 0){cerr << "Error: Illegal index!!" << endl;return CMD_EXEC_ERROR;}
            if(x>=mtest.getObjListSize())
            {
                cerr << "Size of object list (" << mtest.getObjListSize() << ") is <=" << x << "!!" << endl;
                cerr << "Error: Illegal option!!" << endl;
                return CMD_EXEC_ERROR;
            }
            mtest.deleteObj(x);
            return CMD_EXEC_DONE;
        }
        else{cerr << "Error: Illegal command!!" << endl;return CMD_EXEC_ERROR;}
     }
    else if(myStrNCmp("Random",first,1) == 0)
    {
        if(x < 0){cerr << "Error: Illegal index!!" << endl;return CMD_EXEC_ERROR;}
        if(myStrNCmp("Array",sec,1) == 0)
        {
            if(mtest.getArrListSize() == 0){cerr << "Size of array list is 0!!" << endl;return CMD_EXEC_ERROR;}
            for(unsigned int i = 0;i<x;i++)
            {
                mtest.deleteArr(rnGen(mtest.getArrListSize()));
            }
            return CMD_EXEC_DONE;
        }
        else if(sec == "")
        {
            if(mtest.getObjListSize() == 0){cerr << "Size of object list is 0!!" << endl;return CMD_EXEC_ERROR;;}
            for(unsigned int i = 0;i<x;i++)
            {
                mtest.deleteObj(rnGen(mtest.getObjListSize()));
            }
            return CMD_EXEC_DONE;
        }
        else{cerr << "Error: Illegal command!!" << endl;return CMD_EXEC_ERROR;}
    }
    else if(myStrNCmp("Array",first,1) == 0)
    {
       if(myStrNCmp("Index",sec,1) == 0)
        {
            if(x < 0){cerr << "Error: Illegal index!!" << endl;return CMD_EXEC_ERROR;}
            if(x>=mtest.getArrListSize())
            {
                cerr << "Size of array list (" << mtest.getArrListSize() << ") is <=" << x << "!!" << endl;
                return CMD_EXEC_ERROR;
            }
            mtest.deleteArr(x);
            return CMD_EXEC_DONE;
        }
        else if(myStrNCmp("Random",sec,1) == 0)
        {
            if(x < 0){cerr << "Error: Illegal index!!" << endl;return CMD_EXEC_ERROR;}
            if(mtest.getArrListSize() == 0){cerr << "Size of array list is 0!!" << endl;return CMD_EXEC_ERROR;}
            for(unsigned int i = 0;i<x;i++)
            {
                mtest.deleteArr(rnGen(mtest.getArrListSize()));
            }
            return CMD_EXEC_DONE;
        }
        else{cerr << "Error: Illegal command!!" << endl;return CMD_EXEC_ERROR;}
    }
    else{cerr << "Error: Missing option!!" << endl;return CMD_EXEC_ERROR;}

   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


