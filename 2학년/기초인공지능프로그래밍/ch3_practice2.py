Day1 = "2050 12 25"
year, month, day = Day1.split()
print(year, type(year))
print(month, type(month))
print(day, type(day))
Day2 = "2050/12/31"
L = Day2.split("/")
print(L, type(L))
DaywithSpace= " ".join(L)
print(DaywithSpace)
DayNoSpace = "".join(L)
print(DayNoSpace)2
print()
name = "Python"
score = "67"
print(score.isalpha())
print(score.isnumeric())
print(score.isalnum())
print(name.islower())
new_name = name.upper()
print("%s has changed to %s" %(name, new_name))
print()
s = "This is Good chance good"
s1 = s.replace("Good", "GOOD")
s2 = s.replace("good", "")
print(s1)
print(s2)
