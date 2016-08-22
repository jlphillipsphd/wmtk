#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <ctime>
#include "WorkingMemory.h"

using namespace std;

void printWMContents(WorkingMemory& wm);

string concept(int index);

template <typename T>
void naiveRandomize(T[], int);

string colors[] = { "red", "blue", "green", "orange",
                    "purple", "violet", "brown", "teal",
                    "yellow", "scarlet", "ochre", "lime"};

string objects[] = {"ball", "nutcracker", "table", "car",
                    "mouse", "phone", "pencil", "ring" ,
                    "knife", "bottle", "key", "shirt"};

int main(int argc, char** argv) {
    srand(time(0));

    int nConcepts = 7;

    // Create a working memory object with the given properties
    WorkingMemory wm(0.1, 0.9, 0.3, 0.01, 64, 1);

    int successfulEpisodes = 0;
    int nEpisodes = 100000;

    wm.resetWeights();

    for ( int i = 0; i <= nEpisodes; i++) {

        //if (i%100 == 0) cout << "Episode: " << i << "\n";

        // Randomize the order of the items in the colors and objects arrays
        do {
            naiveRandomize(colors, nConcepts);
            naiveRandomize(objects, nConcepts);
        } while (colors[nConcepts-1] == "red");

        // Initialize the episode with the first color in the array
        wm.initializeEpisode(concept(0), 0.0);
        //cout << "concept(0): " << concept(0) << endl;


        for ( int j = 1; j < nConcepts -1; j++ ) {
            wm.step(concept(j), 0.0);
            //cout << "concept(" << j << "): " << concept(j) << endl;
            // printWMContents(wm);
        }
	    // printWMContents(wm);

        // Check working memory contents for the concept "red"
        vector<string> wmc = wm.queryWorkingMemory();
        if ( find( wmc.begin(), wmc.end(), "red" ) != wmc.end() ) {
            // Reward the agent for remembering the concept red
            wm.absorbReward(concept(nConcepts-1), 1.0);
            //cout << "concept(" << nConcepts-1 << "): " << concept(nConcepts-1) << endl;
            //printWMContents(wm);
            successfulEpisodes++;
        } else {
            // Do not reward the agent
            wm.absorbReward(concept(nConcepts-1), 0.0);
            //cout << "concept(" << nConcepts-1 << "): " << concept(nConcepts-1) << endl;
            //printWMContents(wm);
        }

        if (i%1000 == 0) {
            cout << i << ", " << successfulEpisodes << ", " << double(successfulEpisodes) / 1000 << "\n";
            successfulEpisodes = 0;
        }
    }

    return 0;
}

string concept(int index) {
    return (colors[index] + "*" + objects[index]);
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
