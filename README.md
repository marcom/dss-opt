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

## Build instructions

The code can be compiled by running `make`. You need the GSL (GNU
Scientific Library) installed.  The dynamics in sequence space
optimisation is performed with the program `opt-md`. The `opt-sd` and
`opt-sd-gsl` programs perform sequence design only by gradient descent
and are not described in the paper.

There is a little program that uses the Vienna RNA library to compute
the ensemble defect of a designed sequence to a given target
structure.  You can find it under the directory
`rna-ensemble-distance-with-ViennaRNA/`.  Edit the Makefile in this
subdirectory to point to the directory where you installed the Vienna
RNA package, then run `make`.

## Energy parameters

The parameters for the RNA nearest neighbour energy model were
converted from ViennaRNA, many thanks to them.  The parameters were
converted from the ViennaRNA-1.7 `param/vienna17.par` parameter file
to C code with `scripts/vienna_parse_param.py`. The generated C code
containing the parameters can be found in `nn_param.c`.

## Copyright and license

All code was written by Marco Matthies, Centre for Bioinformatics,
Hamburg and is licensed under the GNU General Public License, version
3 or later.
