//
// Created by theo on 7/25/22.
//
#include "OpenCLheader.h"
#include <stdbool.h>

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


unsigned long getInput_d ( unsigned long *  input, unsigned long * processed_symbol, unsigned long * input_base, unsigned long inputID ){
    unsigned long base = input_base[inputID] ;
    unsigned long current =  processed_symbol[inputID];
    unsigned long current_input = input[base  + current];
  //uint32_t current_input = 99;
  return current_input;

}


bool compareSwitch_d( TPGPU* fst, unsigned long lhs, unsigned long rhs, unsigned long ** input,
          unsigned long* processed_symbol, unsigned long * input_base, unsigned long startVar){
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
      inputValue = getInput_d( input[rhs-INPUT_MATCH], processed_symbol, input_base, rhs -INPUT_MATCH);
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
      inputValue = getInput_d( input[rhs-INPUT_MATCH- ALPHABETSIZE], processed_symbol, input_base, rhs - INPUT_MATCH - ALPHABETSIZE);
      if ( lhs != inputValue) match = true;
      break;
      }
    return match;

}

unsigned long transition_eval_d(TPGPU * fst, unsigned long currentState, unsigned long startVar, unsigned long **input,
                                unsigned long *input_base, unsigned long *processed_symbol){
    unsigned long baseID = fst->stateList[currentState].baseID;
    unsigned long numberofTransition = fst->stateList[currentState].numberofTransition;
    unsigned long returnID = fst->transitionCount;
  for ( unsigned long i = baseID; i < (baseID + numberofTransition); i++){
    // input
    bool inputMatch = false;
    if ( fst->transitionList[i].inputID != EPSILON_MATCH){
      // decode left-hand side
        unsigned long lhs = getInput_d(input[ fst->transitionList[i].inputID- INPUT_MATCH], processed_symbol, input_base, fst->transitionList[i].inputID- INPUT_MATCH);
      /*
      printf(" Transition %d :", i);
      printf(" left hand side = %d ;", lhs);
      printf(" right hand side = %d \n", fst->transitionList[i].inputSymbol);
      */
      // call compare to right-hand side
      inputMatch = compareSwitch_d ( fst, lhs,fst->transitionList[i].inputSymbol, input, processed_symbol, input_base, startVar);
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
      varMatch = compareSwitch_d ( fst, lhs,fst->transitionList[i].inputVar , input, processed_symbol, input_base, startVar);
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


unsigned long translateOutput_d ( TPGPU * fst,  unsigned long  ** input, unsigned long * processed_symbol,
                   unsigned long * input_base,  unsigned long outputVar, unsigned long startVar){
    unsigned long character = 1000; // illegal value

  if ( outputVar < ALPHABETSIZE)
    character = outputVar;
  else if (( outputVar >=ALPHABETSIZE) && ( outputVar <= 2*ALPHABETSIZE))
    return character;
  else if ( (outputVar > 2*(ALPHABETSIZE)) &&( outputVar < (2*ALPHABETSIZE + MAXVAR)) ){
    character = fst->var[startVar + outputVar - 2*ALPHABETSIZE -1 ];

  }
  else if ( outputVar >= 2 * ALPHABETSIZE + MAXVAR +1){
    character = getInput_d( input[outputVar- INPUT_MATCH], processed_symbol, input_base, outputVar - INPUT_MATCH);
    //printf("Write to output %d: %d\n", outputVar-INPUT_MATCH, character);
  }
  return character;
}


unsigned long transition_write_d( TPGPU * fst, unsigned long   chosenTransition, unsigned long           startVar,
                      unsigned long **  input, unsigned long *       input_base, unsigned long * processed_symbol,
                      unsigned long ** output, unsigned long * simulated_output, unsigned long *      outputCount,
                      unsigned long *output_base){
  if( chosenTransition == fst->transitionCount)
    return fst->stateCount;

  // write to variable
  if ( fst->transitionList[chosenTransition].OvarID != EPSILON_MATCH){
      unsigned long outputVar = startVar+ fst->transitionList[chosenTransition].OvarID - 2 * ALPHABETSIZE - 1;
    //printf("Write Var %d \n", outputVar);
    fst->var[outputVar]  =
      translateOutput_d ( fst, input, processed_symbol,input_base, fst->transitionList[chosenTransition].outputVar,startVar);
  }
  // write to output

  if ( fst->transitionList[chosenTransition].outputID != EPSILON_MATCH){
      unsigned long output_dst = fst->transitionList[chosenTransition].outputID - OUTPUT_START;
#ifdef DEBUG_GPU
    output[output_dst][output_base[output_dst] + outputCount[output_dst]]  =
#else
  simulated_output[output_dst]  =
 // uint32_t tmp =
#endif
    //output[0][0]  =
    //0;
      translateOutput_d ( fst, input, processed_symbol,input_base, fst->transitionList[chosenTransition].outputSymbol, startVar);
    outputCount[output_dst] ++;
   // printf("AFTER Write output  %d \n", tmp);
    //printf(" From %d ", output_base[output_dst]);
    //printf("To: %d\n",outputCount[output_dst]);

  }

  //actiavate next State
  return fst->transitionList[chosenTransition].nextState;
}


bool is_done_processed ( unsigned long * current, unsigned long * expected, unsigned long number_of_input){
  for ( int i = 0; i < number_of_input; i++){
    //printf(" Current %d: %d| Expected %d\n", i,  current[i] , expected[i]);
    if ( current[i] < expected[i])
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


void process_k( TPGPU * fst, unsigned long **            input_base, unsigned long **      partition_length,
                             unsigned long **      processed_symbol, unsigned long **                 input,
                             unsigned long ** partition_output_base, unsigned long** partition_output_count,
                             unsigned long **                output, unsigned long **      simulated_output ){
    unsigned long globalId = get_global_id(0);
    unsigned long total_thread = GLOBAL_SIZE * LOCAL_SIZE;
    unsigned long startVar = globalId * fst->varCount;
    unsigned long currentState = 0;
    unsigned long chosenTransition = fst->transitionCount;
    bool done = false;
    unsigned long cycle= 0;
    unsigned long t = 0;
#ifdef DEBUG_GPU
    if ( globalId == 2){
    for ( ulong i = 0; i < fst->inputCount; i++){
      printf(" %d, ", input_base[globalId][i]);
      printf(" %d| ", partition_length[globalId][i]);
    }
    for ( ulong i = 0; i < fst->outputCount; i++){
      printf(" %d, ", partition_output_base[globalId][i]);
      printf(" %d, ", partition_output_count[globalId][i]);
    }
    printf("\nSTART EXECUTION on var %d\n", startVar);
  }
#endif
#ifdef DEBUG_GPU
    ulong chosen_thread =1;
  while ( cycle < 10){

    cycle++;
    __syncthreads();
    if ( globalId == chosen_thread){
      printf("-------------%d-----------------\n", cycle);
      printf( " Activated State: %d\n", currentState);
      for ( ulong v = startVar; v < startVar + fst->varCount; v++)
        printf( "V[%d] = %u | ", v, fst->var[v]);
      printf("\n");

      for (ulong p = 0; p < fst->inputCount; p++){
        printf("Input[%d]: ", p);
        for (ulong q = input_base[globalId][p]; q < input_base[globalId][p] + 20; q++){
          printf("%d, ", input[p][q]);
        }

        printf("\n");
        printf("Current[%d]:  %d - %d total %d \n",p,  input_base[globalId][p], processed_symbol[globalId][p], partition_length[globalId][p]);
        for (ulong k = input_base[globalId][p]; k <= input_base[globalId][p] + processed_symbol[globalId][p]; k++){
          printf("%u, ", input[p][k]);
        }
        printf("\n");
      }
      for (ulong p = 0; p < fst->outputCount; p++){
        printf("OutPut[%d]: ", p);
        for (ulong k = partition_output_base[globalId][p]; k < partition_output_base[globalId][p] + partition_output_count[globalId][p]; k++){
          printf("%u, ", output[p][k]);
        }
        printf("\n");
      }
    }
    __syncthreads();
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
        chosenTransition = transition_eval_d( fst, currentState, startVar,  input, input_base[globalId], processed_symbol[globalId]);
        // transition write
#ifdef DEBUG_GPU
        if ( globalId == chosen_thread){
      printf("Chosen Transition %d \n", chosenTransition);
      printf("%d-%d, %d-%d, %d |", fst->transitionList[chosenTransition].inputID, fst->transitionList[chosenTransition].inputSymbol, fst->transitionList[chosenTransition].IvarID,fst->transitionList[chosenTransition].inputVar, fst->transitionList[chosenTransition].currentState);
      printf("%d-%d, %d-%d, %d \n", fst->transitionList[chosenTransition].outputID, fst->transitionList[chosenTransition].outputSymbol, fst->transitionList[chosenTransition].OvarID,fst->transitionList[chosenTransition].outputVar, fst->transitionList[chosenTransition].nextState);
      printf( "%d\n" ,startVar+ fst->transitionList[chosenTransition].OvarID - 2 * ALPHABETSIZE - 1);
      printf("Writing from output %d :  %d to %d\n", fst->transitionList[chosenTransition].outputID - OUTPUT_START, partition_output_base[globalId][0], partition_output_count[globalId][0]);
    }
    __syncthreads();
#endif
        currentState =
                transition_write_d(fst, chosenTransition, startVar,input,input_base[globalId],  processed_symbol[globalId],  output,simulated_output[globalId],  partition_output_count[globalId], partition_output_base[globalId]);
#ifdef DEBUG_GPU
        __syncthreads();
    if ( globalId == chosen_thread)
      printf( " Next State = %d\n", currentState);
#endif
        // consumed input and deactivate transition
        if (currentState == fst->stateCount){
            done = true;
        }
        else {
            if( fst->transitionList[chosenTransition].inputID != EPSILON_MATCH) {
                //  printf(" Increament Input  %d from %d\n", fst->transitionList[chosenTransition].inputID-INPUT_MATCH, processed_symbol[globalId][fst->transitionList[chosenTransition].inputID-INPUT_MATCH]);
                processed_symbol[globalId][fst->transitionList[chosenTransition].inputID-INPUT_MATCH]++;
                // printf(" to %d\n", processed_symbol[globalId][fst->transitionList[chosenTransition].inputID-INPUT_MATCH]);
                chosenTransition = fst->transitionCount;
            }
//        printf("T: %d : \n", globalId);;
            done = is_done_processed( processed_symbol[globalId], partition_length[globalId], fst->inputCount);
        }
        // printf( "done: %d, current State : %d\n", done, currentState);

    }
/*
  if ( globalId == 0)
    printf("Thread %d finished with %d cycle, %d output \n",  globalId, cycle, partition_output_count[globalId][0]);
*/
#ifdef DEBUG_GPU
    if ( globalId == chosen_thread){
    printf("Thread %d finished with %d cycle, %d output \n",  globalId, cycle, partition_output_count[globalId][0]);
    printf( " total Thread %d \n",  total_thread );
    for ( ulong j = 0; j < fst->inputCount; j++){
      printf("T%d:  %d, ",globalId, partition_output_base[globalId][j]);
      printf(" %d|  ", partition_output_count[globalId][j]);
    }
  }
#endif
}
