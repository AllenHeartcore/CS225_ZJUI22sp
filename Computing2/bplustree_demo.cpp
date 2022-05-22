#include <iostream>
#include <stdlib.h>
#include "bplustree.h"
using namespace std;

personalinfo::personalinfo() {
    this->id = 0;
}

personalinfo::personalinfo(int64_t id) {
    this->id = id;
}

BPlusNode::BPlusNode(bool is_leaf, int64_t size, personalinfo* patient) {                       // node init
    this->is_leaf = is_leaf;
    this->size = size;
    this->patients = new personalinfo*[MAX_DEGREE + 1];
    for (int i = 0; i < MAX_DEGREE + 1; i++) this->patients[i] = NULL;
    if (patient != NULL) this->patients[0] = patient;                                           // embed patient info
    this->children = new BPlusNode*[MAX_DEGREE + 1];
    for (int i = 0; i < MAX_DEGREE + 1; i++) this->children[i] = NULL;
    this->parent = this;
    this->next = this;
    this->prev = this;
}

int BPlusNode::find_id(int id, bool travel_downward) {                                          // find id in node
    int pos = 0;
    while (pos < size && patients[pos]->id <= id) pos++;
    if (travel_downward && pos != 0) pos--;                                                     // correction for downward travel
    return pos;
}

BPlusTree::BPlusTree() {
    root = NULL;
}

bool BPlusTree::insert(personalinfo* patient) {
    if (search(patient->id) != NULL) return false;                                              // ? duplicated ID ?
    if (root == NULL) {                                                                         // ? empty tree ?
        root = new BPlusNode(true, 1, patient);
        return true;
    }
    BPlusNode *ptr = root;
    while (!ptr->is_leaf) ptr = ptr->children[ptr->find_id(patient->id, true)];                 // travel to target leaf
    return insert_recursive(ptr, patient, NULL, true);
}

bool BPlusTree::insert_recursive(BPlusNode* target, personalinfo* patient, BPlusNode* child, bool is_leaf) {
    int target_pos = target->find_id(patient->id, false), i, j;                                 // find target position
    for (i = target->size; i > target_pos; i--)
        target->patients[i] = target->patients[i - 1];
    target->patients[target_pos] = patient;                                                     // insert patient
    if (target != root && target_pos == 0) update_internal_keys(target, false);
    if (!is_leaf) {
        for (i = target->size; i > target_pos; i--)
            target->children[i] = target->children[i - 1];
        target->children[target_pos] = child;                                                   // insert child
        child->parent = target;
    }
    target->size++;
    if (target->size <= MAX_DEGREE) return true;                                                // else: overflow, split 
    target->size = (MAX_DEGREE + 1) / 2;                                                        // (target: left, new_node: right)
    BPlusNode* new_node = new BPlusNode(is_leaf, (MAX_DEGREE + 1) - target->size, NULL);        //     modify sizes
    for (i = target->size, j = 0; j < new_node->size; i++, j++)
        new_node->patients[j] = target->patients[i];                                            //     move patients
    if (!is_leaf) for (i = target->size, j = 0; j < new_node->size; i++, j++) {
        new_node->children[j] = target->children[i];                                            //     move children
        target->children[i]->parent = new_node;
    } else {
        new_node->next = target->next;                                                          //     modify linked list
        new_node->prev = target;
        target->next->prev = new_node;
        target->next = new_node;
    }
    if (target == root) {
        BPlusNode* new_root = new BPlusNode(false, 2, target->patients[0]);                     //     create new common root, 
        new_root->patients[1] = new_node->patients[0];
        new_root->children[0] = target;
        new_root->children[1] = new_node;
        target->parent = new_root;
        new_node->parent = new_root;
        root = new_root;
        return true;
    } else return insert_recursive(target->parent, new_node->patients[0], new_node, false);     //     or modify parent
}

bool BPlusTree::remove(int64_t target_id) {
    if (root == NULL) return false;                                                             // ? empty tree ?
    BPlusNode *ptr = root;
    while (!ptr->is_leaf) ptr = ptr->children[ptr->find_id(target_id, true)];                   // travel to target leaf
    bool found = false;
    int child_pos;
    for (child_pos = 0; child_pos < ptr->size; child_pos++)                                     // find child position
        if (ptr->patients[child_pos]->id == target_id) {
            found = true;
            break;
        }
    if (!found) return false;                                                                   // ? ID not found ?
    return remove_recursive(ptr, child_pos, true);
}

bool BPlusTree::remove_recursive(BPlusNode* target, int child_pos, bool is_leaf) {
    int target_pos = target->parent->find_id(target->patients[0]->id, true), i, j;
    for (i = child_pos; i < target->size - 1; i++)
        target->patients[i] = target->patients[i + 1];                                          // delete patient
    target->patients[target->size - 1] = NULL;
    if (!is_leaf) {
        target->children[child_pos]->prev->next = target->children[child_pos]->next;            // modify linked list
        target->children[child_pos]->next->prev = target->children[child_pos]->prev;
        for (i = child_pos; i < target->size - 1; i++)
            target->children[i] = target->children[i + 1];                                      // delete child
        target->children[target->size - 1] = NULL;
    }
    if (target != root && child_pos == 0) update_internal_keys(target, false);
    target->size--;
    if (target->size >= (MAX_DEGREE + 1) / 2) return true;                                      // else: underflow
    if (target == root) {
        if (target->size == 0) root = NULL;                                                     // ? empty root ?
        if (target->size == 1 && !root->is_leaf) {                                              // ? root with a single key ?
            target->patients[0] = NULL;              
            root = target->children[0];
            root->parent = root;
            target->children[0] = NULL;
        }
        return true;
    }
    int left_neighbor_pos = target_pos - 1, right_neighbor_pos = target_pos + 1;
    BPlusNode *left_neighbor  = target->parent->children[left_neighbor_pos], \
              *right_neighbor = target->parent->children[right_neighbor_pos];
    if (left_neighbor_pos >= 0 \
    && left_neighbor->size >= (MAX_DEGREE + 1) / 2 + 1) {                                       // lend from left sibling
        for (i = target->size; i > 0; i--)
            target->patients[i] = target->patients[i - 1];                                      //         make space in T
        target->patients[0] = left_neighbor->patients[left_neighbor->size - 1];                 //     move patient from L to T
        left_neighbor->patients[left_neighbor->size - 1] = NULL;                                //         detach patient from L
        update_internal_keys(target, true);                                                     //         update parent
        if (!is_leaf) {
            for (i = target->size; i > 0; i--)
                target->children[i] = target->children[i - 1];                                  //         make space in T
            target->children[0] = left_neighbor->children[left_neighbor->size - 1];             //     move child from L to T
            left_neighbor->children[left_neighbor->size - 1] = NULL;                            //         detach child from L
            target->children[0]->parent = target;                                               //         update child's parent
        }
        target->size++;                                                                         //     update sizes
        left_neighbor->size--;
        return true;
    }
    if (right_neighbor_pos < target->parent->size \
    && right_neighbor->size >= (MAX_DEGREE + 1) / 2 + 1) {                                      // lend from right sibling
        target->patients[target->size] = right_neighbor->patients[0];                           //     move patient from R to T
        for (i = 0; i < right_neighbor->size - 1; i++)
            right_neighbor->patients[i] = right_neighbor->patients[i + 1];                      //         fill space in R
        right_neighbor->patients[right_neighbor->size - 1] = NULL;                              //         detach patient from R
        update_internal_keys(right_neighbor, false);                                            //         update parent
        if (!is_leaf) {
            target->children[target->size] = right_neighbor->children[0];                       //     move child from R to T
            target->children[target->size]->parent = target;                                    //         update child's parent
            for (i = 0; i < right_neighbor->size - 1; i++)
                right_neighbor->children[i] = right_neighbor->children[i + 1];                  //         fill space in R
            right_neighbor->children[right_neighbor->size - 1] = NULL;                          //         detach child from R
        }
        target->size++;                                                                         //     update sizes
        right_neighbor->size--;
        return true;
    }
    if (left_neighbor_pos >= 0) {                                                               // merge with left sibling
        for (i = 0, j = left_neighbor->size; i < target->size; i++, j++) {
            left_neighbor->patients[j] = target->patients[i];                                   //     move patients from T to L
            target->patients[i] = NULL;                                                         //         detach patients from T
        }
        if (!is_leaf) for (i = 0, j = left_neighbor->size; i < target->size; i++, j++) {
            left_neighbor->children[j] = target->children[i];                                   //     move children from T to L
            left_neighbor->children[j]->parent = left_neighbor;                                 //         update children's parent
            target->children[i] = NULL;                                                         //         detach children from T
        }
        left_neighbor->size += target->size;                                                    //     update sizes
        target->size = 0;
        return remove_recursive(target->parent, target_pos, false);
    }
    if (right_neighbor_pos < target->parent->size) {                                            // merge with right sibling
        for (i = 0, j = target->size; i < right_neighbor->size; i++, j++) {
            target->patients[j] = right_neighbor->patients[i];                                  //     move patients from R to T
            right_neighbor->patients[i] = NULL;                                                 //         detach patients from R
        }
        if (!is_leaf) for (i = 0, j = target->size; i < right_neighbor->size; i++, j++) {
            target->children[j] = right_neighbor->children[i];                                  //     move children from R to T
            target->children[j]->parent = target;                                               //         update children's parent
            right_neighbor->children[i] = NULL;                                                 //         detach children from R
        }
        target->size += right_neighbor->size;                                                   //     update sizes
        right_neighbor->size = 0;
        return remove_recursive(target->parent, right_neighbor_pos, false);
    }
    return false;
}

void BPlusTree::update_internal_keys(BPlusNode *node, bool decreasing_key) {                    // recursively update keys in parents
    BPlusNode *parent_temp = node->parent;
    int target_pos;
    do {
        target_pos = parent_temp->find_id(node->patients[0]->id, true);                         //     find target position
        if (decreasing_key) target_pos++;
        parent_temp->patients[target_pos] = parent_temp->children[target_pos]->patients[0];     //     update key
        if (parent_temp == root) break;
        parent_temp = parent_temp->parent;                                                      //     travel upwards
    } while (target_pos == 0);
}

personalinfo* BPlusTree::search(int64_t target_id) {
    if (root == NULL) return NULL;
    BPlusNode* ptr = root;
    while (!ptr->is_leaf) ptr = ptr->children[ptr->find_id(target_id, true)];
    for (int i = 0; i < ptr->size; i++)
        if (ptr->patients[i]->id == target_id) return ptr->patients[i];
    return NULL;
}

BPlusNode* BPlusTree::get_dummy() {
    BPlusNode* dummy = root;
    while (!dummy->is_leaf) dummy = dummy->children[0];
    return dummy;
}

void BPlusTree::print(bool print_nonleaf) {
    if (root == NULL) {
        cout << "BPlusTree is empty.\n" << endl;
        return;
    }
    if (print_nonleaf) print_recursive(root, 0);
    else {
        BPlusNode *dummy = get_dummy(), *ptr = dummy;
        do {
            for (int i = 0; i < ptr->size; i++) cout << ptr->patients[i]->id << " ";
            cout << endl;
            ptr = ptr->next;
        } while (ptr != dummy);
    }
    cout << endl;
}

void BPlusTree::print_recursive(BPlusNode* subroot, int depth) {
    for (int i = 0; i < subroot->size; i++) {
        for (int j = 0; j < depth; j++) cout << "\t";
        cout << "[" << depth << "] " << subroot->patients[i]->id << endl;
        if (!subroot->is_leaf) print_recursive(subroot->children[i], depth + 1);
    }
}

int main() {
    system("clear");
    BPlusTree* IDsheet = new BPlusTree();
    int64_t input;
    bool success;
    while (true) {
        success = false;
        cout << "Input (positive - insert, negative - remove, zero - quit): ";
        cin >> input;
        if (input > 0) {
            success = IDsheet->insert(new personalinfo(input));
            if (!success) cout << "Insertion failed.\n" << endl;
        } else if (input < 0) {
            success = IDsheet->remove(-input);
            if (!success) cout << "Removal failed.\n" << endl;
        } else break;
        if (success) {
            system("clear");
            if (IDsheet->root == NULL) cout << "BPlusTree died.\n" << endl;
            else {
                IDsheet->print(true);
            }
        }
    }
    return 0;
}
