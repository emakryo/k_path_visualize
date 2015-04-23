CXX = clang++
CXXFLAGS = -O2 -Wall

all: a.out
	./a.out < input.txt

a.out: pruning.cpp
	$(CXX) $(CXXFLAGS) $<
