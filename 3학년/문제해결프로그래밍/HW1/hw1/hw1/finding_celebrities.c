/*
 #include <stdio.h>
 #include <stdlib.h>
 #include <time.h>
 #include <string.h>
 
 // 연결 리스트 노드 구조체
 typedef struct Node {
 char name;
 struct Node* next;
 struct Node** knows; // 이 사람이 아는 사람들의 배열
 int knows_count;    // 아는 사람의 수
 } Node;
 
 // 연결 리스트에 새 노드 추가
 Node* addNode(Node* head, char name) {
 Node* newNode = (Node*)malloc(sizeof(Node));
 newNode->name = name;
 newNode->next = NULL;
 newNode->knows = NULL;
 newNode->knows_count = 0;
 
 if (head == NULL) {
 return newNode;
 }
 
 Node* current = head;
 while (current->next != NULL) {
 current = current->next;
 }
 current->next = newNode;
 return head;
 }
 
 // 연결 리스트에서 특정 노드 제거
 Node* removeNode(Node* head, char name) {
 if (head == NULL) return NULL;
 
 if (head->name == name) {
 Node* temp = head->next;
 free(head->knows);
 free(head);
 return temp;
 }
 
 Node* current = head;
 while (current->next != NULL && current->next->name != name) {
 current = current->next;
 }
 
 if (current->next != NULL) {
 Node* temp = current->next;
 current->next = temp->next;
 free(temp->knows);
 free(temp);
 }
 
 return head;
 }
 
 // 연결 리스트 출력
 void printList(Node* head) {
 Node* current = head;
 printf("현재 리스트: ");
 while (current != NULL) {
 printf("%c ", current->name);
 current = current->next;
 }
 printf("\n");
 }
 
 // 연결 리스트 메모리 해제
 void freeList(Node* head) {
 Node* current = head;
 while (current != NULL) {
 Node* temp = current;
 current = current->next;
 free(temp->knows);
 free(temp);
 }
 }
 
 // 특정 사람이 다른 사람을 아는지 확인하는 함수
 int knowsPerson(Node* person, char target) {
 for (int i = 0; i < person->knows_count; i++) {
 if (person->knows[i]->name == target) {
 return 1;
 }
 }
 return 0;
 }
 
 int main(void) {
 srand(time(NULL));
 
 // A부터 Z까지 연결 리스트 생성
 Node* head = NULL;
 for (char c = 'A'; c <= 'Z'; c++) {
 head = addNode(head, c);
 }
 
 printf("A부터 Z까지의 사람들이 있습니다.\n");
 printList(head);
 
 // 모든 사람들의 노드 포인터를 배열로 저장 (빠른 접근을 위해)
 Node* people[26];
 Node* current = head;
 int idx = 0;
 while (current != NULL) {
 people[idx++] = current;
 current = current->next;
 }
 
 // 아는 사람 관계 랜덤으로 설정
 printf("\n=== 사람들 간의 관계 설정 ===\n");
 for (int i = 0; i < 26; i++) {
 char person = 'A' + i;
 printf("%c가 아는 사람들: ", person);
 
 // 각 사람이 다른 사람들을 아는지 랜덤으로 결정
 for (int j = 0; j < 26; j++) {
 if (i == j) continue; // 자기 자신은 제외
 
 int know = rand() % 2; // 아는지 모르는지(알면 1, 모르면 0)
 if (know) {
 // knows 배열에 추가
 people[i]->knows_count++;
 people[i]->knows = realloc(people[i]->knows, people[i]->knows_count * sizeof(Node*));
 people[i]->knows[people[i]->knows_count - 1] = people[j];
 printf("%c ", 'A' + j);
 }
 }
 printf("\n");
 }
 
 // Celebrity 찾기 알고리즘
 
 
 // 1.1 일단 셀럽이 있는지부터 확인
 // 아는 사람이 0명인 사람이 아무도 없으면 셀럽이 없음. -> 종료
 int i = 0;
 while(people[i]->knows_count){
 i++;
 }
 printf("\n=== Celebrity 찾기 시작 ===\n");
 int question_count = 0;
 int found_celebrities = 0;
 
 // 각 사람에 대해 celebrity인지 확인
 for (int i = 0; i < 26; i++) {
 char candidate = 'A' + i;
 printf("\n--- %c가 celebrity인지 확인 ---\n", candidate);
 
 // 1단계: 다른 모든 사람들이 candidate를 아는지 확인
 int everyone_knows = 1;
 
 for (int j = 0; j < 26; j++) {
 if (i == j) continue; // 자기 자신은 제외
 
 question_count++;
 printf("질문 %d: Hey, you know Mr. (or Ms.) %c?\n", question_count, candidate);
 
 if (knowsPerson(people[j], candidate)) {
 printf("답변: Yes, I know Mr. (or Ms.) %c.\n", candidate);
 } else {
 printf("답변: No, I don't know Mr. (or Ms.) %c.\n", candidate);
 everyone_knows = 0;
 printf("결과: %c는 celebrity가 아닙니다. (모든 사람이 아는 것이 아님)\n", candidate);
 break;
 }
 }
 
 // 2단계: candidate가 다른 사람을 아는지 확인
 if (everyone_knows) {
 printf("\n모든 사람이 %c를 압니다. 이제 %c가 다른 사람을 아는지 확인합니다.\n", candidate, candidate);
 
 int knows_someone = 0;
 
 for (int j = 0; j < 26; j++) {
 if (i == j) continue; // 자기 자신은 제외
 
 question_count++;
 printf("질문 %d: Hey, you know Mr. (or Ms.) %c?\n", question_count, 'A' + j);
 
 if (knowsPerson(people[i], 'A' + j)) {
 printf("답변: Yes, I know Mr. (or Ms.) %c.\n", 'A' + j);
 knows_someone = 1;
 printf("결과: %c는 celebrity가 아닙니다. (다른 사람을 알고 있음)\n", candidate);
 break;
 } else {
 printf("답변: No, I don't know Mr. (or Ms.) %c.\n", 'A' + j);
 }
 }
 
 // Celebrity 조건 확인
 if (!knows_someone) {
 printf("🎉 Celebrity를 찾았습니다! %c는 celebrity입니다!\n", candidate);
 found_celebrities++;
 break; // Celebrity는 최대 1명이므로 찾으면 종료
 }
 }
 }
 
 // 최종 결과 출력
 printf("\n=== 최종 결과 ===\n");
 if (found_celebrities > 0) {
 printf("성공! %d명의 celebrity를 찾았습니다!\n", found_celebrities);
 } else {
 printf("Celebrity가 없습니다.\n");
 }
 
 printf("총 질문 수: %d\n", question_count);
 
 // 메모리 해제
 freeList(head);
 
 return 0;
 }
 */
