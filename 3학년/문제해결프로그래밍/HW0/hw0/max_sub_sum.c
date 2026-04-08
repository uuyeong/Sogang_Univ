#include <stdio.h>

int main(int argc, const char * argv[]) {
    int arr[10] = {31, -41, 59, 26, -53, 58, 97, -93, -23, 84};
    int n = 10;
    
    for(int i=0;i<10;i++){
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    
    int cur_sum = arr[0];
    int max_sum = arr[0];
    
    for(int i=1;i<n;i++){
        if((cur_sum+arr[i]) < arr[i]) cur_sum = arr[i];
        else cur_sum = (cur_sum + arr[i]);
        
        if(max_sum < cur_sum) max_sum = cur_sum;
    }
    
    printf("result: %d\n", max_sum);
    
    
    return 0;
}
