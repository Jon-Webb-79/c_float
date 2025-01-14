@echo off
:: ================================================================================
:: ================================================================================
:: - File:    install.bat
:: - Purpose: Install c_string library files to system directories
::
:: Source Metadata
:: - Author:  Jonathan A. Webb
:: - Date:    January 11, 2025
:: - Version: 1.0
:: - Copyright: Copyright 2025, Jon Webb Inc.
:: ================================================================================
:: ================================================================================

:: Check for administrator privileges
NET SESSION >nul 2>&1
if %errorLevel% neq 0 (
   echo Please run as Administrator
   pause
   exit /b 1
)

:: Set installation directories
set "INCLUDE_DIR=%ProgramFiles(x86)%\c_string\include"
set "LIB_DIR=%ProgramFiles(x86)%\c_float\lib"

:: Create directories if they don't exist
if not exist "%INCLUDE_DIR%" mkdir "%INCLUDE_DIR%"
if not exist "%LIB_DIR%" mkdir "%LIB_DIR%"

:: Copy files
echo Installing header and source files...
copy /Y "..\..\c_float\c_float.h" "%INCLUDE_DIR%"
if errorlevel 1 (
   echo Error installing header file
   pause
   exit /b 1
)

copy /Y "..\..\c_float\c_float.c" "%LIB_DIR%"
if errorlevel 1 (
   echo Error installing source file
   pause
   exit /b 1
)

:: Add to system environment variables
setx C_INCLUDE_PATH "%INCLUDE_DIR%;%C_INCLUDE_PATH%" /M
setx LIBRARY_PATH "%LIB_DIR%;%LIBRARY_PATH%" /M

echo Installation completed successfully
pause
:: ================================================================================
:: ================================================================================
:: eof
