"""A simple tool for editing images
   Damien Gouteux 2020 MIT Licence"""

from sys import argv
from copy import deepcopy
from PIL import Image

class ImageInfo:
    """A class for handling a picture"""

    def __init__(self, filepath_or_size):
        i = None
        if isinstance(filepath_or_size, str):
            i = Image.open(filepath_or_size).convert("RGBA")
        elif isinstance(filepath_or_size, tuple) and len(filepath_or_size) == 2:
            width = filepath_or_size[0]
            height = filepath_or_size[1]
            if isinstance(width, int) and isinstance(height, int):
                i = Image.new("RGBA", (width, height), (0, 0, 0, 255))
        if i is None:
            i = Image.new("RGBA", (32, 32), (0, 0, 0, 255))
        #s = i.getpixel((0, 0))
        self.img = i
        self.__colors = None
        self.colors()
        self.width = i.width
        self.height = i.height
        self.history = []

    def get(self, column, row):
        "Return the color at column/x, row/y"
        return self.img.getpixel((column, row))

    def colors(self):
        "Return all the colors used in the picture"
        colors = []
        for col in range(self.img.width):
            for row in range(self.img.height):
                color = self.img.getpixel((col, row))
                if color not in colors:
                    colors.append(color)
        self.__colors = colors
        return self.__colors

    def replace(self, old, new):
        "Replace a color number by another color"
        if 0 <= old < len(self.__colors):
            self.history.append(deepcopy(self.img))
            for col in range(self.width):
                for row in range(self.height):
                    color = self.img.getpixel((col, row))
                    if color == self.__colors[old]:
                        self.img.putpixel((col, row), new)
                    else:
                        self.img.putpixel((col, row), color)
        else:
            raise IndexError(f"Color index out of bound: {old}")

    def highlight(self, color_number):
        "Replace a color number by magenta"
        self.replace(color_number, (255, 0, 255))

    def undo(self):
        "Undo last action"
        last_image = self.history[-1]
        self.history.remove(last_image)
        del self.img
        self.img = last_image

    def reset(self):
        "Reset history"
        if len(self.history) > 0:
            first_image = self.history[0]
            self.history.clear()
            del self.img
            self.img = first_image

    def save(self, output="out.png"):
        "Set the current buffer as image and save it to the disk"
        self.img.save(output)
        self.history.clear()

    def __len__(self):
        return len(self.history)

#colors[(237,  28, 36, 255)] = (0, 255,   0, 255)
#colors[(255, 127, 39, 255)] = (0,   0, 255, 255)

COMMANDS = {
    'create': 'create a black picture.',
    'load <file>': 'load a picture.',
    'list': 'list all the colors of a picture.',
    'save <file>' : 'save to a file, swap buffer.',
    'save': 'save to out.png.',
    'reset': 'reset current buffer to starting image.',
    'highlight <color index>': 'replace the given color by magenta.',
    'replace <color index> <R G B>': 'replace the given color by another.',
    'len': 'number of saved operations.',
    'help': 'this help.',
    'exit': 'exit the interactive shell.'
    }

def execute_simple_command(current, cmd):
    "Execute the simple commands"
    if cmd.startswith('load'):
        _, target = cmd.split(' ')
        current = ImageInfo(target)
    elif cmd == 'create':
        current = ImageInfo((32, 32))
    elif cmd == 'len':
        print('Number of saved operations:', len(current))
    elif cmd == 'list':
        colors = current.colors()
        print('Number of colors:', len(colors))
        print('Colors:')
        for i, col in enumerate(colors):
            print(f"\t{i:3d}. Red {col[0]:3d} Green {col[1]:3d} Blue " + \
                  f"{col[2]:3d} Alpha {col[3]:3d}")
    elif cmd == 'save':
        current.save()
    elif cmd == 'reset':
        current.reset()
    elif cmd == 'help':
        for name, desc in COMMANDS.items():
            print(f"{name:30} : {desc}")
    return current

def execute(current, cmd, target):
    "Execute a command"
    if cmd in ['create', 'len', 'list', 'save', 'reset', 'help']:
        return execute_simple_command(current, cmd)
    if cmd.startswith('save'):
        _, target = cmd.split(' ')
        current.save(target)
    elif cmd.startswith('highlight'):
        _, old = cmd.split(' ')
        try:
            current.highlight(int(old))
        except IndexError:
            print('Replacement cancelled due to wrong color index')
    elif cmd.startswith('replace'):
        _, old, new_red, new_green, new_blue = cmd.split(' ')
        try:
            current.replace(int(old), (int(new_red), int(new_green),
                                       int(new_blue), 255))
        except IndexError:
            print('Replacement cancelled due to wrong color index')
    else:
        print('Command not known:', cmd.split(' ')[0])
    return current

def main():
    "Main function"
    current = None
    #print(argv)
    if len(argv) >= 3:
        target = argv[1]
        command = argv[2]
        current = execute(current, command, target)
    else:
        command = ''
        while command != 'exit':
            command = input('>>> ')
            target = 'img.png'
            if command != 'exit':
                current = execute(current, command, target)

if __name__ == '__main__':
    main()
