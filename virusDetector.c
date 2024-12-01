#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
int MAX_LENGTH = 10000;
int MAGIC_SIZE = 4;
bool needToChange = false; //little need to change

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
    fread(v, 1, 18, file); //for size and name
    if(needToChange){
        v->SigSize = ((v->SigSize) >> 8) | ((v->SigSize) << 8);
    }
    v->sig = malloc(v->SigSize); 
    if (v->sig == NULL){
        free(v);
        return NULL;
    }
    fread(v->sig, 1, v->SigSize, file); //pointer to where to store - v->sig, char size of v->SigSize from file
    //reading from file - from location of AFTER 18 bytes
    return v;
}
//05 00 56 49 52 55 53 00 00 00 00 00 00 00 00 00 00 00 31 32 33 34 35

void printVirus(virus* virus, FILE* output){
     printf("Enter output file name: ");
    char filename[256];
    //for getting the filename
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        printf("Failed to read filename.\n");
        return;
    }
    filename[strcspn(filename, "\n")] = 0; //removing the newline
    FILE *file = fopen(filename, "w"); //holds the data of all viruses
    if (file == NULL){
        printf("Error in opening file\n");
        return;
    }
    printf("File opened\n");
    

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


/* Add a new link with the given data to the list (at the end CAN ALSO AT BEGINNING), 
and return a pointer to the list (i.e., the first link in the list).
If the list is null - create a new entry and return a pointer to the entry. */
link* list_append(link* virus_list, virus* data){
    if (virus_list == NULL){
        link* newlink = malloc(sizeof(link));
        newlink->vir = data;
        return newlink;
    }
    else{
        virus_list->nextVirus->vir = data;
        return virus_list;
    }
}

/* Free the memory allocated by the list. */
void list_free(link *virus_list){
    if (virus_list == NULL){
        return;
    }
    link* current = virus_list;
    while(current != NULL){
        link* nextVir = current->nextVirus;
        free(current->vir->sig);
        free(current->vir);
        free(current);
        current = nextVir;
    }
    free(virus_list);
}

void load_signatures(){
    printf("Enter the file name: ");
    char filename[256];
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
    }
    /*fill up virus_list the global varialb with signature from a file given by the user*/
    
    /*Added this function*/
    if (virus_list == NULL){
        virus_list = malloc(sizeof(link));
                virus_list->vir = NULL;
        virus_list->nextVirus = NULL;
    }

    link* current = virus_list;

    /*Hendling allocating new size to NULL link*/
    while(!feof(file)){
        virus* v = readVirus(file);
        if (current->vir == NULL) {
        current->vir = v;
        } else if (v->SigSize==0){
            break;
        }else {
        /* Otherwise, create a new node and link it */
        current->nextVirus = malloc(sizeof(link));
        current = current->nextVirus;
        current->vir = v;
        current->nextVirus = NULL;
        }
    
    }
}

void detect_virus(char *buffer, unsigned int size, link *virus_list){
   
    link* v = virus_list;
    while (v!=NULL){
        link* current = v->nextVirus;
        for (int i = 0; i < size; ++i){
            if (memcmp(buffer + i, current->vir->sig, current->vir->SigSize) == 0)
                printf("Virus detected: %s, at offset %d\n", current->vir->virusName, i);
        }
        v = current;
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

    FILE *infile = fopen(filename, "rb"); //holds the data of all viruses
    if (infile == NULL){
        printf("Error in opening file\n");
        return;
    }

    char *buffer = malloc(10000);
    fread(buffer, 1, 10000, infile);
    detect_virus(buffer, 10000, virus_list);
}

void fix_file(){
    printf("Not implemented\n");
}


/*Maybe change the siganture so it will take the virus_list? */
void printSignature(){
    if (virus_list == NULL){
        return;
    }

   link* current = virus_list;  // Start at the head of the list
    while (current != NULL) {
        printVirus(current->vir, stdout);  // Print the virus data
        current = current->nextVirus;     // Move to the next link
    }
}

void stop_program(){
        printf("Exiting...\n");
        exit(0);
}

int main(int argc, char **argv) //stack arguments
{

    char inputBuffer[5];
    struct fun_desc {
    char *name;
    void (*fun)();
    };

    char* carray = malloc(5);
    carray[0] = '\0'; //The pointer points to '0'


    struct fun_desc menu[] = { { "Load signatures", load_signatures}, { "Print signatures", printSignature }, { "Detect viruses", detect_virus_helper },
    {"Fix file" ,fix_file} ,{"Quit" , stop_program},{NULL , NULL} };
    
    while (1){

    printf("Select opration from the following menu : \n");

    for (int i=0; menu[i].name!=NULL; i++ ){
        printf("%d : %s\n", i+1, menu[i].name);

    }

     printf("Please enter youre choice (value between 1-5) : \n");
     if (fgets(inputBuffer, sizeof(inputBuffer) , stdin) == NULL){ //No input from user
        break;
     }
     int choice = inputBuffer[0] -'0'-1; //getting the real int value - the first is always the choice

        if (!(choice>=0 && choice<=4)){
            printf("Not within bounds, %d", choice);
            break;
        }
       
        printf("Within bounds\n");
        
        menu[choice].fun();


    }

    free(carray);
 
}