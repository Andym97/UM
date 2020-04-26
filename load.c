
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "seq.h"
#include "stack.h"
#include "assert.h"

#include "execute.h"
#include "segment.h"


 //Moves contents of register b into register a if register c is not 0
void conditional_move(uint32_t *registers, uint32_t a, uint32_t b, uint32_t c)
{
        if (registers[c] != 0) {
                registers[a] = registers[b];
        }
}


 //Load register b and register c (offset) into register a
void segment_load(Memory info, uint32_t *registers, uint32_t a,
                      uint32_t b, uint32_t c)
{
        registers[a] = segs_load(info, registers[b], registers[c]);
}

/*calls seg_store and passes it the value (registers[c]),along with
 * the memory index (registers[b]) and the offset (registers[c])
 */
void segment_store(Memory info, uint32_t *registers,uint32_t a,
                     uint32_t b, uint32_t c)
{
       seg_store(info, registers[a], registers[b], registers[c]);
}

//Adds values and stores it in reigster a
void add(uint32_t *registers,uint32_t a, uint32_t b, uint32_t c)
{
        registers[a] = registers[b] + registers[c];
}

//Multiply values and stores it in reigster a
void multiply(uint32_t *registers,uint32_t a, uint32_t b, uint32_t c)
{
        registers[a] = registers[b] * registers[c];
}

//Divides values and stores it in reigster a
void divide(uint32_t *registers,uint32_t a, uint32_t b, uint32_t c)
{
        registers[a] = registers[b] / registers[c];
}

//Stores the bitwise nand in registers[a]
void nand(uint32_t *registers,uint32_t a, uint32_t b, uint32_t c)
{
        registers[a] = ~(registers[b] & registers[c]);
}

//stops the computation and frees the associated memory
void halt(Memory info)
{
        free_memory(info);
        exit(0);
}


 //Maps the segment and passes it a number of words to be stored in register b
void map(Memory info, uint32_t *registers, uint32_t b, uint32_t c)
{
        registers[b] = seg_map(info, registers[c]);
}

//Unmaps register and passes the memory index
void unmap(Memory info, uint32_t *registers, uint32_t c)
{
        seg_unmap(info, registers[c]);
}

//Outputs value of registers[c] to standard output */
void output(uint32_t *registers, uint32_t c)
{

        if (registers[c] < 256) {
                uint32_t value = registers[c];
                putchar(value);
        }
}

//Loads values from stdin into register c
void input(uint32_t *registers, uint32_t c)
{
        uint32_t value = getc(stdin);
        if (value == (uint32_t) EOF) {
                registers[c] = ~0;
        }
        else if (value < 256) {
                registers[c] = value;
        }
}

//Passes memory location of reigster b and value in register c into seg_load_program
 void load_program(Memory info, uint32_t *registers, uint32_t b,uint32_t c)
 {
         seg_load_program(info, registers[b], registers[c]);
 }

//Loads value into reigster a
 void load_value(uint32_t *registers, uint32_t a, uint32_t value)
 {
         registers[a] = value;
 }
