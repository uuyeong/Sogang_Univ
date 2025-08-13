#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_ELEMENTS 1000001
#define HEAP_FULL(n) (n == MAX_ELEMENTS-1)
#define HEAP_EMPTY(n) (!n)

typedef struct {
  int key;
} element;

element max_heap[MAX_ELEMENTS];
element min_heap[MAX_ELEMENTS];
element max_heap_save[MAX_ELEMENTS];
element min_heap_save[MAX_ELEMENTS];

int max_n = 0;
int min_n = 0;
int max_n_save = 0;
int min_n_save = 0;

void insert_max_heap(element item, int *n);
void insert_min_heap(element item, int *n);
element delete_max_heap(int *n);
element delete_min_heap(int *n);


int main(int argc, char *argv[]) {
    
    (void)argc;
    clock_t start, end;
    double time;

    start = clock();
    
    //파일 입출력
    char command[10];
    element insert;
    FILE *fp = fopen(argv[1], "r");
    FILE *fp2 = fopen("hw2_result.txt", "w");

    
    if (fp == NULL){
        printf("The input file does not exist.\n");
        return 1;
    }
    while (fscanf(fp, "%s", command) != EOF) {
        if (strcmp(command, "ASCEND") == 0) {
            // 오름차순 정렬 후 출력
            int num = min_n_save;
            for(int k=1; k <= num; k++){
                min_heap[k]=min_heap_save[k];
            }
            min_n = min_n_save;
            for (int j = 0; j < num; j++) {
                fprintf(fp2, "%d ", delete_min_heap(&min_n).key);
            }
            fprintf(fp2, "\n");
            
        }
        else if (strcmp(command, "DESCEND") == 0) {
            // 내림차순 정렬 후 출력
            int num = max_n_save;
            for(int k=1; k <= num; k++){
                max_heap[k]=max_heap_save[k];
            }
            max_n = max_n_save;
            for (int j = 0; j < num; j++) {
                fprintf(fp2, "%d ", delete_max_heap(&max_n).key);
            }
            fprintf(fp2, "\n");
        }
        else if (strcmp(command, "INSERT") == 0) {
            // 원소 삽입
            fscanf(fp, "%d", &insert.key);
            insert_max_heap(insert, &max_n_save);
            insert_min_heap(insert, &min_n_save);
        }
        else printf("Error : incorrect input");
    }
    
    end = clock();
    time = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("output written to hw2_result.txt.\n");
    printf("running time: %f seconds\n", time);

    fclose(fp);
    

  }

void insert_max_heap(element item, int *n) {
  int i;
  if(HEAP_FULL(*n)) {
    fprintf(stderr, "The heap is full.\n");
    exit(1);
  }
  i = ++(*n);
  while((i != 1) && (item.key > max_heap_save[i/2].key)) {
    max_heap_save[i] = max_heap_save[i/2];
    i /= 2;
  }
  max_heap_save[i] = item;
}

void insert_min_heap(element item, int *n) {
    int i;
    if(HEAP_FULL(*n)) {
      fprintf(stderr, "The heap is full.\n");
      exit(1);
    }
    i = ++(*n);
    while((i != 1) && (item.key < min_heap_save[i/2].key)) {
      min_heap_save[i] = min_heap_save[i/2];
      i /= 2;
    }
    min_heap_save[i] = item;
}

element delete_max_heap(int *n) {
  int parent, child;
  element item, temp;
  if(HEAP_EMPTY(*n)) {
    fprintf(stderr, "The heap is empty");
    exit(1);
  }
    item = max_heap[1];
  temp = max_heap[(*n)--];
  parent = 1;
  child = 2;
  while(child <= *n) {
    if((child < *n) && (max_heap[child].key < max_heap[child+1].key)) child++;
    if(temp.key >= max_heap[child].key) break;
    max_heap[parent] = max_heap[child];
    parent = child;
    child *= 2;
  }
  max_heap[parent] = temp;
  return item;
}

element delete_min_heap(int *n) {
    int parent, child;
    element item, temp;
    if(HEAP_EMPTY(*n)) {
      fprintf(stderr, "The heap is empty");
      exit(1);
    }
      item = min_heap[1];
    temp = min_heap[(*n)--];
    parent = 1;
    child = 2;
    while(child <= *n) {
      if((child < *n) && (min_heap[child].key > min_heap[child+1].key)) child++;
      if(temp.key <= min_heap[child].key) break;
      min_heap[parent] = min_heap[child];
      parent = child;
      child *= 2;
    }
    min_heap[parent] = temp;
    return item;
    
}

