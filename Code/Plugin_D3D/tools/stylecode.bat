@echo off

cmd /C "_stylehelper.bat ..\src cpp"
cmd /C "_stylehelper.bat ..\src h"
cmd /C "_stylehelper.bat ..\src hpp"
cmd /C "_stylehelper.bat ..\inc cpp"
cmd /C "_stylehelper.bat ..\inc h"
cmd /C "_stylehelper.bat ..\inc hpp"

pause