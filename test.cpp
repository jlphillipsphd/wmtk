#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include "hrr/hrrengine.h"
#include "hrr/hrrOperators.h"
#include "WorkingMemory.h"

void printTrace( vector<int> trace_pos, vector<string> trace_wm, vector<string> trace_a )
{
    string results_file = "test_results/trace";
    ofstream r;
    r.open(results_file,ofstream::app);

    r << "BEGIN EPISODE" << endl;
    for( int i=0; i<trace_pos.size(); ++i )
    {
        r << trace_pos[i] << " ";
        r << trace_wm[i] << " ";
        r << trace_a[i] << " ";
        r << endl;
    }
    r << "END EPISODE" << endl;

    r << endl;
    r.close();
}

void printEpisode(WorkingMemory &wm, int num_states, int episode)
{
    cout << "Episode: " << episode << endl;

    wm.printWMContents();
    cout << endl;

    vector<string> contents = vector<string>{ "A" };
    for( int i=0; i<num_states; ++i )
    {
        vector<string> state = vector<string>{ to_string(i) };
        double l = wm.findValueOfStateContentsAction( state, contents, "l" );
        double r = wm.findValueOfStateContentsAction( state, contents, "r" );
        cout << (l > r ? "l" : "r") << " ";
    }
    cout << endl << endl;

    contents = vector<string>{ "B" };
    for( int i=0; i<num_states; ++i )
    {
        vector<string> state = vector<string>{ to_string(i) };
        double l = wm.findValueOfStateContentsAction( state, contents, "l" );
        double r = wm.findValueOfStateContentsAction( state, contents, "r" );
        cout << (l > r ? "l" : "r") << " ";
    }
    cout << endl << endl;

    contents = vector<string>{ "I" };
    for( int i=0; i<num_states; ++i )
    {
        vector<string> state = vector<string>{ to_string(i) };
        double l = wm.findValueOfStateContentsAction( state, contents, "l" );
        double r = wm.findValueOfStateContentsAction( state, contents, "r" );
        cout << (l > r ? "l" : "r") << " ";
    }
    cout << endl << endl;
}

int main(int argc, char *argv[])
{
    int seed = atoi(argv[1]);

    // The number of positions in the 1d world
    int num_states = 20;
    // The number of trials
    int num_episodes = 50000;
    // The steps alloted to reach goal within an episode
    int num_steps = 100;

    // Initialize the signal concepts
    // This only works for two concepts right now
    vector<string> metas = vector<string>
    {
        "A",
        "B"
    };
    int nmetas = metas.size(); 

    // Set up random signal, start position, and next move generation
    mt19937 re(seed);
    uniform_int_distribution<int> start_dist(0,num_states-1);
    uniform_int_distribution<int> signal_dist(0,nmetas-1);

    // Set up the reward states
    double default_reward = 0;
    double goal_reward = 1;

    // For now I'm manually setting goals to be first and middle states
    vector<int> goal;
    goal.push_back(0);
    goal.push_back(num_states/2);

    // Define WM parameters
    int hrr_size = 1024;
    double learn_rate = .05;
    double gamma = .9;
    double lambda = .5;
    double epsilon = .01;

    // Set up the reward states
    vector<string> moves{"r","l"};

    WorkingMemory wm( learn_rate, gamma, lambda, epsilon, hrr_size, 1, 1, seed );

    for( int episode=1; episode<=100000; episode++ )
    {
        int signal = signal_dist(re);
        int position = start_dist(re);
        string a;

        wm.initializeEpisode();

        // DEBUG
        vector<int> trace_val;
        vector<string> trace_wm;
        vector<string> trace_a;

        for( int timestep = 1; timestep <= num_steps; ++timestep )
        {
            // Step into the next position
            if( timestep == 1 )
                a = wm.step( to_string(position) + "*" + metas[signal], moves, default_reward );
            else
                a = wm.step( to_string(position), moves, default_reward );
                
            trace_val.push_back(position);
            trace_wm.push_back(wm.workingMemoryChunks[0]);
            trace_a.push_back(a);

            // Absorb the reward if this is the goal
            if( position == goal[signal] )
            {
                // DEBUG
                printTrace(trace_val,trace_wm,trace_a);

                wm.absorbReward( goal_reward );
                break;
            }

            // If this is the last step then absorb default reward
            if( timestep == num_steps )
                wm.absorbReward( default_reward );

            if( a == "l" )
                position = (position + num_states - 1) % num_states;
            else
                position = (position + num_states + 1) % num_states;
        }

        if( episode % 100 == 0 )
        //if( episode % 1 == 0 )
            printEpisode(wm,num_states,episode);
    }
}
