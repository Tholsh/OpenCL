//
// Created by theo on 7/22/22.
//

#ifndef OPENCL_MSPDT_VARIADIC_PROCESSOR_GPU_H
#define OPENCL_MSPDT_VARIADIC_PROCESSOR_GPU_H


#endif //OPENCL_MSPDT_VARIADIC_PROCESSOR_GPU_H
#include <fstream>
#include <assert.h>
#include <iostream>
#include <map>
#define CL_TARGET_OPENCL_VERSION 200
#include <CL/cl.h>
#define DEBUG 1
#include "npdt_host.h"


//Host OpenCL structs

typedef struct {
    /* Full transducer information  */

    cl_ulong stateCount;
    cl_ulong transitionCount;
    cl_ulong varCount;
    cl_ulong inputCount;
    cl_ulong outputCount;

    /* stack and memory info  */
    cl_ulong * var;
    NPDT * stateList;
    Transition * transitionList;

    /* State Action data  */
    cl_bool * pendingTransition;

    /* Transition Match data  */
    cl_bool * confirmedTransition;

    /* OutputUpdate data  */
    cl_bool * activatedState;
}TPGPU;


typedef struct{
    int one[10];
    char two[15];
} type_1;
typedef struct{
    int one[10];
    char two[15];
}type_2;
typedef struct{
    int one[10];
    char two[15];
}type_3;
typedef struct{
    int one[10];
    char two[15];
    char str[2000];
}type_4;

typedef struct{
    cl_int errcode;
    cl_char debug_str[4000];
}debug_t;
