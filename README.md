# HEAN bit flip

## Cambios

- src/Scheme.cpp
    - Agregue una funcion Ciphertext Scheme::encryptMsg(Plaintext& msg, ZZ seed) {
        -   Que es igual a la origianl pero agrega:  NTL::SetSeed(seed);


