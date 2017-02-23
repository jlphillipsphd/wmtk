#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "WCST.h"

using namespace std;

WCST::Trial::Trial(string s, vector<string> a) : state(s), actions(a) {}

WCST::WCST
    ( 
        int random_seed, 
        string file_path,
        char task_mode,
        bool encode_dimensions,
        int dimensions_per_trial,
        int features_per_trial,
        int trials_to_completion
    ) : 
        re(random_seed), 
        task_mode(task_mode), 
        encode_dimensions(encode_dimensions), 
        dimensions_per_trial(dimensions_per_trial), 
        features_per_trial(features_per_trial), 
        dimension_picker(0,dimensions_per_trial-1), 
        feature_picker(0,features_per_trial-1),
        trials_to_completion(trials_to_completion)
{
    // Set appropriate task
    if( task_mode == 'D' )
        dimension_task = true;

    // Read in the stimuli
    ifstream stim_file;
    string line;

    stim_file.open( file_path );
    while( stim_file >> line )
    {
        features.push_back(vector<string>());
        istringstream iss( line );
        string stim;
        getline( iss, stim, '=' );
        dimensions.push_back(stim);
        while( getline( iss, stim, ',' ) )
            features[features.size()-1].push_back(stim);
    }
    stim_file.close();

    setTask();
}

// Print trial details
void WCST::Trial::print()
{
    cout << "State: " << state << endl;
    cout << "Actions: ";
    for( string a : actions )
        cout << a << " ";
    cout << endl;
}

// Make a trial based on the current task
WCST::Trial WCST::getTrial()
{
    string state = "";
    vector<string> actions;

    // A trial must have only one stimuli set that matches the task
    for( int d = 0; d < dimensions_per_trial; d++ )
    {
        string rep = "";
        if( encode_dimensions )
            rep += dimensions[d] + "+";

        // If this task is feature specific, then we have to include that feature 
        if( !dimension_task && d == task_d )
            rep += features[d][task_f] + "+";
        else
            rep += features[d][feature_picker(re)] + "+";

        state += rep;
        actions.push_back(rep.substr(0,rep.length()-1));
    }
    
    return Trial(state.substr(0,state.length()-1),actions);
}

// Print task details
void WCST::printTask()
{
    cout << "Current Task: " << dimensions[task_d] << " ";
    if( !dimension_task )
        cout << features[task_d][task_f];
    cout << endl;
}

// Print general stats
void WCST::printStats()
{
    cout << "Total Tasks: " << total_tasks_completed << endl;
    cout << "Total Trials: " << total_trials_completed << endl;
    cout << "Correct Trials: " << total_correct_trials << endl;
}

// Change the current task
void WCST::setTask()
{
    // We only need to change this if we're doing all tasks
    if( task_mode == 'A' )
        dimension_task = !dimension_task;

    // If we're in dimension only task mode then make sure we get a new dimension
    int new_task_d = dimension_picker(re);
    if( dimension_task )
    {
        while( new_task_d == task_d )
            new_task_d = dimension_picker(re);
    }
    // If we're in feature only task mode then make sure we get a new feature
    int new_task_f = feature_picker(re);
    if( !dimension_task )
    {
        while( new_task_f == task_f )
            new_task_f = feature_picker(re);
    }

    task_d = new_task_d;
    task_f = new_task_f;
}

// Return whether the answer is correct; update tallies and switch tasks if needed
bool WCST::answer(string response)
{
    // Determine correctness - this is always based on feature since we can't assume
    // that dimensions were encoded
    bool correct = false;
    if( dimension_task )
        correct = (find(features[task_d].begin(), features[task_d].end(), response) != features[task_d].end());
    else
        correct = (response.find(features[task_d][task_f]) != string::npos);

    // Update tallies
    total_trials_completed++;
    if( correct )
    {
        total_correct_trials++;
        successive_correct_trials++;
    }
    else
        successive_correct_trials = 0;

    // Switch tasks if needed
    if( successive_correct_trials == trials_to_completion )
    {
        total_tasks_completed++;
        successive_correct_trials = 0;
        setTask();
    }
    
    return correct;
}
