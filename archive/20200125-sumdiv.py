print("This is a list of numbers.")
print("=== The sum of specific dividable numbers in 1 to 1000000. ===")
print()
p = 1
def math():
    a = 0
    p = 2
    for l in range(0, 50):
        for i in range(0, 1000001):
            if i % p == 0:
                a += i
        print("the sum of all numbers that can be divided by",p,"are:")
        print(a)
        print()
        p = p+1
        a = 0

math()