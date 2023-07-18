import pytest
import dssopt

def test_random_vienna():
    for n in range(0, 15):
        for hpmin in range(0, 4):
            dbn = dssopt.random_vienna(n)
            assert type(dbn) == str
            assert len(dbn) == n
