#include "header.h"

void Count_Page(int page, int *arr){
for(int j=1;j<=page;j++){
            int tmp = j;
            while(tmp>0){
                arr[tmp%10]++;
                tmp /= 10;
            }
        }

}
