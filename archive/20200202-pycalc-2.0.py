import time
import pyperclip
import math
import random as r
import os
# PUT YOUR STRINGS HERE! vvv
__name__ = "Calculator"
ver = "v2.0"
dat = "February 2, 2020."
prl = "Python (98%) Batch (2%)"
# OK THEN SO WE CAN SKIP TO THE CODE PART
os.system('color f0 && cls')
def all(x):
    os.system('title Calculator')
    os.system('cls')
    def quit():
        print("Terminating program...")
        os.system('color 0f')
        os.system('title Command Prompt')
        os.system('taskkill /f /im python.exe')
    def fail():
        print("Invalid code entered.")
        quit()
    def rng():
        print()
        print("GENERATING NUMBER...")
        print(r.randint(0,9999))
        input("press enter to continue...")
        all("0")
    def timesup():
        while True:
            os.system('color c0')
            print("TIME IS UP!")
            time.sleep(1)
            os.system('color 0c')
            print("TIME IS UP!")
            time.sleep(1)
    def timer(t):
        print()
        print("=== TIMER FOR",t,"SECONDS ===")
        print("Please note that you can stop it by closing this window/prompt.")
        while t>1:
            print(t,"seconds left.")
            time.sleep(1)
            t = t-1
            if t==1:
                print("1 second left.")
                time.sleep(1)
                timesup()
    def again(y, z):
        if y=="0":
            print("Do you want to try again?")
            print("VALID RESPONSES:")
            print("'y' = Reloads to the main menu of the calculator.")
            print("'n' = Quits by entering a killing task command or executing a invalid command.")
            print()
            agr = input(">")
            if agr=="y":
                all("1")
            elif agr=="n":
                quit()
            else:
                fail()
        if y=="1":
            sect = int(z)
            print("It seems that you are converting time.")
            print("Do you want to try again or convert the time to a timer?")
            print("VALID RESPONSES:")
            print("'y' = Reloads to the main menu of the calculator.")
            print("'n' = Quits by entering a killing task command or executing a invalid command.")
            print("'t' = Starts a timer according to the time given.")
            print()
            agr = input(">")
            if agr=="y":
                all("1")
            elif agr=="n":
                quit()
            elif agr=="t":
                timer(sect)
            else:
                fail()
    def c1():
        print()
        print("Type (1) For +, Otherwise type (2) For -.")
        inc = input(">")
        if inc=="1":
            print("Please input your first number.")
            fn = input(">")
            print("Please input your second number.")
            fnt = input(">")
            print("Please input (a) If number contain decimals, otherwise type (b).")
            fnty = input(">")
            if fnty=="a":
                print("Processing number...")
                b1 = float(fn)
                print("Converting number to decimal format...")
                b2 = float(fnt)
            elif fnty=="b":
                print("Processing number...")
                b1 = int(fn)
                print("Processing integer...")
                b2 = int(fnt)
            else:
                fail()
            rsl = b1 + b2
            print("RESULT:",b1,"+",b2,"=",rsl)
            print("Result has been copied to clipboard.")
            pyperclip.copy(rsl)
            again("0","0")
        elif inc=="2":
            print("Please input your first number.")
            fn = input(">")
            print("Please input your second number.")
            fnt = input(">")
            print("Please input (a) If number contain decimals, otherwise type (b).")
            fnty = input(">")
            if fnty=="a":
                print("Processing number...")
                b1 = float(fn)
                print("Converting number to decimal format...")
                b2 = float(fnt)
            elif fnty=="b":
                print("Processing number...")
                b1 = int(fn)
                print("Processing integer...")
                b2 = int(fnt)
            else:
                fail()
            rsl = b1 - b2
            print()
            print("=== RESULT FOUND ===")
            print("RESULT:",b1,"-",b2,"=",rsl)
            print("Result has been copied to clipboard.")
            pyperclip.copy(rsl)
            print()
            again("0")
        else:
            fail()
        again("0","0")
    def c2():
        print("Please input your number.")
        mulin = input(">")
        mulog = int(mulin)
        print("Processing number...")
        print()
        print("=== RESULT FOUND ===")
        for i in range(1,101):
            rslog = mulog*i
            print("RESULT =",mulin,"*",i,"=",rslog)
        print()
        print("All results done!")
        print("Result has been copied to clipboard.")
        pyperclip.copy(rslog)
        print()
        again("0","0")
    def c3():
        print("Please input your number.")
        mulin = input(">")
        mulog = int(mulin)
        print("Processing number...")
        print()
        print("=== RESULT FOUND ===")
        for i in range(1,101):
            rslog = mulog/i
            print("RESULT =",mulin,"/",i,"=",rslog)
        print()
        print("All results done!")
        print("Result has been copied to clipboard.")
        pyperclip.copy(rslog)
        print()
        again("0","0")
    def c4():
        print("Please input your number.")
        mulin = input(">")
        mulog = int(mulin)
        print("Processing number...")
        print()
        print("=== RESULT FOUND ===")
        temp = mulog
        for i in range(1,51):
            rslog1 = temp**i
            rslog2 = mulog**i
            print("RESULT(A): ",mulin,"^",i,"=",rslog2)
            print("RESULT(B): ",temp,"^",i,"=",rslog1)
            temp=temp+1
        print()
        print("All results done!")
        print("Result has been copied to clipboard.")
        pyperclip.copy(rslog1,rslog2)
        print()
        again("0","0")
    def c5():
        print("Type the factors of (*MYNUMBER*) from range 1 - 1.000.000")
        finder = input(">")
        num = int(finder)
        a = 0
        for i in range(1,1000001):
            if i % num == 0:
                a += i
        print("=== RESULTS FOUND===")
        print("The sum of all numbers in the range 1 - 1.000.000 that is a factor of",num,"are:")
        print(a)
        print("Result has been copied to clipboard.")
        pyperclip.copy(a)
        print()
        again("0","0")
    def c6():
        print()
        print("Please type your first number.")
        inp = input(">")
        num1 = int(inp)
        print("Please input your type of equation.")
        print("Type: (1) For multiply, (2) for divide, (3) for powered.")
        inp2 = input(">")
        inpint = int(inp2)
        if inpint==1:
            t = 1
            u = 1
        elif inpint==2:
            t = 2
            u = 2
        elif inpint==3:
            t = 3
            u = 3
        else:
            fail()
        if t==1:
            eks = "Please input your second number."
        if t==2:
            eks = "Please input your second number."
        if t==3:
            eks = "Please input your powered of (Like i want FIRSTNUMBER the to the power of THISINPUT)"
        print(eks)
        inp3 = input(">")
        num2 = int(inp3)
        print("Processing number and result...")
        if u==1:
            rsl = num1*num2
            print("=== RESULT FOUND ===")
            print("RESULT:",num1,"*",num2,"=",rsl)
        elif u==2:
            rsl = num1/num2
            print("=== RESULT FOUND ===")
            print("RESULT:",num1,"/",num2,"=",rsl)
        elif u==3:
            rsl = num1**num2
            print("=== RESULT FOUND ===")
            print("RESULT:",num1,"^",num2,"=",rsl)
        print("Result has been copied to clipboard.")
        pyperclip.copy(rsl)
        print()
        again("0","0")
    def c7():
        print()
        print("Please input your number first.")
        inum = input(">")
        mynum = int(inum)
        print("Processing number...")
        ans = math.sqrt(mynum)
        print("=== RESULT FOUND ===")
        print("square root of",mynum,"is:",ans)
        print("Result has been copied to clipboard.")
        pyperclip.copy(ans)
        print()
        again("0","0")
    def c8():
        print()
        print("Input your conversion type.")
        print("(1) C > F - CELCIUS TO FAHRENHEIT")
        print("(2) F > C - FAHRENHEIT TO CELCIUS")
        print("(3) C > K - CELCIUS TO KELVIN")
        print("(4) K > C - KELVIN TO CELCIUS")
        print("(5) C > R - CELCIUS TO REAMUR")
        print("(6) R > C - REAMUR TO CELCIUS")
        print()
        inpb4 = input(">")
        if inpb4=="1":
            ct = 1
        elif inpb4=="2":
            ct = 2
        elif inpb4=="3":
            ct = 3
        elif inpb4=="4":
            ct = 4
        elif inpb4=="5":
            ct = 5
        elif inpb4=="6":
            ct = 6
        print("Input your first number in the first type.")
        fn = input(">")
        print("Processing number...")
        num1 = int(fn)
        if ct==1:
            cr1 = num1*9/5
            cr2 = cr1+32
            print("=== RESULT FOUND ===")
            print(num1,"* 9/5 =",cr1)
            print(cr1,"+ 32 =",cr2)
            print(num1,"=",cr2,"DEGREES FAHRENHEIT")
            rsl = cr2
        elif ct==2:
            cr1 = num1-32
            cr2 = cr1*5/9
            print("=== RESULT FOUND ===")
            print(num1,"- 32 =",cr1)
            print(cr1,"* 5/9 -",cr2)
            print(num1,"=",cr2,"DEGREES CELCIUS")
            rsl = cr2
        elif ct==3:
            rsl = num1+273
            print("=== RESULT FOUND ===")
            print("num1 =",rsl,"KELVIN")
        elif ct==4:
            rsl = num1-273
            print("=== RESULT FOUND ===")
            print("num1 =",rsl,"KELVIN")
        elif ct==5:
            cr1 = num1/100
            cr2 = cr1*80
            print("=== RESULT FOUND ===")
            print(num1,"/ 100 =",cr1)
            print(cr1,"* 80 =",cr2)
            print(num1,"=",cr2,"DEGREES REAMUR")
            rsl = cr2
        elif ct==6:
            cr1 = num1/80
            cr2 = cr1*100
            print("=== RESULT FOUND ===")
            print(num1,"/ 80 =",cr1)
            print(cr1,"* 100 =",cr2)
            print(num1,"=",cr2,"DEGREES CELCIUS")
            rsl = cr2
        print("Result has been copied to clipboard.")
        pyperclip.copy(rsl)
        print()
        again("0","0")
    def c9():
        print()
        print("Please input your type of conversion.")
        print("(1) MINUTE TO SECOND")
        print("(2) HOUR TO MINUTE")
        print("(3) DAY TO HOUR")
        print("(4) WEEK TO DAY")
        print("(5) MONTH TO WEEK")
        print("(6) YEAR TO MONTH")
        print("(7) DECADE TO YEAR")
        print("(8) CENTURY TO DECADE")
        print("(9) MILLENIA TO CENTURY")
        print()
        incty = input(">")
        print("Input your number here.")
        inpn = input(">")
        num1 = int(inpn)
        print("Processing number and result...")
        if incty=="1":
            num2 = num1*60
            print("=== RESULT FOUND ===")
            print(num1,"minutes =",num2,"second(s)")
            rsl = num2
            sec = rsl
        elif incty=="2":
            num2 = num1*60
            print("=== RESULT FOUND ===")
            print(num1,"hours =",num2,"minute(s)")
            rsl = num2
            sec = num1*3600
        elif incty=="3":
            rsl = num1*24
            print("=== RESULT FOUND ===")
            print(num1,"days =",rsl,"hour(s)")
            sec = num1*86400
        elif incty=="4":
            rsl = num1*7
            print("=== RESULT FOUND ===")
            print(num1,"weeks =",rsl,"day(s)")
        elif incty=="5":
            decnum1 = float(num1)
            rsl = decnum1*4.345
            print("=== RESULT FOUND ===")
            print(num1,"months =",rsl,"week(s)")
            sec = num1*2592000
        elif incty=="6":
            rsl = num1*12
            print("=== RESULT FOUND ===")
            print(num1,"years =",rsl,"month(s)")
            sec = num1*31536000
        elif incty=="7":
            print("=== RESULT FOUND ===")
            rsl = num1*10
            print(num1,"decades =",rsl,"year(s)")
            sec = num1*315360000
        elif incty=="8":
            print("=== RESULT FOUND ===")
            rsl = num1*10
            print(num1,"centuries =",rsl,"decade(s)")
            sec = num1*3153600000
        elif incty=="9":
            print("=== RESULT FOUND ===")
            rsl = num1*10
            print(num1,"millenium =",rsl,"century(s)")
            sec = num1*31536000000
        print("Result has been copied to clipboard.")
        pyperclip.copy(rsl)
        print()
        again("1",sec)
    def c10():
        print()
        print("Please input your conversion type.")
        print("(1) KM > MI - Kilometers to Miles")
        print("(2) MI > KM - Miles to Kilometers")
        print("(3) M > FT - Meters to Feet")
        print("(4) FT > M - Feet to Meters")
        print("(5) CM > INCH - Centimeters to Inches")
        print("(6) INCH > CM - Inches to Centimeters")
        inp = input(">")
        print("Please input your number to converted.")
        nu = input(">")
        num1 = int(nu)
        print("Processing result...")
        print("=== RESULT FOUND ===")
        if inp=="1":
            rsl = num1/1.609
            print(num1,"km =",rsl,"miles")
        elif inp=="2":
            rsl = num1*1.609
            print(num1,"miles =",rsl,"km")
        elif inp=="3":
            rsl = num1*3.281
            print(num1,"meters =",rsl,"feet")
        elif inp=="4":
            rsl = num1/3.281
            print(num1,"feet =",rsl,"meters")
        elif inp=="5":
            rsl = num1/2.54
            print(num1,"centimeters =",rsl,"inches")
        elif inp=="6":
            rsl = num1*2.54
            print(num1,"inches =",rsl,"centimeters")
        print("Result has been copied to clipboard.")
        pyperclip.copy(rsl)
        print()
        again("0","0")
    def calc():
        print()
        print("Type these numbers, to use the list of calculating types:")
        print("Results from equations will be copied to clipboard.")
        print()
        print("(1) Manual counter (+ -)")
        print("(2) Auto-Multiply")
        print("(3) Auto-Divide")
        print("(4) Auto-powered")
        print("(5) Sum of factors in range counter")
        print("(6) Manual counter (* / ^)")
        print("(7) (MANUAL) Finding a square root of a number")
        print("(8) (UNIT CONVERTER) TEMPERATURE")
        print("(9) (UNIT CONVERTER) TIME")
        print("(10) (UNIT CONVERTER) LENGTHS")
        print()
        calcom = input(">")
        if calcom=="1":
            c1()
        elif calcom=="2":
            c2()
        elif calcom=="3":
            c3()
        elif calcom=="4":
            c4()
        elif calcom=="5":
            c5()
        elif calcom=="6":
            c6()
        elif calcom=="7":
            c7()
        elif calcom=="8":
            c8()
        elif calcom=="9":
            c9()
        elif calcom=="10":
            c10()
        else:
            fail()
    def main():
        print("=== PROMPT CALCULATOR ===")
        print("(C) 2020 null8626 Programming (",ver,")")
        print("Type 'calc' to begin. For other commands, type 'more'.")
        cho = input(">")
        if cho =="calc":
            calc()
        elif cho =="pi":
            print("3.14159265358979323846264338327950288419716939937510")
            print("  58209749445923078164062862089986280348253421170679")
            print("  82148086513282306647093844609550582231725359408128")
            print("  48111745028410270193852110555964462294895493038196")
            print("  44288109756659334461284756482337867831652712019091")
            print("  45648566923460348610454326648213393607260249141273")
            print("  72458700660631558817488152092096282925409171536436")
            print("  78925903600113305305488204665213841469519415116094")
            print("  33057270365759591953092186117381932611793105118548")
            print("  07446237996274956735188575272489122793818301194912")
            print("  98336733624406566430860213949463952247371907021798")
            print("  60943702770539217176293176752384674818467669405132")
            print("  00056812714526356082778577134275778960917363717872")
            print("  14684409012249534301465495853710507922796892589235")
            print("  42019956112129021960864034418159813629774771309960")
            print("  51870721134999999837297804995105973173281609631859")
            print("  50244594553469083026425223082533446850352619311881")
            print("  71010003137838752886587533208381420617177669147303")
            print("  59825349042875546873115956286388235378759375195778")
            print("  18577805321712268066130019278766111959092164201989")
            print()
            input("Press enter to continue...")
            all("0")
        elif cho =="more":
            print("LIST OF AVAILABLE VALID COMMANDS:")
            print()
            print("calc         STARTS THE CALCULATOR PROGRAM.")
            print("rng          STARTS A RANDOM NUMBER GENERATOR.")
            print("pi           LISTS SOME PI DIGITS AND NUMBERS.")
            print("light        CHANGES THE COLOUR OF PROMPT TO LIGHT MODE.")
            print("dark         CHANGES THE COLOUR OF PROMPT TO DARK MODE")
            print("quit         QUITS THE PROGRAM BY TERMINATING ITSELF")
            print("close        QUITS THE PROGRAM BY TERMINATING ITSELF")
            print("changelog    LISTS THE CHANGELOG OF THE PROGRAM'S RECENT UPDATE")
            print()
            input("press enter key to continue...")
            all("0")
        elif cho =="quit":
            quit()
        elif cho =="close":
            quit()
        elif cho =="rng":
            rng()
        elif cho =="dark":
            os.system('color 0f')
            all("0")
        elif cho =="light":
            os.system('color f0')
            all("0")
        elif cho =="changelog":
            print("CURRENT VERSION:",ver)
            print("PROGRAMMING LANGUAGE(S) USED:",prl)
            print("UPDATED IN:",dat)
            print()
            print("=== CHANGELOG FOR VERSION",ver," ===")
            # === CHANGELOG HERE ===
            print("1. This script is now available in my sources site.")
            # === END CHANGELOG ===
            input("press enter key to continue...")
            all("0")
        else:
            fail()
    if x=="0":
        main()
    elif x=="1":
        calc()
# === THE CODE RUNNER ===
all("0")
# === RUN CODE, RUN! ===