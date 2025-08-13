#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_ELEMENTS 5000001
#define MAX_VERTEX 10000
#define HEAP_EMPTY(n) (!n)

typedef struct {
    int x,y;
    int weight;
} T;

FILE *fp;
int total_cost = 0;
int size = 0;
int V, E;

T min_heap[MAX_ELEMENTS];
int parent[MAX_VERTEX];


int find(int i);
void union_parent(int i, int j);
void insert_min_heap(T item, int *n);
T delete_min_heap(int *n);
void Kruskal(int n);

//i의 루트 정점을 찾는다.
int find(int i) {
    while (parent[i] >= 0) {
        i = parent[i];
    }
    return i;
}

//두 집합을 합친다
void union_parent(int i, int j) {
    int temp = parent[i] + parent[j];
    int small = (parent[i] > parent[j]) ? j : i;
    int big = (parent[i] > parent[j]) ? i : j;

    parent[big] = small;
    parent[small] = temp;
}

void insert_min_heap(T item, int *n) {
    int i;
    i = ++(*n);
    while((i != 1) && (item.weight < min_heap[i/2].weight)) {
      min_heap[i] = min_heap[i/2];
      i /= 2;
    }
    min_heap[i] = item;
}

T delete_min_heap(int *n) {
    int parent, child;
    T item, temp;
    if(HEAP_EMPTY(*n)) {
      fprintf(stderr, "The heap is empty");
      exit(1);
    }
    item = min_heap[1];
    temp = min_heap[(*n)--];
    parent = 1;
    child = 2;
    while(child <= *n) {
      if((child < *n) && (min_heap[child].weight > min_heap[child+1].weight)) child++;
      if(temp.weight <= min_heap[child].weight) break;
      min_heap[parent] = min_heap[child];
      parent = child;
      child *= 2;
    }
    min_heap[parent] = temp;
    return item;
}

void Kruskal(int n) {
    int count = 0;
    int tmp = E;
    int u, v;
    T new;
    if (count < (n - 1)) {
        while (1) {
            if (!tmp) break;
            new = delete_min_heap(&tmp);
            u = find(new.x);
            v = find(new.y);
            if (u != v) {
                fprintf(fp, "%d %d %d\n", new.x, new.y, new.weight);
                total_cost += new.weight;
                count++;
                union_parent(u, v);
            }
            if (count == V - 1) break;
        }
    }
    fprintf(fp, "%d\n", total_cost);
    if (count == V - 1) fprintf(fp, "CONNECTED\n");
    else fprintf(fp, "DISCONNECTED\n");
}
    


int main(int argc, char *argv[]) {
    
    (void)argc;
    clock_t start, end;
    double time;
    start = clock();
    
    //파일 입출력
    FILE *fp2 = fopen(argv[1], "r");
    fp = fopen("hw3_result.txt","w");
    
    if (!argv[1] || argv[2]) {
            printf("usage:./hw3 input_filename\n");
            return 1;
        }
        if (!fp2) {
            printf("The input file does not exist.\n");
            return 1;
        }
    
    int count = 1;
    T min;
    
    fscanf(fp2, "%d%d", &V, &E);
    
    for (int i = 0; i < V; i++) {
        parent[i] = -1;
    }
    
    while (1) {
        fscanf(fp2, "%d %d %d", &min.x, &min.y, &min.weight);
        insert_min_heap(min, &size);
        if (count++ == E) break;
    }
    Kruskal(V);
    
    
  
    end = clock();
    time = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("output written to hw3_result.txt.\n");
    printf("running time: %f seconds\n", time);

    fclose(fp);
    fclose(fp2);
    return 0;

  }


