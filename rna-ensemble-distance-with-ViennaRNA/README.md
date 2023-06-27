# `rna-ensemble-distance`: calculate designed sequence properties

This is a small utility program called `rna-ensemble-distance` that
can be used to compute ensemble properties of a designed sequence with
ViennaRNA-1.8.5.  The program takes a sequence and an optional target
structure as input.

## Running `rna-ensemble-distance`

```
./rna-ensemble-distance GGGAAACCC '(((...)))'

# target_p is the probability of the target structure in the ensemble
# target_ens_defect is the ensemble defect of the target structure
```

## Building instructions

```
make
```

Tested with gcc-11.3.0 on Ubuntu 22.04. The Makefile by default
downloads and builds ViennaRNA-1.8.5 as it needs a small patch to
build on newer compilers (see `build-ViennaRNA-1.8.5.bash` for
details).

In case you already have ViennaRNA-1.8.5 installed (header files and
static library), you can use:

```
make VIENNA=/path/to/installed/ViennaRNA-1.8.5
```
