
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "seq.h"
#include "stack.h"
#include "segment.h"
#include "bitpack.h"
#include "assert.h"


struct Memory{
        Seq_T segments;
        Stack_T unmapped;
        unsigned id_tracker;
};


/* Private Helper */
void free_word(Seq_T segment);

/*---------------------------------------------------------------------------------------------*/

// Creates the 32 bit instructions and lods into the 0-segment
void load_instructions(Memory info, FILE *input) {
        uint32_t new_word = ~0;
        int c = 0;
        while (c != EOF) {
                if (new_word != (uint32_t) ~0) {
                        Seq_T zero_segment = Seq_get(info->segments, 0);
                        uint32_t *word = malloc(sizeof(uint32_t));

                        *word = new_word;
                        Seq_addhi(zero_segment, word);
                        new_word = 0;
                }
                for (int i = 3; i >= 0; i--) {
                        c = fgetc(input);
                        if (c == EOF) {
                                break;
                        } else {
                                new_word = Bitpack_newu(new_word, 8,
                                                        8 * i, c);
                        }
                }
        }
}

/* gets the next instruction in the 0-segment
   returns 0 if there are no more instructions */
uint32_t get_instruction(Memory info)
{
        if (finish_instruction(info) == 1) {
                return 0;
        }

        Seq_T zero_segment = Seq_get(info->segments, 0);
        uint32_t *instruction = Seq_get(zero_segment, info->id_tracker);

        info->id_tracker++;

        return *instruction;
}


/*
  Keeps track of number of instructions in the 0 segment
  Returns 0 if there are still more instructions
  Returns 1 if finished  with instructions
*/
uint32_t finish_instruction(Memory info)
{
        Seq_T zero_segment = Seq_get(info->segments, 0);
        if (info->id_tracker >= (unsigned) Seq_length(zero_segment)) {
                return 1;
        }
        return 0;
}

/* frees all words in the given sequence */
void free_word(Seq_T segment)
{
        int length = Seq_length(segment);
        for (int i = 0; i < length; i++) {
                uint32_t *deleted_word = Seq_remhi(segment);
                free(deleted_word);
        }
}


/* Creates a Memory struct
   Mapping just the 0-segment and
   Setting the id tracker to 0
 */
Memory new_mem()
{
        Memory info = malloc(sizeof(struct Memory));


        /* create new memory */
        info->segments = Seq_new(5);
        assert(info->segments);
        info->unmapped = Stack_new();

        seg_map(info, 0);
        info->id_tracker = 0;

        return info;
}


void free_memory(Memory info)
{
        int length;

        length = Seq_length(info->segments);
        for (int i = 0; i < length; i++) {
                Seq_T segment = Seq_get(info->segments, i);
                free_word(segment);
                Seq_free(&segment);
        }
        Seq_free(&(info->segments));

        while (Stack_empty(info->unmapped) != 1) {
                uint32_t *poppedAddress = Stack_pop(info->unmapped);
                free(poppedAddress);

        }
        Stack_free(&info->unmapped);

        free(info);
}


/* If there is an index on the unmapped address stack,
     Pops an address and
     Maps a segment of the given number of words to that address
  If there is NOT an index on the unmapped address stack,
      Maps a segment to the end of the sequence
      Returns the index of new segment in sequence
*/
uint32_t seg_map(Memory info, uint32_t num_words)
{
        uint32_t index;
        /* checking to see if segment was previously mapped  */
        if (Stack_empty(info->unmapped) == 1) {
                index = Seq_length(info->segments);
                Seq_addhi(info->segments, Seq_new(num_words));
        }
        else {
                uint32_t *poppedAddress = Stack_pop(info->unmapped);
                index = *poppedAddress;
                free(poppedAddress);
                Seq_T old_segment = Seq_get(info->segments, index);

                //Free then remap
                free_word(old_segment);
        }
        // new segment = 0s
        Seq_T segment = Seq_get(info->segments, index);
        for (uint32_t i = 0; i < num_words; i++) {
                uint32_t *new_space = malloc(sizeof(*new_space));
                *new_space = 0;
                Seq_addhi(segment, new_space);
        }
        return index;
}



 /* Push index of segment to unmapped address
    Allocated memory will be free'd when the segment index is reused.
 */
void seg_unmap(Memory info, uint32_t seg_index)
{
        uint32_t *new_seg_index = malloc(sizeof(*new_seg_index));
        *new_seg_index = seg_index;
        Stack_push(info->unmapped, new_seg_index);
}

/* return the value at the given segment in the memory sequence at the given
 * offset in that segment sequence
 */
uint32_t segs_load(Memory info, uint32_t seg_index, uint32_t offset)
{
        Seq_T segment = Seq_get(info->segments, seg_index);
        uint32_t *loaded_value = Seq_get(segment, offset);

        return *loaded_value;
}

/*
 Store value in the sequence at a given offset in the sequence.
*/
void seg_store(Memory info, uint32_t seg_index, uint32_t offset,
                    uint32_t value)
{
        Seq_T segment = Seq_get(info->segments, seg_index);
        uint32_t *value_location = Seq_get(segment, offset);
        *value_location = value;
}

 /* Unmaps and remapps the 0 segment with number of words
    Copies each value in given segment into the 0 segment
  */
void seg_load_program(Memory info, uint32_t seg_index,
                           uint32_t offset)
{
        if (seg_index != 0) {
                Seq_T segment = Seq_get(info->segments, seg_index);
                int length = Seq_length(segment);
                /* unmapping the zero segment so that it will be the next
                   possible address on the stack to map to */
                seg_unmap(info, 0);
                seg_map(info, length);

                Seq_T zero_segment = Seq_get(info->segments, 0);
                for (int i = 0; i < length; i++) {
                        uint32_t *value = Seq_get(segment, i);
                        uint32_t *valueLocation = Seq_get(zero_segment, i);
                        *valueLocation = *value;
                }
        }
        info->id_tracker = offset;
}
