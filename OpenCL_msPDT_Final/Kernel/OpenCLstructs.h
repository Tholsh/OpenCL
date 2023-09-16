//
// Created by theo on 7/23/22.
//Device OpenCL structs


#ifndef OPENCL_MSPST_VARIADIC_OPENCLSTRUCTS_H
#define OPENCL_MSPST_VARIADIC_OPENCLSTRUCTS_H

#endif //OPENCL_MSPST_VARIADIC_OPENCLSTRUCTS_H
#include "npdt_kernel.h"

enum COMPARECODE {  ALPHABET      = 0,
    NEGALPHABET = 1,
    EPS         = 2,
    VAR         = 3,
    INPT        = 4,
    PASSTHROUGH = 5,
    NEGVAR      = 6,
    NEGINPT     = 7,
    ILL         = 8
};


typedef struct{
    unsigned int * input;
    unsigned long * input_lengths;
    const unsigned short num_inputs;
}twoD_array;

typedef struct{
    unsigned long * input;
    const unsigned long total_threads;
    const unsigned short num_inputs;
}twoD_thread_array;


//Device  OpenCL Structs
typedef struct {
    /* Full transducer information  */
    unsigned long stateCount;
    unsigned long transitionCount;
    unsigned long varCount;
    unsigned long inputCount;
    unsigned long outputCount;

    /* stack and memory info  */
    unsigned long * var;
    NPDT * stateList;
    Transition * transitionList;

    /* State Action data  */
    bool * pendingTransition;

    /* Transition Match data  */
    bool * confirmedTransition;

    /* OutputUpdate data  */
    bool * activatedState;
}TPGPU;

//Device OpenCL structs
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
    int errcode;
    char debug_str[4000];
}debug_t;

typedef struct{
    int z[10];
    int one;
    unsigned long output_size;
    char str[2000];
}info_t;

typedef struct{
    int z[10];
    int one;
    int two;
    char str[2000];
}easy_t;