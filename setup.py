from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import sys
import setuptools
import os
import subprocess

class CMakeExtension(Extension):
  def __init__(self, name, sourcedir=''):
    Extension.__init__(self, name, sources=[])
    self.sourcedir = os.path.abspath(sourcedir)

class CMakeBuild(build_ext):
  def run(self):
    for ext in self.extensions:
      self.build_extension(ext)

  def build_extension(self, ext):
    extdir = self.get_ext_fullpath(ext.name)
    extdir = os.path.abspath(os.path.dirname(extdir))
    cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                  '-DPYTHON_EXECUTABLE=' + sys.executable,
                  '-DBUILD_PYTHON_BINDINGS=ON']

    cfg = 'Debug' if self.debug else 'Release'
    build_args = ['--config', cfg]

    if not os.path.exists(self.build_temp):
      os.makedirs(self.build_temp)
    subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, cwd=self.build_temp)
    subprocess.check_call(['cmake', '--build', '.'] + build_args, cwd=self.build_temp)

setup(
  name='farfler_network',
  version='0.1',
  author='Daniel Bacsur, Adam Vajda',
  author_email='farfler@farfler.com',
  description='Decentralized, peer-to-peer, publish-subscribe networking library',
  long_description='',
  ext_modules=[CMakeExtension('farfler_network')],
  cmdclass=dict(build_ext=CMakeBuild),
  zip_safe=False,
)
