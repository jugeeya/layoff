@echo off
echo Don't tell me you forgot about windows users :P
set BasePath=%1
if [%BasePath%]==[] set BasePath=%~dp0
if not exist %BasePath%\ goto err
	mkdir %BasePath%\atmosphere
    mkdir %BasePath%\atmosphere\titles
    mkdir %BasePath%\atmosphere\titles\010000000000100C
    copy layoff.nsp %BasePath%\atmosphere\titles\010000000000100C\exefs.nsp

if "%~2"=="romfs" (goto romfs) else goto :eof
:romfs
mkdir %BasePath%\atmosphere\titles\010000000000100C\romfs
xcopy romfs %BasePath%\atmosphere\titles\010000000000100C\romfs
echo > %BasePath%\atmosphere\titles\010000000000100C\fsmitm.flag
goto :eof
:err
echo %BasePath% doesn't exist.