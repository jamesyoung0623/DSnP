/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
    // TODO: decide the initial value for _isSorted
    Array() : _data(0), _size(0), _capacity(0) ,_isSorted(false){}
    ~Array() { delete []_data; }

    // DO NOT add any more data member or function for class iterator
    class iterator
    {
        friend class Array;

    public:
        iterator(T* n= 0): _node(n) {}
        iterator(const iterator& i): _node(i._node) {}
        ~iterator() {} // Should NOT delete _node

        // TODO: implement these overloaded operators
        const T& operator * () const { return (*this); }
        T& operator * () { return (*_node); }
        iterator& operator ++ () {_node++;return (*this);}
        iterator operator ++ (int) {T* n = _node;_node++;return n;}
        iterator& operator -- () {_node--;return (*this);}
        iterator operator -- (int) {T* n = _node;_node--;return n;}

        iterator operator + (int i) const {return _node + i;}
        iterator& operator += (int i) {_node += i;return (*this);}

        iterator& operator = (const iterator& i) {_node = i;return (*this);}

        bool operator != (const iterator& i) const {if(_node != i._node){return true;}return false;}
        bool operator == (const iterator& i) const {if(_node == i._node){return true;}return false;}

    private:
        T*    _node;
    };

    // TODO: implement these functions
    iterator begin() const {if(_size > 0){T* n = &_data[0];iterator i(n);return i;}return 0;}
    iterator end() const {if(_size > 0){T* n = &_data[_size];iterator i(n);return i;}return 0;}
    bool empty() const {if(_size == 0){return true;} return false; }
    size_t size() const {return _size;}

    T& operator [] (size_t i) {if(_size > 0){if(i >=0 && i < _size){return _data[i];}}}
    const T& operator [] (size_t i) const {if(_size > 0){if(i >=0 && i < _size){return _data[i];}}}

    void push_back(const T& x) 
    {
        if(_size == _capacity)
        {
            if(_capacity == 0)
            {
                _data = new T [1];
                _data[0] = x;
                _size = 1;
                _capacity = 1;
            }
            else
            {
                T* d = new T [2*_capacity];
                T* a = _data;
                for(unsigned int i = 0;i<_capacity;i++)
                {
                    d[i] = _data[i];
                }
                d[_capacity] = x;
                _data = d;
                delete []a;
                _size++;
                _capacity*=2;
            }
        }
        else
        {
            _data[_size] = x;
            _size++;
        }
    }
    void pop_front() 
    {
        if(_size > 0)
        {
            _data[0] = _data[_size-1];
            _size--;
            _isSorted = false;
        }
    }
    void pop_back() 
    {
        if(_size > 0)
        {
            _size--;
        }
    }

    bool erase(iterator pos) 
    {
        if(_size > 0)
        {
            *pos._node = _data[_size-1];
            _size--;
            return true;
        }
        return false; 
    }
    bool erase(const T& x) 
    { 
        if(_size > 0)
        {
            for(unsigned int i = 0;i<_size;i++)
            {
                if(_data[i] == x)
                {
                    _data[i] = _data[_size-1];
                    _size--;
                    return true;
                }
            }
            return false;
        }
        return false; 
    }

    iterator find(const T& x) 
    { 
        for(unsigned int i = 0;i<_size;i++)
        {
            if(_data[i] == x)
            {
                T* n = &_data[i];
                iterator k(n);
                return k;
            }
        }
        return end(); 
    }

    void clear() {_size = 0;}

    // [Optional TODO] Feel free to change, but DO NOT change ::sort()
    void sort() const { if (!empty()) ::sort(_data, _data+_size); _isSorted = true;}

    // Nice to have, but not required in this homework...
    // void reserve(size_t n) { ... }
    // void resize(size_t n) { ... }

private:
    // [NOTE] DO NOT ADD or REMOVE any data member
    T*            _data;
    size_t        _size;       // number of valid elements
    size_t        _capacity;   // max number of elements
    mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

    // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
