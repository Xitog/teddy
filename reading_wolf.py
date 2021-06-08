import os
os.chdir(r'C:\Temp\Wolfenstein')
print(os.getcwd())

CHAR8 = 1
INT16 = 2
INT32 = 4

f = open('MAPHEAD-1.2.WL1', 'rb')
h = f.read()
f.close()

magic = h[0:0+INT16] # uint16le = 2x8
print(magic == b'\xcd\xab')
ptr = h[2:INT32 * 100 + 2]
levels = []
for i in range(100):
    levels.append(int.from_bytes(ptr[i*INT32:(i+1)*INT32],
                                 byteorder='little', signed=True))
print('Level 0 pointer:', levels[0]) # 0x08C3 = 2250 debut header 1er niveau

f = open('GAMEMAPS-1.2.WL1', 'rb')
g = f.read()
f.close()

header_ted5 = g[0:8]
print('Header:', header_ted5)

print(chr(g[2272])) # 2272 : (+22) Debut de W donc du nom du niveau

# Hardcoded 64x64 in Wolfenstein 3D
width = int.from_bytes(g[levels[0] + 18:INT16], byteorder='little',
                       signed=True)
height = int.from_bytes(g[levels[0] + 20:INT16], byteorder='little',
                       signed=True)
name = ''
for b in g[levels[0] + 22:levels[0] + 22 + CHAR8 * 16]:
    if b != '\0':
        name += chr(b)
print(width, height, name)

# '!ID!'
print(g[levels[0] + 22 + CHAR8 * 16:levels[0] + 22 + CHAR8 * 16 + CHAR8 * 4])

start = levels[0] + 22 + CHAR8 * 16 + CHAR8 * 4
lvl = g[start:levels[1]]
print('Taille compressee :', len(lvl), 'vs', 'Taille normale:', 64 * 64)

res = []
length = len(lvl)
while i < length:
    near = False
    far = False
    if lvl[i] != 0x00 and i+1 < length:
        near = (lvl[i+1] == 0xA7)
        far = (lvl[i+1] == 0xA8)
    if near:
        nb = lvl[i]
        start_ref = lvl[i+2]
        for j in range(i - start_ref, i - start_ref + nb):
            res.append(lvl[j])
        i += 2
    if far:
        nb = lvl[i]
        start_ref = int.from_bytes(lvl[i+2:i+4], byteorder='little', signed=False)
        for j in range(i - start_ref, i - start_ref + nb):
            res.append(lvl[j])
    else:
        res.append(lvl[i])
    i += 1

print('Taille uncamarck:', len(res))

unrlew = []

print('Level 1 pointer:', levels[1])
