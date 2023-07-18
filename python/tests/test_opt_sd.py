import pytest
import dssopt

@pytest.mark.parametrize(
    "kwargs, expected",
    [
        ({'seed': 42, 'target_dbn': '(((...)))'}, 'GGCAAAGCC'),
        ({'seed': 11, 'target_dbn': '(((...)))'}, 'CGCAAAGCG'),
        ({'seed': 12, 'target_dbn': '((((((((...))).))).(((....)))))'}, 'GCGCUAGCAAAGCUAAGCAGUCGAAAGACGC'),
        ({'seed': 42, 'target_dbn': '((((((((...))).))).(((....)))))'}, 'GCGCCGGCAAAGCCAGGCAGGCGAAAGCCGC'),
    ]
)
def test_opt_sd(kwargs, expected):
    print(kwargs)
    result = dssopt.opt_sd(**kwargs)
    assert result == expected
