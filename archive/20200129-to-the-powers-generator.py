import math
def count(x):
    print("===",x,"TO THE POWERS OF 1 TO 100 ===")
    for i in range(1, 101):
        rsl = x**i
        print(x,"to the power of",i,"=",rsl)
for a in range(1,11):
    count(a)