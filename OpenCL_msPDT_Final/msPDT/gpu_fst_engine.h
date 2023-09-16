//
// Created by theo on 7/22/22.
//

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-narrowing"
#ifndef OPENCL_MSPDT_VARIADIC_GPU_FST_ENGINE_H
#define OPENCL_MSPDT_VARIADIC_GPU_FST_ENGINE_H

#endif //OPENCL_MSPDT_VARIADIC_GPU_FST_ENGINE_H

#include "processor.h"
#include "shared.h"
#define OUTPUT_PER_THREAD 1000
#define MIN_LENGTH 10
#define DEBUG_GPU 1
#undef DEBUG_GPU

#define SAME_START 1
#undef SAME_START

#define TIMING 1
//might need to change these, were intitialized with __constant__ first
//NPDT stateList[STATE_COUNT];
//Transition  transitionList[TRANSITION_COUNT];

#include "../CLfuncs.h"

class FSTGPU {
public:
// IO stream

    cl_ulong inputCount;
    cl_uint **input;
    cl_ulong *input_length;
    cl_ulong outputCount;
    cl_uint *output;
    cl_ulong *output_length;

// IO partition Information
    cl_ulong number_of_block;
    cl_ulong number_of_thread;
    cl_ulong total_threads;

    // temp varaible to simulate writing
    cl_ulong **simulated_output;

    // how many symbol each thread have to process
    cl_ulong **partition_input_length;
    cl_ulong **partition_input_length_cpu;
    // where each thread process from
    cl_ulong **partition_input_base;
    cl_ulong **partition_input_base_cpu;
    // how many symbol each thread have processed
    cl_ulong **partition_input_current;
    cl_ulong **partition_input_current_cpu;
    // where each thread write to
    cl_ulong **partition_output_base;
    cl_ulong **partition_output_base_cpu;
    // how many symbol have each thread written
    cl_ulong **partition_output_current;
    cl_ulong **partition_output_current_cpu;

    //members for holding FST data pointers
    cl_ulong * var;
    NPDT * stateList;
    Transition * transitionList;

    /* State Action data  */
    cl_bool * pendingTransition;

    /* Transition Match data  */
    cl_bool * confirmedTransition;

    /* OutputUpdate data  */
    cl_bool * activatedState;

    // core function
    void IO_setup(TP *cpu_transducer, unsigned long blocks, unsigned long threads);

    void IO_partition(TP *cpu_transducer, int test);
    //void process(TPGPU * transducer);

    void copyBack(TP *transducer);

    // parition scheme
    void partition(TP *cpu_transducer, int test);

    void RLE_ENCODING(TP *cpu_transducer);

    void RLE_DECODING(TP *cpu_transducer);

    void BP_ENCODING(TP *cpu_transducer);

    void COOCSR(TP *cpu_transducer);

    // helper function
    void save_thread_config(TP *cpu_transducer, std::string destination);

    void printFST(TPGPU *transducer, bool state, bool transition, bool variable);

    void action_convert(u_int16_t, u_int16_t src1, u_int16_t src2, u_int16_t dst);

    void transition_convert(u_int16_t alphabet);

    void process(TPGPU *transducer, TP *processor);

    ~FSTGPU(){
        for ( uint32_t i = 0 ; i < total_threads ; i++){

            free(partition_input_length[i]);
            free(partition_input_base[i]);
            free(partition_input_current[i]);
            free(partition_output_base[i]);
            free(partition_output_current[i]);
            free(simulated_output[i]);

            free(partition_input_length_cpu[i]);
            free(partition_input_base_cpu[i]);
            free(partition_input_current_cpu[i]);
            free(partition_output_base_cpu[i]);
            free(partition_output_current_cpu[i]);
        }

        for(uint32_t i = 0; i < inputCount; i++){
            free(input[i]);
        }


        free(input);
        free(input_length);
        free(output);
        free(output_length);
        free(simulated_output);
        free(partition_input_length);
        free(partition_input_base);
        free(partition_input_current);
        free(partition_output_base);
        free(partition_output_current);
        free(partition_input_length_cpu);
        free(partition_input_base_cpu);
        free(partition_input_current_cpu);
        free(partition_output_base_cpu);
        free(partition_output_current_cpu);

    }
};
