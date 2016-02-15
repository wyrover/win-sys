@echo off
cd /d %~dp0
del /F /S /Q *.aps *.idb *.ncp *.obj *.pch *.sbr *.tmp *.pdb *.bsc *.ilk *.res *.ncb *.opt *.suo *.manifest *.dep *.user
del *.suo /s /Q /ah
pause