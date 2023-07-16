from typing import Union
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

DEFAULT_DSSOPT_ndim = c_uint.in_dll(libdssopt, 'DEFAULT_DSSOPT_ndim').value
DEFAULT_DSSOPT_kpi = c_double.in_dll(libdssopt, 'DEFAULT_DSSOPT_kpi').value
DEFAULT_DSSOPT_kpa = c_double.in_dll(libdssopt, 'DEFAULT_DSSOPT_kpa').value
DEFAULT_DSSOPT_kneg = c_double.in_dll(libdssopt, 'DEFAULT_DSSOPT_kneg').value
DEFAULT_DSSOPT_kpur = c_double.in_dll(libdssopt, 'DEFAULT_DSSOPT_kpur').value
DEFAULT_DSSOPT_khet = c_double.in_dll(libdssopt, 'DEFAULT_DSSOPT_khet').value
DEFAULT_DSSOPT_het_window = c_uint.in_dll(libdssopt, 'DEFAULT_DSSOPT_het_window').value

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

def isok_seq_constraints(constraints: str, vienna: str) -> bool:
    if constraints is None:
        return True
    if len(constraints) != len(vienna):
        return False
    n = len(constraints)
    c_n = c_uint(n)
    hard = list_to_carray([0] * n, c_uint)
    n_hard = c_uint(0)
    pairs = list_to_carray([0] * n, c_uint)
    verbose = c_bool(True)
    c_constraints = constraints.encode('utf-8')
    status = libdssopt.parse_seq_constraints_hard(c_n, hard, n_hard, c_constraints, verbose, pairs)
    if status == C_EXIT_SUCCESS:
        return True
    return False

def opt_md(target_dbn: str,
           seed: Union[int, None] = None,
           seq_constraints_hard: Union[str, None] = None,
           # default values taken from main-opt-md.c
           time_total: float = 50.0,
           time_print: float = 2.5,
           time_cool: Union[float, None] = None,
           time_pur: Union[float, None] = None,
           timestep: float = 0.0015,
           T_start: float = 40.0,
           kpi: float = DEFAULT_DSSOPT_kpi,
           kpa: float = DEFAULT_DSSOPT_kpa,
           kneg: float = DEFAULT_DSSOPT_kneg,
           kpur_end: float = DEFAULT_DSSOPT_kpur,
           khet: float = DEFAULT_DSSOPT_khet,
           het_window: int = DEFAULT_DSSOPT_het_window,
           do_exp_cool: bool = False,
           do_movie_output :bool = False,
           verbose: bool = False,
           ) -> str:
    """Design a sequence for a given secondary structure by dynamical
    simulated annealing optimization (dynamics in sequence space).

    Returns the designed sequence.

    Args:
        target_dbn (str): Target secondary structure in Vienna format (dot-bracket)
        seed (int): Seed for random numbers, if set to None current time is used
        seq_constraints_hard (str): Hard sequence constraints (can be None). String of "ACGU" chars and "N" or "." wildcards
        time_total (float): Total optimization time
        time_print (float): Interval between printing optimization state
        time_cool (float): Time after which cooling should begin
        time_pur (float): Time after which purification terms should start to be linearly increased (see kpur_end)
        timestep (float): Simulation timestep
        T_start (float): Starting temperature
        kpi (float): Scoring function constant for penalty terms, keeps :math:`x_{ij} \\in [0,1]`
        kpa (float): Scoring function constant for penalty term, keeps :math:`\sum_j x_{ij} \\approx 1`
        kneg (float): Scoring function constant for mean-field negative design term
        khet (float): Scoring function constant for sequence heterogeneity term
        het_window (int): Window size for sequence heterogeneity term
        kpur_end (float): Scoring function constant for sequence purification term at the end of optimization
        do_exp_cool (bool): Flag to indicate if exponential cooling is to be used, otherwise linear cooling is used
        do_movie_output (bool): Flag to indicate if movie output is to be generated
        verbose (bool): Flag to indicate if verbose output is enabled

    Returns:
        str: The designed sequence

    """
    if time_cool == None:
        time_cool = 0.1 * time_total
    if time_pur == None:
        time_pur = 0.8 * time_total
    if time_total < 0 or time_print < 0 or time_cool < 0:
        raise Exception('All times must be >= 0')
    if timestep < 0:
        raise Exception('timestep must be >= 0')
    if het_window < 0:
        raise Exception('het_window must be >= 0')
    if not isok_seq_constraints(seq_constraints_hard, target_dbn):
        raise Exception(f'seq_constraints_hard are illegal\ntarget_dbn  = {target_dbn}\nconstraints = {seq_constraints_hard}')
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
        seq_constraints_hard = c_char_p()
    else:
        seq_constraints_hard = seq_constraints_hard.encode('utf-8')

    # + 1 for C-style 0-terminated strings
    designed_seq_buf = create_string_buffer(len(target_dbn) + 1)
    status = libdssopt.run_md(vienna, seq_constraints_hard, nsteps, nprint, ncool, npur,
                              timestep, T_start, kpi, kpa, kneg, khet, het_window, kpur_end,
                              do_exp_cool, do_movie_output, verbose, designed_seq_buf)
    if status != C_EXIT_SUCCESS:
        raise Exception(f'run_md returned non-zero exit status: {status}')
    designed_seq = designed_seq_buf.value.decode('utf-8')
    return designed_seq

def opt_sd(target_dbn: str,
           seed: Union[int, None] = None,
           maxsteps: int = 20000,
           nprint: int = 1000,
           wiggle: float = 0.1,
           kpi: float = DEFAULT_DSSOPT_kpi,
           kpa: float = DEFAULT_DSSOPT_kpa,
           kpur: float = DEFAULT_DSSOPT_kpur,
           kneg: float = DEFAULT_DSSOPT_kneg,
           khet: float = DEFAULT_DSSOPT_khet,
           het_window: int = DEFAULT_DSSOPT_het_window,
           do_movie_output :bool = False,
           verbose: bool = False,
           ) -> str:
    """Design a sequence for a given secondary structure by
    steepest descent optimization.

    Returns the designed sequence.

    Args:
        target_dbn (str): Target secondary structure in Vienna format (dot-bracket)
        seed (int): Seed for random numbers, if set to None current time is used
        maxsteps (int): Maximum number of steps for optimization
        nprint (int): Printing interval for showing status of optimization
        kpi (float): Scoring function constant for penalty terms, keeps :math:`x_{ij} \\in [0,1]`
        kpa (float): Scoring function constant for penalty term, keeps :math:`\sum_j x_{ij} \\approx 1`
        kpur (float): Scoring function constant for sequence purification term
        kneg (float): Scoring function constant for mean-field negative design term
        khet (float): Scoring function constant for sequence heterogeneity term
        het_window (int): Window size for sequence heterogeneity term
        do_movie_output (bool): Flag to indicate if movie output is to be generated
        verbose (bool): Flag to indicate if verbose output is enabled

    Returns:
        str: The designed sequence

    """
    if maxsteps <= 0:
        raise Exception('maxsteps must be > 0')
    if het_window < 0:
        raise Exception('het_window must be >= 0')
    if seed == None:
        seed = libdssopt.random_get_seedval_from_current_time()
    libdssopt.random_seed(seed)
    vienna = target_dbn.encode('utf-8')
    # + 1 for C-style 0-terminated strings
    designed_seq_buf = create_string_buffer(len(target_dbn) + 1)
    status = libdssopt.run_sd(vienna, maxsteps, nprint, wiggle,
                              kpi, kpa, kpur, kneg, khet, het_window,
                              do_movie_output, verbose, designed_seq_buf)
    if status != C_EXIT_SUCCESS:
        raise Exception(f'run_sd returned non-zero exit status: {status}')
    designed_seq = designed_seq_buf.value.decode('utf-8')
    return designed_seq


def random_vienna(n: int, hpmin: int=3) -> str:
    pairs = list_to_carray([0] * n, c_uint)
    libdssopt.random_pairs(n, byref(pairs), hpmin)
    # + 1 for C-style 0-terminated strings
    dbn_buf = create_string_buffer(n + 1)
    libdssopt.xpairs_to_vienna(n, pairs, dbn_buf)
    dbn = dbn_buf.value.decode('utf-8')
    return dbn
