@echo off

:::
:::          .--------------------------.
:::          |o                        o|
:::          | .----------------------. |
:::          | |                      | |
:::          | |                      | |
:::        __| |   __ __ /_ ,_/_/_ __ | |__
:::       (o_  |  /_//_//\ / / /  /_/ |  _o)
:::          | | /                    | |
:::          | |                      | |
:::          | |                      | |
:::          | '----------------------' |
:::          |      _              .-.  |
:::          |    _| |_       .-. ( A ) |
:::          |   [_ o _]     ( B ) '-'  |
:::          |     |_|        '-'       |
:::          |          ___             |
:::          |         (___)            |
:::          |o                        o|
:::          | .----------------------. |
:::          |o|                      |o|
:::          '-'                      '-'

for /f "delims=: tokens=*" %%A in ('findstr /b ::: "%~f0"') do @echo(%%A

IF [%1] EQU [] Goto:NoDrop



set VAR=Destination
for /f "skip=1" %%L in ('wmic logicaldisk where volumename^="CRP DISABLD" Get Caption') do @call :SetVar %%L

echo Copying data from "%~1" to %Destination%

del %Destination%\firmware.bin
copy "%~1" %Destination%\firmware.bin

goto :Finished

:SetVar
set Label=%1
if NOT [%Label%]==[] set %VAR%=%Label%
goto :EOF

:Finished
pause
cls
exit

:NoDrop
echo Drag and Drop your .bin file onto _copy.bat to send it to Pokitto.
pause
cls
exit