@echo off
title ElDewrito dedicated server
cd ../../
set n=0

color 0e
echo Starting ElDewrito dedicated server...
echo [%DATE% %TIME:~0,-3%] Started >> DedicatedServer.log

start eldorado.exe -launcher -dedicated -window -height 200 -width 200 -minimized

timeout 10 /nobreak > nul

:loop

tasklist /v /fi "IMAGENAME eq eldorado.exe" /fi "STATUS eq Running" | findstr "eldorado.exe" > nul
if errorlevel 1 (

	cls
	set n=0
	color 0c
	taskkill /f /im eldorado.exe
	if errorlevel 1 (
		echo ElDewrito dedicated server was not running, restarting...
		echo [%DATE% %TIME:~0,-3%] Restarted >> DedicatedServer.log
	) else (
		echo ElDewrito dedicated server has crashed! Restarting...
		echo [%DATE% %TIME:~0,-3%] Crashed >> DedicatedServer.log
	)
	start eldorado.exe -launcher -dedicated -window -height 200 -width 200 -minimized

) else (

	cls
	set /a n=%n%+1
	color 0f
	echo ElDewrito dedicated server is running! ^(%n%^)

)

timeout 10 /nobreak > nul

goto loop