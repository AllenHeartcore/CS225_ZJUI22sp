#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include "medical_system.h"

using namespace std;
typedef struct tm tm_t;
vector<personalinfo*> blacklist;
vector<personalinfo*> localqueue1;
vector<personalinfo*> localqueue2;
vector<personalinfo*> localqueue3;
vector<personalinfo*> localqueue4;
tm_t* timeflow;
vector<personalinfo*> IDsheet;
int passedhdays = 0; //how many halfdays has passed
int num_localqueues = 0;
int num_appointments = 0;
int num_week_report = 0;
int num_mon_report = 0;
report week_report;
report mon_report;
centerqueue Fheap = centerqueue();
hospital* hos[3];

void zerotime(tm_t* time){
    time->tm_sec = 0;
    time->tm_min = 0;
    time->tm_hour = 0;
    time->tm_mday = 1;
    time->tm_mon = 1;
    time->tm_year = 2022;
}





int main(){
    //initial time flow
    time_t now = time(0);
    timeflow = localtime(&now);
    zerotime(timeflow);
    //prepare local queue and ID sheet


    hospital hos1(1);
    hospital hos2(2);
    hospital hos3(3);
    hos[0] = &hos1;
    hos[1] = &hos2;
    hos[2] = &hos3;




    // initialize the registraion.csv file
    ofstream registration_csv;
    registration_csv.open("data/local4.csv");
    registration_csv << ",id,name,profession,age,age_group,medical_risk,registration time,deadline,address,phone,wechat,email,birthday,hospital priority" << endl;
    //prepare local queue and ID sheet
    int i,j;

    int op=1;
    while (op != 0 ){
        cout << "\nWe are on the " << (passedhdays % 2? "afternoon" : "morning") << " of January " << passedhdays / 2 + 1 << ", 2022.\n\n";
        cout << "O: Quit\n";
        cout << "1: Jump half a day\n";
        cout << "2: Jump a week\n";
        cout << "3: Jump a month\n";
        cout << "4: Manual register\n";
        cout << "5: Withdraw\n";
        cout << "6: List all valid IDs\n";
        cout << "7: Update information\n";
        cout << "\nChoose an operation: ";
        cin >> op ;
        switch(op){
            case 0:{
                system("clear");
                break;
            }
            case 1:
            {
                system("clear");
                if (passedhdays >= 56){
                        cout << "We can only simulate a month" << endl;
                        return 0;
                }
                skip_half_day(op);
                cout << "Jump half a day" << endl;
                break;
            }
            case 2:{
                system("clear");
                for (i=0;i<14;i++){
                    if (passedhdays >= 56){
                        cout << "We can only simulate a month" << endl;
                        return 0;
                    }
                    skip_half_day(op);
                }
                break;
            }
            case 3:{
                system("clear");
                for (i=0;i<56;i++){
                    if (passedhdays >= 56){
                        cout << "We can only simulate a month" << endl;
                        return 0;
                    }
                    skip_half_day(op);
                }
                break;
                
            }
            case 4:{
                personalinfo a;
                if (a.local_register(timeflow) == 1)
                    a.local_store();
                system("clear");
                break;
            }
            case 5:{
                cout<<"\nPlease input the ID of the patient you want to withdraw: ";
                int inputid;
                cin >> inputid;
                int flag = 0;
                for (i=0;i<IDsheet.size();i++){
                    if (IDsheet[i]->id == inputid){
                        // add to blacklist
                        blacklist.push_back(IDsheet[i]);
                        //delete from Fheap or hospital
                        if (IDsheet[i]->appointment == 0){
                            Fheap.delete_node(inputid);
                        }
                        else {
                            for (j=0;j<3;j++){
                                int k;
                                for (k=0;k<hos[j]->inhospital.size();k--){
                                    if (hos[j]->inhospital[k]->id == inputid){
                                        hos[j]->inhospital.erase(hos[j]->inhospital.begin()+k);
                                        break;
                                    }
                                }
                            }
                        }
                        //delete from IDsheet
                        IDsheet.erase(IDsheet.begin()+i);
                        flag = 1;
                        mon_report.size--;
                        week_report.size--;
                        week_report.with_drew_people++;
                        mon_report.with_drew_people++;
                        break;
                    }
                }
                system("clear");
                if (flag == 0)
                    cout << "No such patient" << endl;
                else
                    cout << "Patient " << inputid << " has been withdrawn" << endl;
                break;
            }
            case 6:{
                system("clear");
                for (i=0;i<IDsheet.size();i++){
                    cout << IDsheet[i]->id << " " << IDsheet[i]->name << endl;
                }
                break;
            }  
            case 7: {
                cout << "\nPlease input the ID of the patient you want to update: ";
                int inputid,flag,choice;
                flag =0;
                cin >> inputid;

                for (i=0;i<IDsheet.size();i++){
                    if (IDsheet[i]->id == inputid){
                        cout << "\nChoose 1 for new medical risk (0 to 3)\n";
                        cout << "Choose 2 for new profession (1 to 8)\n";
                        cout << "Please select the new information type: ";
                        cin >> choice;
                        switch (choice){
                            case 1:{
                                cout << "\nPlease input the new medical risk: ";
                                int risk;
                                uint64_t new_priority;
                                cin >> risk;
                                if (risk < 0 || risk > 3){
                                    system("clear");
                                    cout << "Invalid option" << endl;
                                    break;
                                }
                                IDsheet[i]->medical_risk = risk;
                                //new priority
                                //profession * e11 + age_group * e10 + month * e8 + day * e6 + hour * e4 + min * e2 + sec
                                IDsheet[i]->priority = IDsheet[i]->profession*1e11+IDsheet[i]->age_group*1e10+
                                IDsheet[i]->registration->registration_time->tm_mon*1e8+IDsheet[i]->registration->registration_time->tm_mday*1e6+
                                IDsheet[i]->registration->registration_time->tm_hour*1e4+IDsheet[i]->registration->registration_time->tm_min*1e2+
                                IDsheet[i]->registration->registration_time->tm_sec;
                                if (IDsheet[i]->medical_risk == 2){
                                    IDsheet[i]->priority += 1e8;
                                }
                                else if (IDsheet[i]->medical_risk == 3){
                                    IDsheet[i]->priority = 9e11;
                                }

                                new_priority = IDsheet[i]->priority;
                                if (IDsheet[i]->appointment == 0){
                                    Fheap.modify_key(inputid, new_priority);
                                }
                                system("clear");
                                cout << "Patient " << inputid << "'s medical risk has been updated to " << risk << endl;
                                break;
                            }
                            case 2:{
                                cout << "\nPlease input the new profession: ";
                                int prof;
                                cin >> prof;
                                uint64_t new_priority;
                                if (prof < 1 || prof > 8){
                                    system("clear");
                                    cout << "Invalid option" << endl;
                                    break;
                                }
                                //new priority
                                IDsheet[i]->profession = prof;
                                IDsheet[i]->priority = IDsheet[i]->profession*1e11+IDsheet[i]->age_group*1e10+
                                IDsheet[i]->registration->registration_time->tm_mon*1e8+IDsheet[i]->registration->registration_time->tm_mday*1e6+
                                IDsheet[i]->registration->registration_time->tm_hour*1e4+IDsheet[i]->registration->registration_time->tm_min*1e2+
                                IDsheet[i]->registration->registration_time->tm_sec;
                                if (IDsheet[i]->medical_risk == 2){
                                    IDsheet[i]->priority += 1e8;
                                }
                                else if (IDsheet[i]->medical_risk == 3){
                                    IDsheet[i]->priority = 9e11;
                                }

                                new_priority = IDsheet[i]->priority;
                                if (IDsheet[i]->appointment == 0){
                                    Fheap.modify_key(inputid, new_priority);
                                }
                                system("clear");
                                cout << "Patient " << inputid << "'s profession has been updated to " << prof << endl;
                                break;
                            }
                            default:
                                system("clear");
                                cout << "Invalid option" << endl;
                                break;
                        }
                        // update information
                        flag = 1;
                        break;
                    }
                }
                if (flag == 0) {
                    system("clear");
                    cout << "No such patient" << endl;
                }
                break;
            }
            default:
                system("clear");
                cout << "Invalid option" << endl;
                break; 
        }
    }
    return 0;
}
