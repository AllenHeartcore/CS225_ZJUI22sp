#include <ctime>
#include <time.h>
#include <vector>
#include <string>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <cstring>
#include "medical_system.h"

using namespace std;
typedef struct tm tm_t;
//this is designed for F-heap methods 
//and some operations on ID sheet or localqueue, black list
//some for hospital

void skip_half_day(int op){ 
    localqueue1 = dataloading(timeflow, blacklist, 1);
    localqueue2 = dataloading(timeflow, blacklist, 2);
    localqueue3 = dataloading(timeflow, blacklist, 3);
    localqueue4 = dataloading(timeflow, blacklist, 4);
    timeflow->tm_hour += 12; //add time
    if (timeflow->tm_hour >=24){ //if time is over 24, we need to add a day
        timeflow->tm_hour -= 24;
        timeflow->tm_mday += 1;
    }
    passedhdays +=1;
    checktime();
}

void checktime(){
    //check halfdays for local queue
    if (passedhdays - num_localqueues > 0 ){//if we passed halfday, clear local queues and upload to center
        num_localqueues+=1;
        int i;
        for (i=0;i<localqueue1.size();i++){
            centernode* newnode = new centernode(localqueue1[i]);
            Fheap.insert_node(newnode);
            week_report.size+=1;
            mon_report.size+=1;
        }
        localqueue1.clear();
        for (i=0;i<localqueue2.size();i++){
            centernode* newnode = new centernode( localqueue2[i] );
            Fheap.insert_node(newnode);
            week_report.size+=1;
            mon_report.size+=1;
        }
        localqueue2.clear();
        for (i=0;i<localqueue3.size();i++){
            centernode* newnode = new centernode( localqueue3[i] );
            Fheap.insert_node(newnode);
            week_report.size+=1;
            mon_report.size+=1;
        }
        localqueue3.clear();
        for (i=0;i<localqueue4.size();i++){
            centernode* newnode = new centernode( localqueue4[i] );
            Fheap.insert_node(newnode);
            week_report.size+=1;
            mon_report.size+=1;
        }
        localqueue4.clear();
    }
    // a day passed
    if ((passedhdays - 2*num_appointments) >= 2){
        num_appointments += 1;
        int i,j;
        for (i=0;i<3;i++){
            hos[i]->clear_hospital(); //clear hospital 
        }
        //check whether we have DDL
        for (i=0;i<IDsheet.size();i++){
            if (IDsheet[i]->deadline[0]>0){
                if (IDsheet[i]->deadline[0] == timeflow->tm_mday +1){
                //change the priority of Fheap
                Fheap.modify_key(IDsheet[i]->id, 1);
                }
            }
        }
        for (i=0;i<15;i++){
            centernode* node = Fheap.extract_min();
            if (node == NULL){
                break;   //no more person in F-heap
            }
            else {
                for (j=0;j<3;j++){
                    int index = node->person->registration->location_id[j];
                    if (hos[index-1]->come_hospital(node) == 1){
                        break; //match person with their preferred hospital
                    }
                }
            }
        }
    }
    // a week
    if ((passedhdays - 14*num_week_report)  >= 14 ){
        num_week_report += (passedhdays/14 - num_week_report);
        cout << "Weekly report generating" << endl;
        week_report.store_data();
        int sorttype;
        cout << "\n1 - By profession, 2 - By name, 3 - By age group\nPlease enter your sort preference: ";
        cin >> sorttype;
        if (sorttype == 1){
            week_report.sort_nodes_profession();
        }
        else if (sorttype == 2){
            week_report.sort_nodes_name();
        }
        else if (sorttype == 3){
            week_report.sort_nodes_age_group();
        }
        else {
            cout << "Invalid option, report entries are not sorted!" << endl;
        }
        week_report.generate_report("weekly_report.csv");
        week_report.clear_report();
    }
    // a month
    if (passedhdays > 55){
        num_mon_report +=(passedhdays/56 - num_mon_report);
        cout << "Monthly report generating\n" << "passedhdays is " << passedhdays << endl;
        mon_report.store_data();
        mon_report.generate_report("monthly_report.csv");
        mon_report.clear_report();
    }
}


int hospital::come_hospital(centernode* node){
    if (inhospital.size() >5){
        return 0; 
    }
    else{
        this->inhospital.push_back(node);
        node->person->registration->hospital = this->location_id;
        node->person->appointment = 1; //push person into hospital and mark their appointment
        return 1;
    }
}


void hospital::clear_hospital(){
    int i,j;
    for (i = 0; i < inhospital.size(); i++){//clacuate waited time
        inhospital[i]->person->waited_time = timeflow->tm_mday - inhospital[i]->person->registration->registration_time->tm_mday;
        inhospital[i]->person->registration->hospital = this->location_id; //which hospital
        week_report.treated.push_back(inhospital[i]);
        mon_report.treated.push_back(inhospital[i]);
        week_report.sum_waited_time += inhospital[i]->person->waited_time;
        mon_report.sum_waited_time += inhospital[i]->person->waited_time;
        week_report.treatment += 1;
        mon_report.treatment += 1; //record treatment
        //delete IDsheet 
        for (j=0;j<IDsheet.size();j++){
            if (IDsheet[j]-> id == inhospital[i]->person->id){
                IDsheet.erase(IDsheet.begin()+j);
                break;
            }
        }
    }
    inhospital.clear();
}
//also need to delete ID sheet?


void report::clear_report(){
    treated.clear();
    appointment.clear();
    not_treated.clear();
    sum_waited_time = 0;
    size = 0;
    treatment = 0;
    with_drew_people = 0;
}


void report::store_data(){
    //store patients in hospital and F-heap
    //if appoinemnt is 1, store appointment, else store not_treated
    for (int i = 0; i < IDsheet.size(); i++){
        centernode* newnode = new centernode( IDsheet[i] );
        newnode->person->waited_time = timeflow->tm_mday - newnode->person->registration->registration_time->tm_mday;
        if (newnode->person->appointment == 1){
            this->appointment.push_back(newnode); 
        }
        else {
            this->not_treated.push_back(newnode);
        }
    }
}


void report::sort_nodes_age_group(){
    //similar to sort_nodes_profession(), sort the nodes in terms of agegroup
    int i,j;
    for (i=0;i<appointment.size();i++){
        for (j=i+1;j<appointment.size();j++){
            if (appointment[i]->person->age_group > appointment[j]->person->age_group){
                centernode* temp = appointment[i];
                appointment[i] = appointment[j];
                appointment[j] = temp;
            }
        }
    }
    for (i=0; i<not_treated.size()-1; i++){
        for (j=0; j<not_treated.size()-1-i; j++){
            if (not_treated[j]->person->age_group > not_treated[j+1]->person->age_group){
                centernode* temp = not_treated[j];
                not_treated[j] =not_treated[j+1];
                not_treated[j+1] = temp;
            }
        }
    }
    //also for treated
    for (i=0; i<treated.size()-1; i++){
        for (j=0; j<treated.size()-1-i; j++){
            if (treated[j]->person->age_group > treated[j+1]->person->age_group){
                centernode* temp = treated[j];
                treated[j] =treated[j+1];
                treated[j+1] = temp;
            }
        }
    }
}

void report::sort_nodes_profession(){
    //bubble sort, sort interms of profession
    int i,j;
    for (i=0; i<treated.size()-1; i++){
        for (j=0; j<treated.size()-1-i; j++){
            if (treated[j]->person->profession > treated[j+1]->person->profession){
                centernode* temp = treated[j];
                treated[j] =treated[j+1];
                treated[j+1] = temp;
            }
        }
    }
    //perform the same operation for not_treated
    for (i=0; i<not_treated.size()-1; i++){
        for (j=0; j<not_treated.size()-1-i; j++){
            if (not_treated[j]->person->profession > not_treated[j+1]->person->profession){
                centernode* temp = not_treated[j];
                not_treated[j] =not_treated[j+1];
                not_treated[j+1] = temp;
            }
        }
    }
    //perform the same operation for appointment
    for (i=0; i<appointment.size()-1; i++){
        for (j=0; j<appointment.size()-1-i; j++){
            if (appointment[j]->person->profession > appointment[j+1]->person->profession){
                centernode* temp = appointment[j];
                appointment[j] =appointment[j+1];
                appointment[j+1] = temp;
            }
        }
    }
}

void report::sort_nodes_name(){
    //bubble sort by name, for 3 vectors
    int i,j;
    for (i=0; i<treated.size()-1; i++){
        for (j=0; j<treated.size()-1-i; j++){
            if (treated[j]->person->name > treated[j+1]->person->name){
                centernode* temp = treated[j];
                treated[j] =treated[j+1];
                treated[j+1] = temp;
            }
        }
    }
    for (i=0; i<not_treated.size()-1; i++){
        for (j=0; j<not_treated.size()-1-i; j++){
            if (not_treated[j]->person->name > not_treated[j+1]->person->name){
                centernode* temp = not_treated[j];
                not_treated[j] =not_treated[j+1];
                not_treated[j+1] = temp;
            }
        }
    }
    for (i=0; i<appointment.size()-1; i++){
        for (j=0; j<appointment.size()-1-i; j++){
            if (appointment[j]->person->name > appointment[j+1]->person->name){
                centernode* temp = appointment[j];
                appointment[j] =appointment[j+1];
                appointment[j+1] = temp;
            }
        }
    }
}

void report::generate_report(char* filename){
	// Write file
	ofstream outFile;
    if (filename == "weekly_report.csv"){
        outFile.open(filename, ios::out); // File mode omitted
        outFile << ",name,profession,age_group,medical_risk,waiting_time" << endl;
        outFile << "Treated" << endl;
        for(int i = 0; i < treated.size(); i++){
            outFile << i << "," << treated[i]->person->name << "," << treated[i]->person->profession << "," << treated[i]->person->age_group << "," << treated[i]->person->medical_risk << "," << treated[i]->person->waited_time << endl;
        }
        outFile << "Appointment" << endl;
        for(int i = 0; i < appointment.size(); i++){
            outFile << i << "," << appointment[i]->person->name << "," << appointment[i]->person->profession << "," << appointment[i]->person->age_group << "," << appointment[i]->person->medical_risk << "," << appointment[i]->person->waited_time << endl;
        }
        outFile << "Not_treated" << endl;
        for(int i = 0; i < not_treated.size(); i++){
            outFile << i << "," << not_treated[i]->person->name << "," << not_treated[i]->person->profession << "," << not_treated[i]->person->age_group << "," << not_treated[i]->person->medical_risk << "," << not_treated[i]->person->waited_time << endl;
        }
        outFile.close();   
    }
    else{
        outFile.open(filename, ios::out); // File mode omitted
        outFile << "Month Statistics" << endl;
        outFile << "Registered patient num: " << size << endl;
        outFile << "Treated patient num: " << treatment << endl;
        outFile << "Waiting patient num: " << not_treated.size() + appointment.size() << endl;
        outFile << "Withdraw patient num: " << with_drew_people << endl;
        outFile << "Appointments num: " << treatment + appointment.size() << endl;
        outFile << "Waiting time: " << sum_waited_time / size << " days" << endl;
        outFile.close();
    }
}
