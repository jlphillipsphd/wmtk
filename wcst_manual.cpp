#include <iostream>
#include <string>
#include <vector>
#include "WCST.h"

using namespace std;

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        cerr << "Invalid arguments" << endl;
        return 1;
    }

    WCST wcst(atoi(argv[1]),argv[2],'A',false,3,3,2);

    // Manual Test
    string a;
    do
    {
        //wcst.printTask();
        wcst.getTrial().print();
        cin >> a;
        cout << wcst.answer(a) << endl;
        cout << endl;

        if( wcst.total_tasks_completed > 0 )
            break;   
    } while ( cin );
    wcst.printStats();

/*
    // Trial setup testing
    for( int i=0; i<10; ++i )
    {
        wcst.printTask();
        wcst.getTrial().print();
        cout << endl;

        // This would normally not be public
        wcst.setTask();
    }
*/
}
