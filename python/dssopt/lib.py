from typing import Union
import pathlib
import ctypes
from ctypes import CDLL, byref, c_bool, c_char_p, c_int, c_uint, c_double, create_string_buffer, pointer

libdssopt_path = str(pathlib.Path(__file__).parent / "libdssopt.so")
#print(f"__file__ = {__file__}")
#print(f"libdssopt_path = {libdssopt_path}")
libdssopt = CDLL(libdssopt_path)

C_EXIT_SUCCESS = 0

DEFAULT_DSSOPT_ndim = c_uint.in_dll(libdssopt, 'DEFAULT_DSSOPT_ndim').value
DEFAULT_DSSOPT_kpi = c_double.in_dll(libdssopt, 'DEFAULT_DSSOPT_kpi').value
DEFAULT_DSSOPT_kpa = c_double.in_dll(libdssopt, 'DEFAULT_DSSOPT_kpa').value
DEFAULT_DSSOPT_kneg = c_double.in_dll(libdssopt, 'DEFAULT_DSSOPT_kneg').value
DEFAULT_DSSOPT_kpur = c_double.in_dll(libdssopt, 'DEFAULT_DSSOPT_kpur').value
DEFAULT_DSSOPT_khet = c_double.in_dll(libdssopt, 'DEFAULT_DSSOPT_khet').value
DEFAULT_DSSOPT_het_window = c_uint.in_dll(libdssopt, 'DEFAULT_DSSOPT_het_window').value

def list_to_carray(lst, ctype=c_int):
    return (ctype * len(lst))(*lst)

def opt_md(target_dbn: str,
           seq_constraints_hard: Union[str, None] = None,
           # taken from main-opt-md.c
           time_total: float = 50.0,
           time_print: float = 2.5,
           time_cool: Union[float, None] = None,
           time_pur: Union[float, None] = None,
           timestep: float   = 0.0015,
           T_start: float    = 40.0,
           kpi: float = DEFAULT_DSSOPT_kpi,
           kpa: float = DEFAULT_DSSOPT_kpa,
           kneg: float = DEFAULT_DSSOPT_kneg,
           kpur_end: float = DEFAULT_DSSOPT_kpur,
           khet: float = DEFAULT_DSSOPT_khet,
           het_window: int = DEFAULT_DSSOPT_het_window,
           do_exp_cool: bool = False,
           do_movie_output :bool = False,
           seed: Union[int, None] = None,
           verbose: bool = False,
           ) -> str:
    # TODO
    # - checks
    #   - seq_constraints_hard
    if time_cool == None:
        time_cool = 0.1 * time_total
    if time_pur == None:
        time_pur = 0.8 * time_total
    if time_total < 0 or time_print < 0 or time_cool < 0:
        raise Exception("All times must be >= 0")
    if timestep < 0:
        raise Exception("timestep must be >= 0")
    if het_window < 0:
        raise Exception("het_window must be >= 0")
    nsteps = c_uint(round(time_total / timestep))
    nprint = c_uint(round(time_print / timestep))
    ncool  = c_uint(round(time_cool  / timestep))
    npur   = c_uint(round(time_pur   / timestep))
    if seed == None:
        seed = libdssopt.random_get_seedval_from_current_time()
    libdssopt.random_seed(seed)

    timestep = c_double(timestep)
    T_start = c_double(T_start)
    vienna = target_dbn.encode('utf-8')
    if seq_constraints_hard == None:
        seq_constraints_hard = ctypes.POINTER(ctypes.c_char)()
    else:
        seq_constraints_hard = seq_constraints_hard.encode('utf-8')

    # + 1 for C-style 0-terminated strings
    designed_seq_buf = create_string_buffer(len(target_dbn) + 1)
    libdssopt.run_md.argtypes = [c_char_p, c_char_p, c_uint, c_uint, c_uint, c_uint,
                                 c_double, c_double, c_double, c_double, c_double, c_double, c_uint, c_double,
                                 c_bool, c_bool, c_bool, c_char_p]
    libdssopt.run_md.restype = c_uint
    status = libdssopt.run_md(vienna, seq_constraints_hard, nsteps, nprint, ncool, npur,
                              timestep, T_start, kpi, kpa, kneg, khet, het_window, kpur_end,
                              do_exp_cool, do_movie_output, verbose, designed_seq_buf)
    if status != C_EXIT_SUCCESS:
        raise Exception(f'run_md returned non-zero exit status: {status}')
    designed_seq = designed_seq_buf.value.decode('utf-8')
    return designed_seq

def random_vienna(n: int, hpmin: int=3):
    pairs = list_to_carray([0] * n, c_uint)
    libdssopt.random_pairs(n, byref(pairs), hpmin)
    # + 1 for C-style 0-terminated strings
    dbn_buf = create_string_buffer(n + 1)
    libdssopt.xpairs_to_vienna(n, pairs, dbn_buf)
    dbn = dbn_buf.value.decode('utf-8')
    return dbn
