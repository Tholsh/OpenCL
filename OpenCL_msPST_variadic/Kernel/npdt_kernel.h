//
// Created by theo on 7/22/22.
//

#ifndef OPENCL_MSPDT_VARIADIC_NPDT_H
#define OPENCL_MSPDT_VARIADIC_NPDT_H

#endif //OPENCL_MSPDT_VARIADIC_NPDT_H
#define MAXVAR 64

#define ALPHABETSIZE 256
#define EPSILON_MATCH 512

#define VARSTART  EPSILON_MATCH  + 1
#define INPUTSTART VARSTART + MAXVAR
#define ANY_MATCH  641
#define OUTPUTSTART ANY_MATCH + 1

#define ANY_PUSH  ANY_MATCH + 256
#define INPUT_MATCH  577
#define NOT_INPUT INPUT_MATCH + 256

#define OUTPUT_START 642
enum OPERATION {  EPSILON = 0,
    ADD     = 1,
    ADDI    = 2,
    SUB     = 3,
    SUBI    = 4,
    MUL     = 5,
    MULI    = 6,
    DIV     = 7,
    DIVI    = 8,
    SET     = 9,
    POP     = 10,
    LSHIFT  = 11,
    LSHIFTI = 12,
    RSHIFT  = 13,
    RSHIFTI = 14,
    OR      = 15,
    ORI     = 16,
    AND     = 17,
    ANDI    = 18
};

typedef struct{
    /* state identifier */
    unsigned long int id;

    /* outgoing transition  */
    unsigned long int  baseID;
    unsigned long int  numberofTransition;

    /* action           */
    unsigned long int   src1;
    unsigned long int   src2;
    unsigned long int   dst;
    unsigned long int    opt;

    /* helper variable  */
    bool accepting;
    bool initial;
    bool epsilonState;
}NPDT;


typedef struct{
    /* helper variable  */
    unsigned long int id;

    /* Matching condition */
    unsigned long int  inputID;
    unsigned long int  inputSymbol;
    unsigned long int  IvarID;
    unsigned long int  inputVar;
    unsigned long int  currentState;

    /* Result */
    unsigned long int  outputID;
    unsigned long int  outputSymbol;
    unsigned long int  OvarID;
    unsigned long int  outputVar;
    unsigned long int  nextState;
}Transition;
