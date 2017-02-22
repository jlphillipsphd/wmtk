all: 

1d_q: CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp td_q_hrr_1d_wm.cpp
	g++ -g -std=c++11 CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp td_q_hrr_1d_wm.cpp -lgsl -lblas -o 1d_q

1d_noq: CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp td_noq_hrr_1d_wm.cpp
	g++ -g -std=c++11 CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp td_noq_hrr_1d_wm.cpp -lgsl -lblas -o 1d_noq

test : CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp test.cpp
	g++ -g -std=c++11 CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp test.cpp -lgsl -lblas -o test
