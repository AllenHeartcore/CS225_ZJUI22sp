#include <iostream>
#define MAX_DEGREE 3
using namespace std;

class personalinfo {
public:
    personalinfo();
    personalinfo(int64_t);
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
    BPlusNode(bool, int64_t, personalinfo*);
    ~BPlusNode();
    int find_id(int, bool);
};

class BPlusTree {
    bool insert_recursive(BPlusNode*, personalinfo*, BPlusNode*, bool);
    bool remove_recursive(BPlusNode*, int, bool);
    void update_internal_keys(BPlusNode*, bool);
    void print_recursive(BPlusNode*, int);
public:
    BPlusTree();
    BPlusNode *root;
    bool insert(personalinfo*);
    bool remove(int64_t);
    personalinfo* search(int64_t);
    BPlusNode* get_dummy();
    void print(bool);
};
