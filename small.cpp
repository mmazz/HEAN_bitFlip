#include "utils.h"
#include "../src/HEAAN.h"
using namespace std;
using namespace NTL;

int main()
{
    long logN = 2;
    size_t ringDim = (1 << logN);
    long h = 4;
    std::cout << "Ringdim: " << ringDim << std::endl;
    long logQ = 60;
    long pBits = 30;
    long logSlots = logN >> 1;
	long slots = (1 << logSlots);
    double* vals = EvaluatorUtils::randomRealArray(slots);
    // Key Generation
    Context context(logN, logQ);
    std::cout<< "Key" <<std::endl;
    SecretKey sk(logN, h);
    std::cout<< "Scheme" <<std::endl;
    Scheme scheme(sk, context);
    std::cout<< "Plain" <<std::endl;
    Plaintext plain = scheme.encode(vals, slots, pBits, logQ);
    Ciphertext cipher = scheme.encryptMsg(plain);
    complex<double>* dvec = scheme.decrypt(sk, cipher);
    complex<double>* golden_val = scheme.decrypt(sk, cipher);
    double golden_norm = norm2(golden_val, vals, slots);

    std::cout << "Norm2: " <<  golden_norm << " Vals: " << *vals<< " Dec: " << dvec[0].real()  << std::endl;
    return 0;
}
