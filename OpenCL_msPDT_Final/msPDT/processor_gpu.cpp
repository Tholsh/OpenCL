//
// Created by theo on 7/26/22.
//
#include "processor.h"

void TP::loadFST_GPU( TPGPU * h_processor, uint32_t blocks, uint32_t threads){

    h_processor->stateCount = stateCount;
    h_processor->transitionCount = transitionCount;
    h_processor->varCount = varCount;
    h_processor->inputCount = inputCount;
    h_processor->outputCount = outputCount;

}
