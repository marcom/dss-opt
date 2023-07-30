import dssopt.lib as lib

# TODO
# - use str_to_useq here

def calc_interactions_useq(seq: str, dbn: str) -> int:
    if len(seq) != len(dbn):
        raise RuntimeError(f"sequence and structure have different lengths, {len(seq)} != {len(dbn)}")
    n = len(dbn)
    useq = lib.list_to_carray([0] * n, lib.c_uint)
    lib.xstr_to_useq(n, seq, useq)
    inter = lib.nn_inter_xnew(n)
    vienna = dbn
    verbose = False
    ret = lib.vienna_to_pairs(n, vienna, verbose, inter.contents.pairs)
    if ret != lib.C_EXIT_SUCCESS:
        raise RuntimeError("vienna_to_pairs failed: n={n} vienna={vienna}")
    lib.find_interactions(inter)
    en = lib.calc_interactions_useq(inter, useq)
    lib.nn_inter_delete(inter)
    return en
