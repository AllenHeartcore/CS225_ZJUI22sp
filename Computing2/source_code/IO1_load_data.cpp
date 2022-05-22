#include <iostream>
#include <vector>
#include "medical_system.h"
#include <string>
#include <time.h>
#include <fstream>
#include <sstream>

using namespace std;

typedef struct tm tm_t;



int find_person(int id){
    int n = blacklist.size();
    for (int i = 0; i < n; i++){
        if (blacklist[i]->id == id)
            return i+1;
    }
    return 0;
}

//  input:  current time: the time when the person is registered
//          blacklist: the vector of people who have been put in blacklist
//          localqueue_id: the id of the local queue the person should be put in
//  output: the localqueue filled with patients.



vector<personalinfo*> dataloading(tm_t* current_time, vector<personalinfo*> blacklist, int localqueue_id){
    char* filename;
    if (localqueue_id == 1)
        filename = "data/local1.csv";
    else if (localqueue_id ==2)
        filename = "data/local2.csv";
    else if (localqueue_id == 3)
        filename = "data/local3.csv";
    else
        filename = "data/local4.csv";
    ifstream inFile(filename, ios::in);
	string lineStr;
    vector<personalinfo*> localqueue;
	vector<vector<string>> strArray;
	while (getline(inFile, lineStr))
	{
		// Stored as a 2D list
		stringstream ss(lineStr);
		string str;
		vector<string> lineArray;
		// Use comma separators
        int i = 0;
		while (getline(ss, str, ','))
			lineArray.push_back(str);
		strArray.push_back(lineArray);
	}
    if (strArray.size() <= 1){
        inFile.close();
        return localqueue;
    }
    int i = 1;
    string date_input = strArray[i][7];
    stringstream ds(date_input);
    string date_string;
    vector<int> date_input_array;   
    while (getline(ds, date_string, ' ')){
        int date_int = stoi(date_string);
        date_input_array.push_back(date_int);
    }
    while(date_input_array[2] <= current_time->tm_mday){
        if (i > strArray.size()-1){
            inFile.close();
            return localqueue;
        }
        date_input = strArray[i][7];
        stringstream ds(date_input);
        date_input_array.clear();
        // turn the string into vector
        while (getline(ds, date_string, ' ')){
            int date_int = stoi(date_string);
            date_input_array.push_back(date_int);
        }
        string hospital_input = strArray[i][14];
        stringstream hs(hospital_input);
        string hospital_string;
        vector<int> hospital_array;
        // turn the string into vector
        while (getline(hs, hospital_string, ' ')){
            int hospital_int = stoi(hospital_string);
            hospital_array.push_back(hospital_int);
        }
        string deadline_input = strArray[i][8];
        stringstream dls(deadline_input);
        string deadline_string;
        vector<int> deadline_array;
        // turn the string into vector
        while (getline(dls, deadline_string, '/')){
            int deadline_int = stoi(deadline_string);
            deadline_array.push_back(deadline_int);
        }
        if (date_input_array[2] < current_time->tm_mday){
            i++;
            continue;
        }
        // store the information into the personalinfo
        if ((date_input_array[2] == current_time -> tm_mday) && ((date_input_array[3] < 12 && current_time->tm_hour == 0) || (date_input_array[3] >= 12 && current_time->tm_hour == 12))){
            personalinfo* information = new personalinfo;
            information->id = stoi(strArray[i][1]);
            information->name = strArray[i][2];
            information->profession = stoi(strArray[i][3]);
            information->age = stoi(strArray[i][4]);
            information->age_group = stoi(strArray[i][5]);
            information->medical_risk = stoi(strArray[i][6]);
            information->treatment = stoi(strArray[i][15]); 
            information->registration->registration_time->tm_mday = date_input_array[2];
            information->registration->registration_time->tm_mon = date_input_array[1];
            information->registration->registration_time->tm_year = date_input_array[0];
            information->registration->registration_time->tm_hour = date_input_array[3];
            information->registration->registration_time->tm_min = date_input_array[4];
            information->registration->registration_time->tm_sec = date_input_array[5];
            information->registration->location_id[0] = hospital_array[0];
            information->registration->location_id[1] = hospital_array[1];
            information->registration->location_id[2] = hospital_array[2];
            if (deadline_array[0] == -1){
                information->deadline[0] = -1;
                information->deadline[1] = -1;
                information->deadline[2] = -1;
            }
            else{
                information->deadline[0] = deadline_array[0];
                information->deadline[1] = deadline_array[1];
                information->deadline[2] = deadline_array[2];        
            }
            information->contact->addr = strArray[i][9];
            information->contact->phone = strArray[i][10];
            information->contact->wechat = strArray[i][11];
            information->contact->email = strArray[i][12];
            information->birthday = strArray[i][13];
            information->appointment = 0;
            information->waited_time = 0;
            //calculate priority
            //profession * e11 + age_group * e10 + month * e8 + day * e6 + hour * e4 + min * e2 + sec
            //need revise
            if (information->treatment == 0){ //treatment 0
                information->priority = information->profession * 1e11 + information->age_group * 1e10 + 
                information->registration->registration_time->tm_mon * 1e8 + information->registration->registration_time->tm_mday * 1e6 +
                information->registration->registration_time->tm_hour * 1e4 + information->registration->registration_time->tm_min * 1e2 + 
                information->registration->registration_time->tm_sec;
            }
            else if (information->treatment == 1){  //treatment 1
                information->priority = information->age_group * 1e11 + information->profession * 1e10 + 
                information->registration->registration_time->tm_mon * 1e8 + information->registration->registration_time->tm_mday * 1e6 +
                information->registration->registration_time->tm_hour * 1e4 + information->registration->registration_time->tm_min * 1e2 + 
                information->registration->registration_time->tm_sec;
            }
            else {  //treatment 2
                information->priority = information->profession * 1e10 + 
                information->registration->registration_time->tm_mon * 1e8 + information->registration->registration_time->tm_mday * 1e6 +
                information->registration->registration_time->tm_hour * 1e4 + information->registration->registration_time->tm_min * 1e2 + 
                information->registration->registration_time->tm_sec;
            }
            int flag = 0;
            if (information->medical_risk == 2){
                information->priority += 1e8;
                flag = 1;
            }
            if (information->medical_risk == 3){
                information->priority = 9e11;    //is infinite accutually
                flag = 1;
            }
            if (find_person(information->id)){
                // if the patient is in the blacklist, then we postpone his/her registration time by 2 weeks
                // in the mean time he is not high risk or middle risk.
                if (flag == 0){
                    if (date_input_array[2] + 14 > 31){
                        information->priority += 1e8;
                        information->priority += (date_input_array[2] + 14 - 31) * 1e6;
                    }
                    else{
                        information->priority += (date_input_array[2] + 14) * 1e6;
                    }
                }
                blacklist.erase(blacklist.begin() + find_person(information->id ) - 1);
            }
            information->get_secKey();
            localqueue.push_back(information);
            IDsheet.push_back(information);
            bplus_tree.insert(information);
            btree.insertion(information);
            order_block.insert_record(information);
        }
        i++;
        if (i >= strArray.size()){
            break;
        }
    }
    inFile.close();
    return localqueue;
}
