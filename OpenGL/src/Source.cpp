#include <iostream>
#include <random>

#include "utility/App.hpp"
#include "utility/ComputeShader.hpp"

#ifdef __cplusplus
extern "C" {
#endif

    __declspec(dllexport) unsigned int NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#ifdef __cplusplus
}
#endif


int main(void)
{
    App::Initialize();

    ComputeShader compute("resources/shaders/compute/sort.comp");

    
}
