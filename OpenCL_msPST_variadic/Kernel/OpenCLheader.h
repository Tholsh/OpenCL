//
// Created by theo on 7/22/22.
//
#ifndef OPENCL_MSPDT_VARIADIC_OPENCLHEADER_H
#define OPENCL_MSPDT_VARIADIC_OPENCLHEADER_H

#define GLOBAL_SIZE 128
#define LOCAL_SIZE 64

#endif //OPENCL_MSPDT_VARIADIC_OPENCLHEADER_H
#include "OpenCLstructs.h"


void process_k( TPGPU * fst, ulong ** input_base, ulong ** partition_length, ulong ** processed_symbol, ulong ** input,
                ulong ** partition_output_base, ulong** partition_output_count, ulong ** output, ulong **simulated_output );
void strcpy(char * dest, const char * src);

void strcpy(char * dest, const char * src){
    if(src == NULL || dest == NULL) return;
    unsigned i = 0;
    while(src[i] != '\000'){
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\000';
}


