/*
Author: Mike Jovanovich
Date: 2/1/2017
Description: This program uses TD learning to solve the WCST
*/
#include <iostream>
#include <string>
#include <vector>
#include "WCST.h"
#include "WorkingMemory.h"

using namespace std;

int main(int argc, char *argv[])
{
    // Sample run: ./wcst_wm 1 "stimuli.dat" F 0 3 3 1 8
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

    // Define WM parameters
    int hrr_size = 1024;
    double learn_rate = .1;
    double gamma = .5;
    double lambda = .0;
    double epsilon = .01;
    double bias = 1.0;
    int wm_chunks = 1;
    int max_concepts_per_chunk = 2;

    WorkingMemory wm( learn_rate, gamma, lambda, epsilon, hrr_size, bias, wm_chunks, random_seed, 
      max_concepts_per_chunk );

    // Whenever trials_per_task number of tasks are completed successively
    // total_tasks_completed will increment, and a new task will begin
    //int cur_task = 0;
    //int task_steps = 0;
    string a;
    WCST::TrialStep t;
    while( wcst.total_tasks_completed < 10 )
    {
        // Each trial in the WCST is an episode for the WMTK
        wm.initializeEpisode();

        vector<string> actions;

        // Each trial is further broken down into steps_per_trial steps
        for( int step = 0; step < steps_per_trial-1; step++ )
        {
            t = wcst.getTrialStep( step );
            //a = wm.step( t.state, t.actions, 0.0 );
            //a = wm.step( t.state + "*_ts_" + to_string(step), t.actions, 0.0 );
            a = wm.step( t.state + "+_ts_" + to_string(step), t.actions, 0.0 );
            wcst.answer( a, step );
        }

        // For the last step in the trial, absorb reward
        t = wcst.getTrialStep( steps_per_trial-1 );
        //a = wm.step( t.state, t.actions, 0.0 );
        //a = wm.step( t.state + "*_ts_" + to_string(steps_per_trial-1), t.actions, 0.0 );
        a = wm.step( t.state + "+_ts_" + to_string(steps_per_trial-1), t.actions, 0.0 );
        if( wcst.answer( a, steps_per_trial-1 ) )
            wm.absorbReward(1.0);
        else
            wm.absorbReward(0.0);

/*
        if( wcst.total_trials_completed % 100 == 0 )
        {
            cout << "Trials: " << wcst.total_trials_completed << endl;
            cout << "Tasks: " << wcst.total_tasks_completed << endl;
            wm.printWMContents();
        }
*/
/*
        task_steps++;
        if( wcst.total_tasks_completed != cur_task )
        {
            cout << task_steps << endl;
            cur_task = wcst.total_tasks_completed;
            task_steps = 0;
        }
*/
    }

    wcst.printStats();
}
