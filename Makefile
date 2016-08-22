all: test.exe complex-test.exe temp.exe

test.exe : CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp wmtest.cpp
	g++ -g -std=c++11 CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp wmtest.cpp -lgsl -lblas -o test.exe

complex-test.exe : CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp complex-test.cpp
	g++ -g -std=c++11 CriticNetwork.cpp WorkingMemory.cpp hrr/hrrengine.cpp hrr/hrrOperators.cpp complex-test.cpp -lgsl -lblas -o complex-test.exe

temp.exe : temp.cpp
	g++ -g -std=c++11 temp.cpp -o temp.exe
