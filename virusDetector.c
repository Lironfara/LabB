#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int MAX_LENGTH = 10000;

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
    /*fill up virus_list the global varialb with signature from a file given by the user*/
    printf("Enter the file name: ");
    char filename[256];
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
    printf("File opened\n");

    /*Added this function*/
    if (virus_list == NULL){
        virus_list = malloc(sizeof(link));
        virus_list->nextVirus = NULL;
    }


    /*Hendling allocating new size to NULL link*/
    while(!feof(infile)){
        virus* v = readVirus(infile);
        if (v == NULL){
            break;
        }
        if (virus_list->nextVirus == NULL){
            virus_list->nextVirus = malloc(sizeof(link));
            virus_list->vir = malloc(sizeof(virus));
        }
        virus_list->nextVirus->vir = v;  //adding the virus to the list
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
void printSignature(FILE *file){
    char buffer[sizeof(unsigned char)];

    if (file == NULL){
        return;
    }
    else{
        fread(buffer, 1, sizeof(unsigned char), file); //reading sig from file
    }
    printf("Signature: %s\n", buffer);
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

     printf("Please enter youre choice (value between 0-5) : \n");
     if (fgets(inputBuffer, sizeof(inputBuffer) , stdin) == NULL){ //No input from user
        break;
     }
     int choice = inputBuffer[0] -'0'-1; //getting the real int value - the first is always the choice


        if (!(choice>=0 && choice<=5)){
            printf("Not within bounds, %d", choice);
            break;
        }
       
        printf("Within bounds\n");
        
        menu[choice].fun();


    }

    free(carray);
 
}