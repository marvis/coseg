CC = gcc
CXX = g++ -g -Wall

CXXFLAGS = -ltiff

OBJ_CREATE = ../myalgorithms.o ../component_tree.o  create_tree.o
OBJ_VIEW = ../myalgorithms.o ../component_tree.o  view_tree.o
OBJ_COSEG_TWO = ../myalgorithms.o ../component_tree.o ../CT3D/cell_track.o ../CT3D/palette.o coseg_two.o
OBJ_COSEG_MORE = ../myalgorithms.o ../component_tree.o ../CT3D/cell_track.o ../CT3D/palette.o coseg_more.o

coseg_more:$(OBJ_COSEG_MORE)
	$(CXX) $(CXXFLAGS) -llpsolve55 $(OBJ_COSEG_MORE) -o coseg_more
coseg_two: $(OBJ_COSEG_TWO)
	$(CXX) $(CXXFLAGS) -llpsolve55 $(OBJ_COSEG_TWO) -o coseg_two
view_tree: $(OBJ_VIEW)
	$(CXX) $(CXXFLAGS) $(OBJ_VIEW) -o view_tree
create_tree: $(OBJ_CREATE)
	$(CXX) $(CXXFLAGS) $(OBJ_CREATE) -o create_tree

(%.o):(%.cpp)
	$(CXX) $(CXXFLAGS) -c $< -o $*.o

clean:
	rm *.o
	rm ../*.o
	rm ../CT3D/*.o
