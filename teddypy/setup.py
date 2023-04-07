from cx_Freeze import setup, Executable

# Dependencies are automatically detected, but it might need
# fine tuning.
buildOptions = dict(packages = [], 
                    excludes = ['numpy', 'PyQt5', 'scipy', 'twisted'])

import sys
base = 'Win32GUI' if sys.platform=='win32' else None

executables = [
    Executable('editor.py', base=base, targetName = 'teddy')
]

setup(name='Teddy',
      version = '0.1',
      description = 'Teddy is a simple tile grid editor',
      options = dict(build_exe = buildOptions),
      executables = executables)
