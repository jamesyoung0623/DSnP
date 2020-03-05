/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
    HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
    ~HashSet() { reset(); }

    class iterator
    {
        friend class HashSet<Data>;

    public:
        iterator(){}
        iterator(size_t bnum,size_t snum,size_t numbuck,vector<Data>* buck){b = bnum;s = snum;nb = numbuck;bucket = buck;}
        iterator(const iterator& i): b(i.b),s(i.s) {}
        ~iterator() {}
        const Data& operator * () const { return bucket[b][s]; }
        iterator& operator ++ () 
        {
            if(b == nb){}
            else if(bucket[b].size() > s+1){s++;}
            else
            {
                b++;
                s = 0;
                while(bucket[b].size() == 0 && b<nb){b++;}
            }
            return *(this);
        }
        iterator operator ++ (int) 
        {
            iterator it(*(this));
            if(b == nb){}
            else if(bucket[b].size() > s+1){s++;}
            else
            {
                b++;
                s = 0;
                while(bucket[b].size() == 0 && b<nb){b++;}
            }
            return it;
        }
        iterator& operator -- () 
        {
            if(s > 0){s--;}
            else
            {
                b--;
                while(bucket[b].size() == 0 && b >= 0){b--;}
                if(b >= 0){s = bucket[b].size()-1;}
            }
            return *(this);
        }
        iterator operator -- (int) 
        {
            iterator it(*(this));
            if(s > 0){s--;}
            else
            {
                b--;
                while(bucket[b].size() == 0 && b >= 0){b--;}
                if(b >= 0){s = bucket[b].size()-1;}
            }
            return it;
        }
        iterator& operator = (const iterator& i) {b = i.b;s = i.s;return *(this);}
        bool operator == (const iterator& i) const {if(b == i.b && s == i.s){return true;}return false;}
        bool operator != (const iterator& i) const {if(b == i.b && s == i.s){return false;}return true;}
    private:
        size_t b;
        size_t s;
        size_t nb;
        vector<Data>* bucket;
    };

    void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
    void reset() 
    {
        _numBuckets = 0;
        if (_buckets) { delete [] _buckets; _buckets = 0; }
    }
    void clear() 
    {
        for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
    }
    size_t numBuckets() const { return _numBuckets; }

    vector<Data>& operator [] (size_t i) { return _buckets[i]; }
    const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

    iterator begin() const 
    {
        for(unsigned int i = 0;i<_numBuckets;i++)
        {
            if(_buckets[i].size() != 0)
            {
                return iterator(i,0,_numBuckets,_buckets);
            }
        }
        return end(); 
    }
    // Pass the end
    iterator end() const 
    {
        return iterator(_numBuckets,0,_numBuckets,_buckets); 
    }
    // return true if no valid data
    bool empty() const 
    { 
        if(size() == 0){return true;}
        return false; 
    }
    // number of valid data
    size_t size() const 
    { 
        size_t s = 0; 
        for(unsigned int i = 0;i<_numBuckets;i++)
        {
            s += _buckets[i].size();
        }
        return s; 
    }
    
    // check if d is in the hash...
    // if yes, return true;
    // else return false;
    bool check(const Data& d) const 
    { 
        for(unsigned int i = 0;i<_buckets[bucketNum(d)].size();i++)
        {
            if(d == _buckets[bucketNum(d)][i])
            {
                return true;
            }
        }
        return false; 
    }

    // query if d is in the hash...
    // if yes, replace d with the data in the hash and return true;
    // else return false;
    bool query(Data& d) const 
    {
        for(unsigned int i = 0;i<_buckets[bucketNum(d)].size();i++)
        {
            if(d == _buckets[bucketNum(d)][i])
            {
                d = _buckets[bucketNum(d)][i];
                return true;
            }
        }
        return false; 
    }
    
    // update the entry in hash that is equal to d (i.e. == return true)
    // if found, update that entry with d and return true;
    // else insert d into hash as a new entry and return false;
    bool update(const Data& d) 
    {
        for(unsigned int i = 0;i<_buckets[bucketNum(d)].size();i++)
        {
            if(d == _buckets[bucketNum(d)][i])
            {
                _buckets[bucketNum(d)][i] = d;
                return true;
            }
        }
        _buckets[bucketNum(d)].push_back(d);
        return false; 
    }

    // return true if inserted successfully (i.e. d is not in the hash)
    // return false is d is already in the hash ==> will not insert
    bool insert(const Data& d) 
    { 
        for(unsigned int i = 0;i<_buckets[bucketNum(d)].size();i++)
        {
            if(d == _buckets[bucketNum(d)][i])
            {
                return false;
            }
        }
        _buckets[bucketNum(d)].push_back(d);
        return true;
    }

    // return true if removed successfully (i.e. d is in the hash)
    // return fasle otherwise (i.e. nothing is removed)
    bool remove(const Data& d) 
    { 
        size_t s = _buckets[bucketNum(d)].size();
        for(unsigned int i = 0;i<s;i++)
        {
            if(d == _buckets[bucketNum(d)][i])
            {
                _buckets[bucketNum(d)][i] = _buckets[bucketNum(d)][s-1];
                _buckets[bucketNum(d)].pop_back();
                return true;
            }
        }
        return false; 
    }

private:
    size_t            _numBuckets;
    vector<Data>*     _buckets;

    size_t bucketNum(const Data& d) const {
        return (d() % _numBuckets); }
};

#endif
