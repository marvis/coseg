CC = gcc
CXX = g++ -g -Wall

CXXFLAGS = -ltiff

OBJ = ../myalgorithms.o ../component_tree.o  create_tree.o

CT3D: $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o create_tree

(%.o):(%.cpp)
	$(CXX) $(CXXFLAGS) -c $< -o $*.o

clean:
	rm *.o
