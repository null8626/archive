echo off && cls && title LAUNCH NEW RELEASE && goto changever
:changever
set /p "ver=Input new version >"
overwrite.py %ver%
git commit -a -m "Update release version %ver%" && git push
:: put your github repo link here.
set githublink=https://github.com/null8626/geomepydash
goto check
:check
start %githublink%/releases/new
set /p "confirmation=Done? [y/n] >"
if %confirmation%==n exit
if %confirmation%==y goto publish
goto check
:publish
cd .. && python setup.py sdist && twine upload --skip-existing dist/*
pause && exit

:: I code using Windows. So this is the code i use to execute a new release.
:: The proccess is not completely automatic though.
:: Linux Bash users, you can code your own .sh file for this. (if you want)