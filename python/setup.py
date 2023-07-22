import subprocess
from setuptools import setup
from distutils.command.install import install as _install
import os
import shutil
from distutils.sysconfig import get_python_lib

class InstallPkg(_install):
    def run(self):
        #subprocess.call(['make', 'clean', '-C', '..'])
        subprocess.call(['make', '-C', '..', 'lib'])
        lib_path = "../libdssopt.so"
        destination = os.path.join(get_python_lib(), "dssopt")
        os.makedirs(destination, exist_ok=True)
        shutil.copy(lib_path, destination)
        _install.run(self)

setup(
    name = 'dssopt',
    version = '0.1',
    author = 'Marco Matthies',
    packages=['dssopt'],
    # package_data={'dssopt': ['../libdssopt.so']},
    cmdclass={'install': InstallPkg},
    # ext_modules=[dssopt_module],
    extras_require = {
        'test': ['pytest',],
    },
)
