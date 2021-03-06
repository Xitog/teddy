"""A map with different layers."""

import json
from traceback import print_exc
from sys import stdout

def pretty_json(data, level=0, indent=4):
    "Pretty print some JSON."
    content = ''
    if isinstance(data, dict):
        content += '{\n'
        level += 1
        count = 0
        for key, val in data.items():
            content += ' ' * indent * level + '"' + str(key) + '": '
            content += pretty_json(val, level, indent)
            count += 1
            if count == len(data):
                level -= 1
                content += '\n'
                content += ' ' * indent * level + '}'
            else:
                content += ',\n'
        if len(data) == 0:
            level -= 1
            content += ' ' * indent * level + '}'
    elif isinstance(data, list):
        list_in_list = False
        for val in data:
            if isinstance(val, list):
                list_in_list = True
                break
        content += '['
        level += 1
        count = 0
        if list_in_list:
            content += '\n'
        for val in data:
            if list_in_list:
                content += ' ' * indent * level
            content += pretty_json(val, level)
            count += 1
            if count == len(data):
                level -= 1
                if list_in_list:
                    content += '\n' + ' ' * indent * level
                content += ']'
            else:
                content += ', '
                if list_in_list:
                    content += '\n'
        if len(data) == 0:
            level -= 1
            content += ' ' * indent * level + ']'
    elif isinstance(data, str):
        content += '"' + data + '"'
    elif isinstance(data, bool):
        content += "true" if data else "false"
    elif isinstance(data, int):
        content += str(data)
    elif isinstance(data, float):
        content += str(data)
    else:
        raise Exception('Type unknown: ' + data.__class__.__name__)
    return content


class Obj:

    def __init__(self, name, attributes):
        self.name = name
        self.attr = attributes

    def __str__(self):
        return f"<Obj {self.name}>"


class Layer:
    "A class representing a layer of the map."

    @staticmethod
    def create_matrix(max_col: int, max_row: int, default: int = 0):
        "Static method for creating a matrix, a list of rows."
        matrix = []
        for _ in range(max_row):
            row = []
            for _ in range(max_col):
                row.append(default)
            matrix.append(row)
        return matrix

    def __init__(self, mymap: str, name: str, default: int, obj : Obj = None):
        self.map = mymap
        self.name = name
        self.default = default
        self.content = Layer.create_matrix(self.map.width,
                                           self.map.height,
                                           default)
        self.prototype = obj
        self.objects = []

    def is_object_layer(self):
        "This layer is an object layer."
        return self.prototype is not None

    def resize(self):
        "Resize a layer."
        old = self.content
        self.content = Layer.create_matrix(self.map.width,
                                           self.map.height,
                                           self.default)
        for row in range(min(len(old), self.map.height)):
            for col in range(min(len(old[0]), self.map.width)):
                self.content[row][col] = old[row][col]

    def get(self, row: int, col: int):
        "Return int at row, col."
        return self.content[row][col]

    def set(self, val, row: int, col: int):
        "Set val at row, col. val can be int or dict"
        prev = self.content[row][col]
        if not isinstance(prev, int):
            self.objects.remove(prev)
        self.content[row][col] = val
        if not isinstance(val, int):
            self.objects.append(val)
        return {'name': self.name, 'row': row, 'col': col,
                'prev': prev, 'next': val}

    def to_json(self):
        "Return a JSON representation of the layer."
        res = {
            "default": self.default
        }
        if self.is_object_layer():
            res['object'] = self.prototype.name
            res['content'] = self.objects
        else:
            res['content'] = self.content
        return res


class Map:
    "A class representing a map/level/floor."

    @staticmethod
    def get_mod(filepath):
        "Get the mod of a map from a JSON file."
        file = open(filepath, mode='r', encoding='utf8')
        data = json.load(file)
        file.close()
        return data['mod']

    @staticmethod
    def from_json(app, filepath):
        "Load a map from a JSON file."
        file = open(filepath, mode='r', encoding='utf8')
        data = json.load(file)
        file.close()
        a_map = Map(app, data["name"], data["width"], data["height"])
        for name, lay in data["layers"].items():
            obj = None
            if 'object' in lay:
                obj = app.mod.layers[name].obj
            a_map.add_layer(name, lay["default"], obj)
            if 'object' in lay:
                # populate with the objects
                for obj in lay['content']:
                    a_map.layers[name].content[obj['y']][obj['x']] = obj
                    a_map.layers[name].objects.append(obj)
            else:
                # replace with the content saved
                a_map.layers[name].content = lay["content"]
        a_map.filepath = filepath
        a_map.modcode = data["mod"]
        return a_map

    def __init__(self, app, name: str, max_col: int, max_row: int):
        self.app = app
        self.modcode = app.mod.code if app is not None and \
                       app.mod is not None else 'undefined'
        self.name = name
        self.layers = {}
        self.width = max_col
        self.height = max_row
        self.filepath = None


    def resize(self, width, height):
        "Resize a map and all its layers."
        self.width = width
        self.height = height
        for _, lay in self.layers.items():
            lay.resize()

    def info(self):
        "Display info on the map."
        print(repr(self))
        if len(self.layers) == 0:
            print('No layer in map')
            return
        for name, lay in self.layers.items():
            print('<' + name + '>')
            if lay.object is not None:
                print("    type =", lay.object)
                print("    ido  =", lay.ido)
            print('    ', end='')
            print(*lay.content, sep='\n    ')
            if lay.object is not None:
                for key, value in lay.objects.items():
                    print(f"{key:5d} : {value}")

    def add_layer(self, name, default: int = 0, obj: str = None):
        "Add a layer to a map."
        if not isinstance(default, int):
            msg = f'[ERROR] Only integer value not {default.__class__.__name__}'
            raise Exception(msg)
        if name in self.layers:
            msg = f'[ERROR] Layer {name} already exists.'
            raise Exception(msg)
        self.layers[name] = Layer(self, name, default, obj)

    def has_objects(self, name):
        "The layer name is an object layer."
        return self.layers[name].is_object_layer()

    def check(self, row, col, layer=None):
        "Check row, col and layer if not None."
        if layer is not None and layer not in self.layers:
            return False
            #raise Exception(f"[ERROR] Layer {layer} not defined.")
        if not 0 <= row < self.height:
            return False
            #raise Exception(f"[ERROR] Out of row: {row} / {self.height}")
        if not 0 <= col < self.width:
            return False
            #raise Exception(f"[ERROR] Out of col: {col} / {self.width}")
        return True

    def get(self, row, col, layer=None):
        "Get value at row, col in layer."
        self.check(row, col, layer)
        if layer is not None:
            return self.layers[layer].get(row, col)
        res = {}
        for _, lay in self.layers.items():
            res[lay.name] = lay.get(row, col)
        return res

    def set(self, val, row, col, layer):
        "Set value at row, col in layer."
        self.check(row, col, layer)
        return self.layers[layer].set(val, row, col)

    def set_name(self, name):
        "Set the name of the map."
        self.name = name

    def __repr__(self):
        mod_name = self.app.mod.code if self.app is not None and \
                   self.app.mod is not None else ''
        return f"{self.name} {self.width}x{self.height}" + \
               f"[{len(self.layers)}]{mod_name}"

    def to_json(self):
        "Return a JSON representation of the map."
        data = {
            "name": self.name,
            "width": self.width,
            "height": self.height
        }
        data["mod"] = self.app.mod.code if self.app is not None and \
                      self.app.mod is not None else ''
        data["layers"] = {}
        for _, lay in self.layers.items():
            data["layers"][lay.name] = lay.to_json()
        return data

    def save(self, filepath):
        "Save to a file."
        file = open(filepath, mode='w', encoding='utf8')
        file.write(pretty_json(self.to_json()))
        file.close()
        try:
            file = open(filepath, mode='r', encoding='utf8')
            json.load(file)
            file.close()
        except json.JSONDecodeError:
            print('Something went wrong when trying to load map. Map may be corrupted.')
            print('Stack info:')
            print_exc(file=stdout)
        self.filepath = filepath


if __name__ == '__main__':
    # Create matrix tests
    L5X10 = Layer.create_matrix(5, 10)
    print(*L5X10, sep='\n')
    # Map and layer tests
    A_MAP = Map(None, "A map", 4, 4)
    A_MAP.info()
    A_MAP.add_layer('ground', 0)
    A_MAP.info()
    A_MAP.set(5, 3, 3, 'ground')
    A_MAP.info()
    A_MAP.resize(7, 7)
    A_MAP.info()
    # Pretty json tests
    A_DICT = {'a': 5, 'b': True, 'c': 3.14}
    print("1:")
    print(pretty_json(A_DICT))
    A_LIST = [1, 2, 3, 4, 5]
    print("2:")
    print(pretty_json(A_LIST))
    A_MIXED = [True, [1, 2, 3], "hello"]
    print("3:")
    print(pretty_json(A_MIXED))
    A_MATRIX = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
    print("4:")
    print(pretty_json(A_MATRIX))
    A_COMBINED = {
        "layers": {
            "ground": [
                [1, 1, 1],
                [2, 2, 2],
                [3, 3, 3]
            ]
        }
    }
    print(pretty_json(A_COMBINED))
    print("5:")
    A_DICT_OF_DICT = {"dA1": {"dB1": 5}, "dA2": [1, 2, 3, "hello"], "dA3": True}
    print(pretty_json(A_DICT_OF_DICT))
    print("Final:")
    print(pretty_json(A_MAP.to_json()))
    A_MAP.save("saved.map")
    A_NEW_MAP = Map.from_json(None, "saved.map")
    A_NEW_MAP.save("saved2.map")
    print("With Unit:")
    Unit = {'name': 'Content_Name', 'player': 'Meta_Player'}
    A_MAP.add_layer('unit', 0, Unit)
    A_MAP.set({"name": "kbot", "player": 1, "val": 28}, 3, 3, "unit")
    A_MAP.info()
