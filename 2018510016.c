#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct students

{
    int id;
    int grade;
    char name[15];
    char surname[15];
    char email[26];
};
typedef struct students StuStruct; //new type definition

struct indexer
{
    int indexLine;
    int id;
};
typedef struct indexer indexStruct;

// ref retrieved on 15/06/2021 https://gist.github.com/sudhanshuptl/d86da25da46aa3d060e7be876bbdb343
struct heap
{
    indexStruct *arr;
    int count;     // the amount of the elements in the heap
    int capacity;  //limit of the heap
    int heap_type; // 0 for min heap , 1 for max heap
};
typedef struct heap Heap;

Heap *CreateHeap(int capacity, int heap_type);     //creates heap
void insert(Heap *h, indexStruct key);             //insert to heap
void heapify_bottom_top(Heap *h, int index);       // heapify sort from bottom to top
void heapify_top_bottom(Heap *h, int parent_node); // heapify sort from top to bottom
indexStruct PopMin(Heap *h);                       // to remove the wanted element from the heap
indexStruct PopMin2(Heap *h);                      // popmin function without heapify
indexStruct fileReading(FILE *stream, int indexCounter, char separator[])
{
    indexStruct indexItem;
    char line[sizeof(StuStruct)];
    bool isEmpty = true;
    while (isEmpty && fgets(line, sizeof(line), stream))
    {
        isEmpty = false; // turns 1 tour
        char *tmp = strtok(line, separator);
        indexItem.id = atoi(tmp);
        indexItem.indexLine = indexCounter;
    }
    if (isEmpty)
        indexItem.id = -9; // emtpy file signal
    return indexItem;
}
indexStruct PopMin2(Heap *h) // popmin function without heapify
{
    indexStruct pop;
    if (h->count == 0)
        printf("\n Heap is Empty \n");
    pop = h->arr[0];
    h->arr[0] = h->arr[h->count - 1];
    h->count--;
    return pop;
}

int main(int argc, char *argv[])
{
    if (strcmp(argv[0], "./XSort") != 0)
    {
        printf("Error! Your file name must be XSort");
        exit(1); // interrupts the compiler
    }
    if (argc != 5)
    {
        printf("4 paramaters must be given");
        exit(2); // interrupts the compiler
    }
    char *fileToRead = argv[1];
    char *outputFileName = argv[2];
    int B = atoi(argv[3]);
    int pageSize = atoi(argv[4]);
    /*
    //creating buffers according to given input
    indexStruct buffers;
    for (int i = 0; i < B; i++)
    {
        buffers[i] = (indexStruct *)malloc(pageSize * sizeof(indexStruct));
    }
    */
    FILE *stream = fopen(fileToRead, "r");
    fseek(stream, 0, SEEK_END); //// goes to the end of the file
    int totalByte = ftell(stream);
    int arrLength = totalByte / sizeof(StuStruct);
    printf("file length= %d\n", arrLength);
    fseek(stream, 0, SEEK_SET); // goes to the beginning of the file
    char line[sizeof(StuStruct)];
    StuStruct stuItem;
    indexStruct indexItem;
    int HEAP_SIZE = pageSize * 1024 / sizeof(StuStruct); // 32 BYTE * kilo(1024)/ (student struct size)
    Heap *heap1 = CreateHeap(HEAP_SIZE, 0);              //Min Heap
    Heap *heap2 = CreateHeap(HEAP_SIZE, 0);              // second heap to collect smaller elements than root
    Heap *tempHeap;                                      // to swap heaps
    char sortedEachFileName[5] = "0";                    // files name : creates names staring from "0"
    bool startingHeapFlag = true;                        // it will be used in replacement sorting first step
    FILE *fw = fopen(sortedEachFileName, "w");           // fw pointer
    if (heap1 == NULL)
    {
        printf("__Memory Issue____\n");
        return -1;
    }
    int fileNameChanger = 1;
    fgets(line, sizeof(StuStruct), stream);
    int indexCounter = 2; // csv startws with 1 so passing the first line 1+1=2
    indexStruct temp;     // for heap1
    indexStruct temp1;    // for heap2 and swap
    //file reading starting point
    while (fgets(line, sizeof(StuStruct), stream))
    {
        char *tmp = strtok(line, ";");
        indexItem.id = atoi(tmp);
        indexItem.indexLine = indexCounter;
        //fillig the heap1 for starting
        if (startingHeapFlag)
        {
            if (heap1->count < heap1->capacity)
            {
                insert(heap1, indexItem);
                if (heap1->count == heap1->capacity)
                    startingHeapFlag = false;
            }
        }
        else
        {
            temp1 = indexItem;
            while (heap1->count > 0)
            {
                // there is no student
                if (temp1.id == -9)
                {
                    fclose(fw);
                    break;
                }
                // if element smaller than root, then insert to heap2
                if (temp1.id < heap1->arr[0].id)
                {
                    insert(heap2, temp1);
                    temp = PopMin(heap1);
                    fprintf(fw, "%d %d\n", temp.id, temp.indexLine);
                }
                else //if element grater than root, then insert to heap1
                {
                    temp = PopMin2(heap1);
                    fprintf(fw, "%d %d\n", temp.id, temp.indexLine);
                    heap1->arr[0] = temp1;
                    heap1->count++;
                    heapify_top_bottom(heap1, 0);
                }
                if (heap1->count == 0)
                {
                    tempHeap = heap1; // swap heaps
                    heap1 = heap2;
                    heap2 = tempHeap;
                    fclose(fw);
                    sprintf(sortedEachFileName, "%d", fileNameChanger); // filename0, filename1 etc.
                    fw = fopen(sortedEachFileName, "w");
                    fileNameChanger++;
                }
                temp1 = fileReading(stream, indexCounter, ";");
                indexCounter++;
            }
        }
        indexCounter++;
    }
    if (heap2->count != 0)
    {
        indexStruct temp;
        sprintf(sortedEachFileName, "%d", fileNameChanger);
        fw = fopen(sortedEachFileName, "w");
        int heap2Len = heap2->count;
        for (int i = 0; i < heap2Len; i++)
        {
            temp = PopMin(heap2);
            fprintf(fw, "%d %d\n", temp.id, temp.indexLine);
        }
        // Close the file
        fclose(fw);
    }
    printf("Phase 0: %d sorted segment created initially!\n", fileNameChanger + 1);
    free(heap1); // free memory
    free(heap2);
    fclose(stream); // flose file pointer

    //merge process
    // i tried to create merge part. but it did not work
    /*
    //FILE *fp;
    //indexStruct data;
    
    for (int i = 0; i < fileNameChanger+1; i++)
    {
        sprintf(fileToRead,"d",i);
        fp=fopen(fileToRead,"r");
        data=fileReading(fp,atoi(strtok(NULL," "))," ");
        printf("%d %d\n",data.id,data.indexLine);
    }
    */
    printf("Phase 1: 2 sorted segment created initially!\n");
    return 0;
}
Heap *CreateHeap(int capacity, int heap_type)
{
    Heap *h = (Heap *)malloc(sizeof(Heap)); //one is number of heap

    //check if memory allocation is fails
    if (h == NULL)
    {
        printf("Memory Error!");
        return NULL;
    }
    else
    {
        h->heap_type = heap_type;                                       //0 min heap, 1 max hep
        h->count = 0;                                                   // amount of heap elements
        h->capacity = capacity;                                         // limit of the heap
        h->arr = (indexStruct *)malloc(capacity * sizeof(indexStruct)); //crates array size using struct size and capacity
        //check if allocation succeed
        if (h->arr == NULL)
        {
            printf("Memory Error!");
            return NULL;
        }
    }
    return h;
}
// insert to the heap
void insert(Heap *h, indexStruct key)
{
    if (h->count < h->capacity) // if there is still some capacity to add, then add
    {
        h->arr[h->count] = key;          // appending
        heapify_bottom_top(h, h->count); // heapify sort
        h->count++;                      // new element has been added
    }
}

void heapify_bottom_top(Heap *h, int index) // heapify sort from bottom to top
{
    indexStruct temp;
    int parent_node = (index - 1) / 2; // index = 2n+1 hence in order to find n parent node must be like this
    //comparison between IDs
    if (h->arr[parent_node].id > h->arr[index].id)
    {
        temp = h->arr[parent_node]; // swap
        h->arr[parent_node] = h->arr[index];
        h->arr[index] = temp;
        heapify_bottom_top(h, parent_node); //recursive call
    }
}

void heapify_top_bottom(Heap *h, int parent_node) // heapify sort from top to bottom
{
    int left = parent_node * 2 + 1;
    int right = parent_node * 2 + 2;
    int min;
    indexStruct temp;

    if (left >= h->count || left < 0)
        left = -1;
    if (right >= h->count || right < 0)
        right = -1;

    if (left != -1 && h->arr[left].id < h->arr[parent_node].id)
        min = left;
    else
        min = parent_node;
    if (right != -1 && h->arr[right].id < h->arr[min].id)
        min = right;

    if (min != parent_node)
    {
        temp = h->arr[min]; // swap
        h->arr[min] = h->arr[parent_node];
        h->arr[parent_node] = temp;
        heapify_top_bottom(h, min); // recursive  call
    }
}

indexStruct PopMin(Heap *h)
{
    indexStruct pop;
    if (h->count == 0)
        printf("\n Heap is Empty \n");
    // replace first node by last and delete last
    pop = h->arr[0];
    h->arr[0] = h->arr[h->count - 1];
    h->count--;
    heapify_top_bottom(h, 0);
    return pop;
}