# HEAN bit flip


## Install

- Download https://github.com/snucrypto/HEAAN/archive/refs/tags/1.0.tar.gz
- Go to directory and uncompres: tar -xvf HEAAN-1.0.tar.gz
- Install NTL: yay -S ntl
- cd HEAN-1.0/HEAN/lib
- make all -j16

## Useaget

- cd ../run
- make bitFlip -j16
- ./bitFlip
- python bitFlip.py


## Cambios

- src/Scheme.cpp
    - Agregue una funcion Ciphertext Scheme::encryptMsg(Plaintext& msg, ZZ seed) {
        -   Que es igual a la origianl pero agrega:  NTL::SetSeed(seed);


