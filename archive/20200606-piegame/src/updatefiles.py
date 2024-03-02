try:
    from src.config import *
except ImportError:
    from config import *
import base64
import random

def opposite(hmm):
    hmm = str(hmm)
    if hmm.lower()=='true': return False
    elif hmm.lower()=='false': return True
    elif hmm.lower()=='dark': return 'light'
    elif hmm.lower()=='light': return 'dark'
def enc(code, times):
    msg = ''
    for i in range(0, int(times)):
        if i==0: toenc = code
        else: toenc = msg
        message_bytes = toenc.encode('ascii')
        base64_bytes = base64.b64encode(message_bytes)
        msg = base64_bytes.decode('ascii')
    return msg
    # yes. copy-pasted i know. #EPICDEVELOPER2020

def decode(code, times): # COPY PASTAS TIME
    message = ''
    for i in range(0, int(times)):
        if i==0: toenc = code
        else: toenc = message
        base64_bytes = toenc.encode('ascii')
        message_bytes = base64.b64decode(base64_bytes)
        message = message_bytes.decode('ascii')
    return message
def updateSetting(newSetting, forwhat):
    setting = open('./src/settings.pie', 'r', encoding='utf8').read()
    if setting=='null' and forwhat==None:
        times = random.randint(2, 9)
        top = ''
        for i in range(0, random.randint(10, 13)):
            if i==0:
                top += str(times)
            else:
                top += str(random.randint(0, 9))
        editor = open('./src/settings.pie', 'w')
        editor.write(str(top)+'\n'+str(enc(str(Default_setting), times)))
        editor.close()
    else:
        if forwhat=='get':
            data = decode(str(setting.split('\n')[1]), list(setting.split('\n')[0])[0])
            return data
        if forwhat=='post':
            times = random.randint(2, 9)
            top = ''
            for i in range(0, random.randint(10, 13)):
                if i==0:
                    top += str(times)
                else:
                    top += str(random.randint(0, 9))
            editor = open('./src/settings.pie', 'w')
            editor.write(str(top)+'\n'+str(enc(str(newSetting), times)))
            editor.close()

# COPY? PASTE!

def updateSave(newSave, forwhat):
    save_file = open('./src/save.pie', 'r', encoding='utf8').read()
    if save_file=='null' and forwhat==None:
        times = random.randint(3, 10)
        top = ''
        for i in range(0, random.randint(10, 13)):
            if i==0:
                top += str(times)
            else:
                top += str(random.randint(0, 9))
        editor = open('./src/save.pie', 'w')
        editor.write(str(top)+'\n'+str(enc(str(Default_save), times)))
        editor.close()
    else:
        if forwhat=='get':
            data = decode(str(save_file.split('\n')[1]), list(save_file.split('\n')[0])[0])
            return data
        if forwhat=='post':
            times = random.randint(3, 10)
            top = ''
            for i in range(0, random.randint(10, 13)):
                if i==0:
                    top += str(times)
                else:
                    top += str(random.randint(0, 9))
            editor = open('./src/save.pie', 'w')
            editor.write(str(top)+'\n'+str(enc(str(newSave), times)))
            editor.close()