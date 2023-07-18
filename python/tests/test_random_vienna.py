import pytest
import dssopt

def test_random_vienna():
    for n in range(10,12):
        dbn = dssopt.random_vienna(n)
        assert type(dbn) == str
        assert len(dbn) == n
