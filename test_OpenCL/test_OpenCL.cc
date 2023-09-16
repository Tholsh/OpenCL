#define PROGRAM_FILE "test_kernel.cl"
#define KERNEL_FUNC "test_kernel"
#define ARRAY_SIZE 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CLfuncs.h"

#include <CL/cl.h>

int main() {

   /* OpenCL structures */
   cl_device_id device;
   cl_context context;
   cl_program program;
   cl_kernel kernel;
   cl_command_queue queue;
   cl_int i, j, err;
   size_t local_size, global_size;

   /* Data and buffers, also a space for ascii art to be a data check    */
   char data_in[ARRAY_SIZE] = "+------+.      +------+       +------+       +------+      .+------+\n"
                              "|`.    | `.    |\\     |\\      |      |      /|     /|    .' |    .'|\n"
                              "|  `+--+---+   | +----+-+     +------+     +-+----+ |   +---+--+'  |\n"
                              "|   |  |   |   | |    | |     |      |     | |    | |   |   |  |   |\n"
                              "+---+--+.  |   +-+----+ |     +------+     | +----+-+   |  .+--+---+\n"
                              " `. |    `.|    \\|     \\|     |      |     |/     |/    |.'    | .'\n"
                              "   `+------+     +------+     +------+     +------+     +------+'\n\n"
                              "   .+------+     +------+     +------+     +------+     +------+.\n"
                              " .' |    .'|    /|     /|     |      |     |\\     |\\    |`.    | `.\n"
                              "+---+--+'  |   +-+----+ |     +------+     | +----+-+   |  `+--+---+\n"
                              "|   |  |   |   | |    | |     |      |     | |    | |   |   |  |   |\n"
                              "|  ,+--+---+   | +----+-+     +------+     +-+----+ |   +---+--+   |\n"
                              "|.'    | .'    |/     |/      |      |      \\|     \\|    `. |   `. |\n"
                              "+------+'      +------+       +------+       +------+      `+------+";
   char data_out[ARRAY_SIZE];

   cl_test_obj test(data_in);
   cl_test_obj * out;

   cl_mem input_buffer, output_buffer, obj_buffer;
   cl_int num_groups;

   /* Create device and context 

   Creates a context containing only one device — the device structure 
   created earlier.
   */
   device = create_device();
   context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
   if(err < 0) {
      perror("Couldn't create a context");
      exit(1);   
   }

   /* Build program */
   program = build_program(context, device, PROGRAM_FILE);

   /* Create data buffer 

   • `global_size`: total number of work items that will be 
      executed on the GPU (e.g. total size of your array)
   • `local_size`: size of local workgroup. Each workgroup contains 
      several work items and goes to a compute unit 

   In this example, the kernel is executed by eight work-items divided into 
   two work-groups of four work-items each. Returning to my analogy, 
   this corresponds to a school containing eight students divided into 
   two classrooms of four students each.   

     Notes: 
   • Intel recommends workgroup size of 64-128. Often 128 is minimum to 
   get good performance on GPU
   • On NVIDIA Fermi, workgroup size must be at least 192 for full 
   utilization of cores
   • Optimal workgroup size differs across applications
   */
   global_size = 1024; // WHY ONLY 8?
   local_size = 1;
   input_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY |
         CL_MEM_COPY_HOST_PTR, ARRAY_SIZE * sizeof(char), data_in, &err); // <=====INPUT
   output_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE |
         CL_MEM_COPY_HOST_PTR, ARRAY_SIZE * sizeof(char), data_out, &err); // <=====OUTPUT
   obj_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE |
                                            CL_MEM_COPY_HOST_PTR, sizeof(cl_test_obj), out, &err); // <=====OUTPUT
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };

   /* Create a command queue

   Does not support profiling or out-of-order-execution
   */
   queue = clCreateCommandQueue(context, device, 0, &err);
   if(err < 0) {
      perror("Couldn't create a command queue");
      exit(1);   
   };

   /* Create a kernel */
   kernel = clCreateKernel(program, KERNEL_FUNC, &err);
   if(err < 0) {
      perror("Couldn't create a kernel");
      exit(1);
   };

   /* Create kernel arguments */
   err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_buffer); // <=====INPUT
   err |= clSetKernelArg(kernel, 1, local_size * sizeof(float), NULL);
   err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &output_buffer); // <=====OUTPUT
    err |= clSetKernelArg(kernel, 2, sizeof(cl_test_obj), &out); // <=====OUTPUT
   if(err < 0) {
      perror("Couldn't create a kernel argument");
      exit(1);
   }

   /* Enqueue kernel 

   At this point, the application has created all the data structures 
   (device, kernel, program, command queue, and context) needed by an 
   OpenCL host application. Now, it deploys the kernel to a device.

   Of the OpenCL functions that run on the host, clEnqueueNDRangeKernel 
   is probably the most important to understand. Not only does it deploy 
   kernels to devices, it also identifies how many work-items should 
   be generated to execute the kernel (global_size) and the number of 
   work-items in each work-group (local_size).
   */
   err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, 
         &local_size, 0, NULL, NULL); 
   if(err < 0) {
      perror("Couldn't enqueue the kernel");
      exit(1);
   }

   /* Read the kernel's output    */
   err = clEnqueueReadBuffer(queue, output_buffer, CL_TRUE, 0,
         sizeof(data_out), data_out, 0, NULL, NULL); // <=====GET OUTPUT
    err = clEnqueueReadBuffer(queue, obj_buffer, CL_TRUE, 0,
                              sizeof(out), out, 0, NULL, NULL); // <=====GET OUTPUT
   if(err < 0) {
      perror("Couldn't read the buffer");
      exit(1);
   }



   printf("%s \n", data_out);   //print all characters in debug string



   /* Deallocate resources */
   clReleaseKernel(kernel);
   clReleaseMemObject(output_buffer);
    clReleaseMemObject(obj_buffer);
   clReleaseMemObject(input_buffer);
   clReleaseCommandQueue(queue);
   clReleaseProgram(program);
   clReleaseContext(context);
   return 0;
}
