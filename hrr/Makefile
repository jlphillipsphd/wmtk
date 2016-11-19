all:	main operators

main:	main.cpp hrrengine.cpp hrrengine.h
	g++ -std=c++11 hrrengine.cpp main.cpp -lgsl -lblas -o main.exe

operators:	hrrengine.cpp hrrOperators.cpp operators.cpp
	g++ -std=c++11 hrrengine.cpp hrrOperators.cpp operators.cpp -lgsl -lblas -o op.exe
