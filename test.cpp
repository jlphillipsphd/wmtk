#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include "hrr/hrrengine.h"
#include "hrr/hrrOperators.h"
#include "WorkingMemory.h"

int main(int argc, char *argv[])
{
    int seed = atoi(argv[1]);

    // The number of positions in the 1d world
    int num_states = 20;
    // The number of trials
    int num_episodes = 50000;
    // The steps alloted to reach goal within an episode
    int num_steps = 100;

    // Set up random signal, start position, and next move generation
    mt19937 re(seed);
    uniform_int_distribution<int> start_dist(0,num_states-1);

    // Set up the reward states
    double default_reward = 0;
    double goal_reward = 1;

    // Define WM parameters
    int hrr_size = 1024;
    double learn_rate = 1;
    double gamma = .5;
    double lambda = .0;
    double epsilon = .1;

    vector<string> moves{"r","l"};

    WorkingMemory wm( learn_rate, gamma, lambda, epsilon, hrr_size, 1, seed );

    for( int episode=1; episode<=100000; episode++ )
    {
        int position = start_dist(re);
        string a = wm.initializeEpisode( to_string(position) + "*TEST", moves, default_reward );

        for( int timestep = 1; timestep <= num_steps; ++timestep )
        {
            // debug
            //cout << "position: " << position << ", action: " << a << endl;

            if( position == 0 )
            {
                // Absorb the reward
                wm.absorbReward( goal_reward );
                break;
            }

            if( a == "l" )
                position = (position + num_states - 1) % num_states;
            else
                position = (position + num_states + 1) % num_states;

            // Step to the next position
            a = wm.step( to_string(position), moves, default_reward );

            // If this is the last step then absorb default reward
            if( timestep == num_steps )
                wm.absorbReward( default_reward );
        }

        if( episode % 100 == 0 )
        {
            wm.printWMContents();
            cout << endl;

/*
            vector<string> contents = vector<string>{ "TEST" };
            for( int i=1; i<=num_states; ++i )
            {
                vector<string> state = vector<string>{ to_string(i) };
                double val = wm.findValueOfStateContents( state, contents );
                cout << val << " ";
            }
            cout << endl;
*/
            vector<string> contents = vector<string>{ "I" };
            for( int i=0; i<num_states; ++i )
            {
                vector<string> state = vector<string>{ to_string(i) };
                double val = wm.findValueOfStateContents( state, contents );
                cout << val << " ";
            }
            cout << endl;
/*
            for( int i=1; i<=num_states; ++i )
            {
                contents = vector<string>{ to_string(i) };
                vector<string> state = vector<string>{ to_string(i) };
                double val = wm.findValueOfStateContents( state, contents );
                cout << val << " ";
            }
            cout << endl << endl;
*/
        }
    }
}
