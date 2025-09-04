rem @echo off
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
rem xcopy /e /i /h /y  lib-9341\* KNJGfx9341_ARD\ >> outlog.txt 2>&1
robocopy lib-9341 KNJGfx9341_ARD /E /COPYALL /XD ForMPY >> outlog.txt 2>&1

cd KNJGfx9341_ARD
for /r %%d in (*.cpp) do move "%%d" . >> ..\outlog.txt 2>&1
for /r %%d in (*.c) do move "%%d" . >> ..\outlog.txt 2>&1

echo "---------- Editing ASCII font files --------" >> outlog.txt 2>&1
echo $replacements = @{ > pswork.ps1
echo     '#include "defines.h"'          = '#include "misc/defines.h"' >> pswork.ps1
echo     '#include "Adafruit_ILI9341.h"' = '#include "Adafruit_ILI9341/Adafruit_ILI9341.h"' >> pswork.ps1
echo } >> pswork.ps1
echo $Regexreplacements = @{ >> pswork.ps1
echo     '^\s*#pragma region' = '// #pragma region ' >> pswork.ps1
echo     '^\s*#pragma endregion' = '// #pragma endregion' >> pswork.ps1
echo } >> pswork.ps1

echo. >> pswork.ps1
echo Get-ChildItem *.cpp ^| ForEach-Object { >> pswork.ps1
echo     $content = Get-Content $_.FullName -Encoding UTF8 >> pswork.ps1
echo     foreach ($pattern in $replacements.Keys) { >> pswork.ps1
echo         $content = $content -replace [regex]::Escape($pattern), $replacements[$pattern] >> pswork.ps1
echo     } >> pswork.ps1

echo     foreach ($pattern in $Regexreplacements.Keys) { >> pswork.ps1
echo         $content = $content -replace $pattern, $Regexreplacements[$pattern] >> pswork.ps1
echo     } >> pswork.ps1
echo     $content ^| Set-Content $_.FullName -Encoding UTF8 >> pswork.ps1
echo } >> pswork.ps1


pause
powershell -ExecutionPolicy Bypass -File pswork.ps1 >> ..\outlog.txt 2>&1
del pswork.ps1


pushd  Adafruit_GFX_Library\Fonts
echo Get-ChildItem *.h ^| ForEach-Object { (Get-Content $_.FullName) -replace '#include ^<Adafruit_GFX.h^>', '#include "../Adafruit_GFX.h"' ^| Set-Content $_.FullName } > pswork1.ps1
powershell -ExecutionPolicy Bypass -File pswork1.ps1 >> ..\outlog.txt 2>&1
del pswork1.ps1
popd
cd Examples

echo  "---------- processing Example ino ----------" >> outlog.txt 2>&1
setlocal enabledelayedexpansion
for /R %%F in (*.txt) do (
    set "newname=%%~dpF%%~nF.ino"
    move "%%F" "!newname!" >> ..\outlog.txt 2>&1
)
for /R %%F in (*.htxt) do (
    set "newname=%%~dpF%%~nF.h"
    move "%%F" "!newname!" >> ..\outlog.txt 2>&1
)
endlocal
cd ../..



echo  "---------- making zip archive and copy it to USERPROFILE% ----------" >> outlog.txt 2>&1
7z a KNJGfx9341_ARD_ONLY.zip KNJGfx9341_ARD -xr!*.bak  >> outlog.txt 2>&1
rmdir /q /s KNJGfx9341_ARD  >> outlog.txt 2>&1
copy  KNJGfx9341_ARD_ONLY.zip %USERPROFILE%  >> outlog.txt 2>&1
rem del KNJGfx9341_ARD.zip  >> outlog.txt 2>&1
echo successfully created KNJGfx9341_ARD_ONLY.zip
echo   *If an error occurs, the execution log was saved in outlog.txt.