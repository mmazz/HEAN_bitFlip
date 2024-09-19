#!/bin/bash

echo "Initializing"
wget -N https://github.com/snucrypto/HEAAN/archive/refs/tags/1.0.tar.gz
tar -xzf 1.0.tar.gz
mv HEAAN-1.0/HEAAN/* ./
rm -rf HEAAN-1.0
echo ""
echo "Patching"
patch_file="changes.diff"

# Verificar si el archivo de parche existe
if [[ ! -f "$patch_file" ]]; then
  echo "El archivo de parche '$patch_file' no existe."
  exit 1
fi

cd src
patch -p1 < ../changes.diff
echo "Patching compleate"
echo ""
echo "Building"
cd ../lib
make all -j16
echo "Building campaign"
cd ../campaigns
make
