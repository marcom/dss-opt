# dss-opt: Dynamics in Sequence Space Optimisation

RNA sequence design for a given target secondary structure.  Sequences
are designed by optimising a design score in a continuous sequence
space, allowing a mixture of bases at each position during
optimisation.

## Citation

Marco C. Matthies, Stefan Bienert, Andrew E. Torda. Dynamics in
Sequence Space for RNA Secondary Structure Design. J. Chem. Theory
Comput., 2012, 8, 10, 3663--3670.
https://doi.org/10.1021/ct300267j

## Running the program

The dynamics in sequence space optimisation is performed with the program `opt-md`.
The `opt-sd` and `opt-sd-gsl` programs perform sequence design by gradient descent alone
and are not described in the paper.
```
# show command-line arguments
./opt-md -h

# design a sequence for the target structure ((((...))))
./opt-md '((((...))))'

# you can often improve the quality of designed sequences by playing with
# the --kneg and --khet parameters, which change the weights of the negative
# design and sequence heterogeneity terms, as well as other parameters
```

## Build instructions

The code can be compiled by running `make` in this directory. You need
the GSL (GNU Scientific Library) header files and library installed
(`libgsl-dev` package in Ubuntu).

Additionally, there is a little utility program called
`rna-ensemble-distance` that uses the ViennaRNA library to compute the
target probability and ensemble defect of a designed sequence with
respect to a given target structure.  You can find it in the
`rna-ensemble-distance-with-ViennaRNA/` directory. The README file in
the
[rna-ensemble-distance-with-ViennaRNA/](./rna-ensemble-distance-with-ViennaRNA/)
directory contains instructions on how to build this utility.


## Energy parameters

The parameters for the RNA nearest neighbour energy model were
converted from ViennaRNA, many thanks to them.  The parameters were
converted from the ViennaRNA-1.8.5 `param/vienna17.par` parameter file
to C code with `scripts/vienna_parse_param.py`. The generated C code
containing the parameters can be found in `nn_param.c`.

## Copyright and license

All code was written by Marco Matthies, Centre for Bioinformatics,
Hamburg and is licensed under the GNU General Public License, version
3 or later.
