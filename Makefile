CXX = clang++
CXXFLAGS = -O2 -Wall

all: a.out
	./a.out

a.out: clip.cpp
	$(CXX) $(CXXFLAGS) $<
