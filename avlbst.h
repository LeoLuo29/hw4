#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing. 我TM谢谢你
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode(){}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    // balance convention: balance = height(right) - height(left)
    void insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* child);
    void removeFix(AVLNode<Key,Value>* node, int8_t diff);
    void leftRotate(AVLNode<Key,Value>* node);
    void rightRotate(AVLNode<Key,Value>* node);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    // traverse BST
    if (this->root_ == nullptr){
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        return;
    }
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    while (true){
        if (new_item.first == current->getKey()){
            current->setValue(new_item.second); // shape unchanged, no rebalance needed
            return;
        }
        else if (new_item.first < current->getKey()){
            if (current->getLeft() == nullptr){
                AVLNode<Key, Value>* child = new AVLNode<Key, Value>(new_item.first, new_item.second, current);
                current->setLeft(child);
                current->updateBalance(-1);             // left side got taller
                if (current->getBalance() != 0) insertFix(current, child);
                return;
            }
            current = current->getLeft();
        }
        else { // .first > key; so go right.
            if (current->getRight() == nullptr){
                AVLNode<Key, Value>* child = new AVLNode<Key, Value>(new_item.first, new_item.second, current);
                current->setRight(child);
                current->updateBalance(+1);             // right side got taller
                if (current->getBalance() != 0) insertFix(current, child);
                return;
            }
            current = current->getRight();
        }
    }
    


}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    AVLNode<Key, Value>* n = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (n == nullptr) return; // key not in tree

    // two children: swap with predecessor so n has at most one (left) child
    if (n->getLeft() != nullptr && n->getRight() != nullptr){
        AVLNode<Key, Value>* pred =
            static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(n));
        nodeSwap(n, pred);
    }

    AVLNode<Key, Value>* parent = n->getParent();
    AVLNode<Key, Value>* child = (n->getLeft() != nullptr) ? n->getLeft() : n->getRight();

    // unlink n, remembering which side of parent got shorter
    int8_t diff = 0;
    if (parent == nullptr){
        this->root_ = child;
    }
    else if (parent->getLeft() == n){
        parent->setLeft(child);
        diff = +1;  // left side shrank -> balance moves right
    }
    else{
        parent->setRight(child);
        diff = -1;  // right side shrank -> balance moves left
    }
    if (child != nullptr) child->setParent(parent);
    delete n;

    removeFix(parent, diff);
}

/*
 * Walks up from a newly inserted node's parent, fixing balances and
 * rotating at most once. Precondition: parent's balance is -1 or +1.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* child)
{
    if (parent == nullptr) return;
    AVLNode<Key, Value>* grand = parent->getParent();
    if (grand == nullptr) return;

    if (parent == grand->getLeft()){
        grand->updateBalance(-1);
        if (grand->getBalance() == 0) return;                     // height unchanged
        if (grand->getBalance() == -1){ insertFix(grand, parent); return; }
        // balance == -2: rotate
        if (child == parent->getLeft()){                          // zig-zig
            rightRotate(grand);
            parent->setBalance(0);
            grand->setBalance(0);
        }
        else{                                                     // zig-zag
            leftRotate(parent);
            rightRotate(grand);
            if (child->getBalance() == -1){ parent->setBalance(0);  grand->setBalance(+1); }
            else if (child->getBalance() == 0){ parent->setBalance(0); grand->setBalance(0); }
            else { parent->setBalance(-1); grand->setBalance(0); }
            child->setBalance(0);
        }
    }
    else{ // parent is right child: mirror image
        grand->updateBalance(+1);
        if (grand->getBalance() == 0) return;
        if (grand->getBalance() == +1){ insertFix(grand, parent); return; }
        // balance == +2
        if (child == parent->getRight()){                         // zig-zig
            leftRotate(grand);
            parent->setBalance(0);
            grand->setBalance(0);
        }
        else{                                                     // zig-zag
            rightRotate(parent);
            leftRotate(grand);
            if (child->getBalance() == +1){ parent->setBalance(0);  grand->setBalance(-1); }
            else if (child->getBalance() == 0){ parent->setBalance(0); grand->setBalance(0); }
            else { parent->setBalance(+1); grand->setBalance(0); }
            child->setBalance(0);
        }
    }
}

/*
 * After a removal, node's balance must change by diff (+1 if its left
 * subtree shrank, -1 if its right subtree shrank). Walks up, rotating as needed.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* node, int8_t diff)
{
    if (node == nullptr) return;

    // compute the next step before any rotation changes node's parent
    AVLNode<Key, Value>* parent = node->getParent();
    int8_t nextDiff = 0;
    if (parent != nullptr) nextDiff = (node == parent->getLeft()) ? +1 : -1;

    int newBal = node->getBalance() + diff;

    if (diff == -1){ // right side shrank
        if (newBal == -2){
            AVLNode<Key, Value>* c = node->getLeft(); // taller child
            if (c->getBalance() == -1){               // zig-zig
                rightRotate(node);
                node->setBalance(0);
                c->setBalance(0);
                removeFix(parent, nextDiff);
            }
            else if (c->getBalance() == 0){           // zig-zig, height unchanged
                rightRotate(node);
                node->setBalance(-1);
                c->setBalance(+1);
            }
            else{                                     // zig-zag
                AVLNode<Key, Value>* g = c->getRight();
                leftRotate(c);
                rightRotate(node);
                if (g->getBalance() == +1){ node->setBalance(0); c->setBalance(-1); }
                else if (g->getBalance() == 0){ node->setBalance(0); c->setBalance(0); }
                else { node->setBalance(+1); c->setBalance(0); }
                g->setBalance(0);
                removeFix(parent, nextDiff);
            }
        }
        else if (newBal == -1){ node->setBalance(-1); }  // height unchanged, stop
        else { node->setBalance(0); removeFix(parent, nextDiff); } // got shorter
    }
    else{ // diff == +1, left side shrank: mirror image
        if (newBal == +2){
            AVLNode<Key, Value>* c = node->getRight();
            if (c->getBalance() == +1){
                leftRotate(node);
                node->setBalance(0);
                c->setBalance(0);
                removeFix(parent, nextDiff);
            }
            else if (c->getBalance() == 0){
                leftRotate(node);
                node->setBalance(+1);
                c->setBalance(-1);
            }
            else{
                AVLNode<Key, Value>* g = c->getLeft();
                rightRotate(c);
                leftRotate(node);
                if (g->getBalance() == -1){ node->setBalance(0); c->setBalance(+1); }
                else if (g->getBalance() == 0){ node->setBalance(0); c->setBalance(0); }
                else { node->setBalance(-1); c->setBalance(0); }
                g->setBalance(0);
                removeFix(parent, nextDiff);
            }
        }
        else if (newBal == +1){ node->setBalance(+1); }
        else { node->setBalance(0); removeFix(parent, nextDiff); }
    }
}

/*
 * Lifts node's right child into node's position.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::leftRotate(AVLNode<Key,Value>* node)
{
    AVLNode<Key, Value>* r = node->getRight();
    AVLNode<Key, Value>* p = node->getParent();
    AVLNode<Key, Value>* b = r->getLeft();

    r->setParent(p);
    if (p == nullptr) this->root_ = r;
    else if (p->getLeft() == node) p->setLeft(r);
    else p->setRight(r);

    node->setRight(b);
    if (b != nullptr) b->setParent(node);

    r->setLeft(node);
    node->setParent(r);
}

/*
 * Lifts node's left child into node's position.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::rightRotate(AVLNode<Key,Value>* node)
{
    AVLNode<Key, Value>* l = node->getLeft();
    AVLNode<Key, Value>* p = node->getParent();
    AVLNode<Key, Value>* b = l->getRight();

    l->setParent(p);
    if (p == nullptr) this->root_ = l;
    else if (p->getLeft() == node) p->setLeft(l);
    else p->setRight(l);

    node->setLeft(b);
    if (b != nullptr) b->setParent(node);

    l->setRight(node);
    node->setParent(l);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
