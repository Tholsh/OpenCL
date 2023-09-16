#include <CL/cl.h>
#include <string.h>
#define MAX_LENGTH 1024

cl_device_id create_device();
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename);

class cl_test_obj{
public:
    uint32_t length;
    char str[MAX_LENGTH];
    cl_test_obj(char * str_in){
        length = strlen(str_in);
        if(length <= MAX_LENGTH) strcpy(str, str_in);
    }
    cl_test_obj(){}
};