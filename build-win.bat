@echo off
title Build Adfectus

set DIR=%~dp0
set BUILD_DIR=%DIR%Build
set PROJECT=%DIR%Adfectus.uproject

set UE_DIR=D:\Program Files\Epic Games\UE_5.1
set UE_UAT=%UE_DIR%\Engine\Build\BatchFiles\RunUAT.bat
set PLATFORM=Win64

@REM Get unreal engine version from uproject file.
for /f "tokens=2 delims=:" %%a in ('findstr "EngineAssociation" "%PROJECT%"') do (
	set UE_VERSION=%%a
)
set UE_VERSION=%UE_VERSION: =%
set UE_VERSION=%UE_VERSION:"=%
set UE_VERSION=%UE_VERSION:~0,-1%

echo Adfectus will use Unreal Engine %UE_VERSION%...

@REM load unreal engine installs from registry
for /f "tokens=2*" %%a in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\EpicGames\Unreal Engine\%UE_VERSION%" /v "InstalledDirectory" 2^>^&1^|find "REG_"') do (
	set UE_DIR=%%b
)

if not exist "%UE_DIR%" (
	echo Unreal Engine installation not found.
	echo Please install Unreal Engine %UE_VERSION%.
	echo https://www.unrealengine.com/en-US/download
	pause
	exit
)

set UE_UAT=%UE_DIR%\Engine\Build\BatchFiles\RunUAT.bat

if not exist "%UE_UAT%" (
	echo Unreal Engine installation not found.
	echo Please install Unreal Engine %UE_VERSION%.
	echo https://www.unrealengine.com/en-US/download
	pause
	exit
)

@REM Build.
echo Found Unreal Engine %UE_VERSION%. Building...

cd "%UE_DIR%"
call "%UE_UAT%" ^
		BuildCookRun ^
			-project="%PROJECT%" -target=CPPThirdPerson -clientconfig=Shipping ^
			-package -platform=%PLATFORM% ^
			-archive -archivedirectory="%BUILD_DIR%" ^
			-compile -build -cook -stage ^
			-nop4 -iostore -pak -prereqs -nodebuginfo -installed -compressed

@echo on