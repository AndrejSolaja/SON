

#include "Simulation.h"
#include "micro-ecc/uECC.h"
#include <iostream>
#include <vector>
#include <cstring>

int main()
{
    // Hardcoded atm, read from arguments later
    int N = 3;
    int m = 0;

    // Main simulation
    Simulation simulation(N, m);

    simulation.init();
    simulation.start();

    

}

