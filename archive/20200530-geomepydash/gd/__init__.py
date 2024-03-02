"""
geomepydash
~~~~~~~~~~~~~~~~~~~
GDBrowser API wrapper and GD Tools generator.
Originally made by GD Colon. For Geometry dash (game), all rights to RobTop Games.
(c) 2020 null8626
"""

__title__ = 'gd'
__author__ = 'null8626'
__license__ = 'MIT'
__copyright__ = 'Copyright 2020 null8626'
__version__ = '0.3.8'

def classify(obj):
    total = 'class Level:\n\t'
    for i in range(0, len(list(obj.keys()))):
        total += list(obj.keys())[i]+' = obj[\''+str(list(obj.keys())[i])+'\']\n\t'
    return total

def GDClass(obj):
    class Result:
        pass
    for i in range(0, len(list(obj.keys()))):
        toexe = 'Result.'+list(obj.keys())[i]+' = obj["'+list(obj.keys())[i]+'"]'
        exec(toexe)
    return Result

# EXTERNAL MODULES
from urllib.request import urlopen as fetchapi
from urllib.parse import quote_plus as urlencode
from sys import path
from os import getcwd
import json
import requests

# ATTRIBUTES
path.append(getcwd())
from characters import Characters
from comments import Comments
from leaderboard import Leaderboard

def daily():
    data = json.loads(fetchapi('https://gdbrowser.com/api/level/daily').read())
    total = {}
    for i in range(0, len(list(data.keys()))):
        if list(data.keys())[i]=='data':
            break
        total[list(data.keys())[i]] = data[list(data.keys())[i]]
    total = GDClass(total)
    return total

def weekly():
    data = json.loads(fetchapi('https://gdbrowser.com/api/level/weekly').read())
    total = {}
    for i in range(0, len(list(data.keys()))):
        if list(data.keys())[i]=='data':
            break
        total[list(data.keys())[i]] = data[list(data.keys())[i]]
    total = GDClass(total)
    return total

def fetchProfile(inputs):
    data = json.loads(fetchapi('https://gdbrowser.com/api/profile/'+str(urlencode(inputs).replace('+', '%20'))).read())
    try:
        temp = data.keys()
    except AttributeError:
        raise ConnectionRefusedError('User not found!')
    total = GDClass(data)
    return total

def fetchLevel(levelid):
    data = json.loads(fetchapi('https://gdbrowser.com/api/level/'+str(levelid)).read())
    total = {}
    try:
        for i in range(0, len(list(data.keys()))):
            if list(data.keys())[i]=='data':
                break
            total[list(data.keys())[i]] = data[list(data.keys())[i]]
        total = GDClass(total)
        return total
    except AttributeError:
        raise ConnectionRefusedError('Level not found!')

def levelSearch(query):
    encoded_query = urlencode(query).replace(" ", "%20")
    data = json.loads(fetchapi('https://gdbrowser.com/api/search/'+str(encoded_query)).read())
    try:
        total = []
        for a in range(0, len(data)):
            total.append(GDClass(data[a]))
    except:
        raise ConnectionRefusedError('Level not found')
    return total

def icon(name, *args, **kwargs):
    name = urlencode(name).replace(' ', '%20')
    forms = ["cube", 'ship', "ball", "ufo", "wave", "robot", "spider", "cursed"]
    form = '&form='+str(kwargs.get('form', None))
    size = '&size='+str(kwargs.get('size', None))
    if str(kwargs.get('size', None))=='None':
        size = ''
    else:
        size = '&size='+str(kwargs.get('size', None))
        if str(size).isnumeric()==False: raise ValueError('Size must be a number')
    if str(kwargs.get('form', None))!='None':
        if str(kwargs.get('form', None)) not in forms:
            raise ValueError("Invalid Form input")
    else: form = ''
    return 'https://gdbrowser.com/icon/'+str(name)+str(form)+str(size)

def logo(text):
    return 'https://gdcolon.com/tools/gdlogo/img/'+str(urlencode(text)).replace('+', '%20')

def text(text ,font, *args, **kwargs):
    hexcol = '&color='+str(kwargs.get('color', None))
    if str(kwargs.get('color', None))=='None': hexcol = ''
    return 'https://gdcolon.com/tools/gdfont/img/'+str(urlencode(text)).replace('+', '%20')+'?font='+str(font)+str(hexcol)
def comment(text, author, *args, **kwargs):
    text = str(urlencode(text)).replace('+', '%20')
    author = str(urlencode(author)).replace('+', '%20')
    likes = '&likes='+str(kwargs.get('likes', None))
    color = '&color='+str(kwargs.get('color', None))
    if kwargs.get('likes', None)==None:
        likes = '&likes=0'
    if kwargs.get('color', None)==None:
        color = ''
    mod = '&mod='+str(kwargs.get('mod', None))
    if kwargs.get('mod', None)==None:
        mod = ''
    uhd = ''
    if kwargs.get('uhd', None)==True:
        uhd = '&uhd'
    deletable = ''
    if kwargs.get('deletable', None)==True:
        deletable = '&deletable'
    return 'https://gdcolon.com/tools/gdcomment/img/'+str(text)+'?name='+str(author)+str(likes)+str(color)+str(mod)+str(uhd)+str(deletable)
def textbox(text, name, color, char, *args, **kwargs):
    text = str(urlencode(text)).replace('+', '%20')
    name = str(urlencode(name)).replace('+', '%20')
    color = '&color='+str(kwargs.get('color', None))
    mobile = ''
    if kwargs.get('mobile', None)==True:
        mobile = '&oldfont'
    if kwargs.get('color', None)==None:
        color = ''
    return 'https://gdcolon.com/tools/gdtextbox/img/'+str(text)+'?name='+str(name)+str(color)+str(char)