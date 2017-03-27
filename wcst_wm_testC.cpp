/*
Author: Mike Jovanovich
Date: 2/1/2017
Description: This program uses TD learning to solve the WCST
*/
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "WCST.h"
#include "WorkingMemory.h"

using namespace std;

/****************************************************************************
  MUST SET suppress_task_switching FLAG IN WCST.H TO TRUE FOR THIS TEST TO WORK
****************************************************************************/

int main(int argc, char *argv[])
{
    // Sample run: ./wcst_wm_testC 1 "stimuli.dat" D 0 3 3 1 8
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

/****************************************************************************
  MUST SET suppress_task_switching FLAG IN WCST.H TO TRUE FOR THIS TEST TO WORK
****************************************************************************/

    // Define WM parameters
    int hrr_size = 1024;
    double learn_rate = .1;
    double gamma = .5;
    double lambda = .0;
    double epsilon = .05;
    double bias = 0.0;  // Optimistic critic causes poor generalization!
    int wm_chunks = 1;
    int max_concepts_per_chunk = 2;

    WorkingMemory wm( learn_rate, gamma, lambda, epsilon, hrr_size, bias, wm_chunks, random_seed, 
      max_concepts_per_chunk );

    // Whenever trials_per_task number of tasks are completed successively
    // total_tasks_completed will increment, and a new task will begin
    string a;
    WCST::TrialStep t;
    while( wcst.total_tasks_completed < 1 )
    {
        // Each trial in the WCST is an episode for the WMTK
        wm.initializeEpisode();

        // For the last step in the trial, absorb reward
        t = wcst.getTrialStep( steps_per_trial-1 );
        a = wm.step( t.state, t.actions, 0.0 );
        if( wcst.answer( a, steps_per_trial-1 ) )
            wm.absorbReward(1.0);
        else
            wm.absorbReward(0.0);
    }
    //wcst.printStats();
    //cout << a << endl;

    // Reset stimuli in WCST
    wcst.dimensions = vector<string>();
    wcst.features = vector<vector<string> >();

    // Read in new stimuli file
    ifstream stim_file2;
    string line;
    stim_file2.open("novel_stimuli.dat");
    while( stim_file2 >> line )
    {
        wcst.features.push_back(vector<string>());
        istringstream iss( line );
        string stim;
        getline( iss, stim, '=' );
        wcst.dimensions.push_back(stim);
        while( getline( iss, stim, ',' ) )
            wcst.features[wcst.features.size()-1].push_back(stim);
    }
    stim_file2.close();

    // Set epsilon to zero
    wm.critic.epsilon = 0.0;

    // Do one novel trial
    wm.initializeEpisode();
    t = wcst.getTrialStep( steps_per_trial-1 );
    a = wm.step( t.state, t.actions, 0.0 );
    if( wcst.answer( a, steps_per_trial-1 ) )
        cout << 1 << endl;
    else
        cout << 0 << endl;

    //debug
    //cout << t.state << endl;
    //cout << a << endl;
}
