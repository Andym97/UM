
#include <stdlib.h>
#include "execute.h"

int main(int argc, char *argv[])
{
        if(argc != 2) {
                printf("Too few arguments\n");
                exit(1);
        }

        FILE *fp = fopen(argv[1], "rb");
        if (fp == NULL) {
                printf("Too many arguments\n");
                exit(1);
        }

        Memory program;
        program= new_mem();
        load_instructions(program, fp);

        fclose(fp);
        extract_instructions(program);

        return 0;
}
