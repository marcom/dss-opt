from typing import Union

import dssopt.lib as lib
from dssopt.lib import libdssopt, list_to_carray
from dssopt.lib import c_bool, c_char_p, c_double, c_uint, create_string_buffer

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
    if status == lib.C_EXIT_SUCCESS:
        return True
    return False

def opt_md(target_dbn: str,
           seed: Union[int, None] = None,
           seq_constraints_hard: Union[str, None] = None,
           # default values taken from main-opt-md.c
           time_total: float = lib.DSSOPT_DEFAULT_OPT_MD_time_total,
           time_print: float = lib.DSSOPT_DEFAULT_OPT_MD_time_print,
           time_cool: Union[float, None] = None,
           time_pur: Union[float, None] = None,
           timestep: float = lib.DSSOPT_DEFAULT_OPT_MD_timestep,
           T_start: float = lib.DSSOPT_DEFAULT_OPT_MD_T_start,
           kpi: float = lib.DSSOPT_DEFAULT_kpi,
           kpa: float = lib.DSSOPT_DEFAULT_kpa,
           kneg: float = lib.DSSOPT_DEFAULT_kneg,
           kpur_end: float = lib.DSSOPT_DEFAULT_kpur,
           khet: float = lib.DSSOPT_DEFAULT_khet,
           het_window: int = lib.DSSOPT_DEFAULT_het_window,
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
    if status != lib.C_EXIT_SUCCESS:
        raise Exception(f'run_md returned non-zero exit status: {status}')
    designed_seq = designed_seq_buf.value.decode('utf-8')
    return designed_seq

def opt_sd(target_dbn: str,
           seed: Union[int, None] = None,
           # TODO: taken from main-opt-sd.c
           maxsteps: int = lib.DSSOPT_DEFAULT_OPT_SD_maxsteps,
           nprint: int = lib.DSSOPT_DEFAULT_OPT_SD_nprint,
           wiggle: float = lib.DSSOPT_DEFAULT_OPT_SD_wiggle,
           kpi: float = lib.DSSOPT_DEFAULT_kpi,
           kpa: float = lib.DSSOPT_DEFAULT_kpa,
           kpur: float = lib.DSSOPT_DEFAULT_kpur,
           kneg: float = lib.DSSOPT_DEFAULT_kneg,
           khet: float = lib.DSSOPT_DEFAULT_khet,
           het_window: int = lib.DSSOPT_DEFAULT_het_window,
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
    if status != lib.C_EXIT_SUCCESS:
        raise Exception(f'run_sd returned non-zero exit status: {status}')
    designed_seq = designed_seq_buf.value.decode('utf-8')
    return designed_seq
