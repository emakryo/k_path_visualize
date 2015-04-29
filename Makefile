CXX = clang++
CXXFLAGS = -O2 -Wall -g

all: a.out
	./a.out

a.out: pruning.cpp
	$(CXX) $(CXXFLAGS) $<
