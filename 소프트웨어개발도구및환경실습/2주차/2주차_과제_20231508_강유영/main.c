#include "header.h"

int main(void){
    int t, page;
    int arr[10];
    
    scanf("%d", &t);
    
    
    for(int i=0;i<t;i++){
        
        for(int i=0;i<10;i++){
            arr[i] = 0;
        }
        
        scanf("%d", &page);
        
        Count_Page(page, arr);
    
        Print_Page(arr);
    }
    
    
    return 0;
}

