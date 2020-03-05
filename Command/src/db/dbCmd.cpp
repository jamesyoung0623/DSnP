/****************************************************************************
  FileName     [ dbCmd.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include <cassert>
#include <cmath>
#include "util.h"
#include "dbCmd.h"
#include "dbJson.h"

// Global variable
DBJson dbjson;

bool
initDbCmd()
{
   if (!(cmdMgr->regCmd("DBAPpend", 4, new DBAppendCmd) &&
         cmdMgr->regCmd("DBAVe", 4, new DBAveCmd) &&
         cmdMgr->regCmd("DBCount", 3, new DBCountCmd) &&
         cmdMgr->regCmd("DBMAx", 4, new DBMaxCmd) &&
         cmdMgr->regCmd("DBMIn", 4, new DBMinCmd) &&
         cmdMgr->regCmd("DBPrint", 3, new DBPrintCmd) &&
         cmdMgr->regCmd("DBRead", 3, new DBReadCmd) &&
         cmdMgr->regCmd("DBSOrt", 4, new DBSortCmd) &&
         cmdMgr->regCmd("DBSUm", 4, new DBSumCmd)
        )) {
      cerr << "Registering \"init\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}

//----------------------------------------------------------------------
//    DBAPpend <(string key)><(int value)>
//----------------------------------------------------------------------
CmdExecStatus
DBAppendCmd::exec(const string& option)
{
    int a = 0;
    string key;
    int value;
    int q = 34;
    string c;
    int first;
    if(option.size() == 0)
    {
        cerr << "Error: Missing option!!" << endl;
        return CMD_EXEC_DONE;
    }
    for(unsigned int i=0;i<option.size();i++)
    {
        if(option[i] == ' ')
        {
            if(option.size() == i)
            {
                cerr << "Error: Missing option!!" << endl;
                return CMD_EXEC_DONE;
            }
            if(a == 1){
                if(option.size()-1 == i)
                {
                    cerr << "Error: Missing option!!" << endl;
                    return CMD_EXEC_DONE;
                }
                key = c;
                first = i+1;
                a = 2;
            }
            if(a == 2){
                if(!myStr2Int(option.substr(first,i-first),value))
                {
                    cerr << "Error: Illegal option!! (" << option.substr(first,i-first) << ")" << endl;
                    return CMD_EXEC_DONE;  
                }
            }
        }
        else if(option[i] != ' ')
        {
            if(a == 0){a = 1;}
            if(option.size()-1 == i && a == 1)
            {
                cerr << "Error: Missing option!!" << endl;
                return CMD_EXEC_DONE;
            }
            c.push_back(option[i]);
        }
    } 
    DBJsonElem j(char(q)+key+char(q),value);
    if(!dbjson.add(j))
    {
        cerr << "Error: Element with key "  << key << " already exists!!" << endl;
    }
    return CMD_EXEC_DONE;
}

void
DBAppendCmd::usage(ostream& os) const
{
   os << "Usage: DBAPpend <(string key)><(int value)>" << endl;
}

void
DBAppendCmd::help() const
{
   cout << setw(15) << left << "DBAPpend: "
        << "append an JSON element (key-value pair) to the end of DB" << endl;
}


//----------------------------------------------------------------------
//    DBAVerage
//----------------------------------------------------------------------
CmdExecStatus
DBAveCmd::exec(const string& option)
{  
   // check option
   if (!CmdExec::lexNoOption(option))
      return CMD_EXEC_ERROR;

   float a = dbjson.ave();
   if (isnan(a)) {
      cerr << "Error: The average of the DB is nan." << endl;
      return CMD_EXEC_ERROR;
   }
   ios_base::fmtflags origFlags = cout.flags();
   cout << "The average of the DB is " << fixed
        << setprecision(2) << a << ".\n";
   cout.flags(origFlags);

   return CMD_EXEC_DONE;
}

void
DBAveCmd::usage(ostream& os) const
{     
   os << "Usage: DBAVerage" << endl;
}

void
DBAveCmd::help() const
{
   cout << setw(15) << left << "DBAVerage: "
        << "compute the average of the DB" << endl;
}


//----------------------------------------------------------------------
//    DBCount
//----------------------------------------------------------------------
CmdExecStatus
DBCountCmd::exec(const string& option)
{  
   // check option
   if (!CmdExec::lexNoOption(option))
      return CMD_EXEC_ERROR;

   size_t n = dbjson.size();
   if (n > 1)
      cout << "There are " << n << " JSON elements in DB." << endl;
   else if (n == 1)
      cout << "There is 1 JSON element in DB." << endl;
   else
      cout << "There is no JSON element in DB." << endl;

   return CMD_EXEC_DONE;
}

void
DBCountCmd::usage(ostream& os) const
{     
   os << "Usage: DBCount" << endl;
}

void
DBCountCmd::help() const
{
   cout << setw(15) << left << "DBCount: "
        << "report the number of JSON elements in the DB" << endl;
}


//----------------------------------------------------------------------
//    DBMAx
//----------------------------------------------------------------------
CmdExecStatus
DBMaxCmd::exec(const string& option)
{  
   // check option
   if (!CmdExec::lexNoOption(option))
      return CMD_EXEC_ERROR;

   size_t maxI;
   int maxN = dbjson.max(maxI);
   if (maxN == INT_MIN) {
      cerr << "Error: The max JSON element is nan." << endl;
      return CMD_EXEC_ERROR;
   }
   cout << "The max JSON element is " << dbjson[maxI] << "." << endl;

   return CMD_EXEC_DONE;
}

void
DBMaxCmd::usage(ostream& os) const
{     
   os << "Usage: DBMAx" << endl;
}

void
DBMaxCmd::help() const
{
   cout << setw(15) << left << "DBMAx: "
        << "report the maximum JSON element" << endl;
}


//----------------------------------------------------------------------
//    DBMIn
//----------------------------------------------------------------------
CmdExecStatus
DBMinCmd::exec(const string& option)
{  
   // check option
   if (!CmdExec::lexNoOption(option))
      return CMD_EXEC_ERROR;

   size_t minI;
   int minN = dbjson.min(minI);
   if (minN == INT_MAX) {
      cerr << "Error: The min JSON element is nan." << endl;
      return CMD_EXEC_ERROR;
   }
   cout << "The min JSON element is " << dbjson[minI] << "." << endl;

   return CMD_EXEC_DONE;
}

void
DBMinCmd::usage(ostream& os) const
{     
   os << "Usage: DBMIn" << endl;
}

void
DBMinCmd::help() const
{
   cout << setw(15) << left << "DBMIn: "
        << "report the minimum JSON element" << endl;
}


//----------------------------------------------------------------------
//    DBPrint [(string key)]
//----------------------------------------------------------------------
CmdExecStatus
DBPrintCmd::exec(const string& option)
{  
    if(option.size() == 0){cout << dbjson;}
    else{
        for(unsigned int i = 0;i<dbjson.size();i++)
        {
            if(dbjson[i].key().substr(1,dbjson[i].key().size()-2) == option)
            {
                cout << "{ " << dbjson[i].key() << " : " << dbjson[i].value() << " }" << endl;
                return CMD_EXEC_DONE;
            }
        }
        cerr << "Error: No JSON element with key " << option << " is found." << endl;
    }
    return CMD_EXEC_DONE;
}

void
DBPrintCmd::usage(ostream& os) const
{
   os << "DBPrint [(string key)]" << endl;
}

void
DBPrintCmd::help() const
{
   cout << setw(15) << left << "DBPrint: "
        << "print the JSON element(s) in the DB" << endl;
}


//----------------------------------------------------------------------
//    DBRead <(string jsonFile)> [-Replace]
//----------------------------------------------------------------------
CmdExecStatus
DBReadCmd::exec(const string& option)
{
   // check option
    vector<string> options;
    if (!CmdExec::lexOptions(option, options))
       return CMD_EXEC_ERROR;

    if (options.empty())
       return CmdExec::errorOption(CMD_OPT_MISSING, "");

    bool doReplace = false;
    string fileName;
    for (size_t i = 0, n = options.size(); i < n; ++i) {
       if (myStrNCmp("-Replace", options[i], 2) == 0) {
          if (doReplace) return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
          doReplace = true;
       }
       else {
          if (fileName.size())
             return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
          fileName = options[i];
       }
    }
    for(unsigned int i = 0;i<fileName.size();i++)
    {
        if(fileName[i] != '.' && fileName[i] != '/')
        {
            fileName = fileName.substr(i,fileName.size()-i);
            break;
        }
    }
    ifstream ifs(fileName.c_str());
    if (!ifs) {
       cerr << "Error: \"" << fileName << "\" does not exist!!" << endl;
       return CMD_EXEC_ERROR;
    }
    if (!dbjson){}
    else{
        if (!doReplace) {
            cerr << "Error: DB exists. Use \"-Replace\" option for "
                 << "replacement.\n";
            return CMD_EXEC_ERROR;
        }
        cout << "DB is replaced..." << endl;
        dbjson.reset();
    }
//   if (!(ifs >> dbtbl)) return CMD_EXEC_ERROR;
    ifs >> dbjson;
    cout << "\"" << fileName << "\" was read in successfully." << endl;
 
    return CMD_EXEC_DONE;
}

void
DBReadCmd::usage(ostream& os) const
{
   os << "Usage: DBRead <(string jsonFile)> [-Replace]" << endl;
}

void
DBReadCmd::help() const
{
   cout << setw(15) << left << "DBRead: "
        << "read data from .json file" << endl;
}


//----------------------------------------------------------------------
//    DBSOrt <-Key | -Value>
//----------------------------------------------------------------------
CmdExecStatus
DBSortCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;

   if (myStrNCmp("-Key", token, 2) == 0) dbjson.sort(DBSortKey());
   else if (myStrNCmp("-Value", token, 2) == 0) dbjson.sort(DBSortValue());
   else return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);

   return CMD_EXEC_DONE;
}

void
DBSortCmd::usage(ostream& os) const
{
   os << "Usage: DBSOrt <-Key | -Value>" << endl;
}

void
DBSortCmd::help() const
{
   cout << setw(15) << left << "DBSOrt: "
        << "sort the JSON object by key or value" << endl;
}


//----------------------------------------------------------------------
//    DBSUm
//----------------------------------------------------------------------
CmdExecStatus
DBSumCmd::exec(const string& option)
{  
   // check option
   if (!CmdExec::lexNoOption(option))
      return CMD_EXEC_ERROR;

   if (dbjson.empty()) {
      cerr << "Error: The sum of the DB is nan." << endl;
      return CMD_EXEC_ERROR;
   }
   cout << "The sum of the DB is " << dbjson.sum() << "." << endl;

   return CMD_EXEC_DONE;
}

void
DBSumCmd::usage(ostream& os) const
{     
   os << "Usage: DBSUm" << endl;
}

void
DBSumCmd::help() const
{
   cout << setw(15) << left << "DBSUm: "
        << "compute the summation of the DB" << endl;
}

