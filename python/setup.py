import subprocess
from setuptools import setup, Command
from distutils.command.install import install as _install
import os
import shutil
from distutils.sysconfig import get_python_lib
import venv

# change to dir where this file (setup.py) is located, so that
# relative paths will be correct
os.chdir(os.path.dirname(os.path.abspath(__file__)))

class InstallPkg(_install):
    def run(self):
        #subprocess.call(['make', 'clean', '-C', '..'])
        subprocess.check_call(['make', '-C', '..', 'lib'])
        lib_path = "../libdssopt.so"
        destination = os.path.join(get_python_lib(), "dssopt")
        os.makedirs(destination, exist_ok=True)
        shutil.copy(lib_path, destination)

        # # automatically generate ctypes bindings
        # # needs ctypesgen installed: `pip install ctypesgen`
        # subprocess.check_call(['ctypesgen', '-o', 'dssopt/pylibdssopt.py', '-l', 'libdssopt.so', '../*.h'])

        _install.run(self)

class RunCtypesGen(Command):
    user_options = []
    def initialize_options(self):
        pass
    def finalize_options(self):
        pass
    def run(self):
        # Create a temporary virtual environment
        venv_dir = "build.ctypesgen-venv"
        venv.create(venv_dir, with_pip=True)
        
        # Activate the virtual environment
        # TODO: Windows needs to use Scripts\activate
        activate = f". {venv_dir}/bin/activate"
        # install all needed deps in venv
        dependencies = ['ctypesgen',]
        pip_install_deps = f"{activate} && pip install {' '.join(dependencies)}"
        subprocess.check_call(pip_install_deps, executable='/bin/bash', shell=True)
        
        # run ctypesgen
        run_script = f"{activate} && ctypesgen -o dssopt/pylibdssopt.py -l libdssopt.so ../*.h ../ctools/random.h"
        subprocess.check_call(run_script, executable='/bin/bash', shell=True)


setup(
    name = 'dssopt',
    version = '0.1',
    author = 'Marco Matthies',
    packages=['dssopt'],
    # package_data={'dssopt': ['../libdssopt.so']},
    cmdclass={
        'install': InstallPkg,
        'ctypesgen': RunCtypesGen
    },
    # ext_modules=[dssopt_module],
    install_requires = [
        'numpy',
    ],
    extras_require = {
        'test': ['pytest',],
    },
)
