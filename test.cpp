#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include "hrr/hrrengine.h"
#include "hrr/hrrOperators.h"
#include "WorkingMemory.h"

int main(int argc, char *argv[])
{
    int seed = atoi(argv[1]);

    // Define WM parameters
    int hrr_size = 1024;
    double learn_rate = .05;
    double gamma = .9;
    double lambda = .5;
    double epsilon = .01;

    WorkingMemory wm( learn_rate, gamma, lambda, epsilon, hrr_size, 1, 1, seed );
    vector<string> candidateChunks = wm.getCandidateChunks("A");
    for(string chunk : candidateChunks)
        cout << chunk << endl;
}
