#include "utils.cpp"
#include "../src/HEAAN.h"
#include <cmath>
using namespace std;
using namespace NTL;

// Agregue un metodo encryptMsg con semilla ya que si no me iba dando cosas diferentes
// ZZX On output, all coefficients will be integers between 0 and p-1,
//  OpenFHE no usa enteros con signo para el plaintext... aca si
//

#define MAX 100
#define MIN 50
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

    string prelog = "logs/log_encode/HEAAN_";
    if (argc>4 && std::stoi(argv[4])>=1)
    {
        slots = slots >> std::stoi(argv[4]);
    }
    std::cout << "logN: " << logN << " logQ: " << logQ << " logP: " << logP << " Ringdim: " << ringDim << " slots: " << slots<< std::endl;
    string fileN2 = "N2_"+to_string(logN) + "_" + to_string(logQ) + "_" + to_string(logP)+ "_" + to_string(slots)+".txt";
    string fileN2_real = "N2_real_"+to_string(logN) + "_" + to_string(logQ) + "_" + to_string(logP)+ "_" + to_string(slots)+".txt";
    string fileDiff = "Diff_"+to_string(logN) + "_" + to_string(logQ) + "_" + to_string(logP)+ "_" + to_string(slots)+".txt";
    ofstream norm2File(prelog+fileN2);
    ofstream norm2_realFile(prelog+fileN2_real);
    ofstream diffFile(prelog+fileDiff);
    double* vals = new double[slots];
    std::ostringstream bufferN2;
    std::ostringstream bufferN2_real;
    std::ostringstream bufferDiff;
    size_t loops = 10;
    if (argc>5)
        loops = std::stoi(argv[5]);
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
    norm2File << logN << ", " << logQ << ", " << logP << ", " << delta << ", " << word << ", " << loops <<std::endl;  // Write the buffer to file
    norm2_realFile << logN << ", " << logQ << ", " << logP << ", " << delta << ", " << word << ", " << loops <<std::endl;  // Write the buffer to file
    for (size_t k=1; k<loops+1; k++)
    {
        std::cout << k << std::endl;
        NTL::ZZ seed;
        seed = k;  // Set your desired seed value
        NTL::SetSeed(seed);
        std::srand(k);
        //complex<double>* vals = EvaluatorUtils::randomComplexArray(slots);
 //       double* vals = EvaluatorUtils::randomRealArray(slots);
        for (uint32_t i=0; i<slots; i++)
            vals[i] = ((double)rand())/RAND_MAX * MAX - MIN;

        std::vector<uint64_t> DIFFVec(slots, 0);

        // Key Generation
        Context context(logN, logQ);
        SecretKey sk(logN, h);
        Scheme scheme(sk, context);

        Plaintext plain_original = scheme.encode(vals, slots, logP, logQ);
        Plaintext plain = scheme.encode(vals, slots, logP, logQ);
        int num_changes = 0;
        for (size_t i=0; i<ringDim; i++)
           num_changes+= plain_original.mx[i]!=plain.mx[i];
        if (num_changes>0)
            std::cout << "Not Equal plaintexts: " << num_changes  << std::endl;

        Ciphertext cipher = scheme.encryptMsg(plain, seed);
        complex<double>* dvec = scheme.decrypt(sk, cipher);
        complex<double>* golden_val = scheme.decrypt(sk, cipher);
        double golden_norm = norm2_real(golden_val, vals, slots);

        if (golden_norm<0.1)
        {
            double count = 0;
            for (size_t i=0; i<ringDim; i++)
            {
                auto plain_coeff = plain.mx[i];
                for (size_t bit=0; bit<word; bit++)
                {
                    if(count>0)
                    {
                        bufferN2 << ", ";
                        bufferN2_real << ", ";
                        bufferDiff << ", ";
                    }
                    plain.mx[i] = bit_flip(plain.mx[i], bit);
                    cipher = scheme.encryptMsg(plain, seed);
                    dvec = scheme.decrypt(sk, cipher);
                    double norm = norm2(golden_val, dvec, slots);
                    double norm_real = norm2_real(golden_val, dvec, slots);
                    diff_elements(golden_val, dvec, DIFFVec);
                    plain.mx[i] = plain_coeff;

                    bufferN2 << norm;
                    bufferN2_real << norm_real;
                    count++;
                    for (size_t k=0; k<slots-1; k++)
                    {
                        bufferDiff << DIFFVec[k] << ", ";
                    }
                    bufferDiff << DIFFVec[slots-1];
                }
            }
            bufferN2 << "\n";
            norm2File << bufferN2.str();  // Write the buffer to file
            bufferN2.str("");        // Clear the buffer
            bufferN2.clear();

            bufferN2_real << "\n";
            norm2_realFile << bufferN2_real.str();  // Write the buffer to file
            bufferN2_real.str("");        // Clear the buffer
            bufferN2_real.clear();

            bufferDiff << "\n";
            diffFile << bufferDiff.str();  // Write the buffer to file
            bufferDiff.str("");        // Clear the buffer
            bufferDiff.clear();
        }
        else
            std::cout<< "ERROR!" << std::endl;
    }
    delete[] vals;
    norm2File.close();
    norm2_realFile.close();
    diffFile.close();

    return 0;
}
