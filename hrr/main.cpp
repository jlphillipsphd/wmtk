#include <iostream>
#include "hrrengine.h"

using namespace std;

int main(int argc, char** argv) {

    string concept;

    cout << "Enter the concept to unpack: ";
    cin >> concept;

    HRREngine hrrengine(1024);

    vector<string> unpackedConcepts = hrrengine.unpack(concept);

    cout << "Unpacked Concepts:\n";
    for ( string concept : unpackedConcepts ) {
        cout << "\t" << concept << "\n";
    }

    

    cout << "END\n\n";

    return 0;
}
