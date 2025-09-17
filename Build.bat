
REM Save current directory
set "OLD_DIR=%CD%"

REM Change to home path
cd /d %HOMEPATH%
if ERRORLEVEL 1 (
    echo Failed to change directory to %HOMEPATH%
    pause
    exit /b 1
)

REM Activate Zephyr virtual environment
call zephyrproject\.venv\Scripts\activate.bat
if ERRORLEVEL 1 (
    echo Failed to activate Zephyr virtual environment
    pause
    exit /b 1
)
:START
REM Go to the Zephyr source directory
cd /d "%HOMEPATH%\zephyrproject\zephyr"
if ERRORLEVEL 1 (
    echo Failed to change directory to %HOMEPATH%\zephyrproject\zephyr
    pause
    exit /b 1
)
rm -r "$env:HOMEDRIVE$env:HOMEPATH\zephyrproject\zephyr\build"
REM Run west build pointing to the original project directory
west build -p always -b nucleo_f091rc "%OLD_DIR%"
if ERRORLEVEL 1 (
    echo west build failed
    pause
    exit /b 1
)

echo Build completed successfully!

:ASK
set /p USER_CHOICE=Do you want to flash (y), rebuild (a), or exit (any other key)? [y/a/other]: 

if /i "%USER_CHOICE%"=="y" (
    west flash
    if ERRORLEVEL 1 (
        echo Flash failed!
        pause
    )
    pause
    exit /b 0
) else if /i "%USER_CHOICE%"=="a" (
    goto START
) else (
    echo Exiting...
    exit /b 0
)