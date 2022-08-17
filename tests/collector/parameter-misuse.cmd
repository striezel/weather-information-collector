:: Script to test executable when parameters are used in the wrong way.
::
::  Copyright (C) 2022  Dirk Stolle
::
::  This program is free software: you can redistribute it and/or modify
::  it under the terms of the GNU Lesser General Public License as published by
::  the Free Software Foundation, either version 3 of the License, or
::  (at your option) any later version.
::
::  This program is distributed in the hope that it will be useful,
::  but WITHOUT ANY WARRANTY; without even the implied warranty of
::  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
::  GNU Lesser General Public License for more details.
::
::  You should have received a copy of the GNU Lesser General Public License
::  along with this program.  If not, see <http://www.gnu.org/licenses/>.

@echo off

:: 1st parameter = executable path
if "%1" EQU "" (
  echo First parameter must be executable file!
  exit /B 1
)

SET EXECUTABLE=%1

:: parameter --conf / -c given twice
"%EXECUTABLE%" --conf foo.conf --conf twice.conf
if %ERRORLEVEL% NEQ 1 (
  echo Executable did not exit with code 1 when --conf was given twice.
  exit /B 1
)

:: no file name after parameter --conf
"%EXECUTABLE%" --conf
if %ERRORLEVEL% NEQ 1 (
  echo Executable did not exit with code 1 when --conf had no file name.
  exit /B 1
)

:: --ignore-limits is given twice
"%EXECUTABLE%" --ignore-limits --ignore-limits
if %ERRORLEVEL% NEQ 1 (
  echo Executable did not exit with code 1 when --ignore-limits was given twice.
  exit /B 1
)

:: --skip-update-check is given twice
"%EXECUTABLE%" --skip-update-check --skip-update-check
if %ERRORLEVEL% NEQ 1 (
  echo Executable did not exit with code 1 when --skip-update-check was given twice.
  exit /B 1
)

:: unknown parameter
"%EXECUTABLE%" --this-is-not-supported
if %ERRORLEVEL% NEQ 1 (
  echo Executable did not exit with code 1 when an unknown parameter was given.
  exit /B 1
)

:: Configuration file does not exist / fail to load.
"%EXECUTABLE%" --conf this-file-is-missing.conf
if %ERRORLEVEL% NEQ 2 (
  echo Executable did not exit with code 2 when configuration file was missing.
  exit /B 1
)

exit 0
