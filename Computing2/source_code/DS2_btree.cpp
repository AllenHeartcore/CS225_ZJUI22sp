// Deleting a key from a B-tree in C++

#include "medical_system.h"
#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>
using namespace std;



// B tree node
BTreeNode::BTreeNode(int t1, bool leaf1) {
    t = t1;
    leaf = leaf1;

    keys = new int64_t[2 * t - 1];
    C = new BTreeNode *[2 * t];
    n = 0;
    patient = new personalinfo *[2 * t - 1];
}

// Function to search key k in subtree rooted with this node
BTreeNode *BTreeNode::search(int64_t k)
{
    // Find the first key greater than or equal to k
    int i = 0;
    while (i < n && k > keys[i])
        i++;
 
    // If the found key is equal to k, return this node
    if (i < n && keys[i] == k)
        return this;
    // If key is not found here and this is a leaf node
    if (leaf == true)
        return NULL;
    // Go to the appropriate child
    return C[i]->search(k);
}

void personalinfo::get_secKey(){
    int64_t newage  = age;
    secKey = id + 1000000000*newage;
}

// Find the key in the overflow block, using the linear search
int BTreeNode::find_overflow_key(int64_t k) {
    int idx = 0;
    while (idx < n && keys[idx] != k)
        ++idx;
    return idx;
}

// Find the key in the main block, using the binary search
int BTreeNode::findKey(int64_t k) {
    int idx = 0;
    while (idx < n && keys[idx] < k)
        ++idx;
    return idx;
}

// Deletion operation
void BTreeNode::deletion(int64_t k) {
    int idx = findKey(k);

    if (idx < n && keys[idx] == k) {
        if (leaf)
        removeFromLeaf(idx);
        else
        removeFromNonLeaf(idx);
    } else {
        if (leaf) {
        cout << "The key " << k << " is does not exist in the tree\n";
        return;
        }

        bool flag = ((idx == n) ? true : false);

        if (C[idx]->n < t)
            fill(idx);

        if (flag && idx > n)
            C[idx - 1]->deletion(k);
        else
            C[idx]->deletion(k);
    }
    return;
    // int idx = findKey(k);
    // if (idx != -1){
    //     if (leaf)
    //     removeFromLeaf(idx);
    //     else
    //     removeFromNonLeaf(idx);
    // }
    // else {
    //     if (leaf) {
    //         cout << "The key " << k << " is does not exist in the tree\n";
    //         return;
    //     }
    //     bool flag = ((idx == n) ? true : false);

    // }
}

// Remove from the leaf
void BTreeNode::removeFromLeaf(int idx) {
    for (int i = idx + 1; i < n; ++i)
        keys[i - 1] = keys[i];
    for (int i = idx + 1; i < n; ++i)
        patient[i-1] = patient[i];
    n--;

    return;
}

// Delete from non leaf node
void BTreeNode::removeFromNonLeaf(int idx) {
    int64_t k = keys[idx];

    if (C[idx]->n >= t) {
        int64_t pred = getPredecessor(idx);
        personalinfo* pred_patient = getPredecessor_patient(idx);
        keys[idx] = pred;
        patient[idx] = pred_patient;
        C[idx]->deletion(pred);
    }

    else if (C[idx + 1]->n >= t) {
        int64_t succ = getSuccessor(idx);
        personalinfo* succ_patient = getSuccessor_patient(idx);
        keys[idx] = succ;
        patient[idx] = succ_patient;
        C[idx + 1]->deletion(succ);
    }

    else {
        merge(idx);
        C[idx]->deletion(k);
    }
    return;
}

int64_t BTreeNode::getPredecessor(int idx) {
    BTreeNode *cur = C[idx];
    while (!cur->leaf)
        cur = cur->C[cur->n];

    return cur->keys[cur->n - 1];
}

personalinfo* BTreeNode::getPredecessor_patient(int idx) {
    BTreeNode *cur = C[idx];
    while (!cur->leaf)
        cur = cur->C[cur->n];

    return cur->patient[cur->n - 1];
}

int64_t BTreeNode::getSuccessor(int idx) {
    BTreeNode *cur = C[idx + 1];
    while (!cur->leaf)
        cur = cur->C[0];

    return cur->keys[0];
}

personalinfo* BTreeNode::getSuccessor_patient(int idx) {
    BTreeNode *cur = C[idx + 1];
    while (!cur->leaf)
        cur = cur->C[0];

    return cur->patient[0];
}

void BTreeNode::fill(int idx) {
    if (idx != 0 && C[idx - 1]->n >= t)
        borrowFromPrev(idx);

    else if (idx != n && C[idx + 1]->n >= t)
        borrowFromNext(idx);

    else {
        if (idx != n)
        merge(idx);
        else
        merge(idx - 1);
    }
    return;
}

// Borrow from previous
void BTreeNode::borrowFromPrev(int idx) {
    BTreeNode *child = C[idx];
    BTreeNode *sibling = C[idx - 1];

    for (int i = child->n - 1; i >= 0; --i){
        child->keys[i + 1] = child->keys[i];
        child->patient[i + 1] = child->patient[i];
    }
    if (!child->leaf) {
        for (int i = child->n; i >= 0; --i)
            child->C[i + 1] = child->C[i];
    }

    child->keys[0] = keys[idx - 1];
    child->patient[0] = patient[idx - 1];

    if (!child->leaf)
        child->C[0] = sibling->C[sibling->n];

    keys[idx - 1] = sibling->keys[sibling->n - 1];
    patient[idx - 1] = sibling->patient[sibling->n - 1];

    child->n += 1;
    sibling->n -= 1;

    return;
}

// Borrow from the next
void BTreeNode::borrowFromNext(int idx) {
    BTreeNode *child = C[idx];
    BTreeNode *sibling = C[idx + 1];

    child->keys[(child->n)] = keys[idx];
    child->patient[(child->n)] = patient[idx];

    if (!(child->leaf))
        child->C[(child->n) + 1] = sibling->C[0];

    keys[idx] = sibling->keys[0];
    patient[idx] = sibling->patient[0];


    for (int i = 1; i < sibling->n; ++i){
        sibling->keys[i - 1] = sibling->keys[i];
        sibling->patient[i - 1] = sibling->patient[i];
    }
    if (!sibling->leaf) {
        for (int i = 1; i <= sibling->n; ++i)
        sibling->C[i - 1] = sibling->C[i];
    }

    child->n += 1;
    sibling->n -= 1;

    return;
}

// Merge
void BTreeNode::merge(int idx) {
    BTreeNode *child = C[idx];
    BTreeNode *sibling = C[idx + 1];

    child->keys[t - 1] = keys[idx];
    child->patient[t - 1] = patient[idx];
    for (int i = 0; i < sibling->n; ++i){
        child->keys[i + t] = sibling->keys[i];
        child->patient[i + t] = sibling->patient[i];
    }
    if (!child->leaf) {
        for (int i = 0; i <= sibling->n; ++i)
            child->C[i + t] = sibling->C[i];
    }

    for (int i = idx + 1; i < n; ++i){
        keys[i - 1] = keys[i];
        patient[i - 1] = patient[i];
    }

    for (int i = idx + 2; i <= n; ++i)
        C[i - 1] = C[i];

    child->n += sibling->n + 1;
    n--;

    delete (sibling);
    return;
}

// Insertion operation
void BTree::insertion(personalinfo* p) {
    int64_t k = p->secKey;
    if (root == NULL) {
        root = new BTreeNode(t, true);
        root->keys[0] = k;
        root->patient[0] = p;
        root->n = 1;
    } else {
        if (root->n == 2 * t - 1) {
        BTreeNode *s = new BTreeNode(t, false);

        s->C[0] = root;

        s->splitChild(0, root);

        int i = 0;
        if (s->keys[0] < k)
            i++;
        s->C[i]->insertNonFull(k, p);

        root = s;
        } else
            root->insertNonFull(k, p);
    }
}

// Insertion non full
void BTreeNode::insertNonFull(int64_t k, personalinfo* p) {
    int i = n - 1;

    if (leaf == true) {
        while (i >= 0 && keys[i] > k) {
            keys[i + 1] = keys[i];
            patient[i + 1] = patient[i];
            i--;
        }

        keys[i + 1] = k;
        patient[i + 1] = p;
        n = n + 1;
    } else {
        while (i >= 0 && keys[i] > k)
            i--;

        if (C[i + 1]->n == 2 * t - 1) {
            splitChild(i + 1, C[i + 1]);

            if (keys[i + 1] < k)
                i++;
        }
        C[i + 1]->insertNonFull(k, p);
    }
}

// Split child
void BTreeNode::splitChild(int i, BTreeNode *y) {
    BTreeNode *z = new BTreeNode(y->t, y->leaf);
    z->n = t - 1;

    for (int j = 0; j < t - 1; j++){
        z->keys[j] = y->keys[j + t];
        z->patient[j] = y->patient[j + t];
    }
    if (y->leaf == false) {
        for (int j = 0; j < t; j++)
            z->C[j] = y->C[j + t];
    }

    y->n = t - 1;

    for (int j = n; j >= i + 1; j--)
        C[j + 1] = C[j];

    C[i + 1] = z;

    for (int j = n - 1; j >= i; j--){
        keys[j + 1] = keys[j];
        patient[j + 1] = patient[j];
    }

    keys[i] = y->keys[t - 1];    
    patient[i] = y->patient[t - 1];
    n = n + 1;
}

// Traverse
void BTreeNode::traverse() {
    int i;
    for (i = 0; i < n; i++) {
        if (leaf == false)
        C[i]->traverse();
        cout << " " << keys[i];
    }

    if (leaf == false)
        C[i]->traverse();
}

// Delete Operation
void BTree::deletion(int64_t k) {
    if (!root) {
        cout << "The tree is empty\n";
        return;
    }

    root->deletion(k);

    if (root->n == 0) {
        BTreeNode *tmp = root;
        if (root->leaf)
        root = NULL;
        else
        root = root->C[0];

        delete tmp;
    }
    return;
}

vector<personalinfo*> BTree::query(int age){
    int64_t newage = age;
    int64_t min_key = newage*1000000000;
    int64_t max_key = newage*1000000000 + 999999999;
    vector<BTreeNode*> queue1;
    vector<BTreeNode*> queue2;
    vector<personalinfo*> result;
    BTreeNode* node = root;
    queue1.push_back(node);
    while (queue1[0]->leaf == false){
        for (int i = 0; i < queue1.size(); i++){
            node = queue1[i];
            for (int j = 0; j < node->n; j++){
                if (node->keys[j] >= min_key && node->keys[j] <= max_key){
                    result.push_back(node->patient[j]);
                }
                queue2.push_back(node->C[j]);
            }
            queue2.push_back(node->C[node->n]);
        }
        queue1.clear();
        queue1 = queue2;
        queue2.clear();
    }
    for (int i = 0; i < queue1.size(); i++){
        node = queue1[i];
        for (int j = 0; j < node->n; j++){
            if (node->keys[j] >= min_key && node->keys[j] <= max_key){
                result.push_back(node->patient[j]);
            }
        }
    }
    return result;
}