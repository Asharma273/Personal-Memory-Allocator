#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <errno.h>
#include <stddef.h>
//#include "mymalloc.h"
#define MEMORY 4096

struct header {
int  size;
int validity;
};
static int Prev(char* prev, char* curr);
static int Next(char * curr, char * next, int iteration);
int frag();
int fragCoalesce();
int fragCoalesceV2();
int fragFreeToMalloc();
int superCoalesce();
void memDump(); 
static char memory [MEMORY];

// Function that allocates memory to the heap in the very beginning 


void memDump()  { // Note to self the increment of 
    int x=0;
    char* MT;
MT = memory;
while (x<MEMORY) {
    printf (" The index is :%d \n", x);
struct header * ptr =  (struct header*) MT;
printf (" The pointer size is %d\n", ptr->size);
printf ("The validity is %d", ptr->validity); if (ptr->validity == 0) printf(" -Free \n"); else printf ("\n");
//change = (struct header*) (memory +ptr->size);
//printf ("%d \n", change->size);
MT = (MT + ptr->size);
   x+=ptr->size;
if (x ==4095) {
 return;
    }
}

}

void *mymalloc(size_t size) {
char* resultPtr;
struct header * ptr; 
resultPtr = memory;
int validSpace;
int x =0;
int firstTime =0;
int spaceRequired = size+8;
  // Error checks to see that there is too little bytes requested. 0 or less than 0
  // Error checks to see that there is too many bytes requested. 0 or > 4088
if(size <1 ||size>=4088) {
fprintf(stderr,"ERROR: Could not allocate the memory because allocation invalid size input (FILE: LINE:) \n");
    return NULL;
    }
// checks to see if memory has been allocated before. Then attaches the metadata (of type struct) to the first index of the heap @ &memory[0]
 for (int x =0; x<MEMORY; x++) {
if (memory[x] == 0) {
   firstTime = 1;
    }
 else {
firstTime =0;
 break;
         }
    } 

if (firstTime ==1){
struct header* p;
ptr = (struct header*)memory;
ptr-> size = 4096;
ptr-> validity =0; 

p= (struct header *) (memory+spaceRequired);
p->size = MEMORY - spaceRequired;
p->validity = 0;
 return (memory +8);
    }

char* memTraverse = memory;  
// Runs through the memory array until the end looking for potential space and allocating it if there is. 
while (x< 4096) {

ptr = (struct header *)memTraverse;
if (ptr==NULL) {
return NULL;
}
int memChunkSize = ptr->size;
validSpace = memChunkSize - spaceRequired;
//Check to see if the block is allocated or not
// Then we check to see if the block (potentially freed) will have enough space to split the block and allocate memory to one and leave the other section unallocated.
//The difference must pass the cost of 8 bytes if the space required is different from the original size in the chunk that is being split. 
if ((ptr->validity==0) && (memChunkSize==spaceRequired|| validSpace > 8) ) {
    ptr->size = spaceRequired;
    ptr->validity =1; 
//printf ("allocating block");
struct header* nextHeader;
    if (memChunkSize- spaceRequired !=0) { 
        nextHeader = (struct header*)(memTraverse +(size +8));
        nextHeader -> size = validSpace;
      //  printf ("%d\n",split->size);
        nextHeader -> validity = 0;
    //printf ("%d\n",split->validity);

resultPtr = ( memTraverse +8); //This is the first byte that the user can store values in memory;
//printf("memory has been allocated \n");
return resultPtr;
        }
 
    }
      //Note: The struct meta pointer should never point to anything that is not a header; Traversal in increments of headers. 
    else {
      x+=memChunkSize; // index moves ptr->size times;
      memTraverse= &memory[x];
    // Memtraverse traverses to the next header
    }
}

  resultPtr = NULL;
    if (resultPtr == NULL) {
fprintf(stderr,"ERROR: Could not find free space for memory allocation (FILE: LINE:) \n");
        }
return NULL;
}

 int Prev(char* prev, char* curr) {
 struct header * prevHPtr = (struct header*) prev;
 int PrevFree =1;
 
  if (prevHPtr -> validity==1) {
  PrevFree = 0;
       }

 if ( curr == prev ) {
    return -1;
        }
  
  if (PrevFree==0) {

    return -1;
        }
  
  if (prevHPtr -> validity ==0) {
    return 1;
        }
  return-1;
  }

int Next(char * curr, char * next, int iteration) {
 struct header * nextHPtr = (struct header*) next;
int nextFree =1;
if (nextHPtr -> validity ==1) {

   nextFree =0;
}

if (nextFree==0) {

    return -1;
}
if (iteration == 4096) {

    return -1;
}

if (nextHPtr -> validity ==0) {

    return 1;
}
return -1;
}

void myfree(void *ptr) {
char * currentTraverse = memory;
char* previousTraverse = memory;
char * nextTraverse = memory;
struct header * headerPtr = NULL;
struct header * prevheaderPtr = NULL;
struct header * nextheaderPtr = NULL;
int x =0; 
int inRangeCount=0;
//  check to see if the address of the pointer is greater or less than mem[0] and mem[4887];
for (int x =0; x<MEMORY; x++) {

if (ptr!= &memory[x]) {
inRangeCount ++;
    }
if (inRangeCount == MEMORY) {
    fprintf(stderr,"ERROR: Invalid Pointer - Pointer has not been allocated by mymalloc (FILE: LINE:)\n");
    return;
    }

}
if (ptr == NULL) {
fprintf (stderr, "ERROR: Pointer points to NULL");
return;
}

while (x <4096 )  {

// This condition checks to see if the pointer returned by mymalloc was 8 bytes after the header current points to
// (if so then it is the pointer needs to be freed and coalesced);
if (ptr == (currentTraverse+8)) {
headerPtr = (struct header *) currentTraverse;
nextTraverse =  (&memory[x+headerPtr->size]); // moves the next header to the header after current.
// checks to see if the first pointer is already free;

if (headerPtr-> validity ==0) {
fprintf(stderr, "ERROR: Attempt to double free (FILE: LINE:)\n");
         return;
              }
   
else if (Prev(previousTraverse, currentTraverse) == 1 && Next(currentTraverse,nextTraverse,x) == -1) {
 //  printf ("case1\n");
    prevheaderPtr = (struct header *) previousTraverse;
    prevheaderPtr ->size= prevheaderPtr->size +headerPtr->size;
    prevheaderPtr ->validity =0;
 //   headerPtr ->validity = -1; 
  // exists=1;
 //printf("freed\n");
        return;
            }
else if (Prev(previousTraverse, currentTraverse) == -1 && Next(currentTraverse,nextTraverse,x) == 1) {
//printf ("case2\n");
nextheaderPtr = (struct header *) nextTraverse;
headerPtr ->size= headerPtr->size+ nextheaderPtr ->size;
headerPtr ->validity =0;
//nextheaderPtr ->validity = -1;
 //  exists=1;
 //printf("freed\n");
        return;
            }
else if (Prev(previousTraverse, currentTraverse) == 1 && Next(currentTraverse,nextTraverse,x) == 1) {
 // printf ("case3\n");
    prevheaderPtr = (struct header *) previousTraverse;
    nextheaderPtr = (struct header *) nextTraverse;
    prevheaderPtr ->size= prevheaderPtr->size +headerPtr->size+ nextheaderPtr ->size;
    prevheaderPtr ->validity =0;
   //headerPtr -> validity = -1;
   //nextheaderPtr ->validity =-1 ;
 //printf("freed\n");
return;
            }
else  {
//printf ("case4\n");
headerPtr -> validity =0;
// exists=1;
 //printf("freed\n");
return;
            }       
        }
    
    
   
    else  {
headerPtr = (struct header *) currentTraverse;
char* check;
for (int x = 0; x< headerPtr->size; x++) {
check = currentTraverse +x;

  if (ptr ==check ) {
 fprintf (stderr, "ERROR: Invalid Ptr- Not at the start of Pointer (FILE: LINE:)\n");
 break; 
    }
}

headerPtr = (struct header *) currentTraverse;
previousTraverse = currentTraverse;
x = x+(headerPtr -> size); 
currentTraverse = &memory[x];
     }

 }


    
fprintf (stderr, "ERROR: Invalid Ptr at (FILE: LINE:)\n");
    

}


// Temporary main Function that tests out mymalloc
int main (int argc, char** argv)
{
 printf("TEST1\n");
   

    for(int y = 0; y<50; y++)
    {
        //1 malloc() and immediately free() a 1-byte chunk, 120 times.
        for(int x = 0; x<120; x++)
        {
            int *p;
            p = (int*)(mymalloc(1));
            myfree(p);
        }
    }

   

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    printf("TEST2\n");

   

    //2 Use malloc() to get 120 1-byte chunks, storing the pointers in an array, then use free() to deallocate the chunks.
    char* arr[120];
    for(int y = 0; y<50; y++)
    {
        for(int x = 0; x<120; x++)
        {
            arr[x] = (mymalloc(1));
        }
        for(int x = 0; x<120; x++)
        {
            myfree(arr[x]);
        }
    }


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


printf("TEST 3\n");
    //3 Randomly choose between
            //•Allocating a 1-byte chunk and storing the pointer in an array
            //•Deallocating one of the chunks in the array (if any)
        //Repeat until you have called malloc() 120 times, then free all remaining allocated chunks
   char *MT = memory;
   struct header * ptr;
     char* arr2[120];
    for(int x = 0; x<120; x++)

    {
//        printf("%d\n", x);
        arr2[x] = NULL;
    }
   
    int mallocCount = 0;
  int c=0;
    while (c < 120)
    {
      ptr =(struct header *) MT;
        long random;
        long random1 = 122131233213;
        long random2 =124453432;
        long random3 = 2145455432;
        
      if(random %2 ==0) {
            arr2[mallocCount] = (mymalloc(1));
            printf("This is the pointer's size %d\n", ptr->size);
            printf("This is the pointer's validity %d\n", ptr->validity);
            printf("malloc'd %d\n", mallocCount);
            // *arr2[mallocCount] = 'c';
            mallocCount++;
        } else  {
            int freedPointer = 0;
            int count = 0;
            while(freedPointer == 0)
            {
                if(arr2[count] != NULL)
                {
                    myfree(arr2[count]);
                    arr2[count] = NULL;
                    printf("freed %d\n", count);
                printf("This is the pointer's size %d \n", ptr->size);
                printf("This is the pointer's validity %d\n", ptr->validity);
                    freedPointer = 1;
                } else {
                    count++;
                    if(count >= 120)
                    {
                        break;
                    }
                }
            }
        }
  c ++;
    }
    printf("Done! Emptying...\n");
    for(int x = 0; x<120; x++)
    {
        if(arr2[x] != NULL)
        {
           myfree(arr2[x]);
            arr2[x] = NULL;
            printf("freed %d\n", x);
        }
    }


   memDump();
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
printf("TEST4\n");


    //2 Use malloc() to get 120 1-byte chunks, storing the pointers in an array, then use free() to deallocate the chunks but backwards.
    char* arr3[120];
    for(int y = 0; y<50; y++)
    {
        for(int x = 0; x<120; x++)
        {
            arr3[x] = (mymalloc(1));
        }
        for(int x = 119; x>=0; x--)
        {
            myfree(arr3[x]);
        }
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


printf("TEST5\n");

    
    //2 Use malloc() to get 120 1-byte chunks, storing the pointers in an array, then use free() to deallocate the chunks.
    char* arr4[120];
    for(int y = 0; y<50; y++)
    {
        for(int x = 0; x<120; x++)
        {
            arr4[x] = (mymalloc(1));
        }
        for(int x = 0; x<120; x+=2)
        {
            myfree(arr4[x]);
        }
        for(int x = 1; x<120; x+=2)
        {
            myfree(arr4[x]);
        }
    }

 */ 
    return EXIT_SUCCESS;

}

//testmemgrind: memgrind.c
    //gcc  memgrind.c -o testmemgrind -DREALMALLOC


void garbage(char *a, int size, char g) {
    for (int i = 0; i < size; i++) {
        a[i] = g;
    }
}

void print(char *a, int size) {
    for (int i = 0; i < size; i++) {
        printf("%c ", a[i]);
    }
    printf("\n");
}

int frag() {
    printf("PRE-FREE\n");
    char one = 0x55;
    char two = 0x60;
    char three = 0x70;
    char four = 0x75;
    char *a = mymalloc(20);
    char *b = mymalloc(15);
    char *c = mymalloc(25);
    memDump();
    garbage(a, 20, one);
    garbage(b, 15, two);
    garbage(c, 25, three);
    print(a, 20);
    print(b, 15);
    print(c, 25);
    
//////////////////////////////

    myfree(b);
    char *d = mymalloc(5);
    garbage(d, 5, four);

    printf("POST-FREE AND ADDITION OF d ARRAY\n");
    print(a, 20);
    print(d, 5);
    print(c, 25);

    if (b == d) {
        return 0;
    } else {
        return -1;
    }

}

int fragCoalesce()
{
    printf("PRE-FREE\n");
    char one = 0x55;
    char two = 0x60;
    char three = 0x70;
    char four = 0x75;
    char five = 0x76;

    char *a = mymalloc(20);
    char *b = mymalloc(15);
    char *c = mymalloc(25);
    char *d = mymalloc(10);
    garbage(a, 20, one);
    garbage(b, 15, two);
    garbage(c, 25, three);
    garbage(d, 10, four);
    print(a, 20);
    print(b, 15);
    print(c, 25);
    print(d, 10);

    /////////////////////

    myfree(b);
    myfree(c);

    char *e = mymalloc(30);
    garbage(e, 30, five);

    printf("POST-FREE+COALESCE AND ADDITION OF e ARRAY\n");
    print(a, 20);
    print(e, 30);
    print(d, 10);

    if (b == e) {
        return 0;
    } else {
        return -1;
    }
}

//////////////////////////////////

int fragCoalesceV2()
{
    printf("PRE-FREE\n");
    char one = 0x55;
    char two = 0x60;
    char three = 0x70;
    char four = 0x75;
    char five = 0x76;

    char *a = mymalloc(20);
    char *b = mymalloc(15);
    char *c = mymalloc(25);
    char *d = mymalloc(10);
    garbage(a, 20, one);
    garbage(b, 15, two);
    garbage(c, 25, three);
    garbage(d, 10, four);
    print(a, 20);
    print(b, 15);
    print(c, 25);
    print(d, 10);

    /////////////////////

    myfree(c);
    myfree(b);

    char *e = mymalloc(30);
    garbage(e, 30, five);

    printf("POST-FREE+COALESCE AND ADDITION OF e ARRAY\n");
    print(a, 20);
    print(e, 30);
    print(d, 10);

    if (b == e) {
        return 0;
    } else {
        return -1;
    }
}

//////////////////////////////////////////

int fragFreeToMalloc()
{
    printf("PRE-FREE\n");
 char one = 0x55;
     char two = 0x60;
     char three = 0x70;
  //   char four = 0x75;
   //  char five = 0x76;

    char *a = mymalloc(20);
    char *b = mymalloc(100);
    char *c = mymalloc(20);

     garbage(a, 20, one);
     garbage(b, 100, two);
     garbage(c, 20, three);

    print(a, 20);
    print(b, 100);
    print(c, 20);


    /////////////////

    myfree(b);

    char *d = mymalloc(5);
    char *e = mymalloc(5);

    // garbage(d, 5, four);
    // garbage(e, 5, five);

    printf("POST-FREE AND ADDITION OF d AND e ARRAYS\n");
    print(a, 20);
    print(d, 5);
    print(e, 5);
    print(c, 20);


    if (b == d && ((b + 5 + 8) == e)) {
        return 0;
    } else {
        return -1;
    }
}

int superCoalesce()
{
    printf("PRE-FREE\n");
   // char one = 0x55;
    //char two = 0x60;
    //char three = 0x70;
    //char four = 0x75;
    //char five = 0x76;

    char *a = mymalloc(1000);
    char *b = mymalloc(1000);
    char *c = mymalloc(1000);
    char *d = mymalloc(1000);
    // garbage(a, 1000, one);
    // garbage(b, 1000, two);
    // garbage(c, 1000, three);
    // garbage(d, 1000, four);

//memDump();
//print(a, 1000);
 //print(b, 1000);
//print(c, 1000);
//print(d, 1000);

    /////////////////

    myfree(d);
    myfree(b);
    myfree(c);
    myfree(a);

    char *e = mymalloc(4000);
  //  garbage(e, 4000, five);

    printf("POST-FREE+COALESCE AND ADDITION OF e ARRAY\n");

    print(e, 4000);

    if (a == e) {
        return 0;
    } else {
        return -1;
    }


    return EXIT_SUCCESS;
}



//testmemgrind: memgrind.c
    //gcc  memgrind.c -o testmemgrind -DREALMALLOC


// MAKE SURE TO ADD FILE AND LINE TO ERROR CHECKS!!!