#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <ctime>
#include "WorkingMemory.h"

using namespace std;

void printWMContents(WorkingMemory& wm);

template <typename T>
T randomItem(T[], int);

template <typename T>
void naiveRandomize(T[], int);

default_random_engine re;

int main(int argc, char** argv) {

    double seed = double(time(NULL));//4;
    re.seed(seed);

    ofstream fout;
    fout.open("results.csv");
    cout << "seed: " << seed << "\n";

    int nColors = 7;
    string colors[] = { "red", "green", "blue", "yellow", "orange", "lime", "brown" };

    // Create a working memory object with the given properties
    WorkingMemory wm(0.1, 0.9, 0.3, 0.01, 64, 1, 1);

    int successfulEpisodes = 0;
    int nEpisodes = 30000;

    wm.resetWeights();

    for ( int i = 0; i <= nEpisodes; i++) {

        //if (i%100 == 0) cout << "Episode: " << i << "\n";

        // Randomize the order of the items in the colors array
        do {
            naiveRandomize(colors, nColors);
        } while (colors[nColors-1] == "red");

        // Initialize the episode with the first color in the array
        wm.initializeEpisode(colors[0], 0.0);
	// cout << colors[0] << " ";
        // printWMContents(wm);

        for ( int j = 1; j < nColors -1; j++ ) {
            wm.step(colors[j], 0.0);
	    // cout << colors[j] << " ";
            // printWMContents(wm);
        }
	// printWMContents(wm);

        // Check working memory contents for the concept "red"
        vector<string> wmc = wm.queryWorkingMemory();
        if ( find( wmc.begin(), wmc.end(), "red" ) != wmc.end() ) {
            // Reward the agent for remembering the concept red
            wm.absorbReward(colors[nColors-1], 1.0);
            //printWMContents(wm);
            successfulEpisodes++;
        } else {
            // Do not reward the agent
            wm.absorbReward(colors[nColors-1], 0.0);
            //printWMContents(wm);
        }

        if (i%1000 == 0) {
            cout << i << ", " << successfulEpisodes << ", " << double(successfulEpisodes) / 1000 << "\n";
            fout << i << ", " << successfulEpisodes << ", " << double(successfulEpisodes) / 1000 << "\n";
            successfulEpisodes = 0;
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

template <typename T>
T randomItem(T items[], int count) {
    return items[re()%count];
}

template <typename T>
void naiveRandomize(T items[], int count) {
    for (int i = 0; i < count; i++) {
        int randomIndex = re()%count;
        T temp = items[i];
        items[i] = items[randomIndex];
        items[randomIndex] = temp;
    }
}
