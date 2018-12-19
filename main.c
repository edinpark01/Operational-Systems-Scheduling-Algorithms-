/*
    Author:     Braulio Tonaco
    Date:       09/30/18
    Version:    1.0

    CS 332 PROJECT - PROGRAMMING SCHEDULING ALGORITHMS IN C

        The skeleton code will define prototypes for five functions, each pertaining to a different scheduling
    algorithm.
        A double pointer (pointer to an array) of process structs will be pre-populated and provided for you to use
    as parameter to each function.
        After running the function, the process should be output in order expected by the given algorithm.
        The only algorithm below not in our notes is the priority-based scheduling algorithm, which will take in a
    second associated array of priorities for the processes, and sort the processes by these priorities before
    outputting them.
        The algorithm will be non-preemptive.

        You will also be given the definition of a struct (similar to a class) representing a single process:

        struct process{
            int id;
            int duration;
            int priority;
        }

        All code changes should be completed withing your main.c file.
        Sched_algos.h: only for your reference, and should remain untouched.

    GROUND RULES:
        *   No using any non-standard libraries that do any of these functions for you.
        *   All #include statements that you will need are already in main.c.
        *   You will have to write a sort at some point in this project.

            *   Please write your own, and only use one of the three following sorts:
                *   merge sort
                *   quick sort
                *   heap sort

    MORE NOTES:
        This one is intended to be a slightly modified version of First-Come-First-Served, and is one of the easiest
    algorithms on the project, since I am asking for a non-preemptive version.

        1.  Given an array of priorities (same length as the processes array),
        2.  Sort the processes by their priority, and output them.
            a.  Element i of the priorities list will be the priority of element i of the processes list.
        3.  The process with the largest priority would become the 0th element to be output by this function.

        For simplicity, I will allow you to make one change  to the process struct in the header file:

        1.  you may add a priority field (int), which you can then use to complete this sort.
        2.  If you can think of a way to complete this without that additional field, that is also acceptable.

        Example:

        Processes:  A, B, C, D, E, F
        Priorities: 5, 1, 9, 2, 7, 2

        Acceptable Output Orders: B,D,F,A,E,C  --OR--  B,F,D,A,E,C

        1.  In algorithms other than FCFS, you will be required to reorder the elements in the arra ybefore outputting them,
            to represent the correct execution order.
        2.  You do NOT need to worry about the arrival time of the processes for this project.
            a.  You may assume that all the processes have already arrived, and they are queued in the
                order of the array.
        3.  The processes are still executing in the queue order, but only for **timeslice** amount of time.
            a.  Once that time is up, the next process executes,
            b.  When you reach the end of the array, you return to the beginning.
            c.  When a process has been executed for its full duration, it is removed from the queue.

    *************** ADDITIONAL CHANGES TO THE PROJECT ***************

    The change will go as follows:
    As you loop through the input array, once two of the queued process have been completed, I want you to
overwrite those two with fresh randomly generated processes. Upon adding these new processes, you will need to
re-sort your input array appropriately.
    For SPN, you may add the new processes immediately after the two processes are completed.
    For SRT, since it is pre-emptive, please execute another time slice on a single process before adding the two
new processes.
    After completing this change, since the processes are randomly generated, you should see the appropriate behavior
from each algorithm after a few runs. If you need to better understand the difference between the two algorithms,
please refer to your notes, or look these algorithms up online.
    This change only effects SRT & SPN. Since you should be creating a copy of the input array for each algorithm
before sorting it, the new processes should only exist in this temporary copy, and therefore should not exist in the
other algorithms.
    If this is at all confusing or you need clarity on what I mean, please post any questions related to this change
here.

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sched_algos.h"


/** PROVIDED FUNCTIONS **/
void fillProcessArr(struct process ** processArr, int arrSize);             // Fills array w/ random processes
void outputProcess(struct process *process);                                // Outputs a single process
void outputProcesses(struct process ** processes, int arrSize);             // Outputs all processes


/** DEVELOPED FUNCTIONS & TYPES **/
enum Criteria{ PRIORITY, DURATION };
void merge(struct process **arr, int l, int m, int r, enum Criteria T);     // Merge Sorting Algorithm
void mergeSorting(struct process ** arr, int l, int r, enum Criteria T);    // Merge Sorting Algorithm Function
void arrayCleanUp(struct process **arr, int size);                          // Performs heap clean up
int getNextActiveProcess(int next, int size, int *state);                   // Retrieves index of next Active Process
struct process **copyArray(struct process ** main, int arrSize);            // Copies the main process array


/** Tests and outputs a few different scheduling algorithms **/
int main(int argc, char ** argv) {
    srand((unsigned int) time(NULL));           // Seed our random with the current time.
    const int ARR_SIZE = 10;                    // The size of our array of randomly generated processes.
    const int TIME_SLICE = 2;                   // The time slice for any preemptive scheduling algorithms.

    // Process array used in each of scheduling algorithms
    struct process ** processes = malloc(sizeof(struct process *) * ARR_SIZE);

    // Fills Processes Array
    fillProcessArr(processes, ARR_SIZE);

    printf("Outputting initial process array:\n");
    outputProcesses(processes, ARR_SIZE);
    printf("\n\n");

    //sched_fcfs(processes, ARR_SIZE);                    // First Come First Serve Scheduling Algorithm
    //sched_priority(processes, ARR_SIZE, NULL);          // NonPreempitive Priority Scheduling Algorithm
    //sched_spn(processes, ARR_SIZE);                     // NonPreempitive Shortest Process Next Scheduling Algorithm
    sched_srt(processes, ARR_SIZE, TIME_SLICE);         // Preempitive Shortest Remaining Time Scheduling Algorithm
    //sched_round_robin(processes, ARR_SIZE, TIME_SLICE); // Round Robin Scheduling Algorithm

    // Deallocate all objects on the original array, including the space allocated for the array itself.
    arrayCleanUp(processes, ARR_SIZE);
    free(processes);
}

void sched_fcfs(struct process ** processes, int arrSize) {
    /**
     * Queue order of processes is represented by their order in the array
     * FCFS in this case would mean just outputting the array.
     **/
    printf("First Come First Serve:\n");

    outputProcesses(processes, arrSize);

    printf("\n\n");
}

void sched_priority(struct process ** processes, int arrSize, int ** priorities) {
    /** In Priority scheduling processes are sorted/executed in ascending order based on priority **/

    printf("Priority:\n");

    // Creates a copy of main process array use
    struct process **copy = copyArray(processes, arrSize);

    // Sorts copy array based on priority
    mergeSorting(copy, 0, arrSize - 1, PRIORITY);

    // Output sorted array
    outputProcesses(copy, arrSize);

    // Heap Clean Up
    arrayCleanUp(copy, arrSize);
    free(copy);

    printf("\n\n");
}

void sched_spn(struct process ** processes, int arrSize) {
    /** In Shortest Process Next, processes are sorted/executed in ascending order based on CPU burst **/

    printf("Shortest Process Next:\n");

    // Creates a copy of main process array
    struct process **copy = copyArray(processes, arrSize);

    // Sorts copy array based on CPU bursts
    mergeSorting(copy, 0, arrSize - 1, DURATION);

    outputProcess(copy[0]); // "Execute" first process
    outputProcess(copy[1]); // "Execute" second process

    printf("\nFinished first two queued processes,\nAdding two new processes and resorting queued...\n");

    // Add two new fresh processes
    copy[0] -> duration = rand() % 5 + 1;
    copy[0] -> priority = rand() % arrSize + 1;
    copy[1] -> duration = rand() % 5 + 1;
    copy[1] -> priority = rand() % arrSize + 1;

    printf("\n");

    // Sorts copy array based on CPU bursts
    mergeSorting(copy, 0, arrSize - 1, DURATION);

    // Output sorted array
    outputProcesses(copy, arrSize);

    // Heap Clean Up
    arrayCleanUp(copy, arrSize);
    free(copy);

    printf("\n\n");
}

void sched_srt(struct process ** processes, int arrSize, int timeSlice) {
    /**
     * SRT keeps track if a process is arriving and its CPU burst is less than current process remaining time
     * For this project, we are assuming that all processes arrived at the same time, so always execute the
     * smallest unfinished process AFTER sorting the processes array
     **/

    printf("Shortest Remaining Time:\n");
    int finished = 0;
    int addNewProcesses = 0;
    int ticker;
    int next = 0;
    int *processState = calloc((size_t) arrSize, sizeof(int));

    // Sets all states as True (Not finished)
    for(int i = 0; i < arrSize; i++){ processState[i] = 1; }

    // Creates a copy of main process array
    struct process **copy = copyArray(processes, arrSize);

    // Sorts copy array based on CPU bursts
    while ( next < arrSize ){
        mergeSorting(copy, 0, arrSize - 1, DURATION);   // sorts copy

        struct process *current = copy[next];           // Gets next smallest unfinished process
        ticker = 0;                                     // Resets ticker

        outputProcess(current);                         // Prints current process

        // Executes process until its time run out or it is finished ( whichever comes first )
        while ( ticker < timeSlice && current->duration > 0 ){
            ticker++;
            current->duration--;
            //outputProcess(current);
        }

        // Add new Processes checker
        if ( finished == 2 ){ addNewProcesses = 1; }

        if ( addNewProcesses ){

            printf("\nFinished first two queued processes,\nAdding two new processes and resorting queued...\n");

            // Add two new fresh processes
            copy[0] -> duration = rand() % 5 + 1;
            copy[0] -> priority = rand() % arrSize + 1;
            copy[1] -> duration = rand() % 5 + 1;
            copy[1] -> priority = rand() % arrSize + 1;

            next = 0;
            addNewProcesses = 0;
            finished += 1;
            printf("\n");
        }

        // if process is finished
        if ( current->duration == 0 ) {
            // Increment next
            next++;
            finished++;
        }
    }

    // Heap Clean Up
    arrayCleanUp(copy, arrSize);
    free(processState);
    free(copy);

    printf("\n\n");
}

void sched_round_robin(struct process ** processes, int arrSize, int timeSlice) {
    /** RR is a preemptive scheduler, which is designed especially for time-sharing systems.
     *  In other words, it does not wait for a process to finish or give up control.
     *  In RR, each process is given a time slot to run.
     *  If the process does not finish, it will “get back in line” and receive another time slot until it has completed.
     *  We will implement RR using a FIFO queue where new jobs are inserted at the tail end. **/

    printf("Round Robin:\n");

    int ticker;
    int next = 0;
    int *processState = calloc((size_t) arrSize, sizeof(int));

    // Sets all states as True (Not finished)
    for(int i = 0; i < arrSize; i++){ processState[i] = 1; }

    // Creates a copy of main process array
    struct process **copy = copyArray(processes, arrSize);

    // Sorts copy array based on CPU bursts
    while ( next != -1 ){
        struct process *current = copy[next];   // "dequeue"
        ticker = 0;                             // Resets ticker

        outputProcess(current);                 // Prints current process

        // Executes process until its time run out or it is finished ( whichever comes first )
        while ( ticker < timeSlice && current->duration > 0 ){
            ticker++;
            current->duration--;
        }

        // if process is finished
        if ( current->duration == 0 ){ processState[next] = 0; }  // Sets process state as False ( Finished

        next = getNextActiveProcess(next, arrSize, processState);
    }

    // Heap Clean Up
    arrayCleanUp(copy, arrSize);
    free(processState);
    free(copy);

    printf("\n\n");
}

int getNextActiveProcess(int next, int size, int *state) {
    while ( ++next < size && !state[next] );

    if ( next == size ){
        next = -1;
        while ( ++next < size && !state[next] );
    }
    return (next != size) ? next : -1;
}

void fillProcessArr(struct process ** processArr, int arrSize) {
    int idx = 0;

    while( idx < arrSize ) {
        processArr[idx] = malloc(sizeof(struct process));
        processArr[idx] -> id = idx + 1;
        processArr[idx] -> duration = rand() % 5 + 1;
        processArr[idx] -> priority = rand() % arrSize + 1;
        idx++;
    }
}

void outputProcess(struct process *process) {
    printf("Process ID: %i\t\tDuration: %i\t\tPriority: %i\n", process -> id, process -> duration, process -> priority);
}

void outputProcesses(struct process ** processes, int arrSize) {
    int idx = 0;

    while( idx < arrSize ) {
        outputProcess(processes[idx]);
        idx++;
    }
}

void merge(struct process **arr, int l, int m, int r, enum Criteria c){
    int i, j, k;

    // STEP 1: Calculate each half size
    int leftSize = m - l + 1;
    int rightSize = r - m;

    // STEP 2: Allocate memory for each half
    struct process **lefty  = malloc(sizeof(struct process) * leftSize);
    struct process **righty = malloc(sizeof(struct process) * rightSize);

    //  STEP 3: Copy each half to temp arrays so we may sort them
    for(i = 0; i < leftSize; i++){ lefty[i] = arr[l + i]; }
    for(j = 0; j < rightSize; j++){ righty[j] = arr[m + 1 + j]; }


    i = 0;    // left halft first element's index
    j = 0;    // right half first element's index
    k = l; // sub-array first element's index in relation to main-array

    //  STEP 4: Compare left half priority against right half priority until we reach the end of one of them

    switch ( c ){
        case PRIORITY:
            while ( i < leftSize && j < rightSize ){
                if ( lefty[i]->priority <= righty[j]->priority ){
                    arr[k] = lefty[i];
                    i++;
                }
                else{
                    arr[k] = righty[j];
                    j++;
                }
                k++;
            }
            break;
        case DURATION:
            while ( i < leftSize && j < rightSize ){
                if ( lefty[i]->duration <= righty[j]->duration ){
                    arr[k] = lefty[i];
                    i++;
                }
                else{
                    arr[k] = righty[j];
                    j++;
                }
                k++;
            }
            break;
        default:
            printf("\nIncorrect Criteria...\n");
            break;
    }

    //  STEP 5: Finish copying remaining elements from the one half that we have yet to finish reading
    while ( i < leftSize ){
        arr[k] = lefty[i];
        i++;
        k++;
    }
    while ( j < rightSize ) {
        arr[k] = righty[j];
        j++;
        k++;
    }

    //  STEP 6: Free each half pointer that we allocated above from the heap
    free(lefty);
    free(righty);
}

void mergeSorting(struct process ** arr, int l, int r, enum Criteria c){
    if ( l < r ){
        // STEP 1: Divide input array into two halves
        int middle = (l + r) / 2;

        // STEP 2: Call itself for the left half
        mergeSorting(arr, l, middle, c);

        // STEP 3: Call itself for the right half
        mergeSorting(arr, middle + 1, r, c);

        // STEP 4: Merge the two sorted halves
        merge(arr, l, middle, r, c);
    }
}

void arrayCleanUp(struct process **arr, int size){
    for(int i = 0; i < size; i++){
        free(arr[i]);
    }
}

struct process **copyArray(struct process ** main, int arrSize) {
    struct process **copy = malloc(sizeof(struct process *) * arrSize);
    int idx = 0;

    while( idx < arrSize ) {
        copy[idx] = malloc(sizeof(struct process));
        copy[idx] -> id = main[idx] -> id;
        copy[idx] -> duration = main[idx] -> duration;
        copy[idx] -> priority = main[idx] -> priority;
        idx++;
    }

    return copy;
}
