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
    unsigned long id;

    /* outgoing transition  */
    unsigned long  baseID;
    unsigned long  numberofTransition;

    /* action           */
    unsigned long   src1;
    unsigned long   src2;
    unsigned long   dst;
    unsigned long   opt;

    /* helper variable  */
    bool accepting;
    bool initial;
    bool epsilonState;
}NPDT;


typedef struct{
    /* helper variable  */
    unsigned long id;

    /* Matching condition */
    unsigned long inputID;
    unsigned long inputSymbol;
    unsigned long IvarID;
    unsigned long inputVar;
    unsigned long currentState;

    /* Result */
    unsigned long outputID;
    unsigned long outputSymbol;
    unsigned long OvarID;
    unsigned long outputVar;
    unsigned long nextState;
}Transition;
