import datetime as clock
import sys
newver = sys.argv[1]

def overwrite(fileName, newData):
    clear = open(fileName, "w")
    clear.close()
    rewrite = open(fileName, "w")
    rewrite.write(newData)
    rewrite.close()
    print("SUCCESS: REWROTE "+str(fileName)+" WITH "+str(len(newData.split('\n')))+' LINES.')

fileNames = ["../setup.py", "../gd/__init__.py", "../README.md"]
old_data = []
for h in range(0, len(fileNames)):
    old_data.append(open(fileNames[h], "r").read())
old_version = old_data[0].split("version = '")[1].split("'")[0]
old_time = old_data[2].split('**LAST UPDATE TIME: "')[1].split('"')[0]

for i in range(0, len(old_data)):
    print("OVERWRITING FILE(S)...")
    overwrite(fileNames[i], old_data[i].replace(old_version, newver).replace(old_time, str(clock.datetime.now())[:-7]+' (UTC+7)'))