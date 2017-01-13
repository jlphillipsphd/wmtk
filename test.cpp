#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include "hrr/hrrengine.h"
#include "hrr/hrrOperators.h"
#include "WorkingMemory.h"

int main(int argc, char *argv[])
{
    // Define WM parameters
    int hrr_size = 1024;
    double learn_rate = .1;
    double gamma = .9;
    double lambda = .3;
    double epsilon = .2;

    int seed = atoi(argv[1]);
    int num_states = 4;

    WorkingMemory wm( learn_rate, gamma, lambda, epsilon, hrr_size, 1, seed );

    for( int episode=1; episode<=100000; episode++ )
    {
        int pos = 1;

        //wm.initializeEpisode( to_string(pos), 0 );

        wm.initializeEpisode( to_string(pos) + "*TEST", 0 );

        for( pos=2; pos<=num_states; ++pos )
            wm.step( to_string(pos), 0 );

        //wm.absorbReward( 1 );

        
        vector<string> wmc = wm.queryWorkingMemory();
        if( find( wmc.begin(), wmc.end(), "TEST" ) != wmc.end() )
            wm.absorbReward( 1 );
        else
            wm.absorbReward( 0 );
        

        if( episode % 100 == 0 )
        {
            wm.printWMContents();
            cout << endl;

            vector<string> contents = vector<string>{ "TEST" };
            for( int i=1; i<=num_states; ++i )
            {
                vector<string> state = vector<string>{ to_string(i) };
                double val = wm.findValueOfStateContents( state, contents );
                cout << val << " ";
            }
            cout << endl;

            contents = vector<string>{ "I" };
            for( int i=1; i<=num_states; ++i )
            {
                vector<string> state = vector<string>{ to_string(i) };
                double val = wm.findValueOfStateContents( state, contents );
                cout << val << " ";
            }
            cout << endl;

            for( int i=1; i<=num_states; ++i )
            {
                contents = vector<string>{ to_string(i) };
                vector<string> state = vector<string>{ to_string(i) };
                double val = wm.findValueOfStateContents( state, contents );
                cout << val << " ";
            }
            cout << endl << endl;
        }
    }
}
