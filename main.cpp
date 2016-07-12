#include <iostream>
#include "WorkingMemory.h"

using namespace std;

int main() {

    string state;

    WorkingMemory workingMemory(0.1, 0.9, 0.25, 0.0, 1024, 3);

    cout << "Working Memory Slots: " << workingMemory.workingMemorySlots() << "\n";

    cout << "Please enter state: ";
    cin >> state;

    workingMemory.state = state;

    vector<string> concepts = workingMemory.getCandidateChunksFromState();

    cout << "Candidate Chunks:\n";
    for ( string concept : concepts ) {
        cout << concept << " ";
        //workingMemory.hrrengine.printHRRHorizontal(workingMemory.hrrengine.query(concept));
        //cout << "size: " << workingMemory.hrrengine.query(concept).size();
        cout << "\n";
    }
    cout << "\n\n";

    cout << "Calculating combinations. Please wait.\n";
    workingMemory.findMostValuableChunks(concepts);

    cout << "\n\nWorking memory contents:\n";
    for (string chunk : workingMemory.workingMemoryChunks) {
        cout << chunk << "\n";
    }

    cout << "\nEND\n\n";

    return 0;
}
