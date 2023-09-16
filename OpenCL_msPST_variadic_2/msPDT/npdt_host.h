//
// Created by theo on 7/23/22.
//

#ifndef OPENCL_MSPST_VARIADIC_NPDT_HOST_H
#define OPENCL_MSPST_VARIADIC_NPDT_HOST_H

#endif //OPENCL_MSPST_VARIADIC_NPDT_HOST_H
#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>

typedef struct{
    /* state identifier */
    cl_uint16 id;

    /* outgoing transition  */
    cl_uint16  baseID;
    cl_uint16  numberofTransition;

    /* action           */
    cl_uint16   src1;
    cl_uint16   src2;
    cl_uint16   dst;
    cl_uint16    opt;

    /* helper variable  */
    cl_bool accepting;
    cl_bool initial;
    cl_bool epsilonState;
}NPDT;


typedef struct{
    /* helper variable  */
    cl_uint16 id;

    /* Matching condition */
    cl_uint16  inputID;
    cl_uint16  inputSymbol;
    cl_uint16  IvarID;
    cl_uint16  inputVar;
    cl_uint16  currentState;

    /* Result */
    cl_uint16  outputID;
    cl_uint16  outputSymbol;
    cl_uint16  OvarID;
    cl_uint16  outputVar;
    cl_uint16  nextState;
}Transition;
