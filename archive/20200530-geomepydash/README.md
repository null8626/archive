# geomepydash
Python wrapper for the GDBrowser API, and other GD tools as well (Original API by GD Colon)

Links to the original website:

\>\>\> [Original GD API Documentation here](https://gdbrowser.com/api)

\>\>\> [Geometry Dash Tools](https://gdcolon.com/tools) 

**VERSION: 0.3.8**

**LAST UPDATE TIME: "2020-07-03 14:40:50 (UTC+7)"**

# setup
To install the package, type the following on your console.

```
# Bash Terminal
$ python3 -m pip install -U geomepydash

# Windows Command Prompt
py -3 -m pip install -U geomepydash
```

# documentation
**[FULL DOCUMENTATION HERE](https://null8626.github.io/geomepydash)**

Open your code, and type:
```py
import gd
```
to import all of the modules from the package.

**Examples**

_All data (except GD Tools) are returned in a **class.**_
```py
import gd
level = gd.daily()
print(level.name)
# returns the current daily level name

mytextbox = gd.textbox(
  text = 'I love geometry dash!',
  name = 'My cute monster',
  color = 'blue',
  char = gd.Characters.Scratch(expression='talk')
)
print(mytextbox)
# returns the link to the image.
```
