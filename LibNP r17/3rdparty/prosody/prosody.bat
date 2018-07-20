@echo off

rem Set the proper path
setlocal
%~d0
cd %~dp0

set datastore=%~dp0
if exist Uninstall.exe set datastore=%APPDATA%\Prosody

set PROSODY_CFGDIR=%datastore%
set PROSODY_DATADIR=%datastore%\data
set PROSODY_SRCDIR=%~dp0\src\
set PROSODY_PLUGINDIR=%~dp0\plugins\

if not exist src\prosody goto no-prosody
if not exist "%PROSODY_CFGDIR%" goto make-folder
if not exist "%PROSODY_CFGDIR%\prosody.cfg.lua" goto install-config
goto run

:make-folder
mkdir "%PROSODY_CFGDIR%"
:install-config
copy src\prosody.cfg.lua.dist "%PROSODY_CFGDIR%\prosody.cfg.lua" > NUL
if exist "%PROSODY_CFGDIR%\certs" goto run
mkdir "%PROSODY_CFGDIR%\certs"
copy certs\localhost.* "%PROSODY_CFGDIR%\certs" > NUL

:run
if not exist "%PROSODY_DATADIR%" mkdir "%PROSODY_DATADIR%"
set path=%~dp0\bin\
set lua_path=%~dp0\src\?.lua;%~dp0\lib\?.lua
set lua_cpath=%~dp0\lib\?.dll
%~d0 && cd %~dp0
for /d %%v in (*) do if exist "%%v\bin" set path=%~dp0%%v\bin;%path%
for /d %%v in (*) do if exist "%%v\lib" set lua_path=%~dp0%%v\lib\?.lua;%lua_path%
for /d %%v in (*) do if exist "%%v\lib" set lua_cpath=%~dp0%%v\lib\?.dll;%lua_cpath%
lua.exe "%~dp0\src\prosody"
goto exit

:no-prosody
echo File 'prosody' not found.
echo Current directory: %~dp0
pause

:exit
