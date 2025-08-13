#include <stdio.h>
#include <string.h>

#define MAX_STRING_SIZE 10000000
#define MAX_PATTERN_SIZE 3000

int start_idx=0;

void pmatch(char*, char*);
void fail(char*);

int failure[MAX_PATTERN_SIZE];
char string[MAX_STRING_SIZE];
char pat[MAX_PATTERN_SIZE];
int idx[MAX_STRING_SIZE];


void pmatch(char* string, char* pat){
    
    int i=0, j=0;
    int string_length = strlen(string);
    int pat_length = strlen(pat);
    
    //kmp알고리즘
    while(i < string_length && j < pat_length) {
      if(string[i] == pat[j]){
          i++;
          j++;
          if(j==pat_length){
              idx[start_idx] = i - pat_length;
              start_idx++;
              j = failure[j-1] +1 ;
          }
          
      }
      else if(j == 0) i++;
      else j = failure[j-1] + 1;
    }
}


void fail(char* pat){
    int i, j, n = strlen(pat);
    failure[0] = -1;
    for(j=1; j<n; j++) {
      i = failure[j-1];
      while((pat[j] != pat[i+1]) && (i >= 0)) i = failure[i];
      if(pat[j] == pat[i+1]) failure[j] = i+1;
      else failure[j] = -1;
    }
}




int main() {
     
    //파일 입력
    FILE *fp1 = fopen("string.txt", "r");
    FILE *fp2 = fopen("pattern.txt", "r");
    
    if(fp1 == NULL || fp2 == NULL){
        printf("The string file does not exist.\n");
        return 1;
        }

    fgets(string, MAX_STRING_SIZE, fp1);
    if (string[strlen(string) - 1] == '\n')
        string[strlen(string) - 1] = '\0';
    fgets(pat, MAX_PATTERN_SIZE, fp2);
    if (pat[strlen(pat) - 1] == '\n')
        pat[strlen(pat) - 1] = '\0';

    fclose(fp1);
    fclose(fp2);
    
    
    
    //kmp 알고리즘
    fail(pat);
    pmatch(string, pat);
    
    
    
    //파일 출력
    FILE  *fp = fopen("result_kmp.txt", "w");
    if (fp == NULL) {
        printf("The string file does not exist.\n");
        return 1;
        }

    fprintf(fp, "%d\n", start_idx);
    for (int i = 0; i < start_idx; i++) {
        fprintf(fp, "%d ", idx[i]);
    }
    fclose(fp);
    printf("Program complete. Result saved to result_kmp.txt\n");
    

    return 0;
}

