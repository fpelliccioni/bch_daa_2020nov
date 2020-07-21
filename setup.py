#!/usr/bin/env python

# 
# Copyright (c) 2020 Fernando Pelliccioni
# 

import glob
import os
import platform
import shutil
import stat
import sys

from setuptools import setup, find_packages
from setuptools.extension import Extension
from distutils import dir_util, file_util
from distutils import log
from setuptools.command.install_lib import install_lib
from setuptools.command.install import install
from setuptools.command.develop import develop
# from setuptools.command.egg_info import egg_info
from setuptools.command.build_ext import build_ext

from distutils.command.build import build
from setuptools.command.install import install

from setuptools.dist import Distribution
import fnmatch

from sys import platform

PKG_NAME = 'aserti3416cpp'
SYSTEM = sys.platform


def get_similar_lib(path, pattern):
    # for file in os.listdir('.'):
    #     if fnmatch.fnmatch(file, '*.txt'):
    #         print file
    for file in os.listdir(path):
        if fnmatch.fnmatch(file, pattern):
            return file
    return ""

def get_libraries():
    return []

def do_build_stuff():

    print('*********************************************************************************************************')
    print(os.path.dirname(os.path.abspath(__file__)))
    print(os.getcwd())
    print('*********************************************************************************************************')

    prev_dir = os.getcwd()

    print('*********************************************************************************************************')
    print(os.path.dirname(os.path.abspath(__file__)))
    print(os.getcwd())
    print('*********************************************************************************************************')

    os.chdir(prev_dir) 

    print('*********************************************************************************************************')
    print(os.path.dirname(os.path.abspath(__file__)))
    print(os.getcwd())
    print('*********************************************************************************************************')

    extensions[0].libraries = get_libraries()
    

class DevelopCommand(develop):
    user_options = develop.user_options

    def initialize_options(self):
        develop.initialize_options(self)

    def finalize_options(self):
        develop.finalize_options(self)

    def run(self):
        do_build_stuff()
        develop.run(self)



class InstallCommand(install):
    user_options = install.user_options

    def initialize_options(self):
        install.initialize_options(self)

    def finalize_options(self):
        install.finalize_options(self)

    def run(self):
        do_build_stuff()
        install.run(self)


class BuildCommand(build):
    user_options = build.user_options

    def initialize_options(self):
        build.initialize_options(self)

    def finalize_options(self):
        build.finalize_options(self)

    def run(self):
        do_build_stuff()
        build.run(self)


# ------------------------------------------------



extensions = [
	Extension('aserti3416cpp',

        # define_macros = [('KTH_LIB_STATIC', None),],
        # include_dirs=['kth/include'],
        # library_dirs=['kth/lib'],

    	sources = ['aserti3-416.cpp',  'aserti3-416_capi.cpp', 'aserti3-416_pyapi.c', 'pyapi_module.c'],
    ),
]



exec(open('./version.py').read())
setup(
    name = PKG_NAME,
    version = __version__,

    description = 'ASERT i3 416 C++ wrapper',
    long_description = 'ASERT i3 416 C++ wrapper',
    url='https://github.com/fpelliccioni',

    author='Fernando Pelliccioni',
    author_email='fpelliccioni@gmail.com',
    license='MIT',

    # See https://pypi.python.org/pypi?%3Aaction=list_classifiers
    classifiers=[
        # How mature is this project? Common values are
        #   3 - Alpha
        #   4 - Beta
        #   5 - Production/Stable
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'Topic :: Software Development :: Build Tools',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: C++',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
    ],

    keywords='bitcoin cash difficulty daa asert',

    setup_requires=["wheel"],

    extras_require={
        'dev': ['check-manifest'],
        'test': ['coverage'],
    },

    ext_modules = extensions,

    cmdclass={
        'build': BuildCommand,
        'install': InstallCommand,
        'develop': DevelopCommand,
        # 'egg_info': EggInfoCommand,
        
    },

)



