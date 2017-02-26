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
    int hrr_size = 8;
    double learn_rate = .05;
    double gamma = .9;
    double lambda = .5;
    double epsilon = .01;

    WorkingMemory wm( learn_rate, gamma, lambda, epsilon, hrr_size, 1, 1, seed );
/*
    vector<string> candidateChunks = wm.getCandidateChunks("color+blue+shape+circle+color+red+shape+square");
    for(string chunk : candidateChunks)
        cout << chunk << endl;k
*/
    HRR a = wm.hrrengine.query("a");
    HRR b = wm.hrrengine.query("b");
    wm.hrrengine.printHRRHorizontal(wm.hrrengine.query("a+b"));
    cout << endl;
    wm.hrrengine.printHRRHorizontal(wm.hrrengine.addHRRs(vector<HRR>{a,b}));
    cout << endl;
    wm.hrrengine.printHRRHorizontal(wm.hrrengine.query("a*b"));
    cout << endl;
    wm.hrrengine.printHRRHorizontal(wm.hrrengine.convolveHRRs(a,b));
    cout << endl;
}
