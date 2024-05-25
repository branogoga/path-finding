@echo off
setlocal enabledelayedexpansion

REM Function to recursively find .dot files
for /r %%f in (*.dot) do (
    set "dot_file=%%f"
    set "png_file=%%~dpnf.png"
    echo !dot_file!
    echo !png_file!
    fdp -Tpng !dot_file! -o !png_file!
)

endlocal
