//
// Created by theo on 7/15/22.
//
#ifndef ADD_NUMBERS_MODIFIED_CLFUNCS_H
#define ADD_NUMBERS_MODIFIED_CLFUNCS_H

#endif //ADD_NUMBERS_MODIFIED_CLFUNCS_H
#define ASCII "8888888 8888888888 8 8888888888⠀     d888888o.   8888888 8888888888\n      8 8888       8 8888          .`8888:' `88.       8 8888\n      8 8888       8 8888          8.`8888.   Y8       8 8888\n      8 8888       8 8888          `8.`8888.           8 8888\n      8 8888       8 888888888888   `8.`8888.          8 8888\n      8 8888       8 8888            `8.`8888.         8 8888\n      8 8888       8 8888             `8.`8888.        8 8888\n      8 8888       8 8888         8b   `8.`8888.       8 8888\n      8 8888       8 8888         `8b.  ;8.`8888       8 8888\n      8 8888       8 888888888888  `Y8888P ,88P'       8 8888       "
#define CL_TARGET_OPENCL_VERSION 200
#include <CL/cl.h>
#include <memory>
#include <string.h>
#include <typeinfo>
#include <iostream>
#include <deque>
#include <vector>
#define PROGRAM_FILE "Kernel/OpenCL_msPDT.cl"
#define KERNEL_FUNC "OpenCL_msPDT"
#define ERR -1 //typical error


#define GLOBAL_SIZE 128
#define LOCAL_SIZE 64
#define NUM_SIZE 1000
#define TPGPU_SIZE  1
#define TYPE_1_SIZE 2
#define TYPE_2_SIZE 10
#define TYPE_3_SIZE 10
#define TYPE_4_SIZE 10
#define ONE_DIM_SIZE 1


//Design-specific OpenCL macros used for compilation functions



struct properties_t{
    unsigned number_of_args = 0;
    unsigned work_dim = 1;
    unsigned offset = 0;
    unsigned num_devices = 1;
    unsigned properties = 0;
    unsigned num_events_in_wait_list = 0;
    size_t   global_size[1] = {GLOBAL_SIZE};
    size_t   local_size[1] = {LOCAL_SIZE};
    cl_mem_flags mem_policy = CL_MEM_READ_WRITE;
    std::vector<cl_mem_flags> mem_policies;
    cl_mem_flags host_policy = CL_MEM_COPY_HOST_PTR;
    std::vector<cl_mem_flags> host_ptr_policies;
};

void errorHandler(const char * message, cl_int err);
cl_device_id create_device();
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename);
