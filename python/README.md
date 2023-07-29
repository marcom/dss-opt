# Dss-Opt python bindings

## Running tests

Installing with tests
```sh
pip install python/[test]
```

Running tests
```sh
pytest
```

## Regenerate ctypes bindings with `ctypesgen`

There is a special `setup.py` command to generate the ctypes python
binding code automatically from the C header files:

```sh
python setup.py ctypesgen
```

You can also generate the ctypes binding code manually if this fails
for whatever reason.  Install `ctypesgen` with `pip install ctypesgen`
and run:

```sh
ctypesgen -o dssopt/pylibdssopt.py -l libdssopt.so ../*.h ../ctools/random.h
```
