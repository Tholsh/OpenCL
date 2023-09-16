/*
Code that contains testing examples of OpenCL hardware
*/
#include "CLfuncs.h"

__kernel void test_kernel(__global char* data_in,
      __local float* local_result, __global char* chars_out, __global cl_test_obj obj_in) {

      if(get_global_id(0) <= 1024) {chars_out[get_global_id(0)] = data_in[get_global_id(0)];}
      
	  if(get_global_id(0) == 0) printf("Cube ascii art data check\n");
		//printf("%d\n", get_global_id(0));
        //printf("%d", get_local_id(0));

}
