import pathlib
import ctypes
from ctypes import CDLL, c_bool, c_char_p, c_int, c_uint, c_double, create_string_buffer, pointer
c_uint_p = ctypes.POINTER(c_uint)

# TODO maybe: try to load from this path, if file not exists load
# __file__/../../libdssopt.so as this is the path in the repo when the
# python module has not been installed with pip

libdssopt_path = str(pathlib.Path(__file__).parent / "libdssopt.so")
#print(f"__file__ = {__file__}")
#print(f"libdssopt_path = {libdssopt_path}")
libdssopt = CDLL(libdssopt_path)

C_EXIT_SUCCESS = 0

# ctypes interface automatically generated with `ctypesgen`
import dssopt.pylibdssopt
from dssopt.pylibdssopt import *

def list_to_carray(lst, ctype=c_int):
    return (ctype * len(lst))(*lst)
