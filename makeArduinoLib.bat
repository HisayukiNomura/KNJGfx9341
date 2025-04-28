@echo off
echo -----ddd
echo This script will create a zip file for the Arduino library.
echo The created ZIP file can be used in Arduino IDE via Sketch ^> Include Library ^> Add .ZIP Library.
echo:
echo It will include all the necessary files and folders.
echo The zip file will be named KNJGfx9341_ARD.zip and will be created in %USERPROFILE%.
echo:
echo note: This script creates KNJGfx9341_ARD folder as a temporary folder.
echo It will be deleted before execution and deleted after the zip file is created.
echo:
echo press any key to continue or Ctrl+C to cancel.
pause
where 7z.exe >nul 2>&1
if %errorlevel% neq 0 (
    echo 7z.exe not found in PATH. Please install 7-Zip and add it to your PATH.
    exit /b 1
)
echo  "---------- setting up the environment ----------" > outlog.txt 2>&1
del /F /Q  KNJGfx9341_ARD.zip >> outlog.txt 2>&1
rmdir /q /s KNJGfx9341_ARD >> outlog.txt 2>&1
mkdir KNJGfx9341_ARD >> outlog.txt 2>&1

echo  "---------- coping file to working folder ----------" >> outlog.txt 2>&1
xcopy /e /i /h /y  lib-9341\* KNJGfx9341_ARD\ >> outlog.txt 2>&1

cd KNJGfx9341_ARD
for /r %%d in (*.cpp) do move "%%d" . >> ..\outlog.txt 2>&1
for /r %%d in (*.c) do move "%%d" . >> ..\outlog.txt 2>&1
cd ..

echo  "---------- making zip archive and copy it to USERPROFILE% ----------" >> outlog.txt 2>&1
7z a KNJGfx9341_ARD.zip KNJGfx9341_ARD -xr!*.bak  >> outlog.txt 2>&1
rmdir /q /s KNJGfx9341_ARD  >> outlog.txt 2>&1
copy  KNJGfx9341_ARD.zip %USERPROFILE%  >> outlog.txt 2>&1
del KNJGfx9341_ARD.zip  >> outlog.txt 2>&1
echo successfully created %USERPROFILE%\KNJGfx9341_ARD.zip
echo "  *If an error occurs, the execution log was saved in outlog.txt."