all:
	g++ -g -std=c++11 CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp wmtest.cpp -o test.exe
