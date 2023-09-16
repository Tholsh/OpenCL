//
// Created by theo on 7/22/22.
//

#ifndef OPENCL_MSPDT_VARIADIC_ARGUMENT_H
#define OPENCL_MSPDT_VARIADIC_ARGUMENT_H

#endif //OPENCL_MSPDT_VARIADIC_ARGUMENT_H

// initialize by reading a config file
//#pragma once
#include  <cstring>
#include  <string>
#include  <stdlib.h>
#include  <stdio.h>
#include  <fstream>
#include  <iostream>
#include  <assert.h>
#include "helper.h"

class ARGUMENT {
public:
    // helper config
    int number_of_input;
    std::string *  InputFile;
    int number_of_output;
    std::string *  OutputFile;
    std::string benchmark_str;
    int benchmark_int;

    helper  * config;

    ~ARGUMENT(){
        delete(config);
        free(InputFile);
        free(OutputFile);
    }
    ARGUMENT( std::string configFile, uint32_t inputsize);
    int benchmarkConvert  ( std::string testname);
    void TestInfo( uint32_t inputsize);
};
