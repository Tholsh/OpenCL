//This OpenCL Kernel accepts a Multi Stack Psuhdown Transducer and associated data, 
//Partitions evenly amoung kernels, and returns the result.
#include "Kernel/OpenCLheader.h"
#define DEBUG_GPU 1
//#undef DEBUG_GPU



__kernel void OpenCL_msPDT(__global TPGPU * transducer,
__global unsigned long * partition_input_length,   __global unsigned long * partition_input_base,
        __global unsigned long * partition_input_current,  __global unsigned long * partition_output_base_1d,
        __global unsigned long * partition_output_current, __global unsigned long* simulated_output_1d,
        __global unsigned int * output1d,                 __global unsigned long * output_length,
        __global unsigned int * input1d,                  __global unsigned long* input_length,
        __global info_t* bulletin,                         __global unsigned long* var,
        __global NPDT * stateList,                         __global Transition * transitionList,
        __global bool * pendingTransition,                 __global bool * confirmedTransition,
        __global bool * activatedState){
//we are putting work in one single kernel for debug, will distribute later
//printf("Hi I'm kernel #%u\n", get_local_id(0));

unsigned long inputCount = transducer->inputCount;
unsigned long outputCount = transducer->outputCount;
unsigned long total_thread = get_global_size(0);
unsigned long globalId = get_global_id(0);

//make the 2d array able to be accessed by kernel
twoD_array                         input = {input1d, input_length, inputCount};
twoD_array                        output = {output1d, output_length, outputCount};
//making 2d thread arrays accessible by kernel
twoD_thread_array       partition_length = {partition_input_length, total_thread, inputCount};
twoD_thread_array             input_base = {partition_input_base, total_thread, inputCount};
twoD_thread_array       processed_symbol = {partition_input_current, total_thread, inputCount};
twoD_thread_array  partition_output_base = {partition_output_base_1d, total_thread, outputCount};
twoD_thread_array partition_output_count = {partition_output_current, total_thread, outputCount};
twoD_thread_array       simulated_output = {simulated_output_1d, total_thread, outputCount};

transducer->var = var;
transducer->stateList = stateList;
transducer->transitionList = transitionList;
transducer->pendingTransition = pendingTransition;
transducer->confirmedTransition = confirmedTransition;
transducer->activatedState = activatedState;

uint16 test = 0;

    if(get_global_id(0) == 0) {
        printf("\n%u; \n", in_place_S(0, 1, input));
        printf("%u; \n", in_place(0, 0, partition_input_length, inputCount, total_thread));
        printf("global_size = %u\n", get_global_size(0));
        printf("local_size = %u\n", get_local_size(0));
        printf("input size = %lu\n", transducer->inputCount);
        printf("output size = %lu\n", transducer->outputCount);
printf("number of transition = %u , baseID = %d, \n", stateList[0].numberofTransition, stateList[0].baseID);
        strcpy(bulletin[1].str, bulletin[0].str);
        bulletin[3].str[0] = 'W';
    }



process_k(transducer, partition_length, input_base,
        processed_symbol, partition_output_base,
        partition_output_count, simulated_output,
        output,         input,
        globalId, get_global_size(0));

//Debug info printout
/*
    //printf("Hi I'm kernel #%u, local id #%u\n", get_global_id(0), get_local_id(0));
    unsigned long total_thread = get_global_size(0);
    //unsigned long outputCount = transducer->outputCount;
    unsigned long inputCount = transducer->inputCount;
    unsigned long globalId = get_global_id(0);
    unsigned long outputCount = 1;



        //for debug. this does slow down kernel #0
        if(get_global_id(0) == 0){
            strcpy(bulletin[1].str, bulletin[0].str);
            bulletin[3].str[0] = 'W';
            //emplace_S(1000, 0, 0, output);

        }

    //in_place_thread(0, 0, partition_input_length, inputCount);
    //emplace_thread(1000, 0, 0, simulated_output, inputCount);

    //set up transducer with correct data

    transducer->var = var;
    transducer->stateList = stateList;
    transducer->transitionList = transitionList;
    transducer->pendingTransition = pendingTransition;
    transducer->confirmedTransition = confirmedTransition;
    transducer->activatedState = activatedState;

    //main processing function

*/


}


