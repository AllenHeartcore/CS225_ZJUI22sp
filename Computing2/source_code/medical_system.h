#include <ctime>
#include <time.h>
#include <vector>
#include <string>
#include <iostream>
#include <stdint.h>
using namespace std;
typedef struct tm tm_t;
#ifndef MEDICAL_SYSTEM_H
#define MEDICAL_SYSTEM_H
#define INF_NEG -2147483648
#define MAX_DEGREE 3
//  the contact information of one patient.





class contact_info{
public:
    string addr;
    string phone;
    string wechat;
    string email;
};

//  the registration time and location_id of one patient

class time_location{
public:
    time_location();
    tm_t* registration_time;
    int location_id[3];
    int hospital;               // -1 at the beginning
};


//  the personal information of one patient, including id number, profession, age, age group, medical risk, contact, name, time and location

class personalinfo{
public:
    personalinfo();
    int64_t id;
    int profession;                         // from 1 to 8
    int appointment;                        // 0 at the beginning, becomes 1 when out of F-heap, need initialization!!!!
    int waited_time;                        // in half days, need initialization!!!!
    int age;      
    int age_group;                          // from 1 to 7
    int medical_risk;                       // from 0 to 3
    time_location* registration;            // eg. 19:30 31sec, 20th Sep 2022 -> [31, 30, 19, 20, 9, 2022] 
    uint64_t priority;                       // calculated based on treatment 
    int treatment;                          // different treatment from 0 to 2
    //profession * e11 + age_group * e10 + month * e8 + day * e6 + hour * e4 + min * e2 + sec for treatment 0
    //age_group *  e11 + profession* e10 + month * e8 + day * e6 + hour * e4 + min * e2 + sec for treatment 1
    //profession* e10 + month * e8 + day * e6 + hour * e4 + min * e2 + sec for treatment 2
    int64_t secKey;                  // in construct function
    int deadline[3];
    string name;
    contact_info* contact;
    string birthday;
    void local_store();
    int local_register(tm_t* current_time);
    int localqueue_id;
    void get_secKey();
};




class centernode {
public:
	int id;
	uint64_t priority;
	int degree;
	bool mark;
	personalinfo* person;
	centernode* parent;
	centernode* child;
	centernode* left;
	centernode* right;
	centernode(personalinfo* patient);
};

class centerqueue {
private:
	void link(centernode* new_child, centernode* new_parent);
	void cut(centernode* target, centernode* target_parent);
	void cascading_cut(centernode* target_parent);
	centernode* search_subheap(centernode* root, int id);
public:
	int size;
	centernode* min;
	centerqueue();
	int insert_node(centernode* node);
	int delete_node(int id);
	int modify_key(int id, uint64_t k);
	int consolidate();
	centernode* extract_min();
	centernode* search_heap(int id);
};

// report class, containing treated patients, registered patrients and waiting patrients.

class report{
    public:
    report();
    int size;                                    //number of registered patients, add to registration step
    //when withdraw, size--
    int treatment;                               // number of have been treated patients
    float sum_waited_time;                         // the day when person has appoinment will still be considered to be waiting
    int with_drew_people;                        // need add when withdraw!
    //avg wating time is sum of waited_time/size

    vector<centernode*> treated;                 // treated patients, will be stored every time when a patient comes out of hospital
    vector<centernode*> appointment;             // patients in hospital
    vector<centernode*> not_treated;             // in F-heap 
    // how many are waitng is appointment plus not_treated
    void store_data();
    void generate_report(char* filename);
    void clear_report();                         // each week clear the report once 
                                                 // or each month clear the report once
    void sort_nodes_profession();
    void sort_nodes_name();
    void sort_nodes_age_group();

};



class hospital{
public:
    public:
    hospital(int id);
    //each hospital holds 5 personalinfo
    //5 in and 5 out everyday
    vector<centernode*> inhospital;
    int location_id;  //from 1 to 3
    int come_hospital(centernode* node);
    void clear_hospital();
};

class BTreeNode {
public:
    int64_t *keys;
    int t;
    BTreeNode **C;
    int n;
    bool leaf;
    personalinfo** patient;
    

// public:
    BTreeNode(int _t, bool _leaf);

    void traverse();
    // A function to search a key in the subtree rooted with this node.
    BTreeNode *search(int64_t k);   // returns NULL if k is not present.
    int findKey(int64_t k);
    int find_overflow_key(int64_t k);
    void insertNonFull(int64_t k, personalinfo* p);
    void splitChild(int i, BTreeNode *y);
    void deletion(int64_t k);
    void removeFromLeaf(int idx);
    void removeFromNonLeaf(int idx);
    int64_t getPredecessor(int idx);
    int64_t getSuccessor(int idx);
    void fill(int idx);
    void borrowFromPrev(int idx);
    void borrowFromNext(int idx);
    void merge(int idx);
    personalinfo* getPredecessor_patient(int idx);
    personalinfo* getSuccessor_patient(int idx);
    friend class BTree;
};



class BTree {
public:
    BTreeNode *root;
    int t;

// public:
    BTree(int _t) {
        root = NULL;
        t = _t;
    }

    void traverse() {
        if (root != NULL)
        root->traverse();
    }

    void insertion(personalinfo* p);

    void deletion(int64_t k);
    vector<personalinfo*> query(int age);
    BTreeNode* search(int64_t k)
    {  return (root == NULL)? NULL : root->search(k); }
};



class Block{
public:
    Block()
    {
        block_data = vector<personalinfo*>(0);
        overflow = vector<personalinfo*>(0);
        max_size = 10;
        overflow_size = 0;

    }

    vector<personalinfo*> block_data; // A vector to store the tuples of people, its maxsize is determined by "fix_size".
    int max_size;  
    int overflow_size;     
    vector<personalinfo*> overflow;
    // Block* previous;
    // Block* next;

    bool contain(int64_t id);
    personalinfo* find(int64_t id);
    // merge the main block with the overflow block
    // notice that the data in the blocks are all continous
    void insert_person(personalinfo* person);
    void delete_person(int64_t id);
    void clearoverflow();
    void print_block();
};

class Block_sequence{
public:
    Block_sequence()
    {
        Blocks = vector<Block*>(0);
    }

    vector<Block*> Blocks;

    // Delete a tuple with a certain "id" from this block.
    // 0 means no tuple, 2 means overblocks, 1 means main data
    void delete_record(int64_t id);
    void insert_record(personalinfo* person);

    // merge the main block with the overflow block
    // notice that the data in the blocks are all continous
    void sort_Person(void);
    void sort_Status(void);
    void sort_Treatment(void);
    void merge(int pos);
    void split();
    void print_all();
    personalinfo* retrieval(int64_t id);
};


class BPlusNode {
public:
    bool is_leaf;
    int size;
    personalinfo **patients;
    personalinfo **overflow_block;
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
    void query(int64_t, int64_t);
    BPlusNode* get_dummy();
    void print(bool);
};




extern vector<personalinfo*> dataloading(tm_t* current_time, vector<personalinfo*> blacklist, int local_queue_id);
extern void skip_half_day(int op);
extern void checktime();
extern vector<personalinfo*> blacklist;
extern vector<personalinfo*> localqueue1;
extern vector<personalinfo*> localqueue2;
extern vector<personalinfo*> localqueue3;
extern vector<personalinfo*> localqueue4;
extern vector<personalinfo*> IDsheet;
extern centerqueue* Fheap[3];
extern tm_t* timeflow;
extern int passedhdays;
extern int num_localqueues;
extern int num_appointments;
extern int num_week_report;
extern int num_mon_report;
extern hospital* hos[3];
extern report week_report;
extern report mon_report;
extern BTree btree;
extern Block_sequence order_block;
extern BPlusTree bplus_tree;
extern bool Person_compare(const personalinfo* a,const personalinfo* b);
extern bool Status_compare(const personalinfo* a,const personalinfo* b);
extern bool Treatment_compare(const personalinfo* a,const personalinfo* b);

#endif
