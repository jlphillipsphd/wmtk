#include <iostream>
#include <string>
#include <vector>
#include "WCST.h"

using namespace std;

int main(int argc, char *argv[])
{
    // Sample run: ./wcst 1 "stimuli.dat" A 0 3 3 1 2
    if(argc != 9)
    {
        cerr << "Invalid arguments" << endl;
        return 1;
    }

    int random_seed = atoi(argv[1]);
    string stim_file = argv[2];
    char task_mode = argv[3][0];
    bool encode_dims = argv[4][0]=='1';
    int dims_per_trial = atoi(argv[5]);
    int feats_per_trial = atoi(argv[6]);
    int steps_per_trial = atoi(argv[7]);
    int trials_per_task = atoi(argv[8]);

    WCST wcst(random_seed,stim_file,task_mode,encode_dims,dims_per_trial,feats_per_trial,
      steps_per_trial,trials_per_task);

    // Manual Test
    string a;
    do
    {
        bool correct = false;
        for( int step=0; step < steps_per_trial; step++ )
        {
            wcst.printTask(step);
            wcst.getTrialStep(step).print();
            cin >> a;
            correct = wcst.answer(a,step);
        }
        cout << correct << endl;
        cout << endl;

        if( wcst.total_tasks_completed > 1 )
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
