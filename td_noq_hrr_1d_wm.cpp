// 1D TD-learning with HRRs proof of concept
// Mike Jovanovich - 2016
// Ported from Joshua L. Phillips, 'td_hrr_1d_wm_1stage.r'

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <string>
#include <math.h>
#include "WorkingMemory.h"
#include "hrr/hrrengine.h"

using namespace std;

string results_file = "test_results/trace";
string action_results_file = "test_results/actionResults";
string trace_file = "test_results/trace";

long correct_trials = 0;
long erroneous_moves = 0;

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

void printValues( WorkingMemory &wm, int episode, vector<string> &metas, int num_states )
{
    string results_file = "test_results/results";
    ofstream r;
    r.open(results_file);

    cout << "Episode: " << episode << endl;

    // Value of being in state i and having signal j in WM
    for( int j=0; j < metas.size(); ++j )
    {
        vector<string> contents = vector<string>{ metas[j] };
        for( int i = 0; i < num_states; ++i )
        {
            vector<string> state = vector<string>{ to_string(i) };
            double val = wm.findValueOfStateContents( state, contents );
            r << val << " ";
        }
        r << endl;
    }

    // Value of being in state i and having I in WM
    vector<string> contents = vector<string>{ "I" };
    for( int i = 0; i < num_states; ++i )
    {
        vector<string> state = vector<string>{ to_string(i) };
        double val = wm.findValueOfStateContents( state, contents );
        r << val << " ";
    }
    r << endl;

    r.close();
}

int main(int argc, char *argv[])
{
    // debug
    //if(remove(trace_file.c_str()) != 0)
    //    ;

    int seed = atoi(argv[1]);

    // The number of positions in the 1d world
    int num_states = 20;
    // The number of trials
    int num_episodes = 1000;
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

    string moves = "r+l";

    WorkingMemory wm( learn_rate, gamma, lambda, epsilon, hrr_size, 1, 1, seed );

    for( int episode = 1; episode <= num_episodes; ++episode )
    {
        int signal = signal_dist(re);
        int position = start_dist(re);
        string a;

        int required_steps = 0;
        if(signal==0)
        {
            if(position < 10)
                required_steps=position;
            else
                required_steps=19-position;
        }
        else
        {
            if(position <= 10)
                required_steps=10-position;
            else
                required_steps=position-10;
        }

        // debug 
/*
        vector<int> trace_val;
        vector<string> trace_wm;
        vector<string> trace_a;
*/

        wm.initializeEpisode();

        for( int timestep = 1; timestep <= num_steps; ++timestep )
        {
            // Step into the next position
            if( timestep == 1 )
                // This will possibly put the signal into WM, but possibly not
                //a = wm.step( to_string(position) + "*" + metas[signal], moves, default_reward );
                a = wm.step( to_string(position) + "*" + metas[signal], metas[signal], moves, default_reward );
            else
                //a = wm.step( to_string(position), moves, default_reward );
                a = wm.step( to_string(position), "I", moves, default_reward );
                
            // debug
/*
            trace_val.push_back(position);
            trace_wm.push_back(wm.workingMemoryChunks[0]);
            trace_a.push_back(a);
*/
            // Absorb the reward if this is the goal
            if( position == goal[signal] )
            {
                if( episode > num_episodes-100 )
		    erroneous_moves += (timestep-1-required_steps);

                // debug
                //printTrace(trace_val,trace_wm,trace_a);

                correct_trials++;
                wm.absorbReward( goal_reward );
                break;
            }

            // If this is the last step then absorb default reward
            if( timestep == num_steps )
                wm.absorbReward( default_reward );

/*
            // NEW WAY OF DOING IT
            if( a == "l" )
                position = (position + num_states - 1) % num_states;
            else
                position = (position + num_states + 1) % num_states;
*/
            // OLD WAY OF DOING IT
            double l = wm.findValueOfStateWM( vector<string>{to_string((position + num_states - 1)%num_states)});
            double r = wm.findValueOfStateWM( vector<string>{to_string((position + num_states + 1)%num_states)});

            if( l > r )
                position = (position + num_states - 1) % num_states;
            else
                position = (position + num_states + 1) % num_states;
        }
        if( episode % 100 == 0 )
        {
            // debug
            printValues( wm, episode, metas, num_states );

            wm.printWMContents();
            cout << endl;
        }
    }
    cout << correct_trials << "\t" << erroneous_moves << endl;
}
