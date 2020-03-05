/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>
#include <limits>

using namespace std;

// TODO: (Optionally) Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.

class HashKey
{
public:
    HashKey(size_t i,size_t j) : in0(i), in1(j) {}
    size_t operator() () const { return in0+in1; }
    bool operator == (const HashKey& k) const 
    {
        if(in0 == k.in0 && in1 == k.in1){return true;} 
        else if(in0 == k.in1 && in1 == k.in0){return true;}
        return false;
    }
private:
    size_t in0, in1; 
};


template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap(size_t b=0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashMap() { reset(); }

   // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the HashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
    {
        friend class HashMap<HashKey, HashData>;

    public:
        iterator(){}
        iterator(size_t bnum,size_t snum,size_t numbuck,vector<HashNode>* buck)
        {b = bnum;s = snum;nb = numbuck;bucket = buck;}
        ~iterator() {}
        const HashNode& operator * () const { return bucket[b][s]; }
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
        vector<HashNode>* bucket;
    };
   
   void init(size_t b) 
   {
      reset(); _numBuckets = b; _buckets = new vector<HashNode>[b]; 
    }
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

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

    // Point to the first valid data
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

    // check if k is in the hash...
    // if yes, return true;
    // else return false;
    bool check(const HashKey& k) const 
    { 
        for(unsigned int i = 0;i<_buckets[bucketNum(k)].size();i++)
        {
            if(k == _buckets[bucketNum(k)][i].first)
            {
                return true;
            }
        }
        return false; 
    }
    // query if k is in the hash...
    // if yes, replace d with the data in the hash and return true;
    // else return false;
    bool query(const HashKey& k, HashData& d) const 
    {
        for(unsigned int i = 0;i<_buckets[bucketNum(k)].size();i++)
        {
            if(k == _buckets[bucketNum(k)][i].first)
            {
                d = _buckets[bucketNum(k)][i].second;
                return true;
            }
        }
        return false; 
    }
    // update the entry in hash that is equal to k (i.e. == return true)
    // if found, update that entry with d and return true;
    // else insert d into hash as a new entry and return false;
    bool update(const HashKey& k, HashData& d) 
    {
        for(unsigned int i = 0;i<_buckets[bucketNum(k)].size();i++)
        {
            if(k == _buckets[bucketNum(k)][i].first)
            {
                _buckets[bucketNum(k)][i] = d;
                return true;
            }
        }
        _buckets[bucketNum(k)].push_back(HashNode(k,d));
        return false; 
    }
    // return true if inserted d successfully (i.e. k is not in the hash)
    // return false is k is already in the hash ==> will not insert
    bool insert(const HashKey& k, const HashData& d) 
    { 
        for(unsigned int i = 0;i<_buckets[bucketNum(k)].size();i++)
        {
            if(k == _buckets[bucketNum(k)][i].first)
            {
                return false;
            }
        }
        _buckets[bucketNum(k)].push_back(HashNode(k,d));
        return true;
    }

    // return true if removed successfully (i.e. k is in the hash)
    // return fasle otherwise (i.e. nothing is removed)
    bool remove(const HashKey& k) 
    { 
        for(unsigned int i = 0;i<_buckets[bucketNum(k)].size();i++)
        {
            if(k == _buckets[bucketNum(k)][i])
            {
                _buckets[bucketNum(k)][i] = _buckets[bucketNum(k)].back();
                _buckets[bucketNum(k)].pop_back();
                return true;
            }
        }
        return false; 
    }
private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }

};


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.

class CacheKey
{
public:
    CacheKey() {}
    size_t operator() () const { return 0; }
    bool operator == (const CacheKey&) const { return true; }
private:
}; 
 
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
   Cache() : _size(0), _cache(0) {}
   Cache(size_t s) : _size(0), _cache(0) { init(s); }
   ~Cache() { reset(); }

   // NO NEED to implement Cache::iterator class

   // TODO: implement these functions
   //
   // Initialize _cache with size s
   void init(size_t s) { reset(); _size = s; _cache = new CacheNode[s]; }
   void reset() {  _size = 0; if (_cache) { delete [] _cache; _cache = 0; } }

   size_t size() const { return _size; }

   CacheNode& operator [] (size_t i) { return _cache[i]; }
   const CacheNode& operator [](size_t i) const { return _cache[i]; }

   // return false if cache miss
   bool read(const CacheKey& k, CacheData& d) const {
      size_t i = k() % _size;
      if (k == _cache[i].first) {
         d = _cache[i].second;
         return true;
      }
      return false;
   }
   // If k is already in the Cache, overwrite the CacheData
   void write(const CacheKey& k, const CacheData& d) {
      size_t i = k() % _size;
      _cache[i].first = k;
      _cache[i].second = d;
   }

private:
   // Do not add any extra data member
   size_t         _size;
   CacheNode*     _cache;
};


#endif // MY_HASH_H
