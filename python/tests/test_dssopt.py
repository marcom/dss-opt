import pytest
import dssopt

@pytest.mark.parametrize(
    "kwargs, expected",
    [
        # same tests as in unittest/test-run_md.c
        ({'seed': 42, 'target_dbn': '(((...)))'}, 'GGGAAACCC'),
        ({'seed': 12, 'target_dbn': '((((((((...))).))).(((....)))))'}, 'GCGCGGCCAAAGGCACGCAGGGGAAACCCGC'),
        ({'seed': 42, 'target_dbn': '((((((((...))).))).(((....)))))'}, 'GCGGGCUGAAACAGACCCAGCCGAAAGGCGC'),
        ({'seed': 42, 'target_dbn': '((((((((...))).))).(((....)))))',
          'seq_constraints_hard': 'NNCAANGCNUCGNNNNNGNNNNNNGGNNNNN'}, 'ACCAAGGCAUCGCCAUUGAGGCCAGGGCCGU'),
    ]
)
def test_opt_md(kwargs, expected):
    print(kwargs)
    result = dssopt.opt_md(**kwargs)
    assert result == expected
