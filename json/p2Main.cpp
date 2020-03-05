/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "p2Json.h"
#include <iomanip>
 using namespace std;

int main()
{
    Json json;
    string jsonFile;
    cout << "Please enter the file name: ";
    cin >> jsonFile;
    if (json.read(jsonFile))
       cout << "File \"" << jsonFile << "\" was read in successfully." << endl;
    else {
       cerr << "Failed to read in file \"" << jsonFile << "\"!" << endl;
       exit(-1); // jsonFile does not exist.
    }
    
     while(true)
    {
    string c;
    getline(cin,c);
    if( int(c[0])==80 && int(c[1])==82 && int(c[2])==73 && int(c[3])==78 && int(c[4])==84)
    {
        if(json.get() != 0)
        {
            json.printobj();
        } 
        else
        {
            cout << "{" << endl << "}" << endl;
        }
    }
    if( int(c[0])==83 && int(c[1])==85 && int(c[2])==77)
    {
        if(json.get() != 0)
        {
            cout << "The summation of the values is: " << json.sum() << "." << endl;
        } 
        else
        {
            cout << "Error: No element found!!" << endl;
        }
    }
    if( int(c[0])==65 && int(c[1])==86 && int(c[2])==69)
    {
        if(json.get() != 0)
        {
            cout << "The average of the values is: " ;
            cout << setprecision(1) << fixed;
            cout << json.ave();
            cout << "." << endl;
        } 
        else
        {
            cout << "Error: No element found!!" << endl;
        }
    }
    if( int(c[0])==77 && int(c[1])==65 && int(c[2])==88)
    {
        if(json.get() != 0)
        {
            int i = json.max();
            cout << "The maximum element is: { " << json.k(i) << " : " << json.v(i) << " }." << endl;
        } 
        else
        {
            cout << "Error: No element found!!" << endl;
        }
    }
    if( int(c[0])==77 && int(c[1])==73 && int(c[2])==78)
    {
        if(json.get() != 0)
        {
            int i = json.min();
            cout << "The minimum element is: { " << json.k(i) << " : " << json.v(i) << " }." << endl;
        } 
        else
        {
            cout << "Error: No element found!!" << endl;
        }
    }
    if( int(c[0])==69 && int(c[1])==88 && int(c[2])==73 && int(c[3])==84)
    {
        break;
    }
    if( int(c[0])==65 && int(c[1])==68 && int(c[2])==68)
    {
        string kk, vv;
        unsigned int d = 4;
        int a = 34;
        kk.push_back(char(a));
        while(int(c[d])!= 32)
        {
            kk.push_back(c[d]);
            d++;
        }
        kk.push_back(char(a));
        d++;
        while(d < c.size())
        {
            vv.push_back(c[d]);
            d++;
        }
        int va = atoi(vv.c_str());
        JsonElem j(kk,va);
        json.add(j);
        
    }
         cout << "Enter command: ";
    }

}
