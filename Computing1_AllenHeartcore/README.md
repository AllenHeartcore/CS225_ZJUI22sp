# ZJUI CS225 SP22 Computing Assignment 1 (Group E1)

## **Authors**

3200112393 LIANG Weijie (Control flow)

3200112407 CHEN Ziyuan (Fibonacci heap)

3200112414 CHEN Zhirong (File I/O)

3180111651 CHEN Xuming

## **Running the Program**

Please use the provided shell script to compile and execute the code in the Linux terminal. You can manually input the patients' information, and the program will also automatically read the `.csv` documents. 
	
	chmod +x ./medical_system.sh
	./medical_system.sh

There is also a demo program `fibonacci_heap_demo` that presents the functionality of the heap structure. 
	
	chmod +x ./fibonacci_heap_demo
	./fibonacci_heap_demo

## **Functionality**

1. **INPUT** (Register): `localX.csv` --> `personalinfo`, `localqueueX`

&emsp; &emsp; For registration, we use 4 csv files `localX.csv` to simulate loading data of the registered patients. For each half day, we (1) load the registered patients to `localqueue` and (2) upload the data to `centerqueue`. 

2. **UPLOAD** (Queue): `personalinfo`, `localqueueX` --> `centernode`, `centerqueue`

3. **DOWNLOAD** (Appoint): `centernode`, `centerqueue` --> `hospital`

&emsp; &emsp; We implement a **fibonacci heap** to extract the patient with the highest priority and assign him/her an appointment. The formula of the bit vector `priority` is defined below. 

	priority = 10^11 * profession     + 10^10 * age_group
	         + 10^8  * register_month + 10^6  * register_day
			 + 10^4  * register_hour  + 10^2  * register_minute + 10 * register_second

4. **OUTPUT** (Report): `hospital` --> `report`

## **Assumptions**

1. The medical system is simulated only for **1 month**. 

2. There are **3 hospitals** with respective capacities of **5 patients** per day. 

3. The patient data in `local1.csv`, `local2.csv`, `local3.csv`, and `local4.csv` are randomly generated, but on average, there will be **18 people** to register in total. 

4. The system supports **interactive interface**. You can input the patient data directly through keyboard, but there are some format restrictions. These patients' data will be stored in `local4.csv`. 

5. A patient's information can only be seen after **at least half a day** has passed. 

6. `weekly_report.csv` will be **overwritten** upon every attempt of generation. 

## **Appendix I: File Organization**

**`README.pdf`** - This document

**`source_code/main.cpp`** - `zerotime(time)`, `main()`

**`source_code/local_registration.cpp`** - `personalinfo::local_register(...)`, `local_store()`

**`source_code/load_data.cpp`** - `find_person(id)`, `dataloading(...)`

**`source_code/controlflow.cpp`** - `skip_half_day(...)`, `checktime()`, `hospital::*()`, `report::*()`

**`source_code/fibonacci_heap.cpp`** - `centerqueue::*()`, `centernode::*()`

**`source_code/medical_system.h`** - Header file of definitions

**`medical_system.sh`** - Compilation and execution script

**`localX.csv`** (**`X`** = `1`, `2`, `3`, `4`) - Simulated patient data

**`fibonacci_heap_demo`** - Demo program of fibonacci heap functionality

**`sample_reports/`** - Two sample reports (weekly & monthly)

## **Appendix II: Data Structures**

**`personalinfo` - Basic information recording unit for a patient**

&emsp; &emsp; `id` - Unique identification number

&emsp; &emsp; `appointment` - Treatment stage indicator (`0` = waiting, `1` = being treated)

&emsp; &emsp; `priority` - Treatment priority (calculated)

&emsp; &emsp; `registration` {`registration_time`, `location_id`, `hospital`}, 

&emsp; &emsp; &emsp; `medical_risk`, `profession`, `age_group` - Info for calculating `priority`

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

&emsp; &emsp; `decrease_key(id, priority)` - Modify the priority of a `centernode`

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

&emsp; &emsp; `with_drew_people` - Counter of withdrawn patients

&emsp; &emsp; `sum_waited_time` - Total waiting time

&emsp; &emsp; `treated` - Vector of treated patients

&emsp; &emsp; `appointment` - Vector of patients with appointment

&emsp; &emsp; `not_treated` - Vector of patients without appointment

&emsp; &emsp; `store_data()` - Collect patients not yet treated

&emsp; &emsp; `generalize_report(filename)` - Generate a report (monthly or weekly)

&emsp; &emsp; `clear_report()` - Clear all the data stored in the report

&emsp; &emsp; `sort_nodes_name()` - Sort the patients in terms of name

&emsp; &emsp; `sort_nodes_agegroup()` - Sort the patients in terms of age_group

&emsp; &emsp; `sort_nodes_profession()` - Sort the patients in terms of profession

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

&emsp; &emsp; `Fheap` - Global fibonacci heap (`centerqueue`)

&emsp; &emsp; `hos[3]` - Records of the three `hospital`s

&emsp; &emsp; `week_report`, `mon_report` - Two `report`s of different scopes

&emsp; &emsp; `num_localqueues`, `num_appointments`, `num_week_report`, `num_mon_report` - Counters
