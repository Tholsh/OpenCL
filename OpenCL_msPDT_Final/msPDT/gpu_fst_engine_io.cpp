//
// Created by theo on 7/26/22.
//
#include "gpu_fst_engine.h"


void printTape_D( unsigned long ** tape, unsigned long * size, unsigned long numberoftapes){
    for ( int i = 0; i < numberoftapes; i++){
        printf("Tape: %d : element: %u:\n", i, (unsigned) size[i]);
        for( int j = 0; j < size[i]; j++){
            printf("%u; ", (unsigned) tape[i][j]);
        }
        printf("\n");
    }
}

void printTape( unsigned long ** tape, unsigned long * size, unsigned long numberoftapes){
    for ( int i = 0; i < numberoftapes; i++){
        printf("Tape: %d : element: %u:\n", i, (unsigned) size[i]);
        for( int j = 0; j < size[i]; j++){
            printf("%d; ", (int) tape[i][j]);
        }
        printf("\n");
    }
}
void FSTGPU::IO_setup( TP* cpu_transducer, unsigned long blocks, unsigned long threads){
    printf (" =====================================================\n");
    printf(" Start setting up input, output\n");
    number_of_thread = threads;
    number_of_block = blocks;
    total_threads = number_of_thread * number_of_block;
    printf("%d Input\n", cpu_transducer->inputCount);
    printf("%d Output\n", cpu_transducer->outputCount);
    inputCount = cpu_transducer->inputCount;
    outputCount = cpu_transducer->outputCount;
    printf("CPU INPUT\n");
    //printTape (&(cpu_transducer->inStream[0]), &(cpu_transducer->input_length[0]), inputCount);

    // copy input length array
    input_length = (cl_ulong*) malloc(inputCount * sizeof (cl_ulong));
    for ( uint32_t i = 0; i < cpu_transducer->inputCount; i++){
        input_length[i] = cpu_transducer->input_length[i];
    }

    // copy output length array and determine sizes
    uint32_t out_elements = 0;
    output_length = (cl_ulong*) malloc(outputCount * sizeof (cl_ulong));
    for ( uint32_t i = 0; i < cpu_transducer->outputCount; i++){
        output_length[i] = cpu_transducer->output_length[i];
        out_elements += cpu_transducer->output_length[i];
    }

    input = (cl_uint**) malloc(inputCount * sizeof(cl_uint*));
    for (uint32_t i = 0; i < cpu_transducer->inputCount; i++){
        input[i] = (cl_uint*) malloc(cpu_transducer->input_length[i] * sizeof(cl_uint));
        for(uint32_t j = 0; j < cpu_transducer->input_length[i]; j++)
            input[i][j] = static_cast<cl_uint>(cpu_transducer->inStream[i][j]);       //element by element copy
    }

    //DEBUG
    printf("PRINTING FIRST AND LAST 10 INPUTS FROM INPUT\n ");
    for ( uint32_t i = 0; i < inputCount; i++){
        printf("input length = %u\n \n", unsigned (cpu_transducer->input_length[i]));
        for(uint j = 0; j < 10; j++) printf("%u; ", input[i][j]); printf("\n");
        for(int j = 10; j > 0; j--) printf("%u; ", input[i][cpu_transducer->input_length[i] - j]); printf("\n \n");
    }

    output = (cl_uint *) malloc(out_elements * sizeof(cl_uint)); //allocate enough space for the output using accumulator function
    //printf("GPU INPUT\n");
    //printTape_D<<< 1,1>>> (input, input_length, inputCount);
    ///cudaDeviceSynchronize();
    // allocate Output
#ifdef DEBUG_GPU
    printf(" ALLOCATE OUTPUT FOR DEBUG\n");
  uint32_t ** temp_output = (uint32_t**) malloc ( outputCount *sizeof( uint32_t*));
  // allocate each input and copy them to device
  for ( uint32_t i = 0; i < outputCount; i++){
    //cudaMalloc ( &(temp_output[i]), OUTPUT_LENGTH * sizeof(uint32_t));
    cudaMalloc ( &(temp_output[i]), cpu_transducer->output_length[i] * sizeof(uint32_t));
  }
  // copy input pointer to device
  cudaMalloc( & (output), outputCount* sizeof( uint32_t*));
  cudaMemcpy ((output), (temp_output), outputCount * sizeof( uint32_t*), cudaMemcpyHostToDevice);
#endif
    printf (" =====================================================\n");
}


void FSTGPU::IO_partition(  TP * cpu_transducer, int test){

    //allocate for FST
    var = (cl_ulong*) malloc(cpu_transducer->varCount * sizeof(cl_ulong));
    stateList = (NPDT*) malloc(cpu_transducer->stateCount * sizeof(NPDT));
    transitionList = (Transition*) malloc(cpu_transducer->transitionCount * sizeof(Transition));
    pendingTransition = (cl_bool*) malloc(cpu_transducer->transitionCount * sizeof(cl_bool));
    confirmedTransition = (cl_bool*) malloc(cpu_transducer->transitionCount * sizeof(cl_bool));
    activatedState = (cl_bool*) malloc(cpu_transducer->stateCount * sizeof(cl_bool));

    //copy element by element and type convert existing data
    for(int i = 0; i < cpu_transducer->varCount; i++)           var[i] = static_cast<cl_ulong>(cpu_transducer->var[i]);

    for(int i = 0; i < cpu_transducer->transitionCount; i++)    pendingTransition[i] = static_cast<cl_bool>(cpu_transducer->pendingTransition[i]);   //
    for(int i = 0; i < cpu_transducer->transitionCount; i++)    confirmedTransition[i] = static_cast<cl_bool>(cpu_transducer->confirmedTransition[i]);
    for(int i = 0; i < cpu_transducer->stateCount; i++)         activatedState[i] = static_cast<cl_bool>(cpu_transducer->activatedState[i]);

    //special attention to type conversion
    for(int i = 0; i < cpu_transducer->stateCount; i++){
        //stateList[i] = stateList[i] for NPDT element by element
        stateList[i].id = static_cast<cl_ulong>(cpu_transducer->stateList[i].id);
        stateList[i].baseID = static_cast<cl_ulong>(cpu_transducer->stateList[i].baseID);
        stateList[i].numberofTransition = static_cast<cl_ulong>(cpu_transducer->stateList[i].numberofTransition);
        stateList[i].src1 = static_cast<cl_ulong>(cpu_transducer->stateList[i].src1);
        stateList[i].src2 = static_cast<cl_ulong>(cpu_transducer->stateList[i].src2);
        stateList[i].dst = static_cast<cl_ulong>(cpu_transducer->stateList[i].dst);
        stateList[i].opt = static_cast<cl_ulong>(cpu_transducer->stateList[i].opt);
        stateList[i].accepting = static_cast<cl_ulong>(cpu_transducer->stateList[i].accepting);
        stateList[i].initial = static_cast<cl_ulong>(cpu_transducer->stateList[i].initial);
        stateList[i].epsilonState = static_cast<cl_ulong>(cpu_transducer->stateList[i].epsilonState);
    }
    for(int i = 0; i < cpu_transducer->transitionCount; i++) {
        //transitionList[i] = transitionList[i]; //Transition
        transitionList[i].id = static_cast<cl_ulong>(cpu_transducer->transitionList[i].id);
        transitionList[i].inputID = static_cast<cl_ulong>(cpu_transducer->transitionList[i].inputID);
        transitionList[i].inputSymbol = static_cast<cl_ulong>(cpu_transducer->transitionList[i].inputSymbol);
        transitionList[i].IvarID = static_cast<cl_ulong>(cpu_transducer->transitionList[i].IvarID );
        transitionList[i].inputVar = static_cast<cl_ulong>(cpu_transducer->transitionList[i].inputVar);
        transitionList[i].currentState = static_cast<cl_ulong>(cpu_transducer->transitionList[i].currentState);
        transitionList[i].outputID = static_cast<cl_ulong>(cpu_transducer->transitionList[i].outputID );
        transitionList[i].outputSymbol = static_cast<cl_ulong>(cpu_transducer->transitionList[i].outputSymbol );
        transitionList[i].OvarID = static_cast<cl_ulong>(cpu_transducer->transitionList[i].OvarID);
        transitionList[i].outputVar = static_cast<cl_ulong>(cpu_transducer->transitionList[i].outputVar);
        transitionList[i].nextState = static_cast<cl_ulong>(cpu_transducer->transitionList[i].nextState);
    }

    printf (" =====================================================\n");
    printf(" START PARTITIONING IO\n");
// initialize partition data on CPU
    printf(" Allocating cpu for %d threads, %d input, %d output\n", total_threads, inputCount, outputCount);
    partition_input_length_cpu  = (cl_ulong **) malloc ( total_threads * sizeof( cl_ulong *));
    partition_input_base_cpu  = (cl_ulong **) malloc ( total_threads * sizeof( cl_ulong *));
    partition_input_current_cpu  = (cl_ulong **) malloc ( total_threads * sizeof( cl_ulong *));
    partition_output_base_cpu  = (cl_ulong **) malloc ( total_threads * sizeof( cl_ulong *));
    partition_output_current_cpu  = (cl_ulong **) malloc ( total_threads * sizeof( cl_ulong *));
    for ( int i = 0; i < total_threads; i++){
        partition_input_length_cpu[i] = ( cl_ulong *) malloc ( inputCount * sizeof(cl_ulong));
        partition_input_base_cpu[i] = ( cl_ulong *) malloc ( inputCount  * sizeof(cl_ulong));
        partition_input_current_cpu[i] = ( cl_ulong *) malloc ( inputCount  * sizeof(cl_ulong));
        partition_output_base_cpu[i] = ( cl_ulong *) malloc ( outputCount  * sizeof(cl_ulong));
        partition_output_current_cpu[i] = ( cl_ulong *) malloc ( outputCount  * sizeof(cl_ulong));
    }
// parition according to the benchmark
    //testing functionality
    printf(" Initializing cpu\n");
    partition(cpu_transducer,  test);

    // allocate and copy partition data to GPU

    printf(" Allocating %d gpu  pointer \n", inputCount);
    uint32_t ** partition_input_length_gpu  = (uint32_t **) malloc ( total_threads * sizeof( uint32_t *));
    uint32_t ** partition_input_base_gpu  = (uint32_t **) malloc ( total_threads * sizeof( uint32_t *));
    uint32_t ** partition_input_current_gpu  = (uint32_t **) malloc ( total_threads * sizeof( uint32_t *));
    uint32_t ** partition_output_base_gpu  = (uint32_t **) malloc ( total_threads * sizeof( uint32_t *));
    uint32_t ** partition_output_current_gpu  = (uint32_t **) malloc ( total_threads * sizeof( uint32_t *));
    uint32_t ** simulated_output_gpu  = (uint32_t **) malloc ( total_threads * sizeof( uint32_t *));



    printf(" Allocating gpu  \n");
    for ( uint32_t i = 0 ; i < total_threads ; i++){
        partition_input_length_gpu[i] = (uint32_t*) malloc (inputCount * sizeof(uint32_t));
        memcpy (partition_input_length_gpu[i], partition_input_length_cpu[i], inputCount * sizeof(uint32_t));

        partition_input_base_gpu[i] = (uint32_t*) malloc (inputCount * sizeof(uint32_t));
        memcpy (partition_input_base_gpu[i], partition_input_base_cpu[i], inputCount *sizeof(uint32_t));

        partition_input_current_gpu[i] = (uint32_t*) malloc (inputCount * sizeof(uint32_t));
        memcpy (partition_input_current_gpu[i], partition_input_current_cpu[i], inputCount *sizeof(uint32_t));

        partition_output_base_gpu[i] = (uint32_t*) malloc (outputCount * sizeof(uint32_t));
        memcpy (partition_output_base_gpu[i], partition_output_base_cpu[i], outputCount *sizeof(uint32_t));

        partition_output_current_gpu[i] = (uint32_t*) malloc (outputCount * sizeof(uint32_t));
        memcpy (partition_output_current_gpu[i], partition_output_current_cpu[i], outputCount *sizeof(uint32_t));

        simulated_output_gpu[i] = (uint32_t*) malloc (outputCount * sizeof(uint32_t));
    }

    printf(" Copy GPU pointer \n");

    partition_input_length = (cl_ulong**) malloc (total_threads * sizeof(cl_ulong*));
    partition_input_base = (cl_ulong**) malloc(total_threads * sizeof(cl_ulong*));
    partition_input_current = (cl_ulong**) malloc (total_threads * sizeof(uint32_t*));
    partition_output_base = (cl_ulong**) malloc (total_threads * sizeof(uint32_t*));
    partition_output_current = (cl_ulong**) malloc(total_threads * sizeof(uint32_t*));
    simulated_output = (cl_ulong**) malloc(total_threads * sizeof(uint32_t*));

    for ( uint32_t i = 0 ; i < total_threads ; i++){
        partition_input_length[i] = (cl_ulong*) malloc(inputCount * sizeof(cl_ulong));
        for(uint32_t j = 0; j < inputCount; j++) partition_input_length[i][j] = static_cast<cl_ulong>(partition_input_length_cpu[i][j]);    //element by element copy

        partition_input_base[i] = (cl_ulong*) malloc(inputCount *sizeof(cl_ulong));
        for(uint32_t j = 0; j < inputCount; j++) partition_input_base[i][j] = static_cast<cl_ulong>(partition_input_base_cpu[i][j]);

        partition_input_current[i] = (cl_ulong*) malloc(inputCount *sizeof(cl_ulong));
        for(uint32_t j = 0; j < inputCount; j++) partition_input_current[i][j] = static_cast<cl_ulong>(partition_input_current_cpu[i][j]);

        partition_output_base[i] = (cl_ulong*) malloc(outputCount *sizeof(cl_ulong));
        for(uint32_t j = 0; j < inputCount; j++) partition_output_base[i][j] = static_cast<cl_ulong>(partition_output_base_cpu[i][j]);

        partition_output_current[i] = (cl_ulong*) malloc(outputCount *sizeof(cl_ulong));
        for(uint32_t j = 0; j < inputCount; j++) partition_output_current[i][j] = static_cast<cl_ulong>(partition_output_current_cpu[i][j]);

        simulated_output[i] = (cl_ulong*) malloc(outputCount *sizeof(cl_ulong));
        for(uint32_t j = 0; j < inputCount; j++) simulated_output[i][j] = static_cast<cl_ulong>(simulated_output[i][j]);
    }

    //don't be a memory hog
    for ( uint32_t i = 0 ; i < total_threads ; i++){
        free(partition_input_length_gpu[i]);
        free(partition_input_base_gpu[i]);
        free(partition_input_current_gpu[i]);
        free(partition_output_base_gpu[i]);
        free(partition_output_current_gpu[i]);
        free(simulated_output_gpu[i]);
    }
    free(partition_input_length_gpu);
    free(partition_input_base_gpu);
    free(partition_input_current_gpu);
    free(partition_output_base_gpu);
    free(partition_output_current_gpu);
    free(simulated_output_gpu);
}


void FSTGPU::copyBack( TP * transducer){
    unsigned long ** tmp = (unsigned long **) malloc( outputCount * sizeof( unsigned long*));
    ///cudaMemcpy ( tmp, output, outputCount * sizeof( unsigned long *), cudaMemcpyDeviceToHost);                       FIX LATER
    for  (int i = 0; i <transducer-> outputCount; i++){
        ///cudaMemcpy( transducer->outStream[i], tmp[i], OUTPUT_LENGTH * sizeof( unsigned long), cudaMemcpyDeviceToHost);

    }
#ifdef DEBUG_GPU
    for ( int i = 0; i < transducer->outputCount; i ++){
    printf("Output %d: \n", i);
    ///for( int j =0; j < OUTPUT_LENGTH; j++){
    for( int j =0; j < 10; j++){
      printf( "%u; ", transducer->outStream[i][j]);
    }
    printf("\n");
  }
#endif
}
