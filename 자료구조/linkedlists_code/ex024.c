#include <stdio.h>
#include <stdlib.h>

struct listNode {
  char data;
  struct listNode *link;
};
struct listNode *first = NULL;

struct hoho {
  char data2;
};

void main() {

  /* add elements to the linked list */
  struct listNode *newNode, *prevNode = NULL;
  struct listNode *currNode;
  char c;

	printf("size of struct: %lu\n", sizeof(newNode));
	printf("size of struct: %lu\n", sizeof(struct listNode));
	printf("size of struct: %lu\n", sizeof(struct hoho));

	return;

  for(c = 'a'; c <= 'e'; c++) {
    newNode = malloc(sizeof(*newNode));
    newNode->data = c;
    newNode->link = NULL;
    if(prevNode == NULL) first = newNode;
    else prevNode->link = newNode;
    prevNode = newNode;
  }

  /* print all elements in the list */
  currNode = first;
  while(currNode) {
    printf("%c ", currNode->data);
    currNode = currNode->link;
  }
  printf("\n"); 

  /* reading the third element from the list */
  currNode = first->link->link;
  printf("reading the 3rd element: %c\n", currNode->data);

  /* deleting the third element from the list */
  prevNode = first->link;
  currNode = prevNode->link;
  prevNode->link = prevNode->link->link;
  printf("deleting the 3rd element\n");
  free(currNode); 

  /* print all elements in the list */
  currNode = first;
  while(currNode) {
    printf("%c ", currNode->data);
    currNode = currNode->link;
  }
  printf("\n"); 

  /* inserting a new element 'f' as the third element in the list */
  newNode = malloc(sizeof(*newNode));
  newNode->data = 'f';
  newNode->link = NULL;
  prevNode = first->link;
  newNode->link = prevNode->link;
  prevNode->link = newNode; 
  printf("inserting 'f' as the 3rd element\n");
  
  /* insert a new element 'g' as the first element in the list */
  newNode = malloc(sizeof(*newNode));
  newNode->data = 'g';
  newNode->link = first;
  first = newNode;

  /* print all elements in the list */
  currNode = first;
  while(currNode) {
    printf("%p ", currNode);
    //printf("%c ", currNode->data);
    currNode = currNode->link;
  }
  printf("\n"); 
}
