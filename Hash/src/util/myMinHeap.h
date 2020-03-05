/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
    MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
    ~MinHeap() {}

    void clear() { _data.clear(); }

    // For the following member functions,
    // We don't respond for the case vector "_data" is empty!
    const Data& operator [] (size_t i) const { return _data[i]; }   
    Data& operator [] (size_t i) { return _data[i]; }

    size_t size() const { return _data.size(); }

    // TODO
    const Data& min() const {return _data[0]; }
    void insert(const Data& d) 
    {
        _data.push_back(d);
        int i = _data.size()-1;
        while((i-1) >= 0)
        {
            if(_data[i].getLoad() < _data[(i-1)/2].getLoad())
            {
                Data t = _data[(i-1)/2];
                _data[(i-1)/2] = _data[i];
                _data[i] = t;
                i = (i-1)/2;
            }
            else
            {
                break;
            }
        }
    }   
    void delMin() 
    {
        _data[0] = _data[_data.size()-1];
        _data.pop_back();
        unsigned int i = 0;
        while(i < _data.size())
        {
            if(2*i+1 < _data.size() && 2*i+2 < _data.size())
            {
                if(_data[2*i+1].getLoad() <= _data[2*i+2].getLoad())
                {
                    if(_data[2*i+1].getLoad() < _data[i].getLoad())
                    {
                        Data t = _data[i];
                        _data[i] = _data[2*i+1];
                        _data[2*i+1] = t;
                        i = 2*i+1;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    if(_data[2*i+2].getLoad() < _data[i].getLoad())
                    {
                        Data t = _data[i];
                        _data[i] = _data[2*i+2];
                        _data[2*i+2] = t;
                        i = 2*i+2;
                    }
                    else
                    {
                        break;
                    } 
                }
            }
            else if(2*i+1 < _data.size())
            {
                if(_data[2*i+1].getLoad() < _data[i].getLoad())
                {
                    Data t = _data[i];
                    _data[i] = _data[2*i+1];
                    _data[2*i+1] = t;
                    i = 2*i+1;
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }
    void delData(size_t i) 
    {
        _data[i] = _data[_data.size()-1];
        _data.pop_back();
        while(i >= 1)
        {
            if(_data[i].getLoad() < _data[(i-1)/2].getLoad())
            {
                Data t = _data[(i-1)/2];
                _data[(i-1)/2] = _data[i];
                _data[i] = t;
                i = (i-1)/2;
            }
            else
            {
                break;
            }
        }
        while(i < _data.size())
        {
            if(2*i+1 < _data.size() && 2*i+2 < _data.size())
            {
                if(_data[2*i+1].getLoad() <= _data[2*i+2].getLoad())
                {
                    if(_data[2*i+1].getLoad() < _data[i].getLoad())
                    {
                        Data t = _data[i];
                        _data[i] = _data[2*i+1];
                        _data[2*i+1] = t;
                        i = 2*i+1;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    if(_data[2*i+2].getLoad() < _data[i].getLoad())
                    {
                        Data t = _data[i];
                        _data[i] = _data[2*i+2];
                        _data[2*i+2] = t;
                        i = 2*i+2;
                    }
                    else
                    {
                        break;
                    } 
                }
            }
            else if(2*i+1 < _data.size())
            {
                if(_data[2*i+1].getLoad() < _data[i].getLoad())
                {
                    Data t = _data[i];
                    _data[i] = _data[2*i+1];
                    _data[2*i+1] = t;
                    i = 2*i+1;
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

private:
    // DO NOT add or change data members
    vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
