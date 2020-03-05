/****************************************************************************
  FileName     [ cmdParser.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command parsing member functions for class CmdParser ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include "util.h"
#include "cmdParser.h"
#include "dirent.h"
#include <vector>
#include <unistd.h>
#include <stack>
using namespace std;

//----------------------------------------------------------------------
//    External funcitons
//----------------------------------------------------------------------
void mybeep();


//----------------------------------------------------------------------
//    Member Function for class cmdParser
//----------------------------------------------------------------------
// return false if file cannot be opened
// Please refer to the comments in "DofileCmd::exec", cmdCommon.cpp
bool
CmdParser::openDofile(const string& dof)
{
    fstream file;
    string name = "dofiles/" + dof;
    file.open(name.c_str());
    if(!file.is_open()){return false;}
    if(previous == dof){counter++;}
    previous = dof;
    if(counter == 1020)
    {
        cout << endl << "Warning : dofile recursion limit!!";
        while(_dofileStack.size() != 0){_dofileStack.pop();}
        return true;
    }
    _dofile = new ifstream(name.c_str());
    _dofileStack.push(_dofile);
    return true;
}

// Must make sure _dofile != 0
void
CmdParser::closeDofile()
{
    assert(_dofile != 0);
    _dofile->close();
    _dofileStack.pop();
    delete _dofile;
    if(!_dofileStack.empty()){_dofile = _dofileStack.top();}
}

// Return false if registration fails
bool
CmdParser::regCmd(const string& cmd, unsigned nCmp, CmdExec* e)
{
   // Make sure cmd hasn't been registered and won't cause ambiguity
   string str = cmd;
   unsigned s = str.size();
   if (s < nCmp) return false;
   while (true) {
      if (getCmd(str)) return false;
      if (s == nCmp) break;
      str.resize(--s);
   }

   // Change the first nCmp characters to upper case to facilitate
   //    case-insensitive comparison later.
   // The strings stored in _cmdMap are all upper case
   //
   assert(str.size() == nCmp);  // str is now mandCmd
   string& mandCmd = str;
   for (unsigned i = 0; i < nCmp; ++i)
      mandCmd[i] = toupper(mandCmd[i]);
   string optCmd = cmd.substr(nCmp);
   assert(e != 0);
   e->setOptCmd(optCmd);
   // insert (mandCmd, e) to _cmdMap; return false if insertion fails.
   return (_cmdMap.insert(CmdRegPair(mandCmd, e))).second;
}

// Return false on "quit" or if excetion happens
CmdExecStatus
CmdParser::execOneCmd()
{
    if(counter == 1021){cout << "System failure"; return CMD_EXEC_QUIT;}
    bool newCmd = false;
    if(_dofile != 0 && !_dofileStack.empty()){
        newCmd = readCmd(*_dofile);
    }
    else{
        newCmd = readCmd(cin);
    }
   // execute the command
    if (newCmd) {
        string option;
        CmdExec* e = parseCmd(option);
        if (e != 0)
        {
            return e->exec(option);
        }
    }
    return CMD_EXEC_NOP;
}

// For each CmdExec* in _cmdMap, call its "help()" to print out the help msg.
// Print an endl at the end.
void
CmdParser::printHelps() const
{
    for (auto& x: _cmdMap) {
    x.second->help();
  }
}

void
CmdParser::printHistory(int nPrint) const
{
   assert(_tempCmdStored == false);
   if (_history.empty()) {
      cout << "Empty command history!!" << endl;
      return;
   }
   int s = _history.size();
   if ((nPrint < 0) || (nPrint > s))
      nPrint = s;
   for (int i = s - nPrint; i < s; ++i)
      cout << "   " << i << ": " << _history[i] << endl;
}


//
// Parse the command from _history.back();
// Let string str = _history.back();
//
// 1. Read the command string (may contain multiple words) from the leading
//    part of str (i.e. the first word) and retrive the corresponding
//    CmdExec* from _cmdMap
//    ==> If command not found, print to cerr the following message:
//        Illegal command!! "(string cmdName)"
//    ==> return it at the end.
// 2. Call getCmd(cmd) to retrieve command from _cmdMap.
//    "cmd" is the first word of "str".
// 3. Get the command options from the trailing part of str (i.e. second
//    words and beyond) and store them in "option"
//
CmdExec*
CmdParser::parseCmd(string& option)
{
    assert(_tempCmdStored == false);
    assert(!_history.empty());
    string str = _history.back();
    string c;
    int a = 0;
    for(unsigned int i=0;i<str.size();i++)
    {
        if(str[i] == ' ')
        {
            if(a == 1){
                for(unsigned int j = i+1;j<str.size();j++)
                {
                    option.push_back(str[j]);
                }
                break;
            }
        }
        else if(str[i] != ' ')
        {
            a = 1;
            c.push_back(str[i]);
        }
    }
    if(!getCmd(c))
    {
        cerr << "Illegal command!! (" << c << ")" << endl;
    }
    else{
        return getCmd(c);
    }
    
    assert(str[0] != 0 && str[0] != ' ');
    return NULL;
}

// Remove this function for TODO...
//
// This function is called by pressing 'Tab'.
// It is to list the partially matched commands.
// "str" is the partial string before current cursor position. It can be 
// a null string, or begin with ' '. The beginning ' ' will be ignored.
//
// Several possibilities after pressing 'Tab'
// (Let $ be the cursor position)
// 1. LIST ALL COMMANDS
//    --- 1.1 ---
//    [Before] Null cmd
//    cmd> $
//    --- 1.2 ---
//    [Before] Cmd with ' ' only
//    cmd>     $
//    [After Tab]
//    ==> List all the commands, each command is printed out by:
//           cout << setw(12) << left << cmd;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location (including ' ')
//
// 2. LIST ALL PARTIALLY MATCHED COMMANDS
//    --- 2.1 ---
//    [Before] partially matched (multiple matches)
//    cmd> h$                   // partially matched
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$                   // and then re-print the partial command
//    --- 2.2 ---
//    [Before] partially matched (multiple matches)
//    cmd> h$llo                // partially matched with trailing characters
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$llo                // and then re-print the partial command
//
// 3. LIST THE SINGLY MATCHED COMMAND
//    ==> In either of the following cases, print out cmd + ' '
//    ==> and reset _tabPressCount to 0
//    --- 3.1 ---
//    [Before] partially matched (single match)
//    cmd> he$
//    [After Tab]
//    cmd> heLp $               // auto completed with a space inserted
//    --- 3.2 ---
//    [Before] partially matched with trailing characters (single match)
//    cmd> he$ahah
//    [After Tab]
//    cmd> heLp $ahaha
//    ==> Automatically complete on the same line
//    ==> The auto-expanded part follow the strings stored in cmd map and
//        cmd->_optCmd. Insert a space after "heLp"
//    --- 3.3 ---
//    [Before] fully matched (cursor right behind cmd)
//    cmd> hElP$sdf
//    [After Tab]
//    cmd> hElP $sdf            // a space character is inserted
//
// 4. NO MATCH IN FIRST WORD
//    --- 4.1 ---
//    [Before] No match
//    cmd> hek$
//    [After Tab]
//    ==> Beep and stay in the same location
//
// 5. FIRST WORD ALREADY MATCHED ON FIRST TAB PRESSING
//    --- 5.1 ---
//    [Before] Already matched on first tab pressing
//    cmd> help asd$gh
//    [After] Print out the usage for the already matched command
//    Usage: HELp [(string cmd)]
//    cmd> help asd$gh
//
// 6. FIRST WORD ALREADY MATCHED ON SECOND AND LATER TAB PRESSING
//    ==> Note: command usage has been printed under first tab press
//    ==> Check the word the cursor is at; get the prefix before the cursor
//    ==> So, this is to list the file names under current directory that
//        match the prefix
//    ==> List all the matched file names alphabetically by:
//           cout << setw(16) << left << fileName;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location
//    Considering the following cases in which prefix is empty:
//    --- 6.1.1 ---
//    [Before] if prefix is empty, and in this directory there are multiple
//             files and they do not have a common prefix,
//    cmd> help $sdfgh
//    [After] print all the file names
//    .               ..              Homework_3.docx Homework_3.pdf  Makefile
//    MustExist.txt   MustRemove.txt  bin             dofiles         include
//    lib             mydb            ref             src             testdb
//    cmd> help $sdfgh
//    --- 6.1.2 ---
//    [Before] if prefix is empty, and in this directory there are multiple
//             files and all of them have a common prefix,
//    cmd> help $orld
//    [After]
//    ==> auto insert the common prefix and make a beep sound
//    // e.g. in hw3/ref
//    cmd> help mydb-$orld
//    ==> DO NOT print the matched files
//    ==> If "tab" is pressed again, see 6.2
//    --- 6.1.3 ---
//    [Before] if prefix is empty, and only one file in the current directory
//    cmd> help $ydb
//    [After] print out the single file name followed by a ' '
//    // e.g. in hw3/bin
//    cmd> help mydb $
//    ==> If "tab" is pressed again, make a beep sound and DO NOT re-print 
//        the singly-matched file
//    --- 6.2 ---
//    [Before] with a prefix and with mutiple matched files
//    cmd> help M$Donald
//    [After]
//    Makefile        MustExist.txt   MustRemove.txt
//    cmd> help M$Donald
//    --- 6.3 ---
//    [Before] with a prefix and with mutiple matched files,
//             and these matched files have a common prefix
//    cmd> help Mu$k
//    [After]
//    ==> auto insert the common prefix and make a beep sound
//    ==> DO NOT print the matched files
//    cmd> help Must$k
//    --- 6.4 ---
//    [Before] with a prefix and with a singly matched file
//    cmd> help MustE$aa
//    [After] insert the remaining of the matched file name followed by a ' '
//    cmd> help MustExist.txt $aa
//    ==> If "tab" is pressed again, make a beep sound and DO NOT re-print 
//        the singly-matched file
//    --- 6.5 ---
//    [Before] with a prefix and NO matched file
//    cmd> help Ye$kk
//    [After] beep and stay in the same location
//    cmd> help Ye$kk
//
//    [Note] The counting of tab press is reset after "newline" is entered.
//
// 7. FIRST WORD NO MATCH
//    --- 7.1 ---
//    [Before] Cursor NOT on the first word and NOT matched command
//    cmd> he haha$kk
//    [After Tab]
//    ==> Beep and stay in the same location
void
CmdParser::listCmd(const string& str)
{
    string command;
    unsigned int front = 0;
    _tabPressCount++;
    string a[13] = {"DBAppend","DBAve","DBCount","DBMax","DBMin","DBPrint",
                    "DBRead","DBSort","DBSum","HELp","HIStory","Quit","DOfile"};
    vector<string> files;
    int o = 0;
    for(unsigned int i = 0;i<str.size();i++)
    {
        if(str[i] == ' ')
        {
            front = i+1;
            if(o == 1){o = 2;}
        }
        else
        {
            if(o == 0){o = 1;}
            if(o == 1){command.push_back(str[i]);front = i+1;}
            if(o == 2){break;}
        }
    }
    string option;
    if(front != str.size()){option = str.substr(front,str.size()-front);}
    if(command.size() == 0)
    {
        cout << endl;
        for(int i =0;i<13;i++)
        {
            cout << setw(12) << left << a[i];
            if(i ==4 || i == 9){cout << endl;}
        }
        cout << endl;
        reprintCmd();
    }
    else if(option.size() == 0)
    {
        int k = 0;
        for(int i =0;i<13;i++)
        {
            if(myStrNCmp(a[i],command,command.size()) == 0)
            {
                k++;
            }
        }
        if(k>1)
        {
            cout << endl;
            int q =0;
            for(int j =0;j<13;j++)
            {
                    if(!myStrNCmp(a[j],command,command.size()))
                    {
                        cout << setw(12) << left << a[j];
                        if(q ==4 || q == 9){cout << endl;}
                        q++;
                    }
                }
            reprintCmd();
        }
        else if(k == 1)
        {
            for(int j =0;j<13;j++)
                {
                    if(myStrNCmp(a[j],command,command.size()) == 0)
                    {
                        if(command.size() == a[j].size() && _tabPressCount/2 == 1)
                        {
                            insertChar(' ',1);
                            _tabPressCount = 0;
                        }
                        else if(command.size() == a[j].size()  && _tabPressCount/2 != 1)
                        {
                            char cwd[PATH_MAX];
                            string curdir;
                            if (getcwd(cwd, sizeof(cwd)) != NULL){curdir = cwd;}
                            const char *p = &curdir[0];
                            DIR *dir;
                            struct dirent *ent;
                            if ((dir = opendir (p)) != NULL) 
                            {
                              while ((ent = readdir (dir)) != NULL) 
                              {
                                files.push_back(ent->d_name);
                              }
                              closedir (dir);
                            }
                            if(files.size() == 3)
                            {
                                for(unsigned int a = 0;a<files[2].size();a++){insertChar(files[2][a],1);}
                                insertChar(' ',1);
                                for(int a = 0;a<=_readBufEnd - _readBufPtr+1;a++){deleteChar();}
                            }
                            else
                            {
                                unsigned int i = 100;
                                unsigned int c = 0;
                                int prefix = 0;
                                int dot = 0;
                                for(unsigned int b = 0;b<files.size();b++)
                                    {
                                        if(files[b][0] != '.'){if(files[b].size() < i){i = files[b].size();}}
                                        else{dot++;}
                                    }
                                for(unsigned int a = 1;a<=i;a++)
                                {
                                    for(unsigned int b = 0;b<files.size();b++)
                                    {
                                        if(files[b][0] != '.'){if(files[b].substr(0,a) == files[0].substr(0,a)){c++;}}
                                    }
                                    if(c == files.size()-dot){prefix = a;c = 0;}else{break;}
                                }
                                if(prefix == 0)
                                {
                                    cout << endl;
                                    for(unsigned int a = 0;a<files.size();a++)
                                    {
                                        cout << setw(17) << left << files[a];
                                        if(a%5 == 4){cout << endl;}
                                    }
                                    reprintCmd();
                                }
                                else
                                {
                                    for(int a = 0;a<prefix;a++){insertChar(files[0][a],1);}
                                    mybeep();
                                }
                            }
                           
                           
                        }
                        else if(command.size()<a[j].size())
                        {
                            for(unsigned int i = command.size();i<a[j].size();i++)
                            {insertChar(a[j][i],1);}
                            insertChar(' ',1);
                            _tabPressCount = 0;
                        }
                    }
                }
        }
        else if(k == 0){mybeep();}
    }
    else
    {
        int l = 0;
        for(int i =0;i<13;i++)
        {
            if(myStrNCmp(a[i],command,command.size()) == 0)
            {
                l++;
            }
        }
        if(l == 1)
        {
            for(int j =0;j<13;j++)
                {
                    if(!myStrNCmp(a[j],command,command.size()))
                    {
                        if(command.size() == a[j].size()  && _tabPressCount/2 == 1)
                        {
                            cout << endl;
                            switch(j)
                            {
                                case 0 : cout <<  "Usage: DBAPpend <(string key)><(int value)>" << endl;break;
                                case 1 : cout <<  "Usage: DBAVerage" << endl;break;
                                case 2 : cout <<  "Usage: DBCount" << endl;break;
                                case 3 : cout <<  "Usage: DBMAx" << endl;break;
                                case 4 : cout <<  "Usage: DBMIn" << endl;break;
                                case 5 : cout <<  "Usage: DBPrint [(string key)]" << endl;break;
                                case 6 : cout <<  "Usage: DBRead <(string jsonFile)> [-Replace]" << endl;break;
                                case 7 : cout <<  "Usage: DBSOrt <-Key | -Value>" << endl;break;
                                case 8 : cout <<  "Usage: DBSUm" << endl;break;
                                case 9 : cout <<  "Usage: HELp [(string cmd)]" << endl;break;
                                case 10 : cout << "Usage: HIStory [(int nPrint)]" << endl;break;
                                case 11 : cout << "Usage: Quit [-Force]" << endl;break;
                                case 12 : cout << "Usage: DOfile <(string file)>" << endl;break;
                            }
                            reprintCmd();
                        } 
                        else if(command.size() == a[j].size()  && _tabPressCount/2 != 1)
                        { 
                            char cwd[PATH_MAX];
                            string curdir;
                            if (getcwd(cwd, sizeof(cwd)) != NULL){curdir = cwd;}
                            const char *p = &curdir[0];
                            DIR *dir;
                            struct dirent *ent;
                            if ((dir = opendir (p)) != NULL) 
                            {
                              while ((ent = readdir (dir)) != NULL) 
                              {
                                files.push_back(ent->d_name);
                              }
                              closedir (dir);
                            }                        
                            if(files.size() == 3)
                            {
                                mybeep();
                            }
                            else
                            {
                                int c ;
                                int f ;
                                unsigned int d= option.size();
                                int w = 0;
                                for(unsigned int g =0;g<files.size();g++)
                                {
                                    if(files[g].substr(0,option.size()) == option){w++;}
                                }
                                if(w == 0){mybeep();}
                                else if(w == 1)
                                {
                                    for(unsigned int g =0;g<files.size();g++)
                                    {
                                        if(myStrNCmp(files[g],option,option.size()) == 0)
                                        {
                                            if(files[g] == option){mybeep();break;}
                                            for(unsigned int e = option.size();e<files[g].size();e++){insertChar(files[g][e],1);}
                                            insertChar(' ',1);
                                            break;
                                        }
                                    }
                                }
                                else
                                {   
                                    
                                    do
                                    {
                                        c = 0;
                                        for(unsigned int g =0;g<files.size();g++)
                                        {
                                            if(files[g].substr(0,option.size()) == option)
                                            {
                                                if(c == 0){f = g; c++;}
                                                else if(files[f][d] == files[g][d]){c++;}
                                            }
                                        }
                                        if(c == w){d++;}
                                    }while(c == w);
                                    c = 0;
                                    if(d == option.size())
                                    {
                                        cout << endl;
                                        for(unsigned int g =0;g<files.size();g++)
                                        {
                                             if(files[g].substr(0,option.size()) == option)
                                             {
                                                 cout << setw(17) << left << files[g];
                                                 c++;
                                                 if(c%5 == 0){cout << endl;}
                                             }
                                        }
                                        reprintCmd();
                                    }
                                    else
                                    {
                                        for(unsigned int g =0;g<files.size();g++)
                                        {
                                            if(files[g].substr(0,option.size()) == option)
                                            {
                                                f = g; 
                                                break;                                                
                                            }
                                        }
                                        for(unsigned int g = option.size();g<d;g++){insertChar(files[f][g],1);}
                                        mybeep();
                                    }
                                }
                            }
                        }
                        else if(command.size() != a[j].size()){mybeep();}
                    }
                }
        }
        else if(l == 0)
        {
            mybeep();
        }
    }
}

// cmd is a copy of the original input
//
// return the corresponding CmdExec* if "cmd" matches any command in _cmdMap
// return 0 if not found.
//
// Please note:
// ------------
// 1. The mandatory part of the command string (stored in _cmdMap) must match
// 2. The optional part can be partially omitted.
// 3. All string comparison are "case-insensitive".
//
CmdExec*
CmdParser::getCmd(string cmd)
    {
        CmdExec* e = 0;
        for(unsigned int i = 0;i<=cmd.size();i++)
        {
            cmd[i] = toupper(cmd[i]);
        }
        for(unsigned int i = 1;i<=cmd.size();i++)
        {
            if(_cmdMap.count(cmd.substr(0,i)) == 1)
            {
                if(myStrNCmp(cmd.substr(0,i) + (*_cmdMap[cmd.substr(0,i)]).getOptCmd(),cmd,i) == 0)
                {e = _cmdMap[cmd.substr(0,i)];}
            }
        }
    return e;
}


//----------------------------------------------------------------------
//    Member Function for class CmdExec
//----------------------------------------------------------------------
// return false if option contains an token
bool
CmdExec::lexNoOption(const string& option) const
{
   string err;
   myStrGetTok(option, err);
   if (err.size()) {
      errorOption(CMD_OPT_EXTRA, err);
      return false;
   }
   return true;
}

// Return false if error options found
// "optional" = true if the option is optional XD
// "optional": default = true
//
bool
CmdExec::lexSingleOption
(const string& option, string& token, bool optional) const
{
   size_t n = myStrGetTok(option, token);
   if (!optional) {
      if (token.size() == 0) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
   }
   if (n != string::npos) {
      errorOption(CMD_OPT_EXTRA, option.substr(n));
      return false;
   }
   return true;
}

// if nOpts is specified (!= 0), the number of tokens must be exactly = nOpts
// Otherwise, return false.
//
bool
CmdExec::lexOptions
(const string& option, vector<string>& tokens, size_t nOpts) const
{
   string token;
   size_t n = myStrGetTok(option, token);
   while (token.size()) {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
   }
   if (nOpts != 0) {
      if (tokens.size() < nOpts) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
      if (tokens.size() > nOpts) {
         errorOption(CMD_OPT_EXTRA, tokens[nOpts]);
         return false;
      }
   }
   return true;
}

CmdExecStatus
CmdExec::errorOption(CmdOptionError err, const string& opt) const
{
   switch (err) {
      case CMD_OPT_MISSING:
         cerr << "Error: Missing option";
         if (opt.size()) cerr << " after (" << opt << ")";
         cerr << "!!" << endl;
      break;
      case CMD_OPT_EXTRA:
         cerr << "Error: Extra option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_ILLEGAL:
         cerr << "Error: Illegal option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_FOPEN_FAIL:
         cerr << "Error: cannot open file \"" << opt << "\"!!" << endl;
      break;
      default:
         cerr << "Error: Unknown option error type!! (" << err << ")" << endl;
      exit(-1);
   }
   return CMD_EXEC_ERROR;
}

