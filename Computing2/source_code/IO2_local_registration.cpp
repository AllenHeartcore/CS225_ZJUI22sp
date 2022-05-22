#include <iostream>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include "medical_system.h"
#include <fstream>
#include <sstream>
#include <vector>


using namespace std;

typedef struct tm tm_t;

// some constructors defined here

time_location::time_location(){
    registration_time = new tm_t;
}

personalinfo:: personalinfo(){
    registration = new time_location();
    contact = new contact_info();
}

hospital::hospital(int id){
    location_id = id;
}


report::report(){
    sum_waited_time = 0;
    size = 0;
    treatment = 0;
    with_drew_people = 0;
}

// the interactive interface of medical system 

// input:   current time: the time when the person is registered
// output:  int: whether the person is registered successfully

int personalinfo::local_register(tm_t* current_time){
    int deadline_state;
    int current_year = 2022;
    int iteration = 0;
    time_t timep = time(0);
    cout << "Please enter your name (Only one word! No space!)\n"; 
    cin  >> name;
    cout << "Please enter your id number (Only one number (9 bits)! No space!)\n";
    cin  >> id;
    int iteration_num = 0;
    while ((id < 100000000 || id > 999999999) && iteration_num < 3){
        if (iteration_num < 3){
            cout << "Invalid id number!\n";
            cout << "Please enter your id number (Only one number (9 bits)! No space!)\n";
            cin  >> id;
            iteration_num++;
        }
        else 
            return 0;
    }
    for (int i = 0; i < IDsheet.size(); i++){
        if (IDsheet[i]->id == id){
            cout << "This id number has been registered!\n";
            return 0;
        }
    }
    cout << "Please enter your birth date (Year/Month/Day, follow this format strictly!)\n";
    cin >> birthday;
    cout << "Please enter your profession (1 ~ 8)\n";
    cin  >> profession;
    if (profession > 8 || profession <= 0){
        cout << "Wrong input! Profession should be 1 ~ 8." << endl;
        return 0;   
    }
    cout << "Please enter your address (Only one word! No space!)\n";
    cin  >> contact->addr;
    cout << "Please enter your hospital preference (Three numbers, from closest to farthest, 1 ~ 3)\n";
    cin  >> registration->location_id[0];
    cin  >> registration->location_id[1];
    cin  >> registration->location_id[2];
    for (int i = 0; i < 3; i++){
        if (registration->location_id[i] > 3 || registration->location_id[i] < 0){
            cout << "Wrong input! Hospital id should be 1 ~ 3." << endl;
            return 0;
        }
    }
    cout << "Please enter your phone number\n";
    cin  >> contact->phone;
    while (contact->phone.length() != 11){
        cout << "Wrong phone length! Your phone number length should be 11" << endl;
        cout << "Please enter your phone number\n";
        cin  >> contact->phone;
    }
    cout << "Please enter your wechat id\n";
    cin  >> contact->wechat;
    cout << "Please enter your email\n";
    cin  >> contact->email;
    cout << "Please enter your medical risk (0 ~ 3)\n";
    cin  >> medical_risk;
    while (medical_risk < 0 || medical_risk > 3){
        cout << "Wrong medical risk! Your risk should be 0 ~ 3" << endl;
        cout << "Please enter your medical risk (0 ~ 3)\n";
        cin  >> medical_risk;
    }
    cout << "Do you have deadline? (0:no, 1:yes)\n";
    cin  >> deadline_state;
    while (deadline_state != 0 && deadline_state != 1){
        cout << "Wrong input!" << endl;
        cout << "Do you have deadline? (0:no, 1:yes)\n";
        cin  >> deadline_state;
    }
    if (deadline_state == 1){
        cout << "Please enter your deadline date (Day Month Year)\n";
        cin  >> deadline[0];
        cin  >> deadline[1];
        cin  >> deadline[2];
    }
    else{
        deadline[0] = -1;
        deadline[0] = -1;
        deadline[0] = -1;
    }
    int treatment; // from 0 to 2
    cout << "Please enter your treatment (0 ~ 2)\n";
    cin  >> treatment;
    treatment = treatment % 3; // from 0 to 2
    stringstream bs(birthday);
    string birth_str;
    vector<int> birth_array;
    while(getline(bs, birth_str, '/')){
        int birthint = stoi(birth_str);
        birth_array.push_back(birthint);
    }
    age = current_year - birth_array[0];
    if (age <= 12)
        age_group = 1;
    else if (age <= 18)
        age_group = 2;
    else if (age <= 35)
        age_group = 3;
    else if (age <= 50)
        age_group = 4;
    else if (age <= 65)
        age_group = 5;
    else if ( age <= 75)
        age_group = 6;
    else
        age_group = 7;
    registration->registration_time->tm_year = current_time->tm_year;
    registration->registration_time->tm_mon = current_time->tm_mon;
    registration->registration_time->tm_mday = current_time->tm_mday;
    registration->registration_time->tm_hour = current_time->tm_hour;
    registration->registration_time->tm_min = current_time->tm_min;
    registration->registration_time->tm_sec = current_time->tm_sec;
    return 1;
}

// input:   none
// output:  none
void personalinfo::local_store(){
	// Write file
	ofstream outFile;
    char* filename;
    filename = new char[100];
    filename = "data/local4.csv";
	outFile.open(filename, ios::app); // File mode omitted
	outFile <<"," << id << "," << name << "," << profession << "," << age << "," << age_group << "," << medical_risk 
    << "," << registration->registration_time->tm_year << ' ' << registration->registration_time->tm_mon << ' '
    << registration->registration_time->tm_mday << ' ' << registration->registration_time->tm_hour << ' '
    << registration->registration_time->tm_min << ' ' << registration->registration_time->tm_sec <<  "," 
    << deadline[0] << '/' << deadline[1] << '/' << deadline[2] << "," << contact->addr << "," << contact->phone << "," 
    << contact->wechat << "," << contact->email << "," << birthday << "," << registration->location_id[0] << " " 
    << registration->location_id[1] << " " << registration->location_id[2] <<","<< treatment <<  endl;
	outFile.close();    
}
