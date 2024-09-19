#include "utils.cpp"
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
    long h = pow(2, logN);
    if (h>64)
        h = 64;

    size_t ringDim = (1 << logN);

    long logSlots = logN - 1;
	long slots = pow(2, logSlots);

    std::cout << "logN: " << logN << " Slots: " << slots << " Ringdim: " << ringDim << " slots: " << slots<< std::endl;
    string prelog = "logs/log_encrypt/smallEncryptN2_";
    if (argc>5 && std::stoi(argv[5])>=1)
    {
        slots = slots >> std::stoi(argv[5]);
        prelog = "logs/log_encrypt/smallEncode_smallBatch";
    }

    string fileC0 = to_string(logN) + "_" + to_string(logQ) + "_" + to_string(logP)+ "_C0.txt";
    string fileC1 = to_string(logN) + "_" + to_string(logQ) + "_" + to_string(logP)+ "_C1.txt";
    ofstream norm2FileC0(prelog+fileC0);
    ofstream norm2FileC1(prelog+fileC1);
    std::ostringstream buffer;
    size_t loops = 10;
    if (argc>4)
        loops = std::stoi(argv[4]);
    int max = 0;
    ZZ delta = ZZ(0);
    ZZ temp_delta = ZZ(0);
    for (size_t k=1; k<loops+1; k++)
    {
        NTL::ZZ seed;
        seed = k;  // Set your desired seed value
        NTL::SetSeed(seed);
        std::srand(k);
        //complex<double>* vals = EvaluatorUtils::randomComplexArray(slots);
        //double* vals = EvaluatorUtils::randomRealArray(slots);
        double* vals = new double[slots];
        for (uint32_t i=0; i<slots; i++)
            vals[i] = ((double)rand())/RAND_MAX * MAX - MIN;

        // Key Generation
        Context context(logN, logQ);
        SecretKey sk(logN, h);
        Scheme scheme(sk, context);

        Plaintext plain = scheme.encode(vals, slots, logP, logQ);
        temp_delta = context.logQ + logP;
        delta += temp_delta;
        int local_max = 0;
        for (size_t i=0; i<ringDim; i++)
        {
            auto temp = NTL::NumBits(plain.mx[i]);
            if(temp>local_max)
                local_max=temp;

        }
        if(local_max>max)
            max = local_max;
    }
    delta = delta/ loops;
    std::cout << "Delta " << delta << std::endl;
    uint16_t word = max;
    std::cout << "Max bits " << word << std::endl;
    std::cout << std::endl;
    norm2FileC0 << logN << ", " << logQ << ", " << logP << ", " << delta << ", " << word << ", " << loops <<std::endl;  // Write the buffer to file
    for (size_t k=1; k<loops+1; k++)
    {
        NTL::ZZ seed;
        seed = k;  // Set your desired seed value
        NTL::SetSeed(seed);
        std::srand(k);
        //complex<double>* vals = EvaluatorUtils::randomComplexArray(slots);
        //double* vals = EvaluatorUtils::randomRealArray(slots);
        double* vals = new double[slots];
        for (uint32_t i=0; i<slots; i++)
            vals[i] = ((double)rand())/RAND_MAX * MAX - MIN;

        // Key Generation
        Context context(logN, logQ);
        SecretKey sk(logN, h);
        Scheme scheme(sk, context);

        Plaintext plain = scheme.encode(vals, slots, logP, logQ);

        std::cout << std::endl;
        Ciphertext cipher = scheme.encryptMsg(plain, seed);
        int local_max = 0;
        for (size_t i=0; i<ringDim; i++)
        {
            auto temp = NTL::NumBits(cipher.ax[i]);
            if(temp>local_max)
                local_max=temp;

            temp = NTL::NumBits(cipher.bx[i]);
            if(temp>local_max)
                local_max=temp;
        }

        if(local_max>max)
            max = local_max;
    }

    std::cout << "Max bits " << max << std::endl;
    std::cout << std::endl;
    for (size_t k=1; k<loops+1; k++)
    {
        NTL::ZZ seed;
        seed = k;  // Set your desired seed value
        NTL::SetSeed(seed);
        std::srand(k);
        //complex<double>* vals = EvaluatorUtils::randomComplexArray(slots);
        //double* vals = EvaluatorUtils::randomRealArray(slots);
        double* vals = new double[slots];
        for (uint32_t i=0; i<slots; i++)
            vals[i] = ((double)rand())/RAND_MAX * MAX - MIN;
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


        Ciphertext cipher = scheme.encryptMsg(plain, seed);

        complex<double>* dvec = scheme.decrypt(sk, cipher);
        complex<double>* golden_val = scheme.decrypt(sk, cipher);
        double golden_norm = norm2(golden_val, vals, slots);
        //std::cout << "Vals: " << *vals<< " Dec: " << dvec[0].real() << std::endl;

        if (golden_norm<0.1)
        {
            double count = 0;
            for (size_t i=0; i<ringDim; i++)
            {
                for (size_t bit=0; bit<max; bit++)
                {
                    if(count>0)
                        buffer << ", ";
                    cipher = scheme.encryptMsg(plain, seed);
                    cipher.bx[i] = bit_flip(cipher.bx[i], bit, max);
                    dvec = scheme.decrypt(sk, cipher);
                    double norm = norm2(golden_val, dvec, slots);
                    buffer << norm;
                    count++;
                }
            }
            buffer << "\n";
            norm2FileC0 << buffer.str();  // Write the buffer to file
            buffer.str("");        // Clear the buffer
            buffer.clear();

            count = 0;
            for (size_t i=0; i<ringDim; i++)
            {
                for (size_t bit=0; bit<max; bit++)
                {
                    if(count>0)
                        buffer << ", ";
                    cipher = scheme.encryptMsg(plain, seed);
                    cipher.ax[i] = bit_flip(cipher.ax[i], bit, max);
                    dvec = scheme.decrypt(sk, cipher);
                    double norm = norm2(golden_val, dvec, slots);
                    buffer << norm;
                    count++;
                }
            }
            buffer << "\n";
            norm2FileC1 << buffer.str();  // Write the buffer to file
            buffer.str("");        // Clear the buffer
            buffer.clear();
        }
        else
            std::cout<< "ERROR!" << std::endl;
        delete[] vals;
    }
    norm2FileC0.close();
    norm2FileC1.close();
    return 0;
}
