CC = gcc
CXX = g++ -g -Wall

CXXFLAGS = -ltiff

OBJ_CREATE = ../../myalgorithms.o ../../component_tree.o  create_tree.o
OBJ_VIEW = ../../myalgorithms.o ../../component_tree.o  view_tree.o

all: view_tree create_tree

view_tree: $(OBJ_VIEW)
	$(CXX) $(CXXFLAGS) $(OBJ_VIEW) -o view_tree
create_tree: $(OBJ_CREATE)
	$(CXX) $(CXXFLAGS) $(OBJ_CREATE) -o create_tree

(%.o):(%.cpp)
	$(CXX) $(CXXFLAGS) -c $< -o $*.o

clean:
	rm -f *.o
	rm -f ../../*.o
