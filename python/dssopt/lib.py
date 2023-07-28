import pathlib
import ctypes
from ctypes import CDLL, byref, c_bool, c_char_p, c_int, c_uint, c_double, create_string_buffer, pointer
c_uint_p = ctypes.POINTER(c_uint)

# TODO maybe: try to load from this path, if file not exists load
# __file__/../../libdssopt.so as this is the path in the repo when the
# python module has not been installed with pip

libdssopt_path = str(pathlib.Path(__file__).parent / "libdssopt.so")
#print(f"__file__ = {__file__}")
#print(f"libdssopt_path = {libdssopt_path}")
libdssopt = CDLL(libdssopt_path)

C_EXIT_SUCCESS = 0

DSSOPT_DEFAULT_ndim = c_uint.in_dll(libdssopt, 'DSSOPT_DEFAULT_ndim').value
DSSOPT_DEFAULT_kpi = c_double.in_dll(libdssopt, 'DSSOPT_DEFAULT_kpi').value
DSSOPT_DEFAULT_kpa = c_double.in_dll(libdssopt, 'DSSOPT_DEFAULT_kpa').value
DSSOPT_DEFAULT_kneg = c_double.in_dll(libdssopt, 'DSSOPT_DEFAULT_kneg').value
DSSOPT_DEFAULT_kpur = c_double.in_dll(libdssopt, 'DSSOPT_DEFAULT_kpur').value
DSSOPT_DEFAULT_khet = c_double.in_dll(libdssopt, 'DSSOPT_DEFAULT_khet').value
DSSOPT_DEFAULT_het_window = c_uint.in_dll(libdssopt, 'DSSOPT_DEFAULT_het_window').value

# C function argtypes and restype
# parse_seq_constraints_hard
libdssopt.parse_seq_constraints_hard.argtypes = [
    c_uint,    # n
    c_uint_p,  # *hard
    c_uint_p,  # *n_hard
    c_char_p,  # *constraint_str
    c_bool,    # verbose
    c_uint_p,  # *pairs
]
libdssopt.parse_seq_constraints_hard.restype = c_uint
# vienna_to_pairs
libdssopt.vienna_to_pairs.argtypes = [
    c_uint,    # n
    c_char_p,  # *vienna
    c_bool,    # verbose
    c_uint_p,  # *pairs
]
libdssopt.vienna_to_pairs.restype = c_int
# run_md
libdssopt.run_md.argtypes = [
    c_char_p,  # *vienna
    c_char_p,  # *seq_constraints_hard
    c_uint,    # nsteps
    c_uint,    # nprint
    c_uint,    # ncool
    c_uint,    # npur
    c_double,  # timestep
    c_double,  # T_start
    c_double,  # kpi
    c_double,  # kpa
    c_double,  # kneg
    c_double,  # khet
    c_uint,    # het_window
    c_double,  # kpur_end
    c_bool,    # do_exp_cool
    c_bool,    # do_movie_output
    c_bool,    # verbose
    c_char_p,  # *designed_seq
]
libdssopt.run_md.restype = c_int
# run_sd
libdssopt.run_sd.argtypes = [
    c_char_p,  # *vienna
    c_uint,    # maxsteps
    c_uint,    # nprint
    c_double,  # wiggle
    c_double,  # kpi
    c_double,  # kpa
    c_double,  # kpur
    c_double,  # kneg
    c_double,  # khet
    c_uint,    # het_window
    c_bool,    # do_movie_output
    c_bool,    # verbose
    c_char_p,  # *designed_seq
]
libdssopt.run_sd.restype = c_int


def list_to_carray(lst, ctype=c_int):
    return (ctype * len(lst))(*lst)
