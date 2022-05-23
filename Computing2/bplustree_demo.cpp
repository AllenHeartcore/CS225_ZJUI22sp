#include <iostream>
#include <stdlib.h>
#define MAX_DEGREE 3
using namespace std;

class personalinfo {
public:
    personalinfo() { this->id = 0; }
    personalinfo(int64_t id) { this->id = id; }
    int64_t id;
};

class BPlusNode {
public:
    bool is_leaf;
    int size;
    personalinfo **patients;
    BPlusNode **children;
    BPlusNode *parent;
    BPlusNode *next;
    BPlusNode *prev;
    BPlusNode(bool is_leaf, int64_t size, personalinfo* patient) {
        this->is_leaf = is_leaf;
        this->size = size;
        this->patients = new personalinfo*[MAX_DEGREE + 1];
        for (int i = 0; i < MAX_DEGREE + 1; i++) this->patients[i] = NULL;
        if (patient != NULL) this->patients[0] = patient;
        this->children = new BPlusNode*[MAX_DEGREE + 1];
        for (int i = 0; i < MAX_DEGREE + 1; i++) this->children[i] = NULL;
        this->parent = this;
        this->next = this;
        this->prev = this;
    }
    int find_id(int id, bool travel_downward) {
        int pos = 0;
        while (pos < size && patients[pos]->id <= id) pos++;
        if (travel_downward && pos != 0) pos--;
        return pos;
    }
};

class BPlusTree {
    bool insert_recursive(BPlusNode* target, personalinfo* patient, BPlusNode* child, bool is_leaf) {
        int target_pos = target->find_id(patient->id, false), i, j;
        for (i = target->size; i > target_pos; i--)
            target->patients[i] = target->patients[i - 1];
        target->patients[target_pos] = patient;
        if (target != root && target_pos == 0) update_internal_keys(target, false);
        if (!is_leaf) {
            for (i = target->size; i > target_pos; i--)
                target->children[i] = target->children[i - 1];
            target->children[target_pos] = child;
            child->parent = target;
        }
        target->size++;
        if (target->size <= MAX_DEGREE) return true;
        target->size = (MAX_DEGREE + 1) / 2;
        BPlusNode* new_node = new BPlusNode(is_leaf, (MAX_DEGREE + 1) - target->size, NULL);
        for (i = target->size, j = 0; j < new_node->size; i++, j++)
            new_node->patients[j] = target->patients[i];
        if (!is_leaf) for (i = target->size, j = 0; j < new_node->size; i++, j++) {
            new_node->children[j] = target->children[i];
            target->children[i]->parent = new_node;
        } else {
            new_node->next = target->next;
            new_node->prev = target;
            target->next->prev = new_node;
            target->next = new_node;
        }
        if (target == root) {
            BPlusNode* new_root = new BPlusNode(false, 2, target->patients[0]);
            new_root->patients[1] = new_node->patients[0];
            new_root->children[0] = target;
            new_root->children[1] = new_node;
            target->parent = new_root;
            new_node->parent = new_root;
            root = new_root;
            return true;
        } else return insert_recursive(target->parent, new_node->patients[0], new_node, false);
    }
    bool remove_recursive(BPlusNode* target, int child_pos, bool is_leaf) {
        int target_pos = target->parent->find_id(target->patients[0]->id, true), i, j;
        for (i = child_pos; i < target->size - 1; i++)
            target->patients[i] = target->patients[i + 1];
        target->patients[target->size - 1] = NULL;
        if (!is_leaf) {
            target->children[child_pos]->prev->next = target->children[child_pos]->next;
            target->children[child_pos]->next->prev = target->children[child_pos]->prev;
            for (i = child_pos; i < target->size - 1; i++)
                target->children[i] = target->children[i + 1];
            target->children[target->size - 1] = NULL;
        }
        if (target != root && child_pos == 0) update_internal_keys(target, false);
        target->size--;
        if (target->size >= (MAX_DEGREE + 1) / 2) return true;
        if (target == root) {
            if (target->size == 0) root = NULL;
            if (target->size == 1 && !root->is_leaf) {
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
        && left_neighbor->size >= (MAX_DEGREE + 1) / 2 + 1) {
            for (i = target->size; i > 0; i--)
                target->patients[i] = target->patients[i - 1];
            target->patients[0] = left_neighbor->patients[left_neighbor->size - 1];
            left_neighbor->patients[left_neighbor->size - 1] = NULL;
            update_internal_keys(target, true);
            if (!is_leaf) {
                for (i = target->size; i > 0; i--)
                    target->children[i] = target->children[i - 1];
                target->children[0] = left_neighbor->children[left_neighbor->size - 1];
                left_neighbor->children[left_neighbor->size - 1] = NULL;
                target->children[0]->parent = target;
            }
            target->size++;
            left_neighbor->size--;
            return true;
        }
        if (right_neighbor_pos < target->parent->size \
        && right_neighbor->size >= (MAX_DEGREE + 1) / 2 + 1) {
            target->patients[target->size] = right_neighbor->patients[0];
            for (i = 0; i < right_neighbor->size - 1; i++)
                right_neighbor->patients[i] = right_neighbor->patients[i + 1];
            right_neighbor->patients[right_neighbor->size - 1] = NULL;
            update_internal_keys(right_neighbor, false);
            if (!is_leaf) {
                target->children[target->size] = right_neighbor->children[0];
                target->children[target->size]->parent = target;
                for (i = 0; i < right_neighbor->size - 1; i++)
                    right_neighbor->children[i] = right_neighbor->children[i + 1];
                right_neighbor->children[right_neighbor->size - 1] = NULL;
            }
            target->size++;
            right_neighbor->size--;
            return true;
        }
        if (left_neighbor_pos >= 0) {
            for (i = 0, j = left_neighbor->size; i < target->size; i++, j++) {
                left_neighbor->patients[j] = target->patients[i];
                target->patients[i] = NULL;
            }
            if (!is_leaf) for (i = 0, j = left_neighbor->size; i < target->size; i++, j++) {
                left_neighbor->children[j] = target->children[i];
                left_neighbor->children[j]->parent = left_neighbor;
                target->children[i] = NULL;
            }
            left_neighbor->size += target->size;
            target->size = 0;
            return remove_recursive(target->parent, target_pos, false);
        }
        if (right_neighbor_pos < target->parent->size) {
            for (i = 0, j = target->size; i < right_neighbor->size; i++, j++) {
                target->patients[j] = right_neighbor->patients[i];
                right_neighbor->patients[i] = NULL;
            }
            if (!is_leaf) for (i = 0, j = target->size; i < right_neighbor->size; i++, j++) {
                target->children[j] = right_neighbor->children[i];
                target->children[j]->parent = target;
                right_neighbor->children[i] = NULL;
            }
            target->size += right_neighbor->size;
            right_neighbor->size = 0;
            return remove_recursive(target->parent, right_neighbor_pos, false);
        }
        return false;
    }
    void update_internal_keys(BPlusNode *node, bool decreasing_key) {
        BPlusNode *parent_temp = node->parent;
        int target_pos;
        do {
            target_pos = parent_temp->find_id(node->patients[0]->id, true);
            if (decreasing_key) target_pos++;
            parent_temp->patients[target_pos] = parent_temp->children[target_pos]->patients[0];
            if (parent_temp == root) break;
            parent_temp = parent_temp->parent;
        } while (target_pos == 0);
    }
    void print_recursive(BPlusNode* subroot, int depth) {
        for (int i = 0; i < subroot->size; i++) {
            for (int j = 0; j < depth; j++) cout << "\t";
            cout << "[" << depth << "] " << subroot->patients[i]->id << endl;
            if (!subroot->is_leaf) print_recursive(subroot->children[i], depth + 1);
        }
    }
public:
    BPlusNode *root;
    BPlusTree() { root = NULL; }
    bool insert(personalinfo* patient) {
        if (search(patient->id) != NULL) return false;
        if (root == NULL) {
            root = new BPlusNode(true, 1, patient);
            return true;
        }
        BPlusNode *ptr = root;
        while (!ptr->is_leaf) ptr = ptr->children[ptr->find_id(patient->id, true)];
        return insert_recursive(ptr, patient, NULL, true);
    }
    bool remove(int64_t target_id) {
        if (root == NULL) return false;
        BPlusNode *ptr = root;
        while (!ptr->is_leaf) ptr = ptr->children[ptr->find_id(target_id, true)];
        bool found = false;
        int child_pos;
        for (child_pos = 0; child_pos < ptr->size; child_pos++)
            if (ptr->patients[child_pos]->id == target_id) {
                found = true;
                break;
            }
        if (!found) return false;
        return remove_recursive(ptr, child_pos, true);
    }
    personalinfo* search(int64_t target_id) {
        if (root == NULL) return NULL;
        BPlusNode* ptr = root;
        while (!ptr->is_leaf) ptr = ptr->children[ptr->find_id(target_id, true)];
        for (int i = 0; i < ptr->size; i++)
            if (ptr->patients[i]->id == target_id) return ptr->patients[i];
        return NULL;
    }
    BPlusNode* get_dummy() {
        BPlusNode* dummy = root;
        while (!dummy->is_leaf) dummy = dummy->children[0];
        return dummy;
    }
    void print(bool print_nonleaf) {
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
};

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
