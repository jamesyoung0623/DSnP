/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
    friend class BSTree<T>;
    friend class BSTree<T>::iterator;
    BSTreeNode(const T& d, BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0, BSTreeNode<T>* p = 0):
        _data(d), _left(l), _right(r), _parent(p) {}

    // [NOTE] DO NOT ADD or REMOVE any data member
    T              _data;
    BSTreeNode<T>*  _left;
    BSTreeNode<T>*  _right;
    BSTreeNode<T>*  _parent;
    BSTreeNode<T>* findsmall(BSTreeNode<T>* n)
    {
        if(n->_left == n)
        {
            if(n->_parent == n){return n;}
            if(n->_parent->_right == n){return n->_parent;}
            else
            {
                while(true)
                {
                    n = n->_parent;
                    if(n->_parent == n){return n;}
                    if(n->_parent->_right == n){return n->_parent;}
                }
            }
        }
        else if(n->_left->_right == n->_left){return n->_left;}
        else
        {
            n = n->_left;
            while(true)
            {
                if(n->_right == n){return n;}
                else{n = n->_right;}
            }
        }
    }
    BSTreeNode<T>* findbig(BSTreeNode<T>* n)
    {
        if(n->_right == n)
        {
            if(n->_parent == n){return n;}
            if(n->_parent->_left == n){return n->_parent;}
            else
            {
                while(true)
                {
                    n = n->_parent;
                    if(n->_parent == n){return n;}
                    if(n->_parent->_left == n){return n->_parent;}
                }
            }
        }
        else if(n->_right->_left == n->_right){return n->_right;}
        else
        {
            n = n->_right;
            while(true)
            {
                if(n->_left == n){return n;}
                else{n = n->_left;}
            }
        }
    }
};


template <class T>
class BSTree
{

public:
    BSTree() {
        _root = new BSTreeNode<T>(T());
        _root->_left = _root->_right = _root->_parent = _root;
        _first = _root;
        _end = _root;
        _size = 0;    
    }
    ~BSTree() { clear(); delete _root; }

    class iterator
    {
        friend class BSTree;

    public:
        iterator(BSTreeNode<T>* n= 0): _node(n) {}
        iterator(const iterator& i) : _node(i._node) {}
        ~iterator() {} 

        const T& operator * () const { return _node->_data; }
        T& operator * () { return _node->_data; }
        iterator& operator ++ () {_node = _node->findbig(_node);return *(this); }
        iterator operator ++ (int) {BSTreeNode<T>* n = _node;_node = _node->findbig(_node);return n;}
        iterator& operator -- () {_node = _node->findsmall(_node);return *(this); }
        iterator operator -- (int) {BSTreeNode<T>* n = _node;_node = _node->findsmall(_node);return n;}

        iterator& operator = (const iterator& i) {_node->_data = i._node->_data;return *(this);}

        bool operator != (const iterator& i) const {if(_node != i._node){return true;} return false; }
        bool operator == (const iterator& i) const {if(_node == i._node){return true;} return false; }

    private:
        BSTreeNode<T>* _node;
    };

    iterator begin() const {iterator i(_first);return i;}
    iterator end() const {iterator i(_end);return i;}
    bool empty() const {if(_size == 0){return true;} return false; }
    size_t size() const {return _size;}

    void insert(const T& x)
    {
        if(_size == 0)
        {
            BSTreeNode<T>* n = new BSTreeNode<T>(x);
            n->_left = n;
            n->_right = _root;
            n->_parent = n;
            _root->_parent = n;
            _end = _root;
            _root = n;
            _first = _root;
            _size++;
        }
        else
        {
            BSTreeNode<T>* r = _root;
            while(true)
            {
                if(r->_data >= x)
                {
                    if(r->_left == r)
                    {
                        if(r == _first)
                        {
                            r->_left = new BSTreeNode<T>(x);
                            r->_left->_left = r->_left;
                            r->_left->_right = r->_left;
                            r->_left->_parent = r;
                            _first = r->_left;
                            _size++;
                            break;
                        }
                        else
                        {
                            r->_left = new BSTreeNode<T>(x);
                            r->_left->_left = r->_left;
                            r->_left->_right = r->_left;
                            r->_left->_parent = r;
                            _size++;
                            break;  
                        }
                    }
                    else 
                    {
                        r = r->_left;
                    }
                }
                else if(r->_data < x)
                {
                    if(r->_right == _end)
                    {
                        r->_right = new BSTreeNode<T>(x);
                        r->_right->_left = r->_right;
                        r->_right->_right = _end;
                        _end->_parent = r->_right;
                        r->_right->_parent = r;
                        _size++;
                        break;
                    }
                    else if(r->_right == r)
                    {
                        r->_right = new BSTreeNode<T>(x);
                        r->_right->_left = r->_right;
                        r->_right->_right = r->_right;
                        r->_right->_parent = r;
                        _size++;
                        break;
                    }
                    else
                    {
                        r = r->_right;
                    }
                }
                
            }
        }
    }
    void pop_front() 
    {
        BSTreeNode<T>* t = _first;
        if(_size == 0){return;}
        else if(_first == _root)
        {
            _root = _first->_right;
            _first = _first->findbig(_first);
            _root->_parent = _root;
        }
        else if(_first->_right != _first)
        {
            _first->_parent->_left = _first->_right;
            _first->_right->_parent = _first->_parent;
            _first = _first->findbig(_first);
        }
        else
        {
            _first = _first->_parent;
            _first->_left = _first;
        }
        _size--;
        delete t;
    }
    void pop_back() 
    {
        BSTreeNode<T>* t = _end->_parent;
        if(_size == 0){return;}
        else if(_size == 1)
        {
            _root = _end;
            _root->_parent = _root;
            _root->_left = _root;
            _root->_right = _root;
            _first = _root;
        }
        else if(_end->_parent == _root)
        {
            _end->_parent = _root->findsmall(_root);
            _end->_parent->_right = _end;
            _root = _root->_left;
            _root->_parent = _root;
        }
        else if(_end->_parent->_left != _end->_parent)
        {
            BSTreeNode<T>* n = _end->_parent;
            _end->_parent = _end->_parent->findsmall(_end->_parent);
            _end->_parent->_right = _end;
            n->_parent->_right = n->_left;
            n->_left->_parent = n->_parent;
        }
        else
        {
            _end->_parent = _end->_parent->_parent;
            _end->_parent->_right = _end;
        }
        _size--;
        delete t;
    }
    
    void print() const { }
    
    bool erase(iterator pos) 
    { 
        if(_size == 0){return false;}
        else if(pos._node == _end->_parent)
        {
            pop_back();
            return true;
        }
        else if(pos._node == _first)
        {
            pop_front();
            return true;
        }
        else if(pos._node->_left == pos._node && pos._node->_right == pos._node)
        {
            if(pos._node->_parent->_left == pos._node){pos._node->_parent->_left = pos._node->_parent;}
            else if(pos._node->_parent->_right == pos._node){pos._node->_parent->_right = pos._node->_parent;}
            _size--;
            delete pos._node;
            return true;
        }
        else if(pos._node->_left != pos._node && pos._node->_right != pos._node)
        {
            BSTreeNode<T>* n = pos._node->findsmall(pos._node);            
            if(n == pos._node->_left)
            {
                if(pos._node == _root){_root = n;}
                else if(pos._node->_parent->_left == pos._node){pos._node->_parent->_left = n;}
                else if(pos._node->_parent->_right == pos._node){pos._node->_parent->_right = n;}
                pos._node->_right->_parent = n;
                if(n == _root){n->_parent = n;}
                else{n->_parent = pos._node->_parent;}
                if(n->_right != _end){n->_right = pos._node->_right;}
            }
            else
            {
                if(pos._node == _root){_root = n;}
                else if(pos._node->_parent->_left == pos._node){pos._node->_parent->_left = n;}
                else if(pos._node->_parent->_right == pos._node){pos._node->_parent->_right = n;}
                pos._node->_left->_parent = n;
                pos._node->_right->_parent = n;
                if(n->_left != n){n->_parent->_right = n->_left;n->_left->_parent = n->_parent;}
                else{n->_parent->_right = n->_parent;}
                if(n == _root){n->_parent = n;}
                else{n->_parent = pos._node->_parent;}
                n->_left = pos._node->_left;
                if(n->_right != _end){n->_right = pos._node->_right;}
            }
            _size--;
            delete pos._node;
            return true;
        }
        else if(pos._node->_left == pos._node)
        {
            BSTreeNode<T>* n = pos._node->findbig(pos._node);
            if(n == pos._node->_right)
            {
                if(pos._node == _root){_root = n;}
                else if(pos._node->_parent->_left == pos._node){pos._node->_parent->_left = n;}
                else if(pos._node->_parent->_right == pos._node){pos._node->_parent->_right = n;}
                if(n == _root){n->_parent = n;}
                else{n->_parent = pos._node->_parent;}
            }
            else
            {
                if(pos._node == _root){_root = n;}
                else if(pos._node->_parent->_left == pos._node){pos._node->_parent->_left = n;}
                else if(pos._node->_parent->_right == pos._node){pos._node->_parent->_right = n;}
                pos._node->_right->_parent = n;
                if(n->_right != n){n->_parent->_left = n->_right;n->_right->_parent = n->_parent;}
                else{n->_parent->_left = n->_parent;}
                if(n == _root){n->_parent = n;}
                else{n->_parent = pos._node->_parent;}
                if(n->_right != _end){n->_right = pos._node->_right;}
            }
            _size--;
            delete pos._node;
            return true;
        }
        else if(pos._node->_right == pos._node)
        {
            BSTreeNode<T>* n = pos._node->findsmall(pos._node);
            if(n == pos._node->_left)
            {
                if(pos._node == _root){_root = n;}
                else if(pos._node->_parent->_left == pos._node){pos._node->_parent->_left = n;}
                else if(pos._node->_parent->_right == pos._node){pos._node->_parent->_right = n;}
                if(n == _root){n->_parent = n;}
                else{n->_parent = pos._node->_parent;}
            }
            else
            {
                if(pos._node == _root){_root = n;}
                else if(pos._node->_parent->_left == pos._node){pos._node->_parent->_left = n;}
                else if(pos._node->_parent->_right == pos._node){pos._node->_parent->_right = n;}
                pos._node->_left->_parent = n;
                if(n->_left != n){n->_parent->_right = n->_left;n->_left->_parent = n->_parent;}
                else{n->_parent->_right = n->_parent;}
                if(n == _root){n->_parent = n;}
                else{n->_parent = pos._node->_parent;}
                n->_left = pos._node->_left;
            } 
            _size--;
            delete pos._node;
            return true;
        }
        return false;
    }
    bool erase(const T& x) 
    { 
        iterator i = find(x);
        if(i._node == _end){return false;}
        erase(i);
        return true;
    }

    iterator find(const T& x) 
    { 
        BSTreeNode<T>* n = _root;
        iterator e(_end);
        while(true)
        {
            if(n->_data == x){iterator i(n);return i;}
            else if(n->_data > x)
            {
                if(n == n->_left){return e;}
                else{n = n->_left;}
            }
            else if(n->_data < x)
            {
                if(n == n->_right || _end == n->_right){return e;}
                else{n = n->_right;}
            }
        }
        return e;
    }

    void clear() 
    { 
        int i = _size;
        for(;i>=0;i--){pop_front();}
    }
    void sort() { }
private:
    BSTreeNode<T>*  _root; 
    BSTreeNode<T>*  _first;
    BSTreeNode<T>*  _end;
    size_t        _size;
};

#endif // BST_H
