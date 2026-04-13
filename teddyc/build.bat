@echo off
rem in order to not run the previous version if build fail
if exist "wt.exe" (
    del wt.exe
)
cl .\main.c .\common.c .\assets.c .\levels.c .\image.c .\os.c .\libraries\miniz.c .\libraries\spng.c /Fe:wt
del *.obj
if exist "wt.exe" (
    wt.exe
) else (
    echo Build failure: Impossible to run wt.exe
)
