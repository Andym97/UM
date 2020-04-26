
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "load.h"
#include "bitpack.h"
#include "execute.h"


#define OP_WIDTH 4;
#define REG_WIDTH 3;
#define VAL_W 25
#define OP_LSB 28;

void run_instruction(Memory info, uint32_t opcode, uint32_t *registers,
                         uint32_t a, uint32_t b, uint32_t c);



extern void extract_instructions(Memory info)
{
      //initialize registers
        uint32_t registers[8] = {0,0,0,0,0,0,0,0};

        //If there are more instructions to read
        while (finish_instruction(info) == 0) {
                uint32_t word = get_instruction(info);
                uint32_t opcode = Bitpack_getu((uint64_t) word, OP_WIDTH,OP_LSB);

                /* load value instruction */
                if (opcode == 13) {
                      /* Unpacks the register/value using bitpack interface
                       *Loads the value */
                      //Register width,Value width
                      uint32_t a = Bitpack_getu((uint64_t) word, REG_WIDTH, VAL_W);
                      //Value width
                      uint32_t b = Bitpack_getu((uint64_t) word, VAL_W, 0);
                      load_value(registers, a, b);
                }
                else {
                  uint32_t a = Bitpack_getu((uint64_t) word,REG_WIDTH, 0 );

                  //  Lowest order 9 bits
                  uint32_t b = Bitpack_getu((uint64_t) word, REG_WIDTH, REG_WIDTH );
                  uint32_t c = Bitpack_getu((uint64_t) word,REG_WIDTH, REG_WIDTH*2 );
                        run_instruction(info, opcode, registers, a, b, c);
                 }
        }
        free_memory(info);
}

/*  Determines which function to call based on opcode. 0-12
    Opcode 13 was previously checked
    If opcode is greater than 13, program exits. */
void run_instruction(Memory info, uint32_t opcode, uint32_t *registers,
                         uint32_t a, uint32_t b, uint32_t c)
{
        assert(opcode<=13);

        switch (opcode) {
                case 0:
                        conditional_move(registers, a, b, c);
                        break;
                case 1:
                        segment_load(info, registers, a, b, c);
                        break;
                case 2:
                        segment_store(info, registers, a, b, c);
                        break;
                case 3:
                        add(registers, a, b, c);
                        break;
                case 4:
                        multiply(registers, a, b, c);
                        break;
                case 5:
                        divide(registers, a, b, c);
                        break;
                case 6:
                        nand(registers, a, b, c);
                        break;
                case 7:
                        halt(info);
                        break;
                case 8:
                        map(info, registers, b, c);
                        break;
                case 9:
                        unmap(info, registers, c);
                        break;
                case 10:
                        output(registers, c);
                        break;
                case 11:
                        input(registers, c);
                        break;
                case 12:
                        load_program(info, registers, b, c);
                        break;
                default:
                  /* op code greater than 13 is invalid */
                        free_memory(info);
                        exit(1);
        }
}
