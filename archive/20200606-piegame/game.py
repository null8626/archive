import os
os.system('echo Starting game... Please wait...')
from sys import version
import datetime
from random import randint, choice
from src.updatefiles import *
from src.config import * # CONFIG FILE

# IMPORT PYGAME.
try:
    import pygame as g
    if windows: os.system('cls') # CLEAR THE GROSS PYGAME WATERMARK
    if linux: os.system('clear')
except ImportError:
    if windows: os.system('pip install pygame')
    if linux: os.system('python3 -m pip install -U pygame')
os.system('echo Building game window...')

# INITIATE
try:
    g.init()
except NameError:
    os.system('echo It seemed that your system have not installed Pygame yet. Try installing the python module and run this game again.')
    exit()

# CHECK IF SYSTEM IS IN PYTHON 3.
if not str(version).startswith('3'):
    os.system('echo Error: You are running an old version of python. Please consider updating.')
    begin_running = False # DISABLE THE GAME FROM STARTING.

# CREATE SETTING AND SAVE FILE IF NOT EXIST
if open('./src/settings.pie', 'r', encoding='utf8').read()=='null': updateSetting(None, None)
if open('./src/save.pie', 'r', encoding='utf8').read()=='null': updateSave(None, None)

# SETTING
settingsobj = updateSetting(None, 'get')
global set1, set2, set3
set1 = settingsobj.split("'sound': ")[1].split(',')[0].replace('"', '').replace("'", "")
set2 = settingsobj.split("'customcursor': ")[1].split(',')[0].replace('"', '').replace("'", "")
set3 = settingsobj.split("'color': '")[1].split("'")[0].replace('"', '').replace("'", "")

# UPDATE ACCORDING TO THE SETTING
global backgroundcolor, foregroundcolor, disablecursor
disablecursor, mute = False, False
backgroundcolor, foregroundcolor = Colors.black, Colors.white
if set3.lower()=='light':
    backgroundcolor, foregroundcolor = Colors.white, Colors.black
    light = True
if set2.lower()=='false': disablecursor = True
if set1.lower()=='false': mute = True

# FONTS
global font_small, font_medium, font_large
font_small = g.font.Font('./assets/joystix monospace.ttf', 20)
font_medium = g.font.Font('./assets/joystix monospace.ttf', 30)
font_large = g.font.Font('./assets/joystix monospace.ttf', 50)

# BASIC PYGAME CONFIGURATION
game = g.display.set_mode((System.width, System.height))

# SOUND EFFECTS
class sfx:
    consumed = g.mixer.Sound('./assets/consumed.wav')
    poisoned = g.mixer.Sound('./assets/poisoned.wav')
    gameover = g.mixer.Sound('./assets/gameover.wav')

# IMAGES
pie = g.image.load('./assets/pie.png').convert_alpha()
poisoned_pie = g.image.load('./assets/pie_poisoned.png').convert_alpha()
s_pie = g.image.load('./assets/s_pie.png').convert_alpha()
s_poisoned_pie = g.image.load('./assets/s_pie_poisoned.png').convert_alpha()
cursor = g.image.load('./assets/cursor.png').convert_alpha()

# CHANGE CURSOR TO BLACK IF IN LIGHT MODE
try:
    if light:
        cursor = cursor = g.image.load('./assets/cursor_dark.png').convert_alpha()
except NameError:
    print('Dark mode? Perfect for your eyes then. Nice!')

# WINDOW DECORATION, AND SET CURSOR TO INVISIBLE (IF CUSTOM CURSOR = TRUE)
if not disablecursor: g.mouse.set_cursor((8,8),(0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0))
g.display.set_caption(System.title)
g.display.set_icon(s_pie)

# BEGIN RUNNING GAME
if begin_running:
    # CRASH THE GAME
    def crash():
        g.quit()
        exit()

    # CHECK IF USER CLICK THE BUTTON
    def check_act(event, getfrom):
        for ev in event:
            if getfrom=='regular':
                if ev.type==g.QUIT: crash()
            if getfrom=='button':
                if ev.type==5 or ev.type==6:
                    return 'CLICK'
    
    # INCREASES THE PROJECTILE FALL SPEED (INCREASE DIFFICULTY)
    def increase_speed(times):
        if times>100:
            return round(times/1000)
        else:
            return round(times/500)
    
    # MAIN MENU
    class MainMenu:
        title = font_large.render('piegame', True, foregroundcolor)
        play = font_small.render('play', True, foregroundcolor)
        settings = font_small.render('settings', True, foregroundcolor)
        about = font_small.render('about', True, foregroundcolor)
        _exit = font_small.render('exit', True, foregroundcolor)

    # ABOUT
    class About:
        title = font_medium.render('about me', True, foregroundcolor)
        stat1 = font_small.render('made by: null8626', True, foregroundcolor)
        stat2 = font_small.render('made using pygame', True, foregroundcolor)
        stat3 = font_small.render('i know pygame is outdated', True, foregroundcolor)
        stat4 = font_small.render('but i was just wanna learn coding', True, foregroundcolor)
        back = font_small.render('back', True, foregroundcolor)

    # SETTINGS
    class Settings:
        title = font_medium.render('settings', True, foregroundcolor)
        setting1 = font_small.render('sound: '+set1, True, foregroundcolor)
        setting2 = font_small.render('cursor: '+set2, True, foregroundcolor)
        setting3 = font_small.render('color: '+set3, True, foregroundcolor)
        save = font_small.render('save changes', True, foregroundcolor)
    
    # PROJECTILES
    class Projectiles:
        x = randint(0, 640)
        y = -50
        bad = choice([True, True, True, False]) # 1 IN 4 CHANCE OF BEING SAFE
        times = 0                                 # HOW MANY PROJECTILES FALLEN?
        speed = 1                                 # SPEED OF THE PROJECTILE FALLING (INCREASES BY TIME)
    
    # GAME OVER SCREEN
    class GameOver:
        title = font_large.render('game over', True, foregroundcolor)
        mainmenu = font_small.render('main menu', True, foregroundcolor)
        _exit = font_small.render('exit', True, foregroundcolor)
    
    # MAIN WHILE LOOP
    while running:
        # BASIC STUFF
        check_act(g.event.get(), 'regular')
        mousepos = g.mouse.get_pos()
        cursorpos = cursor.get_rect()
        cursorpos.x, cursorpos.y = mousepos
        
        # === PAGES ===
        if Pages.main_menu:
            game.fill(backgroundcolor)
            if not disablecursor: game.blit(cursor, cursorpos)
            # POSITIONING
            playpos = MainMenu.play.get_rect()
            playpos.x, playpos.y = 180, 200
            settingspos = MainMenu.settings.get_rect()
            settingspos.x, settingspos.y = 180, 230
            aboutpos = MainMenu.about.get_rect()
            aboutpos.x, aboutpos.y = 180, 260
            exitpos = MainMenu._exit.get_rect()
            exitpos.x, exitpos.y = 180, 290
            
            # HOVERING
            if playpos.collidepoint(mousepos):
                MainMenu.play = font_small.render(' play', True, Colors.red)
                if check_act(g.event.get(), 'button')=='CLICK':
                    global start_playing
                    last_death_time, Player.consumedpie, Player.consumedppie = None, 0, 0
                    start_playing = datetime.datetime.now()
                    Pages.gameplay = True
                    Pages.main_menu = False
            else: MainMenu.play = font_small.render('play', True, foregroundcolor)
            if settingspos.collidepoint(mousepos): 
                MainMenu.settings = font_small.render(' settings', True, Colors.red)
                if check_act(g.event.get(), 'button')=='CLICK':
                    Pages.settings = True
                    Pages.main_menu = False
            else: MainMenu.settings = font_small.render('settings', True, foregroundcolor)
            if aboutpos.collidepoint(mousepos): 
                MainMenu.about = font_small.render(' about', True, Colors.red)
                if check_act(g.event.get(), 'button')=='CLICK':
                    Pages.about = True
                    Pages.main_menu = False
            else: MainMenu.about = font_small.render('about', True, foregroundcolor)
            if exitpos.collidepoint(mousepos):
                MainMenu._exit = font_small.render(' exit', True, Colors.red)
                if check_act(g.event.get(), 'button')=='CLICK': crash()
            else: MainMenu._exit = font_small.render('exit', True, foregroundcolor)

            # RENDER TITLE
            game.blit(MainMenu.title, (180, 100))
            game.blit(MainMenu.play, playpos)
            game.blit(MainMenu.settings, settingspos)
            game.blit(MainMenu.about, aboutpos)
            game.blit(MainMenu._exit, exitpos)
        if Pages.about:
            game.fill(backgroundcolor)
            if not disablecursor: game.blit(cursor, cursorpos)
            # POSITIONING
            backpos = About.back.get_rect()
            backpos.x, backpos.y = 50, 300

            #HOVERING
            if backpos.collidepoint(mousepos):
                
                About.back = font_small.render(' back', True, Colors.red)
                if check_act(g.event.get(), 'button')=='CLICK':
                    Pages.main_menu = True
                    Pages.about = False
            else: About.back = font_small.render('back', True, foregroundcolor)
            #RENDERING
            game.blit(About.title, (50, 50))
            game.blit(About.stat1, (50, 100))
            game.blit(About.stat2, (50, 120))
            game.blit(About.stat3, (50, 180))
            game.blit(About.stat4, (50, 200))
            game.blit(About.back, backpos)
        if Pages.settings:
            game.fill(backgroundcolor)
            if not disablecursor: game.blit(cursor, cursorpos)

            # POSITIONING
            st1 = Settings.setting1.get_rect()
            st1.x, st1.y = 100, 150
            st2 = Settings.setting2.get_rect()
            st2.x, st2.y = 100, 170
            st3 = Settings.setting3.get_rect()
            st3.x, st3.y = 100, 190
            sv = Settings.save.get_rect()
            sv.x, sv.y = 100, 250

            # CHECK CLICK
            if st1.collidepoint(mousepos):
                Settings.setting1 = font_small.render('sound: '+str(set1), True, Colors.yellow)
                if check_act(g.event.get(), 'button')=='CLICK':
                    set1 = opposite(set1)
            else: Settings.setting1 = font_small.render('sound: '+str(set1), True, foregroundcolor)
            if st2.collidepoint(mousepos):
                Settings.setting2 = font_small.render('cursor: '+str(set2), True, Colors.yellow)
                if check_act(g.event.get(), 'button')=='CLICK':
                    set2 = opposite(set2)
            else: Settings.setting2 = font_small.render('cursor: '+str(set2), True, foregroundcolor)
            if st3.collidepoint(mousepos):
                Settings.setting3 = font_small.render('color: '+str(set3), True, Colors.yellow)
                if check_act(g.event.get(), 'button')=='CLICK':
                    set3 = opposite(set3)
            else: Settings.setting3 = font_small.render('color: '+str(set3), True, foregroundcolor)
            if sv.collidepoint(mousepos):
                
                Settings.save = font_small.render(' save changes', True, Colors.yellow)
                if check_act(g.event.get(), 'button')=='CLICK':
                    newChange = {
                        "sound": set1,
                        "customcursor": set2,
                        "color": set3
                    }
                    updateSetting(str(newChange), 'post')
                    # RESTART
                    print('Restarting...')
                    if windows: os.system('taskkill /f /im python.exe && py game.py')
                    if linux: os.system('python3 game.py')
                    g.quit()
                    exit()
            else: Settings.save = font_small.render('save changes', True, foregroundcolor)
            # RENDER TEXT
            game.blit(Settings.title, (100, 100))
            game.blit(Settings.setting1, st1)
            game.blit(Settings.setting2, st2)
            game.blit(Settings.setting3, st3)
            game.blit(Settings.save, sv)
        if Pages.gameplay:
            game.fill(backgroundcolor)
            if Player.health<0:
                if not mute: sfx.gameover.play()
                # RESET EVERYTHING AND GO TO GAME OVER SCREEN
                last_death_time = str(datetime.datetime.now()-start_playing)[:-7]
                
                # POST STATUS TO SAVE FILE
                save_data = {
                    "time":str(last_death_time),
                    "fallen":str(Projectiles.times),
                    "speed":str(Projectiles.speed)
                }
                updateSave(save_data, 'post')
                Player.health, Player.healthcol, Projectiles.times, Projectiles.speed = 500, Colors.green, 0, 1
                Pages.gameover = True
                Pages.gameplay = False
            image = None
            
            # CHANGE HEALTH BAR COLOR IF LESS HEALTH
            if Player.health<250 and Player.health>100: Player.healthcol = Colors.yellow
            elif Player.health<101: Player.healthcol = Colors.red
            else: Player.healthcol = Colors.green

            # RELOAD IMAGE IF PROJECTILE IS BAD OR NOT
            if Projectiles.bad: image = poisoned_pie
            else: image = pie
            
            # FALL
            Projectiles.y += Projectiles.speed
            
            # GENERATE ANOTHER PROJECTILE ONCE Y IS GREATER THAN THE WINDOW Y AXIS
            if Projectiles.y>490:
                Projectiles.times += 1
                Projectiles.speed += increase_speed(Projectiles.times)
                fall_type = randint(0, 3)
                if fall_type<3: Projectiles.color, Projectiles.bad = Colors.red, True
                else: Projectiles.color, Projectiles.bad = Colors.green, False
                Projectiles.x, Projectiles.y = randint(0, 640), -50 
                
            # DRAW PROJECTILE
            projectile = image.get_rect()
            projectile.x, projectile.y = Projectiles.x, Projectiles.y
            
            # CHECK IF CURSOR HIT PROJECTILE
            if cursorpos.colliderect(projectile):
                if Projectiles.bad:
                    if not mute: sfx.poisoned.play()
                    Player.health -= 1
                else: 
                    if not mute: sfx.consumed.play()
                    Player.health += 1
            
            # DRAW THE ENTIRE THING (HEALTH BAR, ETC.)
            health = font_small.render('Health: '+str(Player.health), True, Player.healthcol)
            time_disp = font_small.render(str(datetime.datetime.now()-start_playing)[:-7], True, foregroundcolor)
            healthrec = health.get_rect()
            healthrec.x, healthrec.y = 5, 5
            healthbar = g.draw.rect(game, Player.healthcol, (5, 35, (Player.health), 10))
            game.blit(image, projectile)
            game.blit(health, healthrec)
            game.blit(time_disp, (5, 445))
            if not disablecursor: game.blit(cursor, cursorpos)
        if Pages.gameover:
            game.fill(backgroundcolor)
            if not disablecursor: game.blit(cursor, cursorpos)
            
            # POSITIONING
            mainmenupos = GameOver.mainmenu.get_rect()
            mainmenupos.x, mainmenupos.y = 100, 350
            exitpos = GameOver._exit.get_rect()
            exitpos.x, exitpos.y = 100, 400
            
            # HOVERING
            if mainmenupos.collidepoint(mousepos):
                
                GameOver.mainmenu = font_small.render(' main menu', True, Colors.red)
                if check_act(g.event.get(), 'button')=='CLICK':
                    Pages.main_menu = True
                    Pages.gameover = False
            else: GameOver.mainmenu = font_small.render('main menu', True, foregroundcolor)
            if exitpos.collidepoint(mousepos):
                
                GameOver._exit = font_small.render(' exit', True, Colors.red)
                if check_act(g.event.get(), 'button')=='CLICK': crash()
            else: GameOver._exit = font_small.render('exit', True, foregroundcolor)
            
            # GET INFO FROM SAVE FILE
            savefile = updateSave(None, 'get')
            global sv1, sv2, sv3
            sv1 = savefile.split("'time': '")[1].split("'")[0].replace('"', '').replace("'", "")
            sv2 = savefile.split("'fallen': '")[1].split("'")[0].replace('"', '').replace("'", "")
            sv3 = savefile.split("'speed': '")[1].split("'")[0].replace('"', '').replace("'", "")
            
            # GAME OVER DETAILS RENDERING
            details1 = font_small.render('time: '+str(sv1), True, foregroundcolor)
            details2 = font_small.render('fallen pies: '+str(sv2), True, foregroundcolor)
            details3 = font_small.render('speed: '+str(sv3), True, foregroundcolor)
            
            # RENDERING
            game.blit(GameOver.title, (100, 100))
            game.blit(details1, (100, 200))
            game.blit(details2, (100, 230))
            game.blit(details3, (100, 260))
            game.blit(GameOver.mainmenu, mainmenupos)
            game.blit(GameOver._exit, exitpos)
            
        # RENDERING, SETTING FPS, AND UPDATING SCREEN
        g.display.update()
    g.quit()