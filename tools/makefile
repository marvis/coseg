CC = gcc
CXX = g++ -g -Wall

CXXFLAGS = -ltiff

OBJ_CREATE = ../myalgorithms.o ../component_tree.o  create_tree.o
OBJ_VIEW = ../myalgorithms.o ../component_tree.o  view_tree.o
OBJ_TRACK = ../myalgorithms.o ../component_tree.o ../CT3D/cell_track.o coseg_two.o

all: $(OBJ_CREATE) $(OBJ_VIEW) $(OBJ_TRACK)
	$(CXX) $(CXXFLAGS) $(OBJ_CREATE) -o create_tree
	$(CXX) $(CXXFLAGS) $(OBJ_VIEW) -o view_tree
	$(CXX) $(CXXFLAGS) -llpsolve55 $(OBJ_TRACK) -o coseg_two

(%.o):(%.cpp)
	$(CXX) $(CXXFLAGS) -c $< -o $*.o

clean:
	rm *.o
	rm ../*.o
