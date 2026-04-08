//hw1

#include <syscall.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  if (argc != 5) {
    printf("Usage: %s [num1] [num2] [num3] [num4]\n", argv[0]);
    exit(1);
  }

  int num1 = atoi(argv[1]);
  int num2 = atoi(argv[2]);
  int num3 = atoi(argv[3]);
  int num4 = atoi(argv[4]);

  int fib_result = fibonacci(num1);
  int max_result = max_of_four_int(num1, num2, num3, num4);

  printf("%d %d\n", fib_result, max_result);

  return 0;
}