#include "utils.h"
#include "../src/HEAAN.h"
#include <cmath>
using namespace std;
using namespace NTL;

// Agregue un metodo encryptMsg con semilla ya que si no me iba dando cosas diferentes
// ZZX On output, all coefficients will be integers between 0 and p-1,
//  OpenFHE no usa enteros con signo para el plaintext... aca si
//
inline ZZ BigBound(size_t word)
{
    ZZ res = ((ZZ(1) << word) - (ZZ(1) << 9) - ZZ(1));
    return res;
}


int main()
{

    ofstream norm2File("logs/log_encode/smallEncodeN2_30.txt"   );

    long logN = 2;
    long h = 4;
    size_t ringDim = (1 << logN);
    std::cout << "Ringdim: " << ringDim << std::endl;
    long logQ = 30;
    // Es la cantidad de bits del Delta
    // Aca Q es Delta elevado a la cantidad de levels
    long pBits = 10;
    long logSlots = logN >> 1;
	long slots = (1 << logSlots);
    size_t loops = 10;
    for (size_t k=0; k<loops; k++)
    {
        NTL::ZZ seed;
        seed = k;  // Set your desired seed value
        NTL::SetSeed(seed);
        //complex<double>* vals = EvaluatorUtils::randomComplexArray(slots);
        double* vals = EvaluatorUtils::randomRealArray(slots);

        // Key Generation
        Context context(logN, logQ);
        SecretKey sk(logN, h);
        Scheme scheme(sk, context);

        Plaintext plain_original = scheme.encode(vals, slots, pBits, logQ);
        Plaintext plain = scheme.encode(vals, slots, pBits, logQ);
        int count = 0;
        for (size_t i=0; i<ringDim; i++)
           count+= plain_original.mx[i]!=plain.mx[i];

        std::cout << "Equal plaintexts? " << count  << std::endl;
        size_t word = 0;
        for (size_t i=0; i<ringDim; i++)
        {
            auto temp = NTL::NumBits(plain.mx[i]);
            if(temp>word)
                word =temp;
        }
        std::cout << "Max bits " << word << std::endl;
        word=40;

        Ciphertext cipher = scheme.encryptMsg(plain, seed);
        complex<double>* dvec = scheme.decrypt(sk, cipher);
        complex<double>* golden_val = scheme.decrypt(sk, cipher);
        double golden_norm = norm2(golden_val, vals, slots);
        std::cout << "Vals: " << *vals<< " Dec: " << dvec[0].real() << std::endl;

        if (golden_norm<0.1)
        {
            std::string norms;
            for (size_t i=0; i<ringDim; i++)
            {
                auto plain_coeff = plain.mx[i];
                for (size_t bit=0; bit<word; bit++)
                {
                    plain.mx[i] = bit_flip(plain.mx[i], bit);
                    cipher = scheme.encryptMsg(plain, seed);
                    dvec = scheme.decrypt(sk, cipher);
                    double norm = norm2(golden_val, dvec, slots);
                    norms.append(to_string(norm));
                    norms.append(", ");
                    plain.mx[i] = plain_coeff;
                }
            }
            norms.pop_back();
            norms.pop_back();
            norms.append("\n");
            norm2File << norms;
        }
        else
            std::cout<< "ERROR!" << std::endl;
    }
    norm2File.close();

    return 0;
}
