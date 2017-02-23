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
    struct Trial
    {
        std::string state;
        std::vector<std::string> actions;
        Trial(std::string s, std::vector<std::string> a);
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
    bool task_switching = true;
    int task_d = 0; // index of task dimension
    int task_f = 0; // index of task feature
    int trials_to_completion = -1;

    std::vector<std::string> dimensions;
    std::vector<std::vector<std::string> > features;
    bool encode_dimensions = false;

    int dimensions_per_trial; // This will limit to the first n dimensions
    int features_per_trial; // This will limit to the first n features
    long total_tasks_completed = 0;
    long total_trials_completed = 0;
    long total_correct_trials = 0;
    long successive_correct_trials = 0;

    // Provide a file path to the stimuli file
    WCST
    ( 
        int random_seed, 
        std::string file_path,
        char task_mode,
        bool encode_dimensions,
        int dimensions_per_trial,
        int features_per_trial,
        int trials_to_completion // -1 to never switch tasks
    );

    // Make a trial based on the current task
    Trial getTrial();

    // Print task details
    void printTask();

    // Print general stats
    void printStats();

    // Change the current task
    void setTask();

    // Return whether the answer is correct; update tallies and switch tasks if needed
    bool answer(std::string response);
};

#endif
