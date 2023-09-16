//
// Created by theo on 7/22/22.
//

#ifndef OPENCL_MSPDT_VARIADIC_PROCESSOR_GPU_H
#define OPENCL_MSPDT_VARIADIC_PROCESSOR_GPU_H
#define CL_TARGET_OPENCL_VERSION 120

#endif //OPENCL_MSPDT_VARIADIC_PROCESSOR_GPU_H
#include <fstream>
#include <assert.h>
#include <iostream>
#include <map>
#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>
#include "npdt_host.h"
#define DEBUG 1



//Host OpenCL structs

typedef struct {
    /* Full transducer information  */

    cl_uint16 stateCount;
    cl_uint16 transitionCount;
    cl_uint16 varCount;
    cl_uint16 inputCount;
    cl_uint16 outputCount;

    /* stack and memory info  */
    cl_uint16 * var;
    NPDT * stateList;
    Transition * transitionList;

    /* State Action data  */
    cl_bool * pendingTransition;

    /* Transition Match data  */
    cl_uint16 confirmedTransition;

    /* OutputUpdate data  */
    cl_uint16 activatedState;
}TPGPU;


typedef struct{
    cl_int one[10];
    cl_char two[15];
} type_1;
typedef struct{
    cl_int one[10];
    cl_char two[15];
}type_2;
typedef struct{
    cl_int one[10];
    cl_char two[15];
}type_3;
typedef struct{
    cl_int one[10];
    cl_char two[15];
    cl_char str[2000];
}type_4;

typedef struct{
    cl_int errcode;
    cl_char debug_str[4000];
}debug_t;
