#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>


char xprt(char c){
    /* xprt prints the value of c in a hexadecimal representation followed by a new line, 
    and returns c unchanged. */
    printf("%02x\n", c);
    return c;
}

int main(int argc, char** argv ){
    FILE *f;
    char* filename = argv[1];
    f = fopen(filename, "rb"); // open file in binary mode
    if (f == NULL){
        printf("Error opening file\n");
        return 1;
    }

    int size = 0;
    char buffer[16];

    while ((size = fread(buffer, 1, 16, f)) > 0){
        for (int i = 0; i < size ; i++){
            xprt(buffer[i]);
        }
    }
    fclose(f);
    return 0;
    


}