s = "My score is 89. That's good!"
score = s[12:14:]
print(score)
s1 = s[:12:]
newS = s1 + str(100) + s[14::]
reverseS = newS[::-1]
print(newS)
print(reverseS)

score = 89
s2 = "My score is %d. That's %s" %(score, "good")
print(s2)

print("rate is %s" %3.456)
print("rate is %d" %3.456)
print("rate is %f" %3.456)
print("rate is %d%%" %75)


a = 1/3
print("1/3 =", a, "too many")
print("a = %.3f" %a)

a = -123
print("a = %d" %a)
print("a = %4d" %a)
print("a = %1d" %a)
print("a = %6d" %a)



