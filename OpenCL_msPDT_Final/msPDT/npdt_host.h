//
// Created by theo on 7/23/22.
//

#ifndef OPENCL_MSPST_VARIADIC_NPDT_HOST_H
#define OPENCL_MSPST_VARIADIC_NPDT_HOST_H

#endif //OPENCL_MSPST_VARIADIC_NPDT_HOST_H
#define CL_TARGET_OPENCL_VERSION 200
#include <CL/cl.h>

#define MAXVAR 64

#define ALPHABETSIZE 256
#define EPSILON_MATCH 512

#define VARSTART  EPSILON_MATCH  + 1
#define INPUTSTART VARSTART + MAXVAR
#define ANY_MATCH  641
#define OUTPUTSTART ANY_MATCH + 1

#define ANY_PUSH  ANY_MATCH + 256
#define INPUT_MATCH  577

typedef struct{
    /* state identifier */
    cl_ulong id;

    /* outgoing transition  */
    cl_ulong  baseID;
    cl_ulong  numberofTransition;

    /* action           */
    cl_ulong   src1;
    cl_ulong   src2;
    cl_ulong   dst;
    cl_ulong    opt;

    /* helper variable  */
    cl_bool accepting;
    cl_bool initial;
    cl_bool epsilonState;
}NPDT;


typedef struct{
    /* helper variable  */
    cl_ulong id;

    /* Matching condition */
    cl_ulong  inputID;
    cl_ulong  inputSymbol;
    cl_ulong  IvarID;
    cl_ulong  inputVar;
    cl_ulong  currentState;

    /* Result */
    cl_ulong  outputID;
    cl_ulong  outputSymbol;
    cl_ulong  OvarID;
    cl_ulong  outputVar;
    cl_ulong  nextState;
}Transition;
