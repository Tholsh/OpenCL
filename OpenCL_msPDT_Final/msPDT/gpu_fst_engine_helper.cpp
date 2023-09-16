//
// Created by theo on 7/26/22.
//
#include "gpu_fst_engine.h"
#include <cstring>

#define ARRAY_SIZE 40

typedef struct{
    cl_int z[10];
    cl_int one;
    cl_int two;
    cl_char str[2000];
}easy_t;

typedef struct{
    cl_int z[10];
    cl_int one;
    cl_ulong output_size;
    cl_char str[2000];
}info_t;

#define ASCII "8888888 8888888888 8 8888888888      d888888o.   8888888 8888888888\n      8 8888       8 8888          .`8888:' `88.       8 8888\n      8 8888       8 8888          8.`8888.   Y8       8 8888\n      8 8888       8 8888          `8.`8888.           8 8888\n      8 8888       8 888888888888   `8.`8888.          8 8888\n      8 8888       8 8888            `8.`8888.         8 8888\n      8 8888       8 8888             `8.`8888.        8 8888\n      8 8888       8 8888         8b   `8.`8888.       8 8888\n      8 8888       8 8888         `8b.  ;8.`8888       8 8888\n      8 8888       8 888888888888  `Y8888P ,88P'       8 8888       "

void FSTGPU::process(TPGPU *transducer, TP *processor){
    printf("Begin Execution\n");
    printf(" Number of blocks: %u \n", number_of_block);
    printf(" Number of threads per block: %u\n", number_of_thread);
    printf(" Total Number of threads: %u\n", total_threads);

    unsigned long size_input  = total_threads * inputCount; //for partition
    unsigned long size_output = total_threads * outputCount;

    //info_t info_local[4];    //for handling extraneous information
    //post to info
    //info_local[0].output_size = transducer->outputCount;

    unsigned long in_elements = 0;
        for(uint32_t i = 0; i < inputCount; i++){
            in_elements += input_length[i];
        }

    unsigned long out_elements = 0;
    for(uint32_t i = 0; i < outputCount; i++){
        out_elements += output_length[i];
    }

    printf("length of first input: %u\n", (unsigned) input_length[0]);
    printf("input partition size kernel = %zu\n", size_input);
    printf("output partition size kernel = %zu\n", size_output);
    printf("Input elements = %zu\n", in_elements);

    properties_t properties;
    *properties.global_size = total_threads;
    *properties.local_size = number_of_thread;
    printf("OpenCL global size %zu\n", *properties.global_size);
    printf("OpenCL local size %zu\n", *properties.local_size);

    printf("max const buffer size =: %i\n", CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE);

    cl_uint input_1d                     [in_elements]; //change the rest to 32 bits later!!!

    cl_ulong partition_input_length_1d    [total_threads * inputCount];
    cl_ulong partition_input_base_1d      [total_threads * inputCount];
    cl_ulong partition_input_current_1d   [total_threads * inputCount];
    cl_ulong partition_output_base_1d     [total_threads * outputCount];
    cl_ulong partition_output_current_1d  [total_threads * outputCount];
    cl_ulong simulated_output_1d          [total_threads * outputCount];

    uint32_t offset = 0;
    for(uint32_t i = 0; i < inputCount; i++){
        for(uint32_t j = 0; j < input_length[i]; j++){
            input_1d[offset + j] = input[i][j];
        }
        offset += input_length[i];
    }//element by element copy

    for(uint32_t i = 0; i < total_threads; i++){
        for(uint32_t j = 0; j < inputCount; j++) {
            partition_input_length_1d[(j * total_threads) + i] = partition_input_length[i][j];
            partition_input_base_1d[(j * total_threads) + i] = partition_input_base[i][j];
            partition_input_current_1d[(j * total_threads) + i] = partition_input_current[i][j];
            partition_output_base_1d[(j * total_threads) + i] = partition_input_base[i][j];
            partition_output_current_1d[(j * total_threads) + i] = partition_input_current[i][j];
            simulated_output_1d[(j * total_threads) + i] = simulated_output[i][j];
        }
    }

    printf("PRINTING FIRST AND LAST 10 INPUTS FROM INPUT1d\n ");

    for ( uint32_t i = 0; i < inputCount; i++){
        printf("input length = %u\n \n", unsigned (input_length[i]));
        for(uint j = 0; j < 10; j++) {
            printf("%u; ", (unsigned) input_1d[j]);
        }
        printf("\n");
        for(int j = 10; j > 0; j--) {
            //printf("%u; ", (unsigned) input[i][input_length[i] - j]);
            printf("%u; ", (unsigned) input_1d[input_length[i] - j]);
        }
        printf("\n \n");
    }
        printf("partition_input_length[%u] = \n", (unsigned) total_threads);
    for(uint32_t i = 0; i < total_threads; i++){
        printf(" %u;", (unsigned) partition_input_length_1d[i]);
    }



    //OpenCL variables & structures
    cl_device_id device;
    cl_context context;
    cl_program program;
    cl_kernel kernel;
    cl_command_queue queue;
    cl_int err;

    //Data & buffers
    info_t info_local[4];    //for testing structs
    cl_mem info_buffer;  //buffers




    printf("Creating OpenCL Kernel\n");


    //Data & buffers

    //init normal buffers
    cl_mem transducer_buffer;
    cl_mem partition_input_base_buffer;
    cl_mem partition_input_length_buffer;
    cl_mem partition_input_current_buffer;
    cl_mem partition_output_base_buffer;
    cl_mem partition_output_current_buffer;
    cl_mem simulated_output_buffer;
    cl_mem output_buffer;
    cl_mem output_length_buffer;
    cl_mem input_buffer;
    cl_mem input_length_buffer;
    //init buffers for TPGPU struct
    cl_mem var_buffer;
    cl_mem stateList_buffer;
    cl_mem transitionList_buffer;
    cl_mem pendingTransition_buffer;
    cl_mem confirmedTransition_buffer;
    cl_mem activatedState_buffer;


    simulated_output_1d[0] = 1337;

    std::strcpy((char *) info_local[0].str, ASCII);    //loads test struct up with ascii test

    // Create device and context: one device
    device = create_device();
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    errorHandler("Couldn't create a context", err);

    // Build program using predefined functions
    program = build_program(context, device, PROGRAM_FILE);

    transducer_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
      sizeof(TPGPU), transducer, &err); // <=====INPUT

    partition_input_length_buffer =     clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
    size_input * sizeof(cl_ulong), partition_input_length_1d, &err); // <=====INPUT

    partition_input_base_buffer =   clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
    size_input * sizeof(cl_ulong), partition_input_base_1d, &err); // <=====INPUT

     partition_input_current_buffer =  clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
    size_input * sizeof(cl_ulong), partition_input_current_1d, &err); // <=====INPUT

    partition_output_base_buffer =    clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
    size_output * sizeof(cl_ulong), partition_output_base_1d, &err); // <=====OUTPUT

    partition_output_current_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
    size_output * sizeof(cl_ulong), partition_output_current_1d, &err); // <=====OUTPUT

    simulated_output_buffer =         clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
         (size_output * sizeof(cl_ulong)), simulated_output_1d, &err); // <=====OUTPUT

    output_buffer =         clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
         (out_elements * sizeof(cl_ulong)), output, &err); // <=====OUTPUT

    output_length_buffer =             clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
         (outputCount * sizeof(cl_ulong)), output_length, &err); // <=====INPUT

    input_buffer = clCreateBuffer
            (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, in_elements * sizeof(cl_uint), input_1d, &err);

    input_length_buffer =             clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            (inputCount * sizeof(cl_ulong)), input_length, &err); // <=====INPUT

    info_buffer = clCreateBuffer
            (context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, 4* sizeof(info_t), info_local, &err);

    var_buffer = clCreateBuffer
            (context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, transducer->varCount * sizeof(cl_ulong), var, &err);

    stateList_buffer = clCreateBuffer
            (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, transducer->stateCount * sizeof(NPDT), stateList, &err);

    transitionList_buffer = clCreateBuffer
            (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, transducer->transitionCount * sizeof(Transition), transitionList, &err);

    pendingTransition_buffer = clCreateBuffer
            (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, transducer->transitionCount * sizeof(cl_bool), pendingTransition, &err);

    confirmedTransition_buffer = clCreateBuffer
            (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, transducer->transitionCount * sizeof(cl_bool), confirmedTransition, &err);

    activatedState_buffer = clCreateBuffer
            (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, transducer->stateCount * sizeof(cl_bool), activatedState, &err);




    errorHandler("Couldn't create a buffer", err);


    queue = clCreateCommandQueue(context, device, 0, &err);     // Create command queue
    errorHandler("Couldn't create a command queue", err);   //(Doesn't support out-of-order-execution or profiling)

    //Create Kernel
    kernel = clCreateKernel(program, KERNEL_FUNC, &err);
    errorHandler("Couldn't create a kernel", err);

    // Create kernel arguments

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &transducer_buffer); // <=====INPUT
    errorHandler("Couldn't create a kernel argument 1", err);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &partition_input_length_buffer);
    errorHandler("Couldn't create a kernel argument 2", err);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &partition_input_base_buffer); // <=====OUTPUT
    errorHandler("Couldn't create a kernel argument 3", err);
    err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &partition_input_current_buffer); // <=====OUTPUT
    errorHandler("Couldn't create a kernel argument 4", err);
    err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &partition_output_base_buffer);
    errorHandler("Couldn't create a kernel argument 6", err);
    err |= clSetKernelArg(kernel, 5, sizeof(cl_mem), &partition_output_current_buffer);
    errorHandler("Couldn't create a kernel argument 7", err);
    err |= clSetKernelArg(kernel, 6, sizeof(cl_mem), &simulated_output_buffer);
    errorHandler("Couldn't create a kernel argument 8", err);
    err |= clSetKernelArg(kernel, 7, sizeof(cl_mem), &output_buffer);
    errorHandler("Couldn't create a kernel argument 9", err);
    err |= clSetKernelArg(kernel, 8, sizeof(cl_mem), &output_length_buffer);
    errorHandler("Couldn't create a kernel argument 10", err);
    err |= clSetKernelArg(kernel, 9, sizeof(cl_mem), &input_buffer);
    errorHandler("Couldn't create a kernel argument input1d", err);
    err |= clSetKernelArg(kernel, 10, sizeof(cl_mem), &input_length_buffer);
    errorHandler("Couldn't create a kernel argument 11", err);
    err |= clSetKernelArg(kernel, 11, sizeof(cl_mem), &info_buffer); // <==== OUTPUT2 LEAVE
    errorHandler("Couldn't create a kernel argument 12", err);
    err |= clSetKernelArg(kernel, 12, sizeof(cl_mem), &var_buffer); // <==== OUTPUT2 LEAVE
    errorHandler("Couldn't create a kernel argument 13", err);
    err |= clSetKernelArg(kernel, 13, sizeof(cl_mem), &stateList_buffer); // <==== OUTPUT2 LEAVE
    errorHandler("Couldn't create a kernel argument 14", err);
    err |= clSetKernelArg(kernel, 14, sizeof(cl_mem), &transitionList_buffer); // <==== OUTPUT2 LEAVE
    errorHandler("Couldn't create a kernel argument 15", err);
    err |= clSetKernelArg(kernel, 15, sizeof(cl_mem), &pendingTransition_buffer); // <==== OUTPUT2 LEAVE
    errorHandler("Couldn't create a kernel argument 16", err);
    err |= clSetKernelArg(kernel, 16, sizeof(cl_mem), &confirmedTransition_buffer); // <==== OUTPUT2 LEAVE
    errorHandler("Couldn't create a kernel argument 17", err);
    err |= clSetKernelArg(kernel, 17, sizeof(cl_mem), &activatedState_buffer); // <==== OUTPUT2 LEAVE
    errorHandler("Couldn't create a kernel argument 18", err);

    //Enqueue kernel
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, properties.global_size, properties.local_size, 0, NULL, NULL);
    errorHandler("Failed to create Kernel", err);

    // Read kernel output
    err |= clEnqueueReadBuffer
            (queue, info_buffer, CL_TRUE, 0, 4 * sizeof(info_t), info_local, 0, NULL, NULL); // <==== OUTPUT2
    errorHandler("Failed to create read Buffer 1", err);

    err |= clEnqueueReadBuffer
            (queue, simulated_output_buffer, CL_TRUE, 0, total_threads * outputCount * sizeof(cl_ulong), simulated_output_1d, 0, NULL, NULL); // <==== OUTPUT2
    errorHandler("Failed to create read Buffer 3", err);

    err |= clEnqueueReadBuffer
            (queue, output_buffer, CL_TRUE, 0, out_elements * sizeof(cl_ulong), output, 0, NULL, NULL); // <==== OUTPUT2
    errorHandler("Failed to create read Buffer 4", err);


    printf("\n==================KERNEL FINISHED=====================\n");

    //print out information
    printf("struct char:%c\n", info_local[3].str[0]);
    printf("struct string:\n%s\n", info_local[1].str);


    //Comparison
    if (!strcmp((char *) info_local[1].str,(char *) info_local[0].str))
        printf("Check passed.\n");
    else
        printf("Check failed.\n");

    printf("simulated_output_1d: %u\n", simulated_output_1d[0]);
    printf("output_1d: %u\n", output[0]);

    clFinish(queue);
    //Deallocate resources
    clReleaseKernel(kernel);

    clReleaseMemObject(info_buffer);
    clReleaseMemObject(var_buffer);
    clReleaseMemObject(stateList_buffer);
    clReleaseMemObject(activatedState_buffer);
    clReleaseMemObject(pendingTransition_buffer);
    clReleaseMemObject(confirmedTransition_buffer);
    clReleaseMemObject(transitionList_buffer);
    clReleaseMemObject(partition_input_base_buffer);
    clReleaseMemObject(partition_input_length_buffer);
    clReleaseMemObject(partition_input_current_buffer);
    clReleaseMemObject(partition_output_base_buffer);
    clReleaseMemObject(partition_output_current_buffer);
    clReleaseMemObject(simulated_output_buffer);
    clReleaseMemObject(transducer_buffer);
    clReleaseMemObject(output_buffer);
    clReleaseMemObject(output_length_buffer);
    clReleaseMemObject(input_buffer);
    clReleaseMemObject(input_length_buffer);

    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);


    float execution_time_ms = 0;
    //cudaDeviceSynchronize();
    printf("Execution  Finished\n");
    fprintf(stderr,"Execution Time %f ms\n", execution_time_ms); //ALWAYS 0, FIX

}



void printTransitionKernel1( TPGPU * transducer){
    printf("%d variable \n", transducer->varCount);
    printf("%d state \n", transducer->stateCount);
    printf("%d transition \n", transducer->transitionCount);
    for ( int i =0; i < transducer->varCount; i++){
        printf("variable[%d] = %d \n", i, transducer->var[i]);
    }

    for ( int i =0; i < transducer->stateCount; i++){
        printf("State %d \n", transducer->stateList[i].id);
    }
    for ( int i =0; i < transducer->transitionCount; i++){
        printf("Transition %d \n", transducer->transitionList[i].id);
    }
}

void FSTGPU::printFST( TPGPU * transducer, bool state, bool transition, bool variable){
    ///cudaError_t error;

    TPGPU * TEMP_FST = (TPGPU *) malloc (sizeof( TPGPU));
    ///cudaMemcpy( TEMP_FST, transducer, sizeof(TPGPU), cudaMemcpyDeviceToHost);

    if (state){
        printf("============================================\n");
        printf( "%d States\n", TEMP_FST->stateCount);
        NPDT * stateList = (NPDT *) malloc (TEMP_FST->stateCount * sizeof(NPDT));
        ///error = cudaMemcpy( stateList, TEMP_FST->stateList, TEMP_FST->stateCount * sizeof(NPDT), cudaMemcpyDeviceToHost);

        if ( 1){
            printf( "GPU assert%s \n");
        }
        for (int i = 0; i < TEMP_FST->stateCount; i++){
            printf("STATE %d ", stateList[i].id);
            action_convert ( stateList[i].opt,stateList[i].src1, stateList[i].src2, stateList[i].dst);
        }
    }

    if ( transition){
        printf("============================================\n");
        printf( "%d Transition\n", TEMP_FST->transitionCount);
        Transition * transitionList = (Transition *) malloc (TEMP_FST->transitionCount * sizeof(Transition));
        ///error = cudaMemcpy( transitionList, TEMP_FST->transitionList, TEMP_FST->transitionCount * sizeof(Transition), cudaMemcpyDeviceToHost);

        if ( 1){
            printf( "GPU assert%s \n");
        }
        for (int i = 0; i < TEMP_FST->transitionCount; i++){
            printf("T%d :", transitionList[i].id);
            transition_convert ( transitionList[i].inputID);
            printf("==");
            transition_convert ( transitionList[i].inputSymbol);
            printf(",");
            transition_convert ( transitionList[i].IvarID);
            printf("==");
            transition_convert ( transitionList[i].inputVar);
            printf(" -> ");
            printf(" T%d ", transitionList[i].nextState);
            transition_convert ( transitionList[i].outputID);
            printf("==");
            transition_convert ( transitionList[i].outputSymbol);
            printf(",");
            transition_convert ( transitionList[i].OvarID);
            printf("==");
            transition_convert ( transitionList[i].outputVar);
            printf("\n");
        }
    }
    if( variable){
        printf("============================================\n");
        printf( "%d Variable\n", TEMP_FST->varCount);
        uint32_t * varList = (uint32_t *) malloc (TEMP_FST->varCount * sizeof(uint32_t));
        ///error = cudaMemcpy( varList, TEMP_FST->var, TEMP_FST->varCount * sizeof(uint32_t), cudaMemcpyDeviceToHost);

        if ( 1){
            printf( "GPU assert%s \n");
        }
        for (int i = 0; i < TEMP_FST->varCount; i++){
            printf("S[%d] = %d \n", i,varList[i]);
        }
    }
}



void FSTGPU::action_convert(u_int16_t opcode, u_int16_t src1, u_int16_t src2, u_int16_t dst){
    switch ( opcode) {
        case 1: // ADD
            printf( "S[%d] = S[%d] + S[%d]\n", dst, src1, src2);;
            break;
        case 2: // ADD I
            printf( "S[%d] = S[%d] + %d\n", dst, src1, src2);;
            break;
        case 3: //  SUB
            printf( "S[%d] = S[%d] - S[%d]\n", dst, src1, src2);;
            break;
        case 4: // SUBI
            printf( "S[%d] = S[%d] - %d\n", dst, src1, src2);;
            break;
        case 5: //  MUL
            printf( "S[%d] = S[%d] * S[%d]\n", dst, src1, src2);;
            break;
        case 6: // MULI:
            printf( "S[%d] = S[%d] * %d\n", dst, src1, src2);;
            break;
        case 7: // DIV
            printf( "S[%d] = S[%d] / S[%d]\n", dst, src1, src2);;
            break;
        case 8: //DIVI
            printf( "S[%d] = S[%d] / %d\n", dst, src1, src2);;
            break;
        case 9:
            printf("NO LONGER SUPPORT SET");
            break;
        case 10:
            printf("NO LONGER SUPPORT POP");
            break;
        case 11:  // LSHIFT
            printf( "S[%d] = S[%d] << S[%d]\n", dst, src1, src2);;
            break;
        case 12:  // LSHIFTI
            printf( "S[%d] = S[%d] << %d\n", dst, src1, src2);;
            break;
        case 13:  // RSHIFT
            printf( "S[%d] = S[%d] >> S[%d]\n", dst, src1, src2);;
            break;
        case 14:  // RSHIFTI
            printf( "S[%d] = S[%d] >> %d\n", dst, src1, src2);;
            break;
        case 15:  //  OR
            printf( "S[%d] = S[%d] | S[%d]\n", dst, src1, src2);;
            break;
        case 16:  //  ORI
            printf( "S[%d] = S[%d] | %d\n", dst, src1, src2);;
            break;
        case 17:  //  AND
            printf( "S[%d] = S[%d] & S[%d]\n", dst, src1, src2);;
            break;
        case 18:  //  ANDI
            printf( "S[%d] = S[%d] & %d\n", dst, src1, src2);;
            break;
        case 0:
            printf("Epsilon\n");
            break;
    }
}



void FSTGPU::transition_convert ( u_int16_t alphabet) {
    int epsilon = 222;
    if ( alphabet < ALPHABETSIZE)
        printf("%c",  alphabet);
    else if ( (alphabet >= ALPHABETSIZE)  && ( alphabet < EPSILON_MATCH) ){
        printf( "!%c", alphabet-ALPHABETSIZE);
    }
    else if (   alphabet  == EPSILON_MATCH) {
        printf("%c", epsilon);
    }
    else if ( ( alphabet  >= VARSTART ) && (alphabet  < (VARSTART + MAXVAR ) ) ) {
        printf( "S[%d]", alphabet - (VARSTART));
    }
    else if ( ( alphabet  >= INPUTSTART)  && (alphabet  < (INPUTSTART + MAXVAR) )  ) {
        printf("I[%d]",alphabet- (INPUTSTART));
    }
    else if (   alphabet  == ANY_MATCH) {
        printf("*");
    }
    else if ( ( alphabet  >= OUTPUTSTART) && (alphabet  < (OUTPUTSTART + MAXVAR) )  ){
        printf("O[%d]",alphabet- (OUTPUTSTART));
    }
    else if ( ( alphabet  >=  (VARSTART + ALPHABETSIZE) )  && ( alphabet < ( VARSTART + ALPHABETSIZE + MAXVAR)) ) {
        printf("!S[%d]", alphabet - (VARSTART) - (ALPHABETSIZE));
    }
    else if  ( (alphabet  >= (INPUTSTART + ALPHABETSIZE) ) && (alphabet < (INPUTSTART + MAXVAR + ALPHABETSIZE) )  ) {
        printf("!I[%d]", alphabet- (INPUTSTART) -(ALPHABETSIZE)) ;
    }
    else if (   alphabet  == ANY_PUSH) {
        printf("Push");
    }
}

