
#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 24
#define FALSE 0
#define TRUE 1

struct node {
  int data;
  struct node *link;
};
typedef struct node* nodePointer;

void main() {

  short int out[MAX_SIZE]; //아이템이 들어갔는지 아닌지 나타냄
  nodePointer seq[MAX_SIZE]; //first를 의미
  nodePointer x, y, top;
  int i, j, n;

  printf("Enter the size (<= %d) ", MAX_SIZE);
  scanf("%d", &n);

  for(i=0; i<n; i++) {
    out[i] = TRUE; //해당 원소의 아이템이 아직 클래스에 들어가지 않았다는 뜻
    seq[i] = NULL;
  }

  /* phase 1: input the equivalence pairs */
  printf("Enter a pair of numbers (-1 -1 to quit): ");
  scanf("%d%d", &i, &j);
  while(i >= 0) {
    x = (nodePointer)malloc(sizeof(*x)); //포인터임에 주의하자!
    x->data = j;
    x->link = seq[i]; //맨 처음 seq[i]는 NULL이다. 노드의 형태조차 아니다.
    seq[i] = x;
      
    x = (nodePointer)malloc(sizeof(*x));
    x->data = i;
    x->link = seq[j];
    seq[j] = x;
    printf("Enter a pair of numbers (-1 -1 to quit): ");
    scanf("%d%d", &i, &j);
  }

  /* phase 2: output the equivalence classes */
  for(i = 0; i < n; i++) {
    if(out[i]) {
      printf("\nNew class: %5d", i);
      out[i] = FALSE;
      x = seq[i];
      top = NULL;
      for( ; ; ) {
        while(x) {
          j = x->data;
          if(out[j]) {
            printf("%5d", j);
            out[j] = FALSE; //여기에서 FALSE를 해주지만 어차피 다음 큰 for문으로 넘어가기 전에 안에 있는 for(;;)에서 바로 seq[j]를 다룬다. 
            y = x->link;
            x->link = top; //top은 시작 시 NULL
            top = x;
            x = y;
          }
          else x = x->link;
        }
        if(!top) break;
        x = seq[top->data];
        top = top->link;
      }
    }
  }

  printf("\n");
}



