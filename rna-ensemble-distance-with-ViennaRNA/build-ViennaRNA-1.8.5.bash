#!/bin/bash

set -o errexit
set -o nounset

wget -nc https://www.tbi.univie.ac.at/RNA/download/sourcecode/1_8_x/ViennaRNA-1.8.5.tar.gz
tar xzf ViennaRNA-1.8.5.tar.gz
cd ViennaRNA-1.8.5
patch -p1 < ../fix-ViennaRNA-1.8.5-compile.patch
./configure --prefix=$(pwd)/../install-ViennaRNA-1.8.5 --without-perl --without-cluster --without-forester --without-kinfold
make
make install
cd ..
