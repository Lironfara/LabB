#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
int MAX_LENGTH = 10000;
int MAGIC_SIZE = 4;
bool needToChange = false; //little need to change
char* carray ;

typedef struct virus{
    unsigned short SigSize; //the virus size to allocate
    char virusName[16];
    unsigned char* sig; //where the signature is starts in the array
} virus;

typedef struct link link;
struct link {
link *nextVirus;
virus *vir;
};

link* virus_list = NULL;

virus* readVirus(FILE* file){
    virus* v = malloc(sizeof(virus));
    if (v == NULL){
        return NULL;
    }
    if (fread(v, 1, 18, file) != 18) { // Check if 18 bytes were read
        free(v);
        return NULL;
    }
    if(needToChange){
        v->SigSize = ((v->SigSize) >> 8) | ((v->SigSize) << 8);
    }
    if (v==NULL || v->SigSize == 0){ 
        free(v);
        return NULL;
    }   
    v->sig = malloc(v->SigSize); 
    if (v->sig == NULL){
        free(v);
        return NULL;
    }
    if (fread(v->sig, 1, v->SigSize, file) != v->SigSize) { // Check if SigSize bytes were read
        free(v->sig);
        free(v);
        return NULL;
    }
    return v;
}
//05 00 56 49 52 55 53 00 00 00 00 00 00 00 00 00 00 00 31 32 33 34 35

void printVirus(virus* virus, FILE* output){
    
    if (output == NULL){
        printf("%s",virus->virusName);
        printf("%x", virus->SigSize);
        for (int i = 0; i < virus->SigSize; i++){
            printf("%02hhX ", virus->sig[i]);
        }
    }
    else{
        fprintf(output, "Virus name: %s\n", virus->virusName);
        fprintf(output, "Virus size: %d\n", virus->SigSize);
        fprintf(output, "signature:\n");
        for (int i = 0; i < virus->SigSize; i++){
            fprintf(output, "%02hhX ", virus->sig[i]);
        }
    fprintf(output, "\n\n");
    }
    
}


/* Print the data of every link in list to the given stream. Each item followed by a newline character. */
void list_print(link *virus_list , FILE* file){
    while(virus_list->nextVirus != NULL){
        printVirus(virus_list->vir, file);
        virus_list = virus_list->nextVirus;
    }
}


link* list_append(link* virus_list, virus* data) {
    if (data==NULL || data->SigSize == 0 || data->sig == NULL){
            if (data!=NULL){
                free(data);
                free(data->sig);
            }
            return virus_list;
    }
    link* newLink = malloc(sizeof(link));
    if (newLink == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return virus_list;
    }
    newLink->vir = data;
    newLink->nextVirus = NULL;
    if (virus_list == NULL) {
        virus_list = newLink;
    } else { //adding the new link to the end of the list
        link* current = virus_list;
        while (current->nextVirus != NULL) {
            current = current->nextVirus; //moving in the linked list
        }
        current->nextVirus = newLink;
    }
    return virus_list;
}


/* Free the memory allocated by the list. */
void list_free(link *virus_list){
    if (virus_list == NULL){
        free(virus_list);
        return;
    }
    link* current = virus_list;
    while(current != NULL){
        link* nextVir = current->nextVirus;
          if (current->vir != NULL){
            free(current->vir->sig); // free signature memory
            free(current->vir); // free virus structure memory
        }
        free(current);
        current = nextVir;
    }

}


void load_signatures(){
    printf("Enter the file name: ");
    char filename[256] ={0};
    //for getting the filename
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        printf("Failed to read filename.\n");
        return;
    }
    filename[strcspn(filename, "\n")] = 0; //removing the newline
    FILE *file = fopen(filename, "r"); //holds the data of all viruses
    if (file == NULL){
        printf("Error in opening file\n");
        return;
    }

    unsigned char magic[MAGIC_SIZE];
     if (fread(magic, 1, MAGIC_SIZE, file) != MAGIC_SIZE) {
        fprintf(stderr, "Error reading magic number\n");
        return;
    }
    if (memcmp(magic, "VIRL", MAGIC_SIZE) == 0) {
    }
    // Check for big-endian magic number
    else if (memcmp(magic, "VIRB", MAGIC_SIZE) == 0) {
        needToChange = true;
    } else {
        printf("Unknown or invalid magic number\n");
        fclose(file);
        return;
        
    }
    /*fill up virus_list the global varialb with signature from a file given by the user*/
    
    /*Added this function*/
    if (virus_list == NULL){
        virus_list = malloc(sizeof(link));
        virus_list->vir = NULL;
        virus_list->nextVirus = NULL;
    }


    /*Hendling allocating new size to NULL link*/
    printf("Loading\n");
    while(!feof(file)){
        virus* v = readVirus(file);
        if (v == NULL){
            break;
        }
        virus_list = list_append(virus_list, v);
    }
    fclose(file);
    printf("Loaded\n");
}

//all viruses in the file
void detect_virus(char *buffer, unsigned int size, link *virus_list){
    if (virus_list == NULL){
        return;
    }
    bool found = false;
    link* v = virus_list;
    while (v != NULL){
        if (v->vir!=NULL && v->vir->sig!=NULL &&v->vir->SigSize != 0){
        for (unsigned int i = 0; i < size - v->vir->SigSize ; ++i){
            if (memcmp(buffer + i, v->vir->sig, v->vir->SigSize) == 0){
                found = true;
                printf("Virus detected!\n");
                printf("The starting byte location in the suspected file %d\n", i);
                printf("The virus name is: %s\n", v->vir->virusName);
                printf("The size of the virus signature is: %d\n", v->vir->SigSize);
            }         
        }
    }
    v = v->nextVirus;
    }
    if (!found){
        printf("No virus detected\n");
    }
}

void detect_virus_helper (){

    printf("Enter the file name to detect : ");
    char filename[10000];
    //for getting the filename
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        printf("Failed to read filename.\n");
        return;
    }
    filename[strcspn(filename, "\n")] = 0; //removing the newline

    FILE *infile = fopen(filename, "rb"); //holds the data of all viruses
    if (infile == NULL){
        printf("Error in opening file\n");
        return;
    }

    char *buffer = malloc(10000);
    size_t bytesRead = fread(buffer, 1, 10000, infile);
    detect_virus(buffer, bytesRead, virus_list);
    free(buffer);
    fclose(infile);
}

void neutralize_virus(char *fileName, int signatureOffset){
    FILE *file = fopen(fileName, "r+b");
    if (file == NULL){
        printf("Error in opening file\n");
        return;
    }
    int found = fseek(file, signatureOffset, SEEK_SET);
    if (found != 0){
        printf("Error in finding the signature\n");
        fclose(file);
        return;
    }

    unsigned char replace = 0xC3;
    int switched = fwrite(&replace, sizeof(replace), 1, file);
    if (switched != 1){
        printf("Error in switching the signature\n");
        fclose(file);
        return;
    }
    else {
        printf("Virus neutralized at offset %d.\n", signatureOffset);
    }
    fclose(file);
}


int* find_offset(char *buffer, unsigned int size, link *virus_list){
    int* offsets = malloc(sizeof(int)*10000);
    if(offsets == NULL){
        free(offsets);
        return NULL;
    }
    int counter = 0;
    link* v = virus_list;
    while (v != NULL){
        if (v->vir!=NULL && v->vir->sig!=NULL &&v->vir->SigSize != 0){
        for (unsigned int i = 0; i < size - v->vir->SigSize ; ++i){
            if (memcmp(buffer + i, v->vir->sig, v->vir->SigSize) == 0){
                offsets[counter] = i;
                counter++;
            } 
        }
    }
        v = v->nextVirus;
    }
    offsets[counter] = -1;
    return offsets;
}

void fix_file(){
    printf("Enter the file name to fix : ");
    char fileName[10000];
    //for getting the filename
    if (fgets(fileName, sizeof(fileName), stdin) == NULL) {
        printf("Failed to read filename.\n");
        return;
    }
    fileName[strcspn(fileName, "\n")] = 0; //removing the newline

    FILE *infile = fopen(fileName, "rb"); 
    if (infile == NULL){
        printf("Error in opening file\n");
        return;
    }

    char *buffer = malloc(10000);
    size_t bytesRead = fread(buffer, 1, 10000, infile);
    int* offsets = find_offset(buffer, bytesRead, virus_list); //loactions of the viruses
    int counter = 0;
    while(offsets[counter]!=-1){
        int signatureOffset = offsets[counter];
        printf("Virus found at offset %d\n", signatureOffset);
        neutralize_virus(fileName, signatureOffset);
        counter++;
    }
    free(offsets);
    free(buffer);
    fclose(infile);
}






/*Maybe change the siganture so it will take the virus_list? */
void printSignature(){
    if (virus_list == NULL){
        return;
    }
     printf("Enter output file name: ");
    char filename[256];
    //for getting the filename
    if ( fgets(filename, sizeof(filename), stdin) == NULL) {
        printf("Failed to read filename.\n");
        return;
    }
    filename[strcspn(filename, "\n")] = 0; //removing the newline

    FILE *file;
    if (filename[0] == '\0') {
        printf("No file name given.\n");
        file = NULL;
    }
    else{
        file = fopen(filename, "w"); //holds the data of all viruses
    }

    if (file == NULL){
        printf("Error in opening file, printing to the screen\n");
    }
    printf("File opened\n");
    
   link* current = virus_list;  // Start at the head of the list
    while (current != NULL) {
        if (current->vir != NULL){
            printVirus(current->vir, file);  // Print the virus data
        }
        current = current->nextVirus;     // Move to the next link
        
    }
    if (file!=NULL){
        fclose(file);
    }
}

void stop_program(){
        list_free(virus_list);
        free(carray);
        printf("Exiting...\n");
        exit(0);
}

int main(int argc, char **argv) //stack arguments
{
    carray = malloc(5);
    printf("\nWelcome to the virus detector\n");

    char inputBuffer[5];
    struct fun_desc {
    char *name;
    void (*fun)();
    };

    
    carray[0] = '\0'; //The pointer points to '0'


    struct fun_desc menu[] = { { "Load signatures", load_signatures}, { "Print signatures", printSignature }, { "Detect viruses", detect_virus_helper },
    {"Fix file" ,fix_file} ,{"Quit" , stop_program},{NULL , NULL} };
    
    while (1){

    printf("Select opration from the following menu : \n");

    for (int i=0; menu[i].name!=NULL; i++ ){
        printf("%d : %s\n", i+1, menu[i].name);

    }

     printf("Please enter your choice (value between 1-5) : \n");
     if (fgets(inputBuffer, sizeof(inputBuffer) , stdin) == NULL){ //No input from user
        break;
     }
     int choice = inputBuffer[0] -'0'-1; //getting the real int value - the first is always the choice

        if (!(choice>=0 && choice<=4)){
            printf("Not within bounds, %d", choice);
            stop_program();
            break;
        }
       
        printf("Within bounds\n");
        
        menu[choice].fun();


    }

 
}