This repository contains the source codes for the paper.

## Language Used for code
1.  C-language
2.  Python

## Setup
1.  To install gcc, use the following command:
   	* `sudo apt update`
	* `sudo apt install build-essential`
3.  To install python kernel, use the following command:
   	* `sudo apt update`
	* `sudo apt install python3`
	    
## File Structure
1.  `coverage_one_round.py`and `coverage_two_round.py`: Script for finding minimal number of fault location for Koala-P.
2.  `2nd_round_fault.c`: Code for fault location identification when the fault is injected in the penultimate round of the Koala-P permutation
3.  `fault_Koala_per.c`: Code for full state (before chi function in Koala-P permuatation) recovery when faults are injected at 52 locations independently in the last round of the Koala-P permutation.
4.  `fault_last_round.c`: Code for injecting fault randomly in the last round of permutation followed with identification of the fault location and thereafter full state (before chi function in Koala-P permutation) recovery.
5.  `K_per.c`: Implementation of Koala-P permutation
6. `verify_state_dependency.c`: For 100 states, faults are injected randomly in the penultimate round and the state before non-linear layer (chi layer) of Koala-P permuatation is recovered. For each state, 1000 times the faults are injected in random and we observe the number of faults required to recover the state  every time (out of the 1000 times). Then we take an average of how many faults are needed for each state. Finally, we do a GRAND Average of the average no. of fauts reqired to recover each state.


## Usage
## For file_name.py file:
1.  compile and run the programme  :`python3 file_name.py`

## For file_name.c file:
1.  compile the programme	:`gcc file_name.c`
2.  run the file			:`./a.out`

