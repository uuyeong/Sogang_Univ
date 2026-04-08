#include <stdio.h>

int main(void){
    
    int n;
    printf("Input: N=");
    scanf("%d", &n);
    
    
    // 소인수분해 알고리즘
    int temp = n;
    int output = 0;
    
    printf("소인수 분해 결과: ");
    
    while(temp%2==0){ //2로 나눌 수 있는 동안
        printf("2 ");
        temp /= 2;
        output += 2;
    }
    
    int i = 3;
    while(i<=n){
        while(temp%i==0){ //i로 나누어 떨어지면..
            printf("%d ", i);
            temp /= i;
            output += i;
            
        }
        i++;
    }
    
    printf("\nOutput: %d\n", output);
    
    return 0;
}
