f = open(r'..\editor.py', mode='r', encoding='utf8')
content = f.readlines()
f.close()

class Class:
    def __init__(self, name):
        self.name = name
        self.methods = {}

class Method:
    def __init__(self, name):
        self.name = name
        self.calls = []

last_class = None
classes = {}
methods = {}

for raw in content:
    line = raw.strip()
    if line.startswith('class') and line.endswith(':'):
        name = line[6:-1]
        classes[name] = Class(name)
        last_class = name
    elif line.startswith('def') and line.endswith(':'):
        name = line[4:-1]
        if raw.startswith('    ') and last_class is not None:
            classes[last_class].methods[name] = Method(name)
        else:
            methods[name] = Method(name)

#for line in content:
#   if line.index('(') != -1:
#       print(line)

print('Classes:\n')

for cname in classes:
    c = classes[cname]
    print('    class', c.name, 'with', len(c.methods), 'methods.')
    for m in c.methods:
        print('       ', m)

print(f'\nNumber of classes: {len(classes)}\n')

print('Free Methods:\n')

for m in methods:
    print('   ', m)

print(f'\nNumber of free methods: {len(methods)}\n')
