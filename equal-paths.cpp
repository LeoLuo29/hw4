#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#include <iostream>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int pathLength(Node* root);

/**
 * @brief Returns true if all paths from leaves to root are the same length (height),
 *        and false otherwise
 *
 *        Note: this doesn't mean we are checking if the tree is full, but just that
 *        any leaf node (wherever it may exist) has the same length path to the root 
 *        as all others.
 * 
 * @param root Pointer to the root of the tree to check for equal paths
 */

bool equalPaths(Node * root){
    return pathLength(root) != -1;
}

int pathLength(Node* root){
    if (root == nullptr) return 0;
    if (root->left == nullptr && root->right == nullptr) return 1;

    int left  = pathLength(root->left);
    int right = pathLength(root->right);

    if (root->left == nullptr) {
        if (right == -1) return -1;
        return right + 1;
    }
    if (root->right == nullptr) {
        if (left == -1) return -1;
        return left + 1;
    }

    if (left == -1 || right == -1 || left != right) return -1;
    return left + 1;
}

