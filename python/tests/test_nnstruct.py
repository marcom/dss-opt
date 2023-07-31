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
def test_nnstruct(kwargs, expected):
    print(kwargs)
    seq = kwargs['seq']
    dbn = kwargs['dbn']
    n = len(seq)
    na = 4
    s = dssopt.NNstruct(dbn)

    # energy_useq
    result = s.energy_useq(seq)
    assert result == expected

    # energy_pseq
    result_pseq = s.energy_pseq_from_str(seq)
    assert result_pseq == expected

    # dGdp
    pseq = 0.25 * np.ones((n, na))
    g = s.dGdp(pseq)
    assert isinstance(g, np.ndarray)
    assert g.shape == (n, na)

    # random_seq
    randseq = s.random_seq()
    assert len(randseq) == n

    # U_negdesign_nj
    U = s.U_negdesign_nj(pseq)
    assert isinstance(U, float)
    U = s.U_negdesign_nj(pseq, kneg = 20.0)
    assert isinstance(U, float)
    U = s.U_negdesign_nj(pseq, K_nj = np.ones((na,na)))
    assert isinstance(U, float)
    # gradU_negdesign_nj
    g = s.gradU_negdesign_nj(pseq)
    assert isinstance(g, np.ndarray)
    assert g.shape == (n, na)
    g = s.gradU_negdesign_nj(pseq, kneg=42.0)
    assert isinstance(g, np.ndarray)
    assert g.shape == (n, na)
    g = s.gradU_negdesign_nj(pseq, K_nj = np.ones((na,na)))
    assert isinstance(g, np.ndarray)
    assert g.shape == (n, na)

    # U_het
    U = s.U_het(pseq)
    assert isinstance(U, float)
    U = s.U_het(pseq, khet = 20.0)
    assert isinstance(U, float)
    U = s.U_het(pseq, het_window_size = 5)
    assert isinstance(U, float)
    # gradU_het
    g = s.gradU_het(pseq)
    assert isinstance(g, np.ndarray)
    assert g.shape == (n, na)
    g = s.gradU_het(pseq, khet = 42.0)
    assert isinstance(g, np.ndarray)
    assert g.shape == (n, na)
    g = s.gradU_het(pseq, het_window_size = 5)
    assert isinstance(g, np.ndarray)
    assert g.shape == (n, na)

    # U_pa
    U = s.U_pa(pseq)
    assert isinstance(U, float)
    U = s.U_pa(pseq, kpa = 20.0)
    assert isinstance(U, float)
    # gradU_pa
    g = s.gradU_pa(pseq)
    assert isinstance(g, np.ndarray)
    assert g.shape == (n, na)
    g = s.gradU_pa(pseq, kpa = 42.0)
    assert isinstance(g, np.ndarray)
    assert g.shape == (n, na)

    # U_pi
    U = s.U_pi(pseq)
    assert isinstance(U, float)
    U = s.U_pi(pseq, kpi = 20.0)
    assert isinstance(U, float)
    # gradU_pi
    g = s.gradU_pi(pseq)
    assert isinstance(g, np.ndarray)
    assert g.shape == (n, na)
    g = s.gradU_pi(pseq, kpi = 42.0)
    assert isinstance(g, np.ndarray)
    assert g.shape == (n, na)

    # U_pur_cauchy
    U = s.U_pur_cauchy(pseq)
    assert isinstance(U, float)
    U = s.U_pur_cauchy(pseq, kpur = 20.0)
    assert isinstance(U, float)
    # gradU_pur_cauchy
    g = s.gradU_pur_cauchy(pseq)
    assert isinstance(g, np.ndarray)
    assert g.shape == (n, na)
    g = s.gradU_pur_cauchy(pseq, kpur = 42.0)
    assert isinstance(g, np.ndarray)
    assert g.shape == (n, na)
