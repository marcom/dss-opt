import dssopt.lib as lib

# TODO
# - use str_to_useq here instead of xstr_to_useq

class NNstruct(lib.struct_nn_inter):
    def __init__(self, dbn: str):
        self.n = len(dbn)
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
