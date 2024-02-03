@echo off
title Push build to Itch.io using Butler Push
@echo on

set DIR=%~dp0
set PUSH_DIR=%DIR%Build\Windows

set USER=colorcrow
set GAME=adfectus

:: The name of a channel has meaning:
::     If it contains win or windows, it'll be tagged as a Windows executable
::     If it contains linux, it'll be tagged as a Linux executable
::     If it contains mac or osx, it'll be tagged as a Mac executable
::     If it contains android, it'll be tagged as an Android application
::     Channel names may contain multiple platforms (for example, a Java game could be pushed to win-linux-mac-stable)
set CHANNEL=win

butler push "%PUSH_DIR%" %USER%/%GAME%:%CHANNEL%
