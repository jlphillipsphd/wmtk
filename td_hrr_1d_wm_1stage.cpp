// 1D TD-learning with HRRs proof of concept
// Mike Jovanovich - 2016
// Ported from Joshua L. Phillips, 'td_hrr_1d_wm_1stage.r'

#include <stdlib.h>
#include <iostream>
#include <random>
#include <vector>
#include <string>
#include <math.h>
#include "WorkingMemory.h"
#include "hrr/hrrengine.h"

/* GENERAL NOTES / TODOs

I'm seeing weights in the NN implementation, but no bias vector
-JP: this works without a bias vector

Why is eligibility trace set as it is in WorkingMemory.cpp, line 295?
-JP: We set this to the initial state (hrr)

Setting learning rate to 1 will cause it to break
*/

using namespace std;


void printContents( WorkingMemory& wm )
{
    cout << "WM Contents:" << endl;
    for( string chunk : wm.queryWorkingMemory() )
        cout << chunk << " | ";
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
    double learn_rate = .25;
    double gamma = .9;
    double lambda = .3;
    double epsilon = .03;

    WorkingMemory wm( learn_rate, gamma, lambda, epsilon, hrr_size, 1, seed );

    for( int episode = 1; episode <= num_episodes; ++episode )
    {
        int signal = signal_dist(re);
        int position = start_dist(re);

        // This will possibly put the signal into WM, but possibly not
        wm.initializeEpisode( to_string(position) + "*" + metas[signal], default_reward );

        for( int timestep = 1; timestep <= num_steps; ++timestep )
        {
            if( position == goal[signal] )
            {
                // Absorb the reward
                wm.absorbReward( goal_reward );
                break;
            }

            // Move to adjascent position based on best value
            int move_left = (position + num_states - 1) % num_states;
            int move_right = (position + num_states + 1) % num_states;
            double val_left = wm.findValueOfStateWM( vector<string>{ to_string(move_left) } );
            double val_right = wm.findValueOfStateWM( vector<string>{ to_string(move_right) } );

            // Ties are broken by moving right
            if( val_left > val_right )
                position = move_left;
            else
                position = move_right;

            // Step to the next position
            wm.step( to_string(position), default_reward );

            // If this is the last step then absorb default reward
            if( timestep == num_steps )
                wm.absorbReward( default_reward );
        }
        if( episode % 100 == 0 )
        {
            printContents(wm);
        }
        if( episode % 500 == 0 )
        {
            // signal trace 1
            cout << "Episode: " << episode << endl;
            for( int j=0; j < metas.size(); ++j )
            {
                vector<string> contents = vector<string>{ metas[j] };
                for( int i = 0; i < num_states; ++i )
                {
                    vector<string> state = vector<string>{ to_string(i) };
                    double val = wm.findValueOfStateContents( state, contents );
                    cout << val << " ";
                }
                cout << endl;
            }
            vector<string> contents = vector<string>{ "I" };
            for( int i = 0; i < num_states; ++i )
            {
                vector<string> state = vector<string>{ to_string(i) };
                double val = wm.findValueOfStateContents( state, contents );
                cout << val << " ";
            }
            cout << endl << endl;
        }
    }
}
