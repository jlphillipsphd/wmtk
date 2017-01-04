all: 1stage

1stage: CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp td_hrr_1d_wm_1stage.cpp
	g++ -g -std=c++11 CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp td_hrr_1d_wm_1stage.cpp -lgsl -lblas -o 1stage

1stage_Wall: CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp td_hrr_1d_wm_1stage.cpp
	g++ -g -Wall -std=c++11 CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp td_hrr_1d_wm_1stage.cpp -lgsl -lblas -o 1stage

test : CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp test.cpp
	g++ -g -std=c++11 CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp test.cpp -lgsl -lblas -o test
