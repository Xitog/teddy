"""-----------------------------------------------------------
typedini: An extension for typed ini file
-----------------------------------------------------------
Created: March 2, 2020
Last Modified: March 2, 2020
version: 0.1.0
-----------------------------------------------------------
MIT Licence (Expat License Wording)
-----------------------------------------------------------
Copyright © 2020, Damien Gouteux

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE."""

#-------------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------------

import os.path
import configparser

#-------------------------------------------------------------------------------
# Classes
#-------------------------------------------------------------------------------

class OptionSet:
    "An set of options"
    
    def __init__(self, filepath: str, autosave : bool = False):
        "Filepath: the file where are stored the values of the options. autosave: write the file each time an option is changed."
        self.filepath = filepath
        self.options = {}
        self.config = None
        self.true_values = ['TRUE', 'YES', 'True', 'Yes', 'true', 'yes']
        self.false_values = ['FALSE', 'NO', 'False', 'No', 'false', 'no']
        self.autosave = autosave

    def register(self, name, kind, base, value=None):
        "Register an option in the OptionSet."
        self.options[name] = Option(self, name, kind, base, value)

    def reset(self):
        "Reset all the options to their default values and rewrite the file."
        for key, opt in self.options.items():
            opt.reset()
        self.write()

    def read_or_create(self):
        "Read the file where are stored the values of the options or create it."
        if os.path.isfile(self.filepath):
            conf = configparser.ConfigParser()
            conf.read(self.filepath)
            for section in conf:
                if section in ['DEFAULT', 'MAIN']:
                    for key in conf[section]:
                        if key in self.options:
                            self.options[key].load_from_string(conf[section][key])
                        else:
                            print('[INFO] option not known: ' + str(key) + ', val = ' + str(conf[section][key]))
        else:
            self.write()

    def write(self):
        "Write all the option values in a file."
        f = open(self.filepath, mode='w', encoding='utf8')
        conf = configparser.ConfigParser()
        conf.add_section('MAIN')
        for key, opt in self.options.items():
            conf.set('MAIN', key, str(opt))
        conf.write(f)
        f.close()

    def change(self, opt, val):
        "Change the value of the option."
        if not isinstance(val, self.options[opt].kind):
            raise TypeError("value not of type: " + str(self.options[opt].kind))
        else:
            self.options[opt].value = val
            if self.autosave:
                self.write()

    def __getitem__(self, opt: str):
        "Retrieve the value of an option."
        return self.options[opt].value

    def __setitem__(self, opt, val):
        "Change the value of the option."
        self.change(opt, val)

    def __repr__(self):
        "Representation of the option set."
        s = ''
        for key, opt in self.options.items():
            s += f'{key:10} -> {str(opt.value):10} : {opt.kind.__name__:6} {opt.base}\n'
        return s


class Option:
    "An option defined by a type (kind), a default value and a name."

    def __init__(self, optset, name, kind, base, value=None):
        "Optset: the root optset. name: name of the option. kind: type of the option. base: default value. value: actual value."
        self.optset = optset
        self.name = name
        self.kind = kind
        self.base = base
        self.value = value if value is not None else base
        if kind not in [int, str, bool, float]:
            raise TypeError("unsupported option type: " + str(kind))

    def reset(self):
        "Reset the value of the option to its base."
        self.value = self.base

    def load_from_string(self, value: str):
        "Load from a string the value of the option."
        if self.kind == int:
            self.value = int(value)
        elif self.kind == float:
            self.value = float(value)
        elif self.kind == bool:
            if value in self.optset.true_values:
                self.value = True
            elif value in self.optset.false_values:
                self.value = False
            else:
                raise ValueError("unrecognized value for boolean option: " + str(self.value))
        elif self.kind == str:
            self.value = value
        else:
            raise TypeError("unsupported value type: " + str(value.__class__))

    def __str__(self):
        "Get the value of the option as a string."
        return str(self.value)

    def __repr__(self):
        "Get a representation of the option."
        return f"{self.name} -> {self.value} : {self.kind.__name__}, def = {self.base}"

#-------------------------------------------------------------------------------
# Main
#-------------------------------------------------------------------------------

if __name__ == '__main__':
    o = OptionSet('config.ini')
    o.register('name', str, 'default')
    o.register('age', int, 32)
    o.register('precision', float, 1.359)
    o.register('open', bool, True)
    o.read_or_create()
    #o.change('name', 'hexen')
    #o.write()
