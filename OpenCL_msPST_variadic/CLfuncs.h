//
// Created by theo on 7/15/22.
//
#ifndef ADD_NUMBERS_MODIFIED_CLFUNCS_H
#define ADD_NUMBERS_MODIFIED_CLFUNCS_H

#endif //ADD_NUMBERS_MODIFIED_CLFUNCS_H
#define ASCII "8888888 8888888888 8 8888888888      d888888o.   8888888 8888888888\n      8 8888       8 8888          .`8888:' `88.       8 8888\n      8 8888       8 8888          8.`8888.   Y8       8 8888\n      8 8888       8 8888          `8.`8888.           8 8888\n      8 8888       8 888888888888   `8.`8888.          8 8888\n      8 8888       8 8888            `8.`8888.         8 8888\n      8 8888       8 8888             `8.`8888.        8 8888\n      8 8888       8 8888         8b   `8.`8888.       8 8888\n      8 8888       8 8888         `8b.  ;8.`8888       8 8888\n      8 8888       8 888888888888  `Y8888P ,88P'       8 8888       "

#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>
#include <memory>
#include <string.h>
#include <typeinfo>
#include <iostream>
#include <array>
#include <vector>
#define PROGRAM_FILE "Kernel/OpenCL_msPDT.cl"
#define KERNEL_FUNC "OpenCL_msPDT"
#define ERR -1 //typical error


#define GLOBAL_SIZE 128
#define LOCAL_SIZE 64
#define NUM_SIZE 1000
#define TPGPU_SIZE  2
#define TYPE_1_SIZE 2
#define TYPE_2_SIZE 10
#define TYPE_3_SIZE 10
#define TYPE_4_SIZE 10
#define ONE_DIM_SIZE 1


//Design-specific OpenCL macros used for compilation functions
#define CL_MAX_ARGS 32
#define PROPERTIES 0
#define WORK_DIM 1
#define NUM_EVENTS_IN_WAIT_LIST 0
#define OFFSET 0
#define NUM_DEVICES 1

class kernel_variadic;

struct properties_t{
    unsigned number_of_args = 0;
    unsigned word_dim = 0;
    unsigned offset = 0;
    unsigned num_devices = 0;
    size_t   global_size = GLOBAL_SIZE;
    size_t   local_size = LOCAL_SIZE;
    cl_mem_flags mem_policy = CL_MEM_READ_WRITE;
    std::vector<cl_mem_flags> mem_policies;
    cl_mem_flags host_policy = CL_MEM_COPY_HOST_PTR;
    std::vector<cl_mem_flags> host_ptr_policies;
};

void errorHandler(const char * message, cl_int err);
cl_device_id create_device();
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename);

class kernel_variadic{
public:
    std::initializer_list<unsigned> arg_sizes;
    std::vector<cl_mem> buffers;
    properties_t p;

    //OpenCL variables & structures
    cl_device_id device;
    cl_context context;
    cl_program program;
    cl_kernel kernel;
    cl_command_queue queue;
    cl_int err;

    kernel_variadic(){}

    //inititalize with generic properties
    kernel_variadic(std::initializer_list<unsigned> sizes){
        arg_sizes = sizes;
        if (sizes.size() == 0) return;
        for(unsigned int i = 0; i <= sizes.size(); i++){p.mem_policies.assign(i, p.mem_policy);}
        for(unsigned int i = 0; i <= sizes.size(); i++){p.host_ptr_policies.assign(i, p.host_policy);}

        buffers.resize(sizes.size());
        p.number_of_args = sizes.size();
    }

    //initialize with external properties list
    kernel_variadic(std::initializer_list<unsigned> sizes, properties_t properties){
        arg_sizes = sizes;
        p = properties;
        if (sizes.size() == 0) return;
        for(unsigned int i = 0; i < sizes.size() + 1; i++){p.mem_policies.assign(i, properties.mem_policy);}
        for(unsigned int i = 0; i < sizes.size() + 1; i++){p.host_ptr_policies.assign(i, properties.host_policy);}

        buffers.resize(sizes.size());
        p.number_of_args = sizes.size();
    }


    template <class ... Ts>
    void clSummonKernel(Ts && ... inputs){
        if(arg_sizes.size() == 0) errorHandler("Error: No initializer list given! ", -420);

        for(unsigned i = 0; i < p.number_of_args; i++) p.mem_policies[i] =      p.mem_policy;
        for(unsigned i = 0; i < p.number_of_args; i++) p.host_ptr_policies[i] = p.host_policy;

        // Create device and context: one device
        device = create_device();
        context = clCreateContext(NULL, NUM_DEVICES, &device, NULL, NULL, &err);
        errorHandler("Couldn't create a context", err);
        // Build program using predefined functions
        program = build_program(context, device, PROGRAM_FILE);

        int i = 0;
        ([&]{
            ///Create buffers
            buffers[i]         = clCreateBufferWithStruct       (inputs, i, * (arg_sizes.begin() + i));
            i++;
        } (), ...);

        queue = clCreateCommandQueue(context, device, PROPERTIES, &err);     // Create command queue
        errorHandler("Couldn't create a command queue", err);   //(Doesn't support out-of-order-execution or profiling)
        //Create Kernel
        kernel = clCreateKernel(program, KERNEL_FUNC, &err);
        errorHandler("Couldn't create a kernel", err);
        // Create kernel arguments
        for(unsigned i =0; i < p.number_of_args; i++) clSetKernelArgWithBuffer(&buffers[i],  i);
        //Enqueue kernel
        err = clEnqueueNDRangeKernel(queue, kernel, WORK_DIM, NULL, &p.global_size, &p.local_size, NUM_EVENTS_IN_WAIT_LIST, NULL, NULL);
        errorHandler("Failed to create Kernel", err);

        i = 0;
        ([&]{
            /// Read kernel output
            clEnqueueReadBufferWithStruct           (buffers[i],   &inputs, * (arg_sizes.begin() + i));
            i++;
        } (), ...);

        //Comparison
        //if (!strcmp((char *) arg_2.data[1].str,(char *) arg_2.data[0].str))
        //    printf("Check passed.\n");
        //else
        //    printf("Check failed.\n");
        clCleanup();
    }

    void clCleanup(){
        clReleaseKernel(kernel);
        for(unsigned i = 0; i < p.number_of_args; i++) clReleaseMemObject(buffers[i]);
        clReleaseCommandQueue(queue);
        clReleaseProgram(program);
        clReleaseContext(context);
    }

    void clEnqueueReadBufferWithStruct(cl_mem buffer, auto * arg, size_t elements){
        err |= clEnqueueReadBuffer
                (queue, buffer, CL_TRUE, p.offset, elements * sizeof(**arg), *arg, NUM_EVENTS_IN_WAIT_LIST, NULL, NULL);               // <==== OUTPUT1
        errorHandler(("Failed to create read Buffer" + (std::string) typeid(*arg).name()).c_str(), err);

    }

    cl_mem clCreateBufferWithStruct(auto arg, unsigned arg_index, size_t elements){
        cl_mem temp = clCreateBuffer
                (context, p.mem_policies[arg_index] | p.host_ptr_policies[arg_index], elements * sizeof(*arg), arg, &err); // <==== OUTPUT
        errorHandler(("Couldn't create buffer" + (std::string) typeid(arg).name()).c_str(), err);
        return temp;
    }

    void clSetKernelArgWithBuffer(cl_mem * buffer, cl_uint arg_index){
        err |= clSetKernelArg(kernel, arg_index, sizeof(cl_mem), buffer); // <==== OUTPUT1
        errorHandler(("Couldn't create a kernel argument " + std::to_string(arg_index++)).c_str(), err);
    }
};