import pytest
import dssopt
import numpy as np

@pytest.mark.parametrize(
    "kwargs, expected",
    [
        # same tests as in unittest/test-calc_interactions_useq.c
        # hairpins
        ({'seq': 'GAAAC', 'dbn': '(...)'}, 570),
        ({'seq': 'GAAAAC', 'dbn': '(....)'}, 450),
        ({'seq': 'GAAAAAAC', 'dbn': '(......)'}, 430),
        # special hairpins
        ({'seq': 'GGAAAC', 'dbn': '(....)'}, 20),
        # stacks
        ({'seq': 'GGGAAACCC', 'dbn': '(((...)))'}, -90),
        ({'seq': 'CGGAAACCG', 'dbn': '(((...)))'},   0),
        ({'seq': 'GGGGUUUUCCCC', 'dbn': '((((....))))'}, -580),
        ({'seq': 'GGCGUUAUCGCC', 'dbn': '((((....))))'}, -500),
        ({'seq': 'GACGUUAUCGUC', 'dbn': '((((....))))'}, -290),
        # bulges
        ({'seq': 'GGGGAAAAAUCCC', 'dbn': '(((.(....))))'},   10),
        ({'seq': 'GGGGAAAACACCC', 'dbn': '((((....).)))'}, -160),
        # internal loops
        ({'seq': 'GGAGGAAAACACCC', 'dbn': '((.((....).)))'},  220),
        ({'seq': 'GGAGGAAAACCACC', 'dbn': '((.((....)).))'}, -170),
        # multiway loops
        ({'seq': 'GGAAGGGAAAACCCGGGAAACCCACC', 'dbn': '((..(((....)))(((...))).))'},   -720),        
        ({'seq': 'GGAAACGAAAACGUGGGAAACCCAUC', 'dbn': '((..(((....)))(((...))).))'},    -70),        
        ({'seq': 'GGAAACGAAAACGUAGGGAAACCCAUC', 'dbn': '((..(((....))).(((...))).))'},  -90),
        ({'seq': 'GGAGGAAAACCAACGAAAACGUAGUGAAACGCAUC', 'dbn': '((.((....)).(((....))).(((...))).))'}, 150),
        # empty structures
        ({'seq': 'A',      'dbn': '.'},      0),
        ({'seq': 'CA',     'dbn': '..'},     0),
        ({'seq': 'CAA',    'dbn': '...'},    0),
        ({'seq': 'UGAC',   'dbn': '....'},   0),
        ({'seq': 'GGACC',  'dbn': '.....'},  0),
        ({'seq': 'AGGACC', 'dbn': '......'}, 0),
    ]
)
def test_nnstruct_energy_useq(kwargs, expected):
    print(kwargs)
    seq = kwargs['seq']
    dbn = kwargs['dbn']
    s = dssopt.NNstruct(dbn)
    result = s.energy_useq(seq)
    assert result == expected
    result_pseq = s.energy_pseq_from_str(seq)
    assert result_pseq == expected
    n = len(seq)
    na = 4
    g = s.dGdp(0.25 * np.ones((n, na)))
    assert isinstance(g, np.ndarray)
    assert g.shape == (n, na)
