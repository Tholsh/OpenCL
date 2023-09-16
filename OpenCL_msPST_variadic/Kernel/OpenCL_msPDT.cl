//HEADER
#include "Kernel/OpenCLheader.h"
#define DEBUG_GPU

__kernel void OpenCL_msPDT(__global int* num, __global TPGPU * TPGPU, __global debug_t* debug,
 __global type_2* type2, __global type_3* type3, __global type_4* type4){
    //we are putting work in one single kernel for debug, will distribute later

   if(get_global_id(0) == 0) {
    strcpy(type4[1].str, type4[0].str);
    printf("Test\n ");
   }

   ulong i = 0;



   //easy[1].str[3] = 81;
}
