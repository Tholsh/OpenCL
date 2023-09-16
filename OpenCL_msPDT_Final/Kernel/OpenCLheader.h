//
// Created by theo on 7/22/22.
//
#ifndef OPENCL_MSPDT_VARIADIC_OPENCLHEADER_H
#define OPENCL_MSPDT_VARIADIC_OPENCLHEADER_H


#endif //OPENCL_MSPDT_VARIADIC_OPENCLHEADER_H
#include "OpenCLstructs.h"

#define DEBUG_GPU 1
//#undef DEBUG_GPU

void process_k( TPGPU * fst, twoD_thread_array  partition_length, twoD_thread_array input_base,
                twoD_thread_array partition_input_current, twoD_thread_array partition_output_base,
                twoD_thread_array partition_output_count, twoD_thread_array       simulated_output,
                twoD_array              output,         twoD_array               input,
                unsigned long         globalIdentifier,  unsigned long total_thread);

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


void emplace_S(unsigned long val, unsigned long x, unsigned long y, twoD_array outStruct){
    unsigned long offset = 0;
    if(x >= outStruct.num_inputs) {printf("ERROR - OUT OF BOUNDS ACCESS emplace S, x= %u; y = %u\n", x, y); return;}
    for(int i = 0; i < outStruct.num_inputs; i++){
        if(i == x) {
            outStruct.input[offset + y] = val;
            return;
        }
        offset += outStruct.input_lengths[i];
    }
    printf("emplace_S funky behavior, offset = %u \n", offset);
}

unsigned long in_place_S(unsigned long x, unsigned long y, twoD_array inStruct){
    unsigned long offset = 0;
    if(x >= inStruct.num_inputs) {return *(inStruct.input);}
    for(unsigned short i = 0; i < inStruct.num_inputs; i++){
        if(i == x) {return inStruct.input[offset + y];}
        offset += inStruct.input_lengths[i];
    }
    printf("in_place funky behavior, offset = %u \n", offset);
    return inStruct.input[offset];
}

unsigned long in_place_thread(unsigned long x, unsigned long y, twoD_thread_array input){
    //if(x > input.total_threads) {printf("ERROR - OUT OF BOUNDS ACCESS in place thread read OVER TOTAL THREAD, x = %u:,\n", x); return *(input.input);}  //out of bounds
    if(y >= input.num_inputs) {printf("ERROR - OUT OF BOUNDS ACCESS in place thread read OVER NUM INPUTS,  y = %hu:,\n",y); return *(input.input);}  //out of bounds
    return input.input[(y * input.total_threads) + x];
}

unsigned long in_place(unsigned long x, unsigned long y, unsigned long * input, unsigned long num_inputs, unsigned long total_threads){
    //if(x > input.total_threads) {printf("ERROR - OUT OF BOUNDS ACCESS in place thread read OVER TOTAL THREAD, x = %u:,\n", x); return *(input.input);}  //out of bounds
    if((unsigned) y >= num_inputs) {return 0;}  //out of bounds, so output zero. according to logic.
    return input[(y * total_threads) + x];
}

void emplace_thread(unsigned long val, unsigned long x, unsigned long y,
                    unsigned long * input, unsigned long num_input, unsigned long total_thread){
    if(y >= num_input) {printf("ERROR - OUT OF BOUNDS ACCESS thread write: X = %u, Y = %u, num_inputs = %hu\n", x, y, num_input); input[x] = val; return;}  //broken! FIX!!
    //if(x >= input.total_threads) {printf("ERROR - OUT OF BOUNDS ACCESS: X = %u, Y = %u\n", x,y); return;}  //out of bounds
    input[(y * total_thread) + x] = val;
}



void ExecuteAction_d (TPGPU * fst, unsigned long src1, unsigned long src2,
                      unsigned long dst, unsigned long  opt, unsigned long startVar){
    unsigned long src1Value, src2Value;
    switch (opt){
        case ADD:
            src1Value = (fst->var[src1 + startVar]);
            src2Value = (fst->var[src2+ startVar]);
            fst->var[dst + startVar] = src1Value + src2Value;
            break;
        case ADDI:
            src1Value = (fst->var[src1 + startVar]);
            src2Value = src2;
            fst->var[dst + startVar] = src1Value + src2Value;
            break;
        case SUB:
            src1Value = (fst->var[src1 + startVar]);
            src2Value = (fst->var[src2+ startVar]);
            fst->var[dst + startVar] = src1Value - src2Value;
            break;
        case SUBI:
            src1Value = (fst->var[src1 + startVar]);
            src2Value = src2;
            fst->var[dst + startVar] = src1Value - src2Value;
            break;
        case MUL:
            src1Value = (fst->var[src1 + startVar]);
            src2Value = (fst->var[src2+ startVar]);
            fst->var[dst + startVar] = src1Value * src2Value;
            break;
        case MULI:
            src1Value = (fst->var[src1 + startVar]);
            src2Value = src2;
            fst->var[dst + startVar] = src1Value * src2Value;
            break;
        case DIV:
            src1Value = (fst->var[src1 + startVar]);
            src2Value = (fst->var[src2+ startVar]);
            fst->var[dst + startVar] = src1Value / src2Value;
            break;
        case DIVI:
            src1Value = (fst->var[src1 + startVar]);
            src2Value = src2;
            fst->var[dst + startVar] = src1Value / src2Value;
            break;
        case LSHIFT:
            src1Value = (fst->var[src1 + startVar]);
            src2Value = (fst->var[src2+ startVar]);
            fst->var[dst + startVar] = src1Value << src2Value;
            break;
        case LSHIFTI:
            src1Value = (fst->var[src1 + startVar]);
            src2Value = src2;
            fst->var[dst + startVar] = src1Value << src2Value;
            break;
        case RSHIFT:
            src1Value = (fst->var[src1 + startVar]);
            src2Value = (fst->var[src2+ startVar]);
            fst->var[dst + startVar] = src1Value >> src2Value;
            break;
        case RSHIFTI:
            src1Value = (fst->var[src1 + startVar]);
            src2Value = src2;
            fst->var[dst + startVar] = src1Value >> src2Value;
            break;
        case OR:
            src1Value = (fst->var[src1 + startVar]);
            src2Value = (fst->var[src2+ startVar]);
            fst->var[dst + startVar] = src1Value | src2Value;
            break;
        case ORI:
            src1Value = (fst->var[src1 + startVar]);
            src2Value = src2;
            fst->var[dst + startVar] = src1Value | src2Value;
            break;
        case AND:
            src1Value = (fst->var[src1 + startVar]);
            src2Value = (fst->var[src2+ startVar]);
            fst->var[dst + startVar] = src1Value & src2Value;
            break;
        case ANDI:
            src1Value = (fst->var[src1 + startVar]);
            src2Value = src2;
            fst->var[dst + startVar] = src1Value & src2Value;
            break;
        case EPSILON:
        default:
            break;
    }
}


void state_action_d ( TPGPU * fst, unsigned long  currentState, unsigned long startVar){
    unsigned long src1 = fst->stateList[currentState].src1 ;
    unsigned long src2 = fst->stateList[currentState].src2 ;
    unsigned long dst = fst->stateList[currentState].dst ;
    unsigned long opt = fst->stateList[currentState].opt;
    ExecuteAction_d ( fst, src1, src2, dst, opt, startVar);
}


unsigned long getInput_d (twoD_array input, twoD_thread_array processed_symbol, twoD_thread_array input_base, long int inputID, unsigned long globalId, unsigned long inputCount, unsigned long total_thread){
    unsigned long base = in_place(globalId, inputID, input_base.input, inputCount, total_thread); //element of
    unsigned long current =  in_place(globalId, inputID, processed_symbol.input, inputCount, total_thread); //element of no of thread
    unsigned long current_input = in_place_S(inputID, (base + current), input);
    //uint32_t current_input = 99;
    return current_input;

}


bool compareSwitch_d( TPGPU * fst, unsigned long lhs, unsigned long rhs, twoD_array input,
                      twoD_thread_array processed_symbol, twoD_thread_array input_base, unsigned long startVar, unsigned long globalId,
                      unsigned long inputCount, unsigned long total_thread){
    int condition = ILL;
    // alphabet match:  0-256
    if ( rhs < ALPHABETSIZE) {
        //printf(" alphabet match\n");
        condition = ALPHABET;
    }
        // negation alphabet  match: 256-511
    else if ( (rhs >= ALPHABETSIZE) && ( rhs < (2 * ALPHABETSIZE)) ) {
        condition = NEGALPHABET;
    }
        // EPSILON match: 512
    else if ( rhs == (2 * ALPHABETSIZE)) {
        condition = EPS;
    }
        // Variable match: 513-576
    else if (( rhs >  (2 * ALPHABETSIZE)) && ( rhs <= (2 * (ALPHABETSIZE) + MAXVAR))) {
        condition = VAR;
    }
        //  Input Match:  577-640
    else if( ( rhs >= INPUT_MATCH) && ( rhs <( INPUT_MATCH + MAXVAR))) {
        condition = INPT;
    }
        //  Passthrough Match:  641
    else if ( rhs == ANY_MATCH) {
        condition = PASSTHROUGH;
    }
        // negation variable match:  769-832
    else if (( rhs >  (3 * ALPHABETSIZE)) && ( rhs <= (3 * ALPHABETSIZE + MAXVAR))) {
        condition = NEGVAR;
    }
        // Negation Input Match:  833-896
    else if( ( rhs >=( INPUT_MATCH + ALPHABETSIZE)) && ( rhs <( INPUT_MATCH+ ALPHABETSIZE+ MAXVAR))) {
        condition = NEGINPT;
    }

    unsigned long varValue, inputValue;
    bool match = false;
    switch (condition)  {
        case  ALPHABET:
            if ( lhs == rhs) match = true;
            break;
        case  NEGALPHABET:
            if ( lhs  != (rhs- ALPHABETSIZE )) match = true;
            break;
        case  EPS:
            match = true;
            break;
        case  VAR:
            varValue = fst->var[rhs- 2*ALPHABETSIZE + startVar -1 ];
            if ( lhs == varValue) match = true;
            break;
        case  INPT:
            inputValue = getInput_d( input, processed_symbol, input_base, rhs - INPUT_MATCH, globalId, inputCount, total_thread);
            if ( lhs == inputValue) match = true;
            break;
        case  PASSTHROUGH:
            match = true;
            break;
        case  NEGVAR:
            varValue = fst->var[rhs- 3*ALPHABETSIZE -1 + startVar];
            if ( lhs != varValue) match = true;
            break;
        case  NEGINPT:
            inputValue = getInput_d( input, processed_symbol, input_base, rhs - INPUT_MATCH - ALPHABETSIZE, globalId, inputCount, total_thread);
            if ( lhs != inputValue) match = true;
            break;
    }
    return match;

}

unsigned long transition_eval_d(TPGPU * fst, unsigned long currentState, unsigned long startVar, twoD_array input,
                                twoD_thread_array input_base, twoD_thread_array processed_symbol, unsigned long globalId,
                                unsigned long inputCount, unsigned long total_thread){
    //input base and processed symbol have been altered to take global id and input on final func.
    //this kernel is going to need so so so much cleanup
    unsigned long baseID = fst->stateList[currentState].baseID;
    unsigned long numberofTransition = fst->stateList[currentState].numberofTransition;
    unsigned long returnID = fst->transitionCount;
    for ( unsigned long i = baseID; i < (baseID + numberofTransition); i++){
        // input
        bool inputMatch = false;
        if ( fst->transitionList[i].inputID != EPSILON_MATCH){
            // decode left-hand side
            //chg input and proc symb and in base
            unsigned long lhs = getInput_d(input, processed_symbol, input_base, fst->transitionList[i].inputID - INPUT_MATCH, globalId, inputCount, total_thread); //given input in func so carry and run
/*
            printf(" Transition %d :", i);
            printf(" left hand side = %d ;", lhs);
            printf(" right hand side = %u \n", fst->transitionList[i].inputSymbol);
            printf("inputID = %d \n", fst->transitionList[0].inputID);
            printf("number of transition = %u , baseID = %d, currentState = %u\n", numberofTransition, fst->stateList[currentState].baseID, currentState);
*/
            // call compare to right-hand side
            inputMatch = compareSwitch_d ( fst, lhs,fst->transitionList[i].inputSymbol, input, processed_symbol, input_base, startVar, globalId, inputCount, total_thread);
        }
        else {
            inputMatch = true;
        }
        // variable
        bool varMatch = false;
        if ( fst->transitionList[i].IvarID != EPSILON_MATCH){
            // decode left-hand side
            unsigned long lshVar = fst->transitionList[i].IvarID + startVar - 2 *ALPHABETSIZE -1;
            unsigned long lhs = fst->var[lshVar];
            // call compare to right-hand side
            varMatch = compareSwitch_d ( fst, lhs,fst->transitionList[i].inputVar , input, processed_symbol, input_base, startVar, globalId, inputCount, total_thread);
        }
        else {
            varMatch = true;
        }

        if ( inputMatch && varMatch){
            //printf(" Match transition %d\n", i);
            return i;
            //returnID = i;
        }
    }
    return returnID;
}


unsigned long translateOutput_d ( TPGPU * fst,  twoD_array  input, twoD_thread_array processed_symbol,
                                  twoD_thread_array input_base,  unsigned long outputVar, unsigned long startVar, unsigned long globalId,
                                  unsigned long inputCount, unsigned long total_thread){
    unsigned long character = 1000; // illegal value

    if ( outputVar < ALPHABETSIZE)
        character = outputVar;
    else if (( outputVar >=ALPHABETSIZE) && ( outputVar <= 2*ALPHABETSIZE))
        return character;
    else if ( (outputVar > 2*(ALPHABETSIZE)) &&( outputVar < (2*ALPHABETSIZE + MAXVAR)) ){
        character = fst->var[startVar + outputVar - 2*ALPHABETSIZE -1 ];

    }
    else if ( outputVar >= 2 * ALPHABETSIZE + MAXVAR +1){
        character = getInput_d(input, processed_symbol, input_base, outputVar - INPUT_MATCH, globalId, inputCount, total_thread);
        //printf("Write to output %d: %d\n", outputVar-INPUT_MATCH, character);
    }
    return character;
}


unsigned long transition_write_d( TPGPU * fst, unsigned long   chosenTransition, unsigned long           startVar,
                                  twoD_array  input, twoD_thread_array       input_base, twoD_thread_array processed_symbol,
                                  twoD_array  output, twoD_thread_array simulated_output, twoD_thread_array      outputCount,
                                  twoD_thread_array output_base, unsigned long globalId, unsigned long num_outputs,
                                  unsigned long total_thread){

    if( chosenTransition == fst->transitionCount)
        return fst->stateCount;

    // write to variable
    if ( fst->transitionList[chosenTransition].OvarID != EPSILON_MATCH){
        unsigned long outputVar = startVar+ fst->transitionList[chosenTransition].OvarID - 2 * ALPHABETSIZE - 1;
        //printf("Write Var %d \n", outputVar);
        fst->var[outputVar]  =
                translateOutput_d ( fst, input, processed_symbol,input_base, fst->transitionList[chosenTransition].outputVar,startVar, globalId, input.num_inputs, total_thread);
    }
    // write to output

    if ( fst->transitionList[chosenTransition].outputID != EPSILON_MATCH){
        unsigned long output_dst = fst->transitionList[chosenTransition].outputID - OUTPUT_START;

        unsigned long out = translateOutput_d ( fst, input, processed_symbol,input_base, fst->transitionList[chosenTransition].outputSymbol, startVar, globalId, input.num_inputs, total_thread);
#ifdef DEBUG_GPU
        emplace_S(out, output_dst, (in_place(globalId, output_dst, output_base.input, num_outputs, total_thread) + outputCount.input[output_dst]), output);
#else
                simulated_output.input[output_dst] = out;
 // uint32_t tmp =
#endif
                //output[0][0]  =
                //0;
        outputCount.input[output_dst] ++;
        // printf("AFTER Write output  %d \n", tmp);
        //printf(" From %d ", output_base[output_dst]);
        //printf("To: %d\n",outputCount[output_dst]);

    }

    //actiavate next State
    return fst->transitionList[chosenTransition].nextState;

}

//partition symbol         //partition length
bool is_done_processed ( twoD_thread_array current, twoD_thread_array expected, unsigned long number_of_input, unsigned long globalId){
    for (unsigned short i = 0; i < number_of_input; i++){
        //printf(" Current %d: %d| Expected %d\n", i,  current[i] , expected[i]);
        if (in_place(globalId, i, current.input, number_of_input, get_global_size(0)) < in_place(globalId, i, current.input, number_of_input, get_global_size(0)))
            return false;
    }
    return true;
}

int testk ( unsigned long testarray){
    return ++testarray;
}


//////////////////
// main processing function
// partition_length: number of input symbol each stream have to processed
// input_base: element that each stream have to processed from
// processed_symbol: number of input each stream have processed
// partition_output_base: element each stream have to process from
// output_count: number of symbol have been written to each stream


void process_k( TPGPU * fst, twoD_thread_array  partition_length, twoD_thread_array input_base,
                twoD_thread_array processed_symbol, twoD_thread_array partition_output_base,
                twoD_thread_array partition_output_count, twoD_thread_array       simulated_output,
                twoD_array              output,         twoD_array               input,
                unsigned long         globalIdentifier, unsigned long total_thread){
    unsigned long globalId = globalIdentifier;
    unsigned long startVar = globalId * fst->varCount;
    unsigned long currentState = 0;
    unsigned long inputCount = fst->inputCount;
    unsigned long outputCount = fst->outputCount;
    unsigned long chosenTransition = fst->transitionCount;
    bool done = false;
    unsigned long cycle= 0;
    unsigned long t = 0;




#ifdef DEBUG_GPU
    if ( globalId == 2){
        printf("inputs: %u", partition_length.num_inputs);
        printf("inputs: %u", input_base.num_inputs);
        printf("inputs: %u", partition_length.num_inputs);
        for (unsigned int i = 0; i < inputCount; i++){
            printf(" %d, ", in_place(globalId, i, input_base.input, inputCount, total_thread));
            printf(" %d| ", in_place(globalId, i, partition_length.input, inputCount, total_thread));
        }
        for (unsigned int i = 0; i < outputCount; i++){
            printf(" %d, ", in_place(globalId, i, partition_output_base.input, outputCount, total_thread));
            printf(" %d, ", in_place(globalId, i, partition_output_count.input, outputCount, total_thread));
        }
        printf("\nSTART EXECUTION on var %d\n", startVar);
    }
#endif

#ifdef DEBUG_GPU

    unsigned long chosen_thread = 1;


    while ( cycle < 10){

        cycle++;

        barrier(CLK_LOCAL_MEM_FENCE);
        if ( globalId == chosen_thread){


            printf("-------------%d-----------------\n", cycle);
            printf( " Activated State: %d\n", currentState);
            for ( unsigned long v = startVar; v < startVar + fst->varCount; v++)
                printf( "V[%d] = %u | ", v, fst->var[v]);
            printf("\n");

            for (unsigned int p = 0; p < inputCount; p++){
                printf("Input[%d]: ", p);
                unsigned long in_base_p = in_place(globalId, p, input_base.input, inputCount, total_thread);
                printf("In_base_p =%u \n", in_base_p);
                for (unsigned int q = in_base_p; q < (in_base_p + 20); q++){
                    printf("%d, ", in_place_S(p,q,input));
                }

                printf("\n");
                printf("Current[%d]:  %d - %d total %d \n",p,  in_place(globalId, p, input_base.input, inputCount, total_thread), in_place(globalId, p, processed_symbol.input, inputCount, total_thread), in_place(globalId, p, partition_length.input, inputCount, total_thread));
                for (unsigned int k = in_place(globalId, p, input_base.input, inputCount, total_thread); k <= in_place(globalId, p, input_base.input, inputCount, total_thread) + in_place(globalId, p, processed_symbol.input, inputCount, total_thread); k++){
                    printf("%u, ", in_place_S(p,k,input));
                }
                printf("\n");
            }

            for (unsigned short p = 0; p < outputCount; p++){
                printf("OutPut[%d]: ", p);
                for (unsigned int k = in_place(globalId, p, partition_output_base.input, outputCount, total_thread); k < in_place(globalId, p, partition_output_base.input, outputCount, total_thread) + in_place(globalId, p, partition_output_count.input, outputCount, total_thread); k++){
                    printf("%u, ", in_place_S(p,k,output));
                }
                printf("\n");

            }

        }

        barrier(CLK_LOCAL_MEM_FENCE);
        //printf("Rerform Action\n");

#else
        while (( !done) && (currentState != fst->stateCount)){
#endif

        // state action
        state_action_d ( fst, currentState, startVar);
        // transition evaluate
        unsigned long baseID = fst->stateList[currentState].baseID;
        unsigned long numberofTransition = fst->stateList[currentState].numberofTransition;
        //printf(" Considering Transition %d to %d\n", baseID, baseID+ numberofTransition -1 );
        chosenTransition = transition_eval_d( fst, currentState, startVar,  input, input_base, processed_symbol, globalId, inputCount, total_thread);
        // transition write

#ifdef DEBUG_GPU
        if ( globalId == chosen_thread){
            printf("Chosen Transition %d \n", chosenTransition);
            printf("%d-%d, %d-%d, %d |", fst->transitionList[chosenTransition].inputID, fst->transitionList[chosenTransition].inputSymbol, fst->transitionList[chosenTransition].IvarID,fst->transitionList[chosenTransition].inputVar, fst->transitionList[chosenTransition].currentState);
            printf("%d-%d, %d-%d, %d \n", fst->transitionList[chosenTransition].outputID, fst->transitionList[chosenTransition].outputSymbol, fst->transitionList[chosenTransition].OvarID,fst->transitionList[chosenTransition].outputVar, fst->transitionList[chosenTransition].nextState);
            printf( "%d\n" ,startVar+ fst->transitionList[chosenTransition].OvarID - 2 * ALPHABETSIZE - 1);
            printf("Writing from output %d :  %d to %d\n", fst->transitionList[chosenTransition].outputID - OUTPUT_START, in_place(globalId, 0,  partition_output_base.input, outputCount, total_thread), in_place(globalId, 0, partition_output_count.input, outputCount, total_thread));
        }
        barrier(CLK_LOCAL_MEM_FENCE);

#endif


        currentState =
                transition_write_d(fst, chosenTransition, startVar,input , input_base,  processed_symbol,  output,simulated_output,  partition_output_count, partition_output_base, globalId, outputCount, total_thread);

#ifdef DEBUG_GPU

        barrier(CLK_LOCAL_MEM_FENCE);
        if ( globalId == chosen_thread)
            printf( " Next State = %d\n", currentState);

#endif


        // consumed input and deactivate transition
        if (currentState == fst->stateCount){
            done = true;
            printf("kernel #%u has finished \n", globalId);
        }
        else {
            if( fst->transitionList[chosenTransition].inputID != EPSILON_MATCH) {
                //  printf(" Increament Input  %d from %d\n", fst->transitionList[chosenTransition].inputID-INPUT_MATCH, processed_symbol[globalId][fst->transitionList[chosenTransition].inputID-INPUT_MATCH]);
                //processed_symbol(index)++... \/
                       unsigned long tmp = in_place(globalId, fst->transitionList[chosenTransition].inputID-INPUT_MATCH, processed_symbol.input, inputCount, total_thread);                        //Debug!
                emplace_thread(tmp++, globalId, fst->transitionList[chosenTransition].inputID-INPUT_MATCH, processed_symbol.input, inputCount, total_thread);

                // printf(" to %d\n", processed_symbol[globalId][fst->transitionList[chosenTransition].inputID-INPUT_MATCH]);
                chosenTransition = fst->transitionCount;
            }
//        printf("T: %d : \n", globalId);;
            done = is_done_processed(processed_symbol, partition_length, inputCount, globalId);
        }
       // printf( "done: %d, current State : %d\n", done, currentState);


    }

  if ( globalId == 0)
    printf("Thread %d finished with %d cycle, %d output \n",  globalId, cycle, in_place(globalId, 0, partition_output_count.input, outputCount, total_thread));
#ifdef DEBUG_GPU
    if ( globalId == chosen_thread){
        printf("Thread %d finished with %d cycle, %d output \n",  globalId, cycle, in_place(globalId, 0, partition_output_count.input, outputCount, total_thread));
        printf( " total Thread %d \n",  total_thread );
        for (unsigned long j = 0; j < inputCount; j++){
            printf("T%d:  %d, ",globalId, in_place(globalId, j, partition_output_base.input, outputCount, total_thread));
            printf(" %d|  ", in_place(globalId, j, partition_output_count.input, inputCount, total_thread));
        }
    }
#endif

}
