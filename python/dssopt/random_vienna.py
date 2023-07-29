import dssopt.lib as lib
# from dssopt.lib import libdssopt
from dssopt.lib import c_uint, create_string_buffer

def random_vienna(n: int, hpmin: int=3) -> str:
    """Generate random secondary structure in Vienna (dot-bracket) format.
    Note that the secondary structures are not uniformly sampled.

    Args:
        n (int):      Length of secondary structure
        hpmin (int):  Minimum hairpin length (default = 3)

    Returns:
        str: The secondary structure in Vienna (dot-bracket) notation.

    """
    pairs = lib.list_to_carray([0] * n, c_uint)
    lib.random_pairs(n, pairs, hpmin)
    # + 1 for C-style 0-terminated strings
    dbn_buf = create_string_buffer(n + 1)
    lib.xpairs_to_vienna(n, pairs, dbn_buf)
    dbn = dbn_buf.value.decode('utf-8')
    return dbn
