from distutils.core import setup
from Cython.Build import cythonize
from distutils.extension import Extension

import os

pid_dir = os.path.join(os.path.dirname(__file__), "../firmware/lib/pid")

setup(
    ext_modules = cythonize([
        Extension("cpid", ["cpid.pyx", os.path.join(pid_dir, "src/pid.c")],
            include_dirs=[os.path.join(pid_dir, "inc")])
    ])
)