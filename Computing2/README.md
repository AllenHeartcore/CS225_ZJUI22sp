# ZJUI CS225 SP22 Computing Assignment 2 (Group E1)

## **Authors**

3200112393 LIANG Weijie (Fibonacci heaps, control flow, user interface)

3200112407 CHEN Ziyuan ($B^+$-tree, primary keys)

3200112414 CHEN Zhirong ($B$-tree, secondary keys)

3180111651 CHEN Xuming (Block sequence, queries, file I/O)

## **Running the Program**

Please use the provided shell script to compile and execute the code in the Linux terminal. You can manually input the patients' information, and the program will also automatically read the `.csv` documents. 
	
	chmod +x ./run_medical_system.sh
	./run_medical_system.sh


## **Functionality**

1. **INPUT** (Register): `localX.csv` --> `personalinfo`, `localqueueX`

&emsp; &emsp; For registration, we use 4 csv files `localX.csv` to simulate loading data of the registered patients. For each half day, we (1) load the registered patients to `localqueue` and (2) upload the data to `centerqueue`. 

2. **UPLOAD** (Queue): `personalinfo`, `localqueueX` --> `BPlusTree`, `BTree`, `Block_sequence`, `centerqueue`

3. **DOWNLOAD** (Appoint): `BPlusTree`, `BTree`, `Block_sequence`, `centerqueue` --> `hospital`

&emsp; &emsp; We implement a **fibonacci heap** to extract the patient with the highest priority and assign him/her an appointment. The formula of the bit vector `priority` is defined below. We have 3 treatment and each treatment has 3 kinds of priority calculation rules. Consequently, we also need 3 fibonacci heap and each with an individual pirority rules and deal with registration individually. 

	if (treatment == 0)
		priority = profession * 10e11 + age_group * 10e10
				 + month * 10e8 + day * 10e6 + hour * 10e4 + min * 10e2 + sec
	else if (treatment == 1)
		priority = age_group * 10e11 + profession * 10e10
				 + month * 10e8 + day * 10e6 + hour * 10e4 + min * 10e2 + sec
	else if (treatment == 2)
		priority = profession * 10e10
				 + month * 10e8 + day * 10e6 + hour * 10e4 + min * 10e2 + sec


4. **OUTPUT** (Report): `hospital` --> `report`

5. **ARRANGE PERSON** (Report): Personal information are stored in an **Ordered sequence**, a **B tree** and a **Bplus tree**. **Ordered sequence** with overflow, blcok can split and merge like the linked list in **Bplus tree**, it's used to sort patients. **B tree** use age as secondary key and allow us to search patients using age. **Bplus tree** with overflow block, use unique ID as key and allow us to search a person using ID. 

## **Assumptions**

1. The medical system is simulated only for **1 month**. 

2. There are **3 hospitals** with respective capacities of **5 patients** per day. 

3. The patient data in `local1.csv`, `local2.csv`, `local3.csv`, and `local4.csv` are randomly generated, but on average, there will be **18 patients** to register in total. 

4. The system supports **interactive interface**. You can input the patient data directly through keyboard, but there are some format restrictions. These patients' data will be stored in `local4.csv`. 

5. A patient's information can only be seen after **at least half a day** has passed. 

6. `weekly_report.csv` will be **overwritten** upon every attempt of generation. 

<br\>

## **Appendix I: File Organization**

**`README.pdf`** - This document

**`source_code/main.cpp`** - `zerotime(time)`, `main()`

**`source_code/DS1_bplustree.cpp`** - `BPlusTree::*()`, `BPlusNode::*()`

**`source_code/DS2_btree.cpp`** - `BTree::*()`, `BTreeNode::*()`

**`source_code/DS3_block_sequence.cpp`** - `Block_sequence::*()`, `Block::*()`

**`source_code/DS4_fibonacci_heap.cpp`** - `centerqueue::*()`, `centernode::*()`

**`source_code/CF_controlflow.cpp`** - `skip_half_day(...)`, `checktime()`, `hospital::*()`, `report::*()`

**`source_code/IO1_load_data.cpp`** - `find_person(id)`, `dataloading(...)`

**`source_code/IO2_local_registration.cpp`** - `personalinfo::local_register(...)`, `local_store()`

**`source_code/medical_system.h`** - Header file of definitions

**`data/localX.csv`** (**`X`** = `1`, `2`, `3`, `4`) - Simulated patient data

**`sample_reports/`** - Two sample reports (weekly & monthly)

**`medical_system.sh`** - Script for compilation and execution

**`bplustree_demo`** - Demo program of $B^+$-tree functionality



## **Appendix II: Data Structures**

**`personalinfo` - Basic information recording unit for a patient**

&emsp; &emsp; `id` - Unique identification number

&emsp; &emsp; `appointment` - Treatment stage indicator (`0` = waiting, `1` = being treated)

&emsp; &emsp; `priority` - Treatment priority (calculated)

&emsp; &emsp; `registration` {`registration_time`, `location_id`, `hospital`}, 

&emsp; &emsp; `medical_risk`, `profession`, `age_group` - Info for calculating `priority`

&emsp; &emsp; `treatment` - From 0 to 2, each macthes different priority rules. 

&emsp; &emsp; `deadline` - Deadline [Y, M, D] of treatment (for overriding `priority` with `1`)

&emsp; &emsp; `waited_time` - Total waiting time (for reporting)

&emsp; &emsp; `contact` {`addr`, `phone`, `wechat`, `email`}, `name`, `birthday`, `age` - Additional info

&emsp; &emsp; `localqueue_id` - ID of `localqueue` (one of `1`, `2`, `3`, `4`)

&emsp; &emsp; `local_store()`, `local_register(current_time)`



**`centerqueue` - Waiting list as a fibonacci heap**

&emsp; &emsp; `size` - Total number of `centernode`s

&emsp; &emsp; `min` - Pointer to the minimum `centernode`

&emsp; &emsp; `insert_node(node)` - Insert a `centernode` into the heap

&emsp; &emsp; `delete_node(id)` - Delete a `centernode` with given ID

&emsp; &emsp; `modify_key(id, priority)` - Modify the priority of a `centernode`

&emsp; &emsp; `extract_min()` - Extract the minimum `centernode` from the heap

&emsp; &emsp; `search_heap()`, `search_subheap()` - Subroutines for searching

&emsp; &emsp; `cut(...)`, `cascading_cut(...)` - Subroutines of `delete_node`

&emsp; &emsp; `consolidate()`, `link(...)` - Subroutines of `extract_min`

**`centernode` - Node of the fibonacci heap**

&emsp; &emsp; `id` - A patient's unique ID

&emsp; &emsp; `priority` - A patient's treatment priority (criterion for sorting)

&emsp; &emsp; `person` - A patient's information as a `personalinfo`

&emsp; &emsp; `parent`, `child`, `left`, `right` - Pointers to other `centernode`s

&emsp; &emsp; `degree`, `mark` - Additional node properties

**`hospital` - Information recording unit for hospitals**

&emsp; &emsp; `location_id` - The hospital's ID

&emsp; &emsp; `inhospital` - `Centernode`s of patients in the hospital (up to 5)

&emsp; &emsp; `come_hospital(node)` - Add a `centernode` to the hospital

&emsp; &emsp; `clear_hospital()` - Clear all `centernode`s from the hospital

**`report` - Information recording unit for reports**

&emsp; &emsp; `size` - Counter of registrations

&emsp; &emsp; `treatment` - Counter of treatments

&emsp; &emsp; `with_drew_patients` - Counter of withdrawn patients

&emsp; &emsp; `sum_waited_time` - Total waiting time

&emsp; &emsp; `treated` - Vector of treated patients

&emsp; &emsp; `appointment` - Vector of patients with appointment

&emsp; &emsp; `not_treated` - Vector of patients without appointment

&emsp; &emsp; `store_data()` - Collect patients not yet treated

&emsp; &emsp; `generate_report(filename)` - Generate a report (monthly or weekly)

&emsp; &emsp; `clear_report()` - Clear all the data stored in the report

&emsp; &emsp; `sort_nodes_name()` - Sort the patients in terms of name

&emsp; &emsp; `sort_nodes_agegroup()` - Sort the patients in terms of age_group

&emsp; &emsp; `sort_nodes_profession()` - Sort the patients in terms of profession

**`Block` - A block in the block sequence**

&emsp; &emsp; `overflow` - The overflow block

&emsp; &emsp; `block_data` - The main block

&emsp; &emsp; `max_size` - Max_size of the block

&emsp; &emsp; `contain(id)`, `find(id)` - Find the person in a block using id. 

&emsp; &emsp; `insert_person(...)`, `delete_person(id)` - Insert and delete in a block

**`Block_sequence` - A sequence of blocks**

&emsp; &emsp; `insert_record(...)`, `delete_record(id)` - Insert and delete in the blocks

&emsp; &emsp; `merge(pos)` - When two blocks have small size, merge them. 

&emsp; &emsp; `split()` - When size of block is large, it will be split into two blocks

&emsp; &emsp; `sort_Person()`, `sort_Status()`, `sort_Treatment` - Sort the blocks using different keys

&emsp; &emsp; `retrieval(id)` - Find a person in the block sequence using id

**`BTreeNode` - Node of $B$-tree**

&emsp; &emsp; `patient` - Store the personal information

&emsp; &emsp; `keys` - An array of Btree's keys

&emsp; &emsp; `leaf` - Whether it's a leaf

&emsp; &emsp; `borrowFromPrev()`, `borrowFromNext()` - When size is too small, borrow from brothers

&emsp; &emsp; `merge()` - Merge with brothers when it cannot borrow from them

&emsp; &emsp; `splitChild()` - Split when the size is too big

**`BTree` - A $B$-tree using age as a secondary key**

&emsp; &emsp; `t` - Degree of Btree

&emsp; &emsp; `traverse()` - Traverse the whole $B$-tree. 

&emsp; &emsp; `insertion(...)` - Store the personal information

&emsp; &emsp; `deletion(key)` - Delete a node in $B$-tree using the key

&emsp; &emsp; `query(age)` - Search all the patients in $B$-tree using an age

**`BPlusNode` - Node of Bplus-tree**

&emsp; &emsp; `is_leaf` - Whether it's leaf

&emsp; &emsp; `size` - Size of the node 

&emsp; &emsp; `next`, `prev` - The next and previous node

&emsp; &emsp; `parent` - Parent of the node

&emsp; &emsp; `patients` - Personal information stored in the node

&emsp; &emsp; `find_id(...)` - Find person in the node using an id

**`BPlusTree` - Bplus-tree using id as its key**

&emsp; &emsp; `root` - Root of Bplus tree

&emsp; &emsp; `insert()` - Insert a person

&emsp; &emsp; `remove(id)` - Delete a person using id

&emsp; &emsp; `search(id)` - Search a person in Bplus-tree using id 

&emsp; &emsp; `get_dummy()` - Get the head of the linked list

&emsp; &emsp; `query()` - Search id in a range

&emsp; &emsp; `print()` - Print the elements in Bplus-tree, using DFS


**Global variables**

&emsp; &emsp; `dataloading(current_time, blacklist, local_queue_id)`

&emsp; &emsp; `passedhdays` - Counter of passed *half* days

&emsp; &emsp; `timeflow` - Global timeflow controller

&emsp; &emsp; `skip_half_day(op)` - Skip a time period based on `op`code

&emsp; &emsp; &emsp; (`1` = skip half a day, `2` = skip a week, `3` = skip a month.)

&emsp; &emsp; `checktime()` - Check the timeflow and arrange the queues

&emsp; &emsp; `IDsheet` - Vector of all patients in the system (waiting & being treated)

&emsp; &emsp; `blacklist` - Vector of withdrawn patients

&emsp; &emsp; `localqueueX` - `localqueue`s generated from `localX.csv` (`X` = `1`, `2`, `3`, `4`)

&emsp; &emsp; `Fheap[3]` - Three global fibonacci heap (`centerqueue`)

&emsp; &emsp; `hos[3]` - Records of the three `hospital`s

&emsp; &emsp; `week_report`, `mon_report` - Two `report`s of different scopes

&emsp; &emsp; `btree` - A $B$-tree using age as its secondary key to search patients. 

&emsp; &emsp; `bplus_tree` - A $B^+$-tree using id as its key to search patients. 

&emsp; &emsp; `order_block` - A list of blocks arranged similarly to the linked list of $B^+$-tree, used to sort patients. 

&emsp; &emsp; `num_localqueues`, `num_appointments`, `num_week_report`, `num_mon_report` - Counters

