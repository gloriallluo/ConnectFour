all: main

main:
	g++ -O2 sdk/main.cpp sdk/json/jsoncpp.cpp Judge.cpp Strategy.cpp UcTree.cpp -o main

clean:
	rm main