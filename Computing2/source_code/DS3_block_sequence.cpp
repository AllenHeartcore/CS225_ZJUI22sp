#include <ctime>
#include <time.h>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <stdint.h>
#include <math.h>
#include "medical_system.h"
using namespace std;
typedef struct tm tm_t;

extern bool Person_compare(const personalinfo* a,const personalinfo* b){
    return a->id < b->id;
}

extern bool Status_compare(const personalinfo* a,const personalinfo* b){
    return a->medical_risk < b->medical_risk;
}

extern bool Treatment_compare(const personalinfo* a,const personalinfo* b){
    return a->treatment < b->treatment;
}

void Block::insert_person(personalinfo* person)
{
    overflow.push_back(person);
    overflow_size++;
    //sort by person id as default
    if (overflow_size > 5){
        sort(overflow.begin(), overflow.end(), Person_compare);
        //push all the overflow data into the main block
        for (int i = 0; i < overflow.size(); i++){
            block_data.push_back(overflow[i]);
        }
        //clear data in overflow
        overflow.clear();
        overflow_size = 0;
    }
}

void Block::clearoverflow(){
    int i;
    for (i=0;i<overflow.size();i++){
        block_data.push_back(overflow[i]);
    }
    overflow.clear();
    overflow_size = 0;
}

bool Block::contain(int64_t id)
{   int i;
    for (i = 0; i < block_data.size(); i++){
        if (block_data[i]->id == id){
            return true;
        }
    }
    //find overflowblock
    for (i = 0; i < overflow.size(); i++){
        if (overflow[i]->id == id){
            return true;
        }
    }
    return false;
}

personalinfo* Block::find(int64_t id)
{
    int i;
    for (i = 0; i < block_data.size(); i++){
        if (block_data[i]->id == id){
            return block_data[i];
        }
    }
    //find overflowblock
    for (i = 0; i < overflow.size(); i++){
        if (overflow[i]->id == id){
            return overflow[i];
        }
    }
    return NULL;
}

void Block::delete_person(int64_t id)
{
    int i;
    for (i = 0; i < block_data.size(); i++){
        if (block_data[i]->id == id){
            block_data.erase(block_data.begin() + i);
            return;
        }
    }
    //find overflowblock
    for (i = 0; i < overflow.size(); i++){
        if (overflow[i]->id == id){
            overflow.erase(overflow.begin() + i);
            overflow_size--;
            return;
        }
    }
    return;
}



void Block_sequence::insert_record(personalinfo* person)
{
    //insert to the last block
    int i;
    if (Blocks.size() == 0){
        Block* new_block = new Block();
        new_block->block_data.push_back(person);
        Blocks.push_back(new_block);
        return;
    }
    else{
        Blocks[Blocks.size() - 1]->insert_person(person);
    }
    
}


void Block_sequence::split(){
    //split the last block
    int i;
    if (Blocks.size() == 0){
        return;
    }
    else{
        //split the last block
        if (Blocks[Blocks.size()-1]->block_data.size()>10){
            //create a new block
            Block* new_block = new Block();
            // from 5 to block_data.size()-1 comes to the new block
            for (i = 5; i < Blocks[Blocks.size()-1]->block_data.size(); i++){
                new_block->insert_person(Blocks[Blocks.size()-1]->block_data[i]);
            }
            //delete 5 to block_data.size()-1 from the last block
            int size = Blocks[Blocks.size()-1]->block_data.size();
            for (i=5;i<size;i++){
                Blocks[Blocks.size()-1]->block_data.pop_back();
            }
            //insert the new block to the Blocks
            Blocks.push_back(new_block);
        }
    }
}

void Block_sequence::delete_record(int64_t id){
    //find id in each block
    int i;
    for (i = 0; i < Blocks.size(); i++){
        if (Blocks[i]->contain(id)){
            Blocks[i]->delete_person(id);
            merge(i);
            return;
        }
    }
}

void Block_sequence::merge(int pos){
    //merge the last two blocks
    int i;
    if (Blocks.size()<2){
        return;
    }
    else{
        Blocks[pos]->clearoverflow();
        Blocks[pos-1]->clearoverflow();
        if (Blocks[pos]->block_data.size()<5){
            if (Blocks[pos-1]->block_data.size()>5){
                int size = Blocks[pos-1]->block_data.size();
                personalinfo* temp = Blocks[pos-1]->block_data[size-1];
                Blocks[pos-1]->block_data.pop_back();
                Blocks[pos]->insert_person(temp);
            }
            else {
                //merge the two blocks
                int size = Blocks[pos]->block_data.size();
                for (i = 0; i < size; i++){
                    Blocks[pos-1]->insert_person(Blocks[pos]->block_data[i]);
                }
                //delete the pos
                Blocks.erase(Blocks.begin() + pos);
            }
        }

    }
}

personalinfo* Block_sequence::retrieval(int64_t id){
    //find id in each block
    int i;
    for (i = 0; i < Blocks.size(); i++){
        if (Blocks[i]->contain(id)){
            return Blocks[i]->find(id);
        }
    }
    return NULL;
}

void Block_sequence::sort_Person(){
    //sort by person id
    int i,j;
    vector<personalinfo*> temp;
    for (i=0;i<Blocks.size();i++){
        Blocks[i]->clearoverflow();
        for (j=0;j<Blocks[i]->block_data.size();j++){
            temp.push_back(Blocks[i]->block_data[j]);
        }
    }
    //sort the temp vector
    sort(temp.begin(),temp.end(),Person_compare);
    Blocks.clear();
    //push back
    for (i=0;i<temp.size();i++){
        this->insert_record(temp[i]);
        cout << temp[i]->id << " " << temp[i]->treatment <<  " " << temp[i]->medical_risk << endl;
    }
}

void Block_sequence::sort_Status(){
    //sort by status
    int i,j;
    vector<personalinfo*> temp;
    for (i=0;i<Blocks.size();i++){
        Blocks[i]->clearoverflow();
        for (j=0;j<Blocks[i]->block_data.size();j++){
            temp.push_back(Blocks[i]->block_data[j]);
        }
    }
    //sort the temp vector
    sort(temp.begin(),temp.end(),Status_compare);
    Blocks.clear();
    //push back
    for (i=0;i<temp.size();i++){
        this->insert_record(temp[i]);
        cout << temp[i]->id << " " << temp[i]->treatment << " " << temp[i]->medical_risk << endl;
    }
}

void Block_sequence::sort_Treatment(void){
    //sort by treatment
    int i,j;
    vector<personalinfo*> temp;
    for (i=0;i<Blocks.size();i++){
        Blocks[i]->clearoverflow();
        for (j=0;j<Blocks[i]->block_data.size();j++){
            temp.push_back(Blocks[i]->block_data[j]);
        }
    }
    //sort the temp vector
    sort(temp.begin(),temp.end(),Treatment_compare);
    Blocks.clear();
    //push back
    for (i=0;i<temp.size();i++){
        this->insert_record(temp[i]);
        cout << temp[i]->id << " " << temp[i]->treatment  << " " << temp[i]->medical_risk << endl;
    }
}

void Block_sequence::print_all(){
    int i;
    for (i = 0; i < Blocks.size(); i++){
        Blocks[i]->print_block();
    }
}
void Block::print_block(){
    this->clearoverflow();
    int i;
    for (i = 0; i < block_data.size(); i++){
        cout << block_data[i]->id << " " << block_data[i]->medical_risk << " " << block_data[i]->treatment << endl;
    }
}

