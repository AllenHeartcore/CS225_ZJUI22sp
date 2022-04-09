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
    uint64_t priority;                       // in construct function 
    int deadline[3];
    string name;
    contact_info* contact;
    string birthday;
    void local_store();
    int local_register(tm_t* current_time);
    int localqueue_id;
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

extern vector<personalinfo*> dataloading(tm_t* current_time, vector<personalinfo*> blacklist, int local_queue_id);
extern void skip_half_day(int op);
extern void checktime();
extern vector<personalinfo*> blacklist;
extern vector<personalinfo*> localqueue1;
extern vector<personalinfo*> localqueue2;
extern vector<personalinfo*> localqueue3;
extern vector<personalinfo*> localqueue4;
extern vector<personalinfo*> IDsheet;
extern centerqueue Fheap;
extern tm_t* timeflow;
extern int passedhdays;
extern int num_localqueues;
extern int num_appointments;
extern int num_week_report;
extern int num_mon_report;
extern hospital* hos[3];
extern report week_report;
extern report mon_report;

#endif
