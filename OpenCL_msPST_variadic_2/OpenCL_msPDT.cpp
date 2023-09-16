#include "CLfuncs.h"
#include "msPDT/processor_gpu.h"

int main() {
    int      num_v    [NUM_SIZE];
    TPGPU    tpgpu    [TPGPU_SIZE];
    debug_t  debug    [ONE_DIM_SIZE];
    type_2   type2_v  [TYPE_2_SIZE];
    type_3   type3_v  [TYPE_3_SIZE];
    type_4   type4_v  [TYPE_4_SIZE];

    strcpy((char *)type4_v[0].str, ASCII);  //making arbitrary data to test functionality of the kernel

    kernel_variadic  Kernel({NUM_SIZE,TPGPU_SIZE,ONE_DIM_SIZE,TYPE_2_SIZE,TYPE_3_SIZE,TYPE_4_SIZE});   //from 5 to 2 lines!
    Kernel.clSummonKernel  ( num_v,   tpgpu,     debug,       type2_v,    type3_v,    type4_v);
    //printf("struct string \n%s \n", type4_v[1].str);
    //printf("Debug String  \n%s \n", debug->debug_str);

    return 0;
}