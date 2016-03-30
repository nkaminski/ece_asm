#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#define RDELIM '@'
#define COMDELIM '#'
#define MAX_LINE 20
#define N_INSTRUCTIONS 10

void stripspace(char *s){
    char* i = s;
    char* j = s;
    while(*j != 0)
    {
        *i = *j++;
        if(*i != ' ')
            i++;
    }
    *i = 0;
}
uint16_t assembleLine(char *in){
    const char *isa[] = {"sll","srl","add","sub","and","xnor","loadi","jz","jnz","stop"};
    const uint8_t opcodes[] = {0b00000000,0b00000001,0b00000010,0b00000011,0b00000100,0b00000101,0b00001010,0b00001000,0b00001001,0b00001111};
    uint16_t output = 0;
    uint16_t rs=0,rt=0,rd=0,imm=0;
    char *op,*operand;
    char temp[MAX_LINE+1];
    char *d=" ";
    char *stringp = temp;
    int i, j, cr;
    assert(strlen(in) < MAX_LINE);
    strncpy(temp,in,MAX_LINE);
    op=strsep(&stringp," ");
    for(i=0;i<N_INSTRUCTIONS+1;i++){
        if(i == N_INSTRUCTIONS){
            //All mnemonics compared!
            fprintf(stderr,"Invalid Instruction: %s\n",op);
            exit(1);
        }
        cr = strcmp(op,isa[i]);
        if(cr == 0){
            //instruction found
#ifdef DEBUG
            printf("Opcode: %#04x\n", opcodes[i]);
#endif
            if((opcodes[i] & 0x0F) == 0x0F){
                //Stop instruction
                printf("I(stop): Op=%#02x",opcodes[i]);
                output |= 0xFFFF;
                break;
            } 
            output |= ((uint16_t)opcodes[i] << 12);
            if( stringp == NULL ){
                fprintf(stderr,"Missing Operands!");
                exit(1);
            }
            stripspace(stringp);
            operand = strsep(&stringp,",");
            if( operand == NULL || stringp == NULL ){
                fprintf(stderr,"Invalid Operands!");
                exit(1);
            }
            //Make sure that a register was specified and hop over the @
            if(operand[0] != RDELIM){
                fprintf(stderr,"Missing @ sign in Rd specification\n");
                exit(1);
            }
            operand++;
            rd = atoi(operand);
            if(opcodes[i] & 0x08){
                //I or J Type, next token is the immediate
                imm = atoi(stringp);
                imm = imm & 0x00FF;
                if((opcodes[i] & 0x02) != 0){
                    //I type
                    printf("I: Op=%#02x, Rd=%#02x, Imm=%#02x",opcodes[i],rd,imm);
                    rd = ((rd & 0b00001111) << 8);
                    output |= rd | imm;
                } else {
                    //J type
                    printf("J: Op=%#02x, Rd=%#02x, Addr=%#02x",opcodes[i],rd,imm);
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
                if(operand[0] != RDELIM){
                    fprintf(stderr,"Missing @ sign in Rs specification\n");
                    exit(1);
                }
                operand++;
                rs = atoi(operand);
                //operand 2
                operand = strsep(&stringp,",");
                if( operand == NULL || stringp != NULL ){
                    fprintf(stderr,"Invalid 3rd operand in R-type instruction!");
                    exit(1);
                }
                //Make sure that a register was specified and hop over the @
                if(operand[0] != RDELIM){
                    fprintf(stderr,"Missing @ sign in Rt specification\n");
                    exit(1);
                }
                operand++;
                rt = atoi(operand);
                printf("R: Op:%#02x, Rd=%#02x, Rs=%#02x, Rt=%#02x",opcodes[i],rd,rs,rt);
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
#ifdef DEBUG
    printf("%#04x\n",assembleLine("add @3,@3,@1"));
    printf("%#04x\n",assembleLine("jz @2,8"));
    printf("%#04x\n",assembleLine("loadi @1,30"));
#endif
    FILE *ifp, *ofp;
    char line[MAX_LINE], *nu_line;
    int lctr = 0;
    uint16_t asmres;
    if(argc != 3){
        fprintf(stderr,"Usage: %s <input_file> <output_file>\n",argv[0]);
        exit(1);
    }
    ifp = fopen(argv[1], "r");

    if (ifp == NULL) {
        fprintf(stderr, "Unable to open input file!\n");
        exit(1);
    }
    ofp = fopen(argv[2], "w");

    if (ofp == NULL) {
        fprintf(stderr, "Unable to open output file!\n");
        exit(1);
    }
    while(1){
        //print newlines between instructions, but dont leave an empty line at the end
        if(lctr > 0)
            fprintf(ofp,"\n");
        //Read a line
        if(fgets(line,MAX_LINE,ifp) == NULL)
            break;
        //Valid line
        printf("Line %d: ",lctr);
        if(strlen(line) < 2){
            printf("empty line\n");
            continue;
        }
        if(line[0] == COMDELIM){
            printf("comment\n");
            continue;
        }
        //Clobber newline
        nu_line = strrchr(line,'\n');
        if(nu_line != NULL)
            nu_line[0]='\0';
        //Assemble the line
        asmres = assembleLine(line);
        //print and save results
        printf(" | %s -> %#04x\n",line,asmres);
        fprintf(ofp,"%04X",asmres);

        lctr++;
    }
    fclose(ifp);
    fclose(ofp);
}
