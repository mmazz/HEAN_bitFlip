#!/bin/bash


wget -N https://github.com/snucrypto/HEAAN/archive/refs/tags/1.0.tar.gz
tar -xzf 1.0.tar.gz
mv HEAAN-1.0/HEAAN/* ./
rm -rf HEAAN-1.0
cd lib
make all -j16
cd ..
mv *.py *.h *.cpp run
