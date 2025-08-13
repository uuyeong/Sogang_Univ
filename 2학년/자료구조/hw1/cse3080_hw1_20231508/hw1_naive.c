#include <stdio.h>
#include <string.h>

#define MAX_STRING_SIZE 10000000
#define MAX_PATTERN_SIZE 3000

char string[MAX_STRING_SIZE];
char pat[MAX_PATTERN_SIZE];
int idx[MAX_STRING_SIZE];

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

      

    //naive 알고리즘
    int i = 0;
    int num = 0;
    
    int string_length = strlen(string);
    int  pat_length  = strlen(pat);

    while (i <= string_length - pat_length) {
        int j = 0;
        while (j < pat_length  && string[i + j] == pat[j])
            j++;
        if (j == pat_length) {
            idx[num++] = i;
            i += 1;
        } else {
            i++;
        }
    }
        

    //파일 출력

    FILE  *fp = fopen("result_naive.txt", "w");
        if (fp == NULL) {
            printf("The string file does not exist.\n");
            return 1;
        }
 
        fprintf(fp, "%d\n", num);
        for (int i = 0; i < num; i++) {
            fprintf(fp, "%d ", idx[i]);
        }
        fclose(fp);
    printf("Program complete. Result saved to result_naive.txt\n");
    return 0;
}

