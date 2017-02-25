all: 

wcst: WCST.cpp wcst_manual.cpp
	g++ -g -std=c++11 WCST.cpp wcst_manual.cpp -o wcst

wcst_wm: CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp WCST.cpp wcst_wm.cpp
	g++ -g -std=c++11 CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp WCST.cpp wcst_wm.cpp -lgsl -lblas -o wcst_wm

1d_q: CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp td_q_hrr_1d_wm.cpp
	g++ -g -std=c++11 CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp td_q_hrr_1d_wm.cpp -lgsl -lblas -o 1d_q

1d_noq: CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp td_noq_hrr_1d_wm.cpp
	g++ -g -std=c++11 CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp td_noq_hrr_1d_wm.cpp -lgsl -lblas -o 1d_noq

test : CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp test.cpp
	g++ -g -std=c++11 CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp test.cpp -lgsl -lblas -o test
