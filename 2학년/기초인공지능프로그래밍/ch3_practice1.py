s= "This is sample data"
final_s = s[:8] + "final" + s[14:]
print(final_s)
print(final_s[len(final_s)-1])
print("***************************")
print("Tax is %s" % 12.345)
print("Tax is %5d" % 12.345)
print("Tax is %10f" % 12.345)
print("Tax is %10.2f" % 12.345)
print("Tax is %d%%" % 12.345)
print("***************************")
a = -100
print("a = %d"%a)
print("a = %3d"%a)
print("a = %4d"%a)
print("a = %10d"%a)
print("***************************")
b = -12345.1234567890123456789
print("b = %f"%b)
print("b = %.20f"%b)
print("b = %.2f"%b)
print("b = %5.2f"%b)
print("b = %15.3f"%b)

print("extra = %10.3f"%123456789.1234567)



