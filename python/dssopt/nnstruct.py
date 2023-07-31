import dssopt.lib as lib
import numpy as np

# TODO
# - use str_to_useq here instead of xstr_to_useq

def one_hot_seq(s: str, alphabet: str='ACGU', dtype=np.dtype('float64')) -> np.ndarray:
    n = len(s)
    na = len(alphabet)
    p = np.zeros((n, na), dtype=dtype)
    for i in range(n):
        j = alphabet.index(s[i])
        p[i,j] = 1.0
    return p

class NNstruct(lib.struct_nn_inter):
    def __init__(self, dbn: str):
        self.n = len(dbn)
        self.nbase = lib.NA_NBASES.value
        self.inter = lib.nn_inter_xnew(self.n)
        self.dbn = dbn
        ret = lib.vienna_to_pairs(self.n, self.dbn, False, self.inter.contents.pairs)
        if ret != lib.C_EXIT_SUCCESS:
            raise RuntimeError("vienna_to_pairs failed: n={self.n} vienna={self.dbn}")
        lib.find_interactions(self.inter)
    def __del__(self):
        lib.nn_inter_delete(self.inter)

    def energy_useq(self, seq: str) -> int:
        # original function: calc_interactions_useq
        useq = lib.list_to_carray([0] * self.n, lib.c_uint)
        lib.xstr_to_useq(self.n, seq, useq)
        en = lib.calc_interactions_useq(self.inter, useq)
        return en

    def energy_pseq(self, pseq: np.ndarray) -> float:
        # original function: calc_interactions_pseq
        req_shape = (self.n, self.nbase)
        if pseq.shape != req_shape:
            raise RuntimeError(f'pseq must have shape {req_shape}')
        wrap_pseq = lib.c_npmat(pseq)
        ret = lib.calc_interactions_pseq(self.inter, wrap_pseq.ctype_arr)
        return ret

    def energy_pseq_from_str(self, seq: str) -> float:
        if len(seq) != self.n:
            raise RuntimeError(f'seq must have length {self.n}, but has length {len(seq)}')
        pseq = one_hot_seq(seq)
        return self.energy_pseq(pseq)
