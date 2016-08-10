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

int main(int argc, char** argv) {
    srand(time(0));

    ofstream fout;
    fout.open("results.csv");

    string colors[] = { "red", "green", "blue", "yellow", "orange", "lime", "brown" };

    // Create a working memory object with the given properties
    WorkingMemory wm(0.1, 0.9, 0.3, 0.05, 64, 1);

    int successfulEpisodes = 0;

    wm.resetWeights();

    for ( int i = 0; i <= 100000; i++) {

        //if (i%100 == 0) cout << "Episode: " << i << "\n";

        // Randomize the order of the items in the colors array
        naiveRandomize(colors, 7);

        // Initialize the episode with the first color in the array
        wm.initializeEpisode(colors[0], 0.0);
	// cout << colors[0] << " ";
        // printWMContents(wm);

        for ( int j = 1; j < 7; j++ ) {
            wm.step(colors[j], 0.0);
	    // cout << colors[j] << " ";
            // printWMContents(wm);
        }
	// printWMContents(wm);
	
        // Check working memory contents for the concept "red"
        vector<string> wmc = wm.queryWorkingMemory();
        if ( find( wmc.begin(), wmc.end(), "red" ) != wmc.end() ) {
            // Reward the agent for remembering the concept red
            wm.absorbReward("I", 1.0);
            //printWMContents(wm);
            successfulEpisodes++;
        } else {
            // Do not reward the agent
            wm.absorbReward("I", 0.0);
            //printWMContents(wm);
        }

        if (i%1000 == 0) {
            cout << i << ", " << successfulEpisodes << "\n";
            fout << i << ", " << successfulEpisodes << "\n";
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
    return items[rand()%count];
}

template <typename T>
void naiveRandomize(T items[], int count) {
    for (int i = 0; i < count; i++) {
        int randomIndex = rand()%count;
        T temp = items[i];
        items[i] = items[randomIndex];
        items[randomIndex] = temp;
    }
}
