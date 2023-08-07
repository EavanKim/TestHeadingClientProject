@echo off

SET "START_PATH=%~dp0"
SET "EXEC=%START_PATH%libiconv\bin\iconv.exe"

SET CONVERT_PATH_LIST=%START_PATH%..\HeadingNet\HeadingNet
SET CONVERT_PATH_LIST=%CONVERT_PATH_LIST%;%START_PATH%..\HeadingNet\HeadingNet\Inc
SET CONVERT_PATH_LIST=%CONVERT_PATH_LIST%;%START_PATH%..\HeadingNet\HeadingNet\Inc\Base
SET CONVERT_PATH_LIST=%CONVERT_PATH_LIST%;%START_PATH%..\HeadingNet\HeadingNet\Inc\Util
SET CONVERT_PATH_LIST=%CONVERT_PATH_LIST%;%START_PATH%..\HeadingNet\Testing
SET CONVERT_PATH_LIST=%CONVERT_PATH_LIST%;%START_PATH%..\TestHeadingClientProject

FOR %%i IN (%CONVERT_PATH_LIST%) DO (
	PUSHD %%i
	FOR /f %%j IN ('DIR /b *.h, *.cpp') DO (
		%EXEC% -f utf-8 -t euc-kr %%j > %%j.tmp
		IF %ERRORLEVEL% EQU 0 (
			mv %%j.tmp %%j
			REM ECHO "Success Convert...(%%i\%%j)"
		)
	)
	POPD
)

PAUSE
