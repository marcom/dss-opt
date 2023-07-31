import pathlib
import ctypes
from ctypes import CDLL, c_bool, c_char_p, c_int, c_uint, c_double, create_string_buffer, pointer, POINTER
c_uint_p = ctypes.POINTER(c_uint)
import numpy as np

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

class c_npmat():
    # A wrapper for a numpy 2-d array (matrix) that is also a C matrix,
    # accessible in C with a[i][j].
    # - all data is stored in one block in the numpy array
    # - for the C array-of-arrays, there is an additional array with pointers
    #   to the first element of each row
    def __init__(self, arr: np.ndarray):
        if arr.ndim != 2:
            raise RuntimeError('numpy array must have ndim = 2')
        self._arr = arr
        nrow = self.arr.shape[0]
        self._ctype_elem = np.ctypeslib.as_ctypes_type(self._arr.dtype)
        ptr_ctype = ctypes.POINTER(self._ctype_elem)
        self._row_cptrs = list_to_carray([ptr_ctype(self._ctype_elem(0))] * nrow, ptr_ctype)
        for i in range(nrow):
            index = (i, 0)
            ptr_addr = self._arr.ctypes.data + sum(i * s for i, s in zip(index, self._arr.strides))
            self._row_cptrs[i] = ctypes.cast(ptr_addr, ptr_ctype)
    def __getitem__(self, index):
        return self._arr.__getitem__(index)
    def __setitem__(self, index, value):
        return self._arr.__setitem__(index, value)
    def __repr__(self):
        return f"c_npmat wrapped numpy array\n\n{self._arr.__repr__()}"

    @property
    def arr(self):
        return self._arr
    @property
    def ctype_elem(self):
        return self._ctype_elem
    @property
    def ctype_arr(self):
        return self._row_cptrs


NBASE = 4

K_nj = np.zeros((NBASE, NBASE))
_tmp_k_nj_ptr = helper_make_K_nj_alloc(NBASE)
for i in range(NBASE):
    for j in range(NBASE):
        K_nj[i, j] = _tmp_k_nj_ptr[i][j]
# del _tmp_k_nj_ptr
K_nj_wrap = c_npmat(K_nj)
