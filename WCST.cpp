/*
Author: Mike Jovanovich
Date: 2/1/2017
Description: This library is meant to povide a common interface for several tasks related to the
Wisconsin Card Sort Test.
*/
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "WCST.h"

using namespace std;

WCST::TrialStep::TrialStep(){}
WCST::TrialStep::TrialStep(string s, vector<string> a) : state(s), actions(a) {}

WCST::WCST
    ( 
        int random_seed, 
        string file_path,
        char task_mode,
        bool encode_dimensions,
        int dimensions_per_trial,
        int features_per_trial,
        int steps_per_trial,
        int trials_per_task
    ) : 
        re(random_seed), 
        task_mode(task_mode), 
        encode_dimensions(encode_dimensions), 
        dimensions_per_trial(dimensions_per_trial), 
        features_per_trial(features_per_trial), 
        dimension_picker(0,dimensions_per_trial-1), 
        feature_picker(0,features_per_trial-1),
        steps_per_trial(steps_per_trial),
        trials_per_task(trials_per_task),
        task_d(steps_per_trial),
        task_f(steps_per_trial)
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

    setTasks();
}

// Print trial details
void WCST::TrialStep::print()
{
    cout << "State: " << state << endl;
    cout << "Actions: ";
    for( string a : actions )
        cout << a << " ";
    cout << endl;
}

// Make a trial based on the current task
WCST::TrialStep WCST::getTrialStep(int step)
{
    string connector = use_conjunctive ? "*" : "+";
    string state = "";
    vector<string> actions;

    // This string is used to make an HRR that differentiates between
    // trial steps; to turn it off set it to empty string
    string step_hrr_str = "";
    //string step_hrr_str = "_ts_" + to_string(step) + "*";

    // A trial must have only one stimuli set that matches the task
    for( int d = 0; d < dimensions_per_trial; d++ )
    {
        string rep = "";
        if( encode_dimensions )
        {
            if( !use_conjunctive )
                rep += step_hrr_str;
            rep += dimensions[d] + connector;
        }

        // If this task is feature specific, then we have to include that feature 
        if( !use_conjunctive )
            rep += step_hrr_str;
        if( !dimension_task && d == task_d[step] )
            rep += features[d][task_f[step]] + connector;
        else
            rep += features[d][feature_picker(re)] + connector;

        state += rep;
        if( use_conjunctive )
            rep = step_hrr_str + rep;
        actions.push_back( rep.substr(0,rep.length()-1) );
    }
    
    if( use_conjunctive )
        state = step_hrr_str + state;

    return TrialStep(state.substr(0,state.length()-1),actions);
}

// Print task details
void WCST::printTask(int step)
{
    cout << "Current Task: " << dimensions[task_d[step]] << " ";
    if( !dimension_task )
        cout << features[task_d[step]][task_f[step]];
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
void WCST::setTasks()
{
    // We only need to change this if we're doing all tasks
    if( task_mode == 'A' )
        dimension_task = !dimension_task;

    for( int step = 0; step < steps_per_trial; step++ )
    {
        // If we're in dimension only task mode then make sure we get a new dimension
        int new_task_d = dimension_picker(re);
        if( dimension_task )
        {
            while( new_task_d == task_d[step] )
                new_task_d = dimension_picker(re);
        }
        // If we're in feature only task mode then make sure we get a new feature
        int new_task_f = feature_picker(re);
        if( !dimension_task )
        {
            while( new_task_f == task_f[step] )
                new_task_f = feature_picker(re);
        }

        task_d[step] = new_task_d;
        task_f[step] = new_task_f;
    }
}

// Return whether the answer is correct; update tallies and switch tasks if needed
// There is a class variable, current_trial_correct, that keeps track of whether any
// step in the current trial has failed
bool WCST::answer(string response,int step)
{
    // Initialize current n-step trial
    if( step == 0 )
        current_trial_correct = true;

    // Determine correctness of this step - this is always based on feature since we can't assume
    // that dimensions were encoded
    bool correct = false;
    char connector = use_conjunctive ? '*' : '+';
    if( dimension_task )
    {
        string token = response.substr(response.find(connector) + 1);
        for( string f : features[task_d[step]] )
        {
            if( response.find(f) != string::npos )
            {
                correct = true;
                break;
            }
        }
    }
    else
        correct = (response.find(features[task_d[step]][task_f[step]]) != string::npos);

    // Update trial results
    if( !correct )
        current_trial_correct = false;

    // Update trial tallies
    if( step + 1 == steps_per_trial )
    {
        total_trials_completed++;
        if( current_trial_correct )
        {
            total_correct_trials++;
            successive_correct_trials++;
        }
        else
            successive_correct_trials = 0;
    }

    // Switch tasks if needed
    if( successive_correct_trials == trials_per_task )
    {
        total_tasks_completed++;
        successive_correct_trials = 0;
        setTasks();
    }
    
    // Note that result is only valid on the nth step
    // Otherwise it is an intermediate result
    return current_trial_correct;
}
