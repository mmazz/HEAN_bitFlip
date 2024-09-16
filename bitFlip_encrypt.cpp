#include "utils.h"
#include "../src/HEAAN.h"
#include <cmath>
using namespace std;
using namespace NTL;

// Agregue un metodo encryptMsg con semilla ya que si no me iba dando cosas diferentes
// ZZX On output, all coefficients will be integers between 0 and p-1,
//  OpenFHE no usa enteros con signo para el plaintext... aca si
//

#define MAX 5
#define MIN 0
int main(int argc, char *argv[])
{

    long logN = 4;
    if (argc>1)
        logN = std::stol(argv[1]);
    // Es la cantidad de bits del Delta
    // Aca Q es Delta elevado a la cantidad de levels
    long logQ = 35;
    if (argc>2)
        logQ = std::stol(argv[2]);
    long logP = 25;
    if (argc>3)
        logP = std::stol(argv[3]);
    size_t word = logQ + logP + 4;
    long h = pow(2, logN);
    if (h>64)
        h = 64;
    size_t ringDim = (1 << logN);

    long logSlots = logN - 1;
	long slots = pow(2, logSlots);
    std::cout << "logN: " << logN << " Slots: " << slots << " Ringdim: " << ringDim << " slots: " << slots<< std::endl;
    size_t loops = 30;
    string prelog = "logs/log_encrypt/smallEncryptN2_";
    string fileC0 = to_string(logN) + "_" + to_string(logQ) + "_" + to_string(logP)+ "_C0.txt";
    string fileC1 = to_string(logN) + "_" + to_string(logQ) + "_" + to_string(logP)+ "_C01txt";
    ofstream norm2FileC0(prelog+fileC0);
    ofstream norm2FileC1(prelog+fileC1);
    for (size_t k=0; k<loops; k++)
    {
        NTL::ZZ seed;
        seed = k;  // Set your desired seed value
        NTL::SetSeed(seed);
        //complex<double>* vals = EvaluatorUtils::randomComplexArray(slots);
        //double* vals = EvaluatorUtils::randomRealArray(slots);
        double* vals = new double[slots];
        for (uint32_t i=0; i<slots; i++)
            vals[i] = ((double)rand())/RAND_MAX * MAX - MIN;

        std::cout<< "Vals" << std::endl;
        for (int i=0 ; i<(int)slots;i++)
            std::cout<<vals[i] <<" ";
        std::cout<<std::endl;

        // Key Generation
        Context context(logN, logQ);
        SecretKey sk(logN, h);
        Scheme scheme(sk, context);

        Plaintext plain_original = scheme.encode(vals, slots, logP, logQ);
        Plaintext plain = scheme.encode(vals, slots, logP, logQ);
        int count = 0;
        for (size_t i=0; i<ringDim; i++)
           count+= plain_original.mx[i]!=plain.mx[i];
        //std::cout << "Equal plaintexts? " << count  << std::endl;

//        for (size_t i=0; i<ringDim; i++)
//        {
//            auto temp = NTL::NumBits(plain.mx[i]);
//            if(temp>word)
//                word =temp;
//        }
//        std::cout << "Max bits " << word << std::endl;
//        word=40;

        Ciphertext cipher = scheme.encryptMsg(plain, seed);
        complex<double>* dvec = scheme.decrypt(sk, cipher);
        complex<double>* golden_val = scheme.decrypt(sk, cipher);
        double golden_norm = norm2(golden_val, vals, slots);
        //std::cout << "Vals: " << *vals<< " Dec: " << dvec[0].real() << std::endl;

        if (golden_norm<0.1)
        {
            std::string normsC0;
            for (size_t i=0; i<ringDim; i++)
            {
                for (size_t bit=0; bit<word; bit++)
                {
                    cipher = scheme.encryptMsg(plain, seed);
                    cipher.bx[i] = bit_flip(cipher.bx[i], bit);
                    dvec = scheme.decrypt(sk, cipher);
                    double norm = norm2(golden_val, dvec, slots);
                    normsC0.append(to_string(norm));
                    normsC0.append(", ");
                }
            }
            normsC0.pop_back();
            normsC0.pop_back();
            normsC0.append("\n");
            norm2FileC0 << normsC0;

            std::string normsC1;
            for (size_t i=0; i<ringDim; i++)
            {
                for (size_t bit=0; bit<word; bit++)
                {
                    cipher = scheme.encryptMsg(plain, seed);
                    cipher.ax[i] = bit_flip(cipher.ax[i], bit);
                    dvec = scheme.decrypt(sk, cipher);
                    double norm = norm2(golden_val, dvec, slots);
                    normsC1.append(to_string(norm));
                    normsC1.append(", ");
                }
            }
            normsC1.pop_back();
            normsC1.pop_back();
            normsC1.append("\n");
            norm2FileC1 << normsC1;

        }
        else
            std::cout<< "ERROR!" << std::endl;
        delete[] vals;
    }
    norm2FileC0.close();
    norm2FileC1.close();
    return 0;
}
