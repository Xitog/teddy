cl .\main.c .\common.c .\assets.c .\levels.c .\image.c .\os.c .\libraries\miniz.c .\libraries\spng.c /Fe:wt
del *.obj
wt.exe
