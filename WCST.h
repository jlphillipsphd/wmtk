/*
Author: Mike Jovanovich
Date: 2/1/2017
Description: This library is meant to povide a common interface for several tasks related to the
Wisconsin Card Sort Test.
*/

#ifndef WCST_
#define WCST_

#include <string>
#include <vector>
#include <random>

class WCST
{
public:
    struct TrialStep
    {
        std::string state;
        std::vector<std::string> actions;
        TrialStep();
        TrialStep(std::string s, std::vector<std::string> a);
        void print();
    };

    std::mt19937 re;
    std::uniform_int_distribution<int> dimension_picker;
    std::uniform_int_distribution<int> feature_picker;

    // Can be D, F, or A for dimension, feature, or all
    // In the case of all, the dimension task bit will be flipped each time the task changes,
    // so that it alternames between feature and dimension rules
    char task_mode = 'A';
    bool dimension_task = false;
    std::vector<int> task_d; // index of task dimension for each step 
    std::vector<int> task_f; // index of task feature for each step 
    int steps_per_trial = 1;
    int trials_per_task = -1;

    std::vector<std::string> dimensions;
    std::vector<std::vector<std::string> > features;
    bool encode_dimensions = false;
    bool use_conjunctive = false;  // currently not exposed
    bool suppress_task_switching = false;  // currently not exposed

    int dimensions_per_trial; // This will limit to the first n dimensions
    int features_per_trial; // This will limit to the first n features
    long total_tasks_completed = 0;
    long total_trials_completed = 0;
    long total_correct_trials = 0;
    long successive_correct_trials = 0;
    long current_task_trials = 0;
    bool current_trial_correct = false;

    // Provide a file path to the stimuli file
    WCST
    ( 
        int random_seed, 
        std::string file_path,
        char task_mode,
        bool encode_dimensions,
        int dimensions_per_trial,
        int features_per_trial,
        int steps_per_trial,
        int trials_per_task // -1 to never switch tasks
    );

    // Make a trial based on the current task
    TrialStep getTrialStep(int step = 0);

    // Print task details
    void printTask(int step = 0);

    // Print general stats
    void printStats();

    // Change the current task
    void setTasks();

    // Return whether the answer is correct; update tallies and switch tasks if needed
    bool answer(std::string response,int step = 0);
};

#endif
