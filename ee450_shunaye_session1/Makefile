CPP = g++
CPPFLAGS  = -g -Wall -std=c++17
all:
	g++ -std=c++14 -o aws aws.cpp
	g++ -std=c++14 -o client client.cpp
	g++ -std=c++14 -o serverA serverA.cpp
	g++ -std=c++14 -o serverB serverB.cpp
clean:
	rm -f client aws serverA serverB
.PHONY: serverA serverB aws
serverA:
	./serverA
serverB:
	./serverB
aws: 
	./aws