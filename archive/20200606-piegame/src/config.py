import random
import os

begin_running = True
running = True
windows, linux = False, False
if os.name=='nt': windows = True
if os.name=='posix': linux = True

# COLORS
class Colors:
    red = (255, 0, 0)
    black = (0, 0, 0)
    white = (255, 255, 255)
    yellow = (255, 255, 0)
    green = (0, 255, 0)

# PLAYER CONFIG
class Player:
    x = 300
    y = 300
    width = 50
    height = 50
    health = 500
    healthcol = Colors.green
    consumedpie = 0
    consumedppie = 0

# SYSTEM CONFIG
class System:
    width = 640
    height = 480
    title = 'piegame'
    color = (100, 100, 100)

# PAGES CONFIG
class Pages:
    main_menu = True
    settings = False
    gameplay = False
    about = False
    gameover = False

Default_setting = {
    "sound":True,
    "customcursor":True,
    "color":"dark"
}

Default_save = {
    "time":"None",
    "fallen":"None",
    "speed":"None"
}