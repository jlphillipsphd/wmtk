#include <iostream>
#include "hrrengine.h"
#include "hrrOperators.h"

using namespace std;

int main () {

    HRR myVec = { 1.0, 1.0, 1.0, 1.0, 1.0 };

    HRR addVec = { 1.0, 1.0, 1.0, 1.0, 1.0 };

    myVec = (myVec * 0.5) + addVec;

    for (double d : myVec) {
        cout << d << " ";
    }
    cout << "\n";

    return 0;
}
