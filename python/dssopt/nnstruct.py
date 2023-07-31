import dssopt.lib as lib
import numpy as np
from typing import Union

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

def seqargmax(pseq: np.ndarray, alphabet: str='ACGU'):
    if pseq.ndim != 2:
        raise RuntimeError('input array shape must have ndim = 2')
    (n, na) = pseq.shape
    if len(alphabet) != na:
        raise RuntimeError(f'array and alphabet have incompatible lengths, {na} != {len(alphabet)}')
    return ''.join(list(map(lambda i: alphabet[i], np.argmax(pseq, axis=1))))

class NNstruct(lib.struct_nn_inter):
    def __init__(self, dbn: str):
        self.n = len(dbn)
        self.nbase = lib.NA_NBASES.value
        self.inter = lib.nn_inter_xnew(self.n)
        self.dbn = dbn
        self._wrap_out_dGdp = None
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

    def dGdp(self, pseq: np.ndarray) -> np.ndarray:
        if pseq.shape != (self.n, self.nbase):
            raise RuntimeError(f'pseq must have shape ({self.n}, {self.nbase})')
        if pseq.dtype != np.dtype('float64'):
            raise RuntimeError('pseq must have dytpe float64')
        if self._wrap_out_dGdp is None:
            self._wrap_out_dGdp = lib.c_npmat(np.zeros_like(pseq))
        else:
            self._wrap_out_dGdp[:] = 0.0
        wrap_pseq = lib.c_npmat(pseq)
        # wrap_out_dGdp = lib.c_npmat(self._out_dGdp)
        lib.calc_interactions_dGdp_pseq(self.inter, wrap_pseq.ctype_arr, self._wrap_out_dGdp.ctype_arr)
        return self._wrap_out_dGdp.arr

    def dGdp_from_str(self, seq: str) -> np.ndarray:
        if len(seq) != self.n:
            raise RuntimeError(f'seq must have length {self.n}, but has length {len(seq)}')
        pseq = one_hot_seq(seq)
        return self.dGdp(pseq)

    def U_negdesign_nj(self, pseq: np.ndarray, *,
                       kneg: float=lib.DSSOPT_DEFAULT_kneg.value,
                       K_nj: Union[np.ndarray,None]=None) -> float:
        if pseq.ndim != 2:
            raise RuntimeError('pseq must be a matrix')
        (n, na) = pseq.shape
        wrap_pseq = lib.c_npmat(pseq)
        if K_nj is None:
            wrap_K_nj = lib.K_nj_wrap
        else:
            if K_nj.shape != (na, na):
                raise RuntimeError(f'K_nj must have shape ({na}, {na}), as pseq has shape ({n}, {na})')
            wrap_K_nj = lib.c_npmat(K_nj)
        en = lib.dss_calc_U_negdesign_nj(wrap_pseq.ctype_arr, n, na, kneg,
                                         wrap_K_nj.ctype_arr, self.inter.contents.pairs)
        return en

    def gradU_negdesign_nj(self, pseq: np.ndarray, *,
                           kneg: float=lib.DSSOPT_DEFAULT_kneg.value,
                           K_nj: Union[np.ndarray,None]=None) -> np.ndarray:
        if pseq.shape != (self.n, self.nbase):
            raise RuntimeError(f'pseq must have shape ({self.n}, {self.nbase})')
        if pseq.dtype != np.dtype('float64'):
            raise RuntimeError('pseq must have dytpe float64')
        if self._wrap_out_dGdp is None:
            self._wrap_out_dGdp = lib.c_npmat(np.zeros_like(pseq))
        else:
            self._wrap_out_dGdp[:] = 0.0
        if K_nj is None:
            wrap_K_nj = lib.K_nj_wrap
        else:
            if K_nj.shape != (self.nbase, self.nbase):
                raise RuntimeError(f'K_nj must have shape ({self.nbase}, {self.nbase}),'
                                   f' as pseq has shape ({self.n}, {self.nbase})')
            wrap_K_nj = lib.c_npmat(K_nj)
        wrap_pseq = lib.c_npmat(pseq)
        lib.dss_calc_gradU_negdesign_nj(
            wrap_pseq.ctype_arr, self.n, self.nbase, kneg, wrap_K_nj.ctype_arr,
            self.inter.contents.pairs, self._wrap_out_dGdp.ctype_arr
        )
        return self._wrap_out_dGdp.arr

    def U_het(self, pseq: np.ndarray, *,
              khet: float=lib.DSSOPT_DEFAULT_khet.value,
              het_window_size: int=lib.DSSOPT_DEFAULT_het_window.value
              ) -> float:
        if pseq.shape != (self.n, self.nbase):
            raise RuntimeError(f'pseq must have shape ({self.n}, {self.nbase})')
        if pseq.dtype != np.dtype('float64'):
            raise RuntimeError('pseq must have dytpe float64')
        wrap_pseq = lib.c_npmat(pseq)
        en = lib.dss_calc_U_het(wrap_pseq.ctype_arr, self.n, self.nbase, khet,
                                het_window_size, self.inter.contents.pairs)
        return en

    def gradU_het(self, pseq: np.ndarray, *,
                  khet: float=lib.DSSOPT_DEFAULT_khet.value,
                  het_window_size: int=lib.DSSOPT_DEFAULT_het_window.value
                  ) -> np.ndarray:
        if pseq.shape != (self.n, self.nbase):
            raise RuntimeError(f'pseq must have shape ({self.n}, {self.nbase})')
        if pseq.dtype != np.dtype('float64'):
            raise RuntimeError('pseq must have dytpe float64')
        if self._wrap_out_dGdp is None:
            self._wrap_out_dGdp = lib.c_npmat(np.zeros_like(pseq))
        else:
            self._wrap_out_dGdp[:] = 0.0
        wrap_pseq = lib.c_npmat(pseq)
        lib.dss_calc_gradU_het(
            wrap_pseq.ctype_arr, self.n, self.nbase, khet, het_window_size,
            self.inter.contents.pairs, self._wrap_out_dGdp.ctype_arr
        )
        return self._wrap_out_dGdp.arr

    def U_pa(self, pseq: np.ndarray, *,
             kpa: float=lib.DSSOPT_DEFAULT_kpa.value) -> float:
        if pseq.shape != (self.n, self.nbase):
            raise RuntimeError(f'pseq must have shape ({self.n}, {self.nbase})')
        if pseq.dtype != np.dtype('float64'):
            raise RuntimeError('pseq must have dytpe float64')
        wrap_pseq = lib.c_npmat(pseq)
        en = lib.dss_calc_U_pa(wrap_pseq.ctype_arr, self.n, self.nbase, kpa)
        return en

    def gradU_pa(self, pseq: np.ndarray, *,
                 kpa: float=lib.DSSOPT_DEFAULT_kpa.value) -> np.ndarray:
        if pseq.shape != (self.n, self.nbase):
            raise RuntimeError(f'pseq must have shape ({self.n}, {self.nbase})')
        if pseq.dtype != np.dtype('float64'):
            raise RuntimeError('pseq must have dytpe float64')
        if self._wrap_out_dGdp is None:
            self._wrap_out_dGdp = lib.c_npmat(np.zeros_like(pseq))
        else:
            self._wrap_out_dGdp[:] = 0.0
        wrap_pseq = lib.c_npmat(pseq)
        lib.dss_calc_gradU_pa(
            wrap_pseq.ctype_arr, self.n, self.nbase, kpa,
            self._wrap_out_dGdp.ctype_arr
        )
        return self._wrap_out_dGdp.arr
