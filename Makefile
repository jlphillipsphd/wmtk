all:
	g++ -g -std=c++11 CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp wmtest.cpp -o test.exe
	g++ -g -std=c++11 CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp compex-test.cpp -o complex-test.exe
	g++ -g -std=c++11 temp.cpp -o temp.exe
