#include <stdio.h>

int main(void){
    int x,y,z; //1의 개수
    scanf("%d %d %d", &x, &y, &z);
    
    int tmp = ~(!x);
    
    printf("tmp = %x", tmp);
    x = (tmp & z) | (~tmp & y);
    
    printf("%d\n", x);
    printf("0x%x\n", x);
    
    return 0;
    
    
    
    
    
}
