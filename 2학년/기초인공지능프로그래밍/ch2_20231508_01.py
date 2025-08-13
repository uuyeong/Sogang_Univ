num = 1002
n1 = num // 1000
n2 = num // 100 % 10
n3 = num // 10 % 10
n4 = num % 10

print("Number ", num, ":", n1, "+", n2, "+", n3,"+", n4,"=",n1+n2+n3+n4)
print("Average: ", (n1+n2+n3+n4)/4)

