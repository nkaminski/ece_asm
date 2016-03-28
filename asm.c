#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#define RDELIM '@'
#define MAX_LINE 20
#define N_INSTRUCTIONS 10

uint16_t assembleLine(char *in){
    const char *isa[] = {"sll","srl","add","sub","and","xnor","loadi","jz","jnz","stop"};
    const uint8_t opcodes[] = {0b00000000,0b00000001,0b00000010,0b00000011,0b00000100,0b00000101,0b00001010,0b00001000,0b00001001,0b00001111};
    uint16_t output = 0;
    uint16_t rs=0,rt=0,rd=0,imm=0;
    char *op,*operand;
    char temp[MAX_LINE+1];
    char *stringp = temp;
    int i, j, cr;
    assert(strlen(in) < MAX_LINE);
    strncpy(temp,in,MAX_LINE);
    op=strsep(&stringp," ");
    for(i=0;i<N_INSTRUCTIONS+1;i++){
        if(i == N_INSTRUCTIONS){
            //All mnemonics compared!
            fprintf(stderr,"Invalid Instruction!");
            exit(1);
        }
        cr = strcmp(op,isa[i]);
        if(cr == 0){
            //instruction found
            printf("Opcode: %#04x\n", opcodes[i]);
            output |= ((uint16_t)opcodes[i] << 12);
            if( stringp == NULL ){
                fprintf(stderr,"Missing Operands!");
                exit(1);
            }
            operand = strsep(&stringp,",");
            if( operand == NULL || stringp == NULL ){
                fprintf(stderr,"Invalid Operands!");
                exit(1);
            }
            //Make sure that a register was specified and hop over the @
            assert(operand[0] == RDELIM);
            operand++;
            rd = atoi(operand);
            if(opcodes[i] & 0x08){
                //I or J Type, next token is the immediate
                imm = atoi(stringp);
                imm = imm & 0x00FF;
                if((opcodes[i] & 0x02) != 0){
                //I type
                printf("I: Rd=%#04x, Imm=%#04x\n", rd,imm);
                rd = ((rd & 0b00001111) << 8);
                output |= rd | imm;
                } else {
                //J type
                printf("J: Rd=%#04x, Addr=%#04x\n", rd,imm);
                output |= ((imm & 0xF0) << 4);
                output |= (imm & 0x0F);
                output |= ((rd & 0x0F) << 4);
                }
            } else {
                //R Type, parse 2 more registers
                //operand 1
                operand = strsep(&stringp,",");
                if( operand == NULL || stringp == NULL ){
                    fprintf(stderr,"Invalid 2nd operand in R-type instruction!");
                    exit(1);
                }
                //Make sure that a register was specified and hop over the @
                assert(operand[0] == RDELIM);
                operand++;
                rs = atoi(operand);
                //operand 2
                operand = strsep(&stringp,",");
                if( operand == NULL || stringp != NULL ){
                    fprintf(stderr,"Invalid 3rd operand in R-type instruction!");
                    exit(1);
                }
                //Make sure that a register was specified and hop over the @
                assert(operand[0] == RDELIM);
                operand++;
                rt = atoi(operand);
                printf("R: Rd=%#04x, Rs=%#04x, Rt=%#04x\n", rd,rs,rt);
                rd = (rd & 0x0f) << 8;
                rs = (rs & 0x0f) << 4;
                rt = (rt & 0x0f);
                output |= rd | rs | rt;
            }
            break;
        }
    }
    return output;
}
int main(int argc, char **argv){
    printf("%#04x\n",assembleLine("add @3,@3,@1"));
    printf("%#04x\n",assembleLine("jz @2,8"));
    printf("%#04x\n",assembleLine("loadi @1,30"));
    return 0;
/*
    FILE *ifp, *ofp;
    assert(argc == 3);
    ifp = fopen(argv[1], "r");

    if (ifp == NULL) {
        fprintf(stderr, "Unable to open input file!\n");
        exit(1);
    }
    ofp = fopen(argv[2], "w");

    if (ofp == NULL) {
        fprintf(stderr, "Unable to open output file!");
        exit(1);
    }
    */
}

