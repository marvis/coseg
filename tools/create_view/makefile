CC = gcc
CXX = g++ -g -Wall

CXXFLAGS = -ltiff

OBJ_CREATE = src/myalgorithms.o src/component_tree.o  create_tree.o
OBJ_VIEW = src/myalgorithms.o src/component_tree.o  view_tree.o

all: $(OBJ_VIEW) $(OBJ_CREATE)
	$(CXX) $(CXXFLAGS) $(OBJ_CREATE) -o create_tree
	$(CXX) $(CXXFLAGS) $(OBJ_VIEW) -o view_tree

view_tree: $(OBJ_VIEW)
	$(CXX) $(CXXFLAGS) $(OBJ_VIEW) -o view_tree
create_tree: $(OBJ_CREATE)
	$(CXX) $(CXXFLAGS) $(OBJ_CREATE) -o create_tree

(%.o):(%.cpp)
	$(CXX) $(CXXFLAGS) -c $< -o $*.o

clean:
	rm *.o
	rm src/*.o
