typedef struct virus{
    unsigned short SigSize; //the virus size to allocate
    char virusName[16];
    unsigned char* sig; //where the signature is starts in the array
} virus;

virus* readVirus(FILE* file){
    virus* v = malloc(sizeof(virus));
    if (v == NULL){
        return NULL;
    }
    fread(v, 1, 18, file); //for size and name
    v->sig = malloc(v->SigSize); 
    if (v->sig == NULL){
        free(v);
        return NULL;
    }
    fread(v->sig, 1, v->SigSize, file); //pointer to where to store - v->sig, char size of v->SigSize from file
    //reading from file - from location of AFTER 18 bytes
    return v;
}

void printVirus(virus* virus, FILE* output){
    fprintf(output, "Virus name: %s\n", virus->virusName);
    fprintf(output, "Virus size: %d\n", virus->SigSize);
    fprintf(output, "signature:\n");
    for (int i = 0; i < virus->SigSize; i++){
        fprintf(output, "%02hhX ", virus->sig[i]);
    }
    fprintf(output, "\n\n");
}