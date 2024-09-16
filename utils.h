#include <NTL/ZZ.h>
#include <vector>
#include <complex>


using namespace NTL;

inline ZZ bit_flip(ZZ original, size_t bit){
    ZZ mask = ZZ((ZZ(1) << bit)); // I set the bit to flip. 1ULL is for a one of 64bits
    ZZ res = ZZ(0);

    res =  mask^original; // I flip the bit using xor with the mask.
    if(original<0)
        res = -1*res;
        //std::cout << " bit to change " << bit << " Original value: " <<  original <<  " mask: " << mask << " res " << res << std::endl;
    return res;
}

inline void bit_flip_verbose(ZZ original, size_t bit){
    ZZ mask = ZZ((ZZ(1) << bit)); // I set the bit to flip. 1ULL is for a one of 64bits
    ZZ res = ZZ(0);
    NTL::ZZ two_complement;
    if (original<0)
    {
        // Obtener el valor absoluto del número
        NTL::ZZ abs_num = NTL::abs(original);

        std::cout << "Original value: " <<  original << " bit to change " << bit << " mask: " << mask << std::endl;
        // Crear un número con 1 en la posición más significativa para el tamaño de bits deseado
        // Calcular el complemento a dos
        NTL::ZZ complement = mask - abs_num;

        // El complemento a dos es complement + 1
        two_complement = complement + 1;
        original = two_complement;
        std::cout << "two complement: "<< two_complement << std::endl;
    }
    res =  mask^original; // I flip the bit using xor with the mask.
}

inline double norm2(std::complex<double>* vecInput, std::complex<double>* vecOutput, size_t size){
    double res = 0;
    double diff = 0;
    // Itero sobre el del input por si el del output por construccion quedo mas grande
    for (size_t i=0; i<size; i++)
    {
        diff = vecOutput[i].real() - vecInput[i].real();
        res += pow(diff, 2);
    }
    res = std::sqrt(res/size);
    return res;
}

inline double norm2(double* vecInput, std::complex<double>* vecOutput, size_t size){
    double res = 0;
    double diff = 0;
    // Itero sobre el del input por si el del output por construccion quedo mas grande
    for (size_t i=0; i<size; i++)
    {
        diff = vecOutput[i].real() - vecInput[i];
        res += pow(diff, 2);
    }
    res = std::sqrt(res/size);
    return res;
}

inline void diff_elements(std::complex<double>*  &vecInput, std::complex<double>* &vecOutput, std::vector<uint64_t> &res){
    double diff = 0;
    uint64_t ires = 0;
    size_t size = res.size();
    // Itero sobre el del input por si el del output por construccion quedo mas grande
    for (size_t i=0; i<size; i++)
    {
        diff = abs(vecInput[i].real() - vecOutput[i].real());
        if (diff < 0.000001)
            ires = 0;
        else
            ires = (uint64_t)(diff+0.5); // sumo 0.5 para que sea redondeo y no truncar
        res[i] = ires;
    }
}


