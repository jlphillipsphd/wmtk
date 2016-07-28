#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <ctime>
#include "WorkingMemory.h"

using namespace std;

void printWMContents(WorkingMemory& wm);
string randomItem(string[], int);

int main(int argc, char** argv) {
    srand(time(0));

    ofstream fout;
    fout.open("results.csv");

    string colors[] = { "red", "green", "blue", "yellow", "orange", "lime", "brown" };

    // Create a working memory object with the given properties
    WorkingMemory wm(0.75, 0.9, 0.0, 0.0, 256, 2);

    wm.initializeEpisode(randomItem(colors, 7));
    //printWMContents(wm);

    for ( int i = 0; i < 200; i++) {
        bool done = false;
        int numberOfStepsToGoal = 0;
        while (!done) {
            numberOfStepsToGoal++;
            wm.step(randomItem(colors, 7));
            //printWMContents(wm);

            vector<string> wmc = wm.queryWorkingMemory();
            if ( find( wmc.begin(), wmc.end(), "red" ) != wmc.end() ) {
                wm.absorbReward(randomItem(colors, 7));
                done = true;
                //cout << "Done!\n\n";
                cout << numberOfStepsToGoal << /*"," << <<*/ "\n";
                fout << numberOfStepsToGoal << /*"," << <<*/ "\n";
            }
            if ( numberOfStepsToGoal > 199 ) {
                done = true;
                cout << numberOfStepsToGoal << /*"," << <<*/ "\n";
                fout << numberOfStepsToGoal << /*"," << <<*/ "\n";
            }
        }
    }

    fout.close();

    return 0;
}

void printWMContents(WorkingMemory& wm) {

    vector<string> wmContents = wm.queryWorkingMemory();

    for (string chunk : wmContents) {
        cout << chunk << " | ";
    }
    cout << "\n";

    return;
}

string randomItem(string items[], int count) {
    return items[rand()%count];
}
