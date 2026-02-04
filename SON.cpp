

#include "Simulation.h"
#include "micro-ecc/uECC.h"
#include <iostream>
#include <vector>
#include <cstring>
#include "CertificationBody.h"

int main()
{

    // Hardcoded atm, read from arguments later
    int N = 4;
    int m = 1;
    bool isGeneralLoyal = true;

    // Main simulation
    Simulation simulation(N, m, isGeneralLoyal);

    simulation.init();
    simulation.start();
    simulation.end();




}

