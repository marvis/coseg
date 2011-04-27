CC = gcc
CXX = g++ -g -Wall

CXXFLAGS = -ltiff

OBJ_COSEG_TWO = ../../myalgorithms.o ../../component_tree.o ../../CT3D/cell_track.o ../../CT3D/palette.o ../../CT3D/bipartite.o coseg_two.o
OBJ_COSEG_MORE = ../../myalgorithms.o ../../component_tree.o ../../CT3D/cell_track.o ../../CT3D/palette.o ../../CT3D/bipartite.o coseg_more.o

all: coseg_more coseg_two

coseg_more:$(OBJ_COSEG_MORE)
	$(CXX) $(CXXFLAGS) -llpsolve55 $(OBJ_COSEG_MORE) -o coseg_more
coseg_two: $(OBJ_COSEG_TWO)
	$(CXX) $(CXXFLAGS) -llpsolve55 $(OBJ_COSEG_TWO) -o coseg_two

(%.o):(%.cpp)
	$(CXX) $(CXXFLAGS) -c $< -o $*.o

clean:
	rm -f *.o
	rm -f ../../*.o
	rm -f ../../CT3D/*.o
