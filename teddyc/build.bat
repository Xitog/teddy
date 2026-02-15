cl .\main.c .\libattopng.c .\common.c .\assets.c .\levels.c .\image.c .\os.c /Fe:wt
del *.obj
wt.exe
