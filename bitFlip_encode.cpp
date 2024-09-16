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
    //slots = slots >> 1;
    std::cout << "logN: " << logN << " Slots: " << slots << " Ringdim: " << ringDim << " slots: " << slots<< std::endl;
    string prelog = "logs/log_encode/smallEncodeN2_";
    string file = to_string(logN) + "_" + to_string(logQ) + "_" + to_string(logP)+ ".txt";
    ofstream norm2File(prelog+file);
    size_t loops = 30;
    for (size_t k=0; k<loops; k++)
    {
        NTL::ZZ seed;
        seed = k;  // Set your desired seed value
        NTL::SetSeed(seed);
        //complex<double>* vals = EvaluatorUtils::randomComplexArray(slots);
 //       double* vals = EvaluatorUtils::randomRealArray(slots);
        double* vals = new double[slots];
        for (uint32_t i=0; i<slots; i++)
            vals[i] = ((double)rand())/RAND_MAX * MAX - MIN;
     //   for (int i=0 ; i<(int)slots;i++)
     //       std::cout<<vals[i] <<" ";
     //   std::cout<<std::endl;

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
        delete[] vals;
    }
    norm2File.close();

    return 0;
}
