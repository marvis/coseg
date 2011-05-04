CC = gcc
CXX = g++ -g -Wall

CXXFLAGS = -ltiff -llpsolve55

OBJ_COSEG = ../../myalgorithms.o ../../component_tree.o ../../CT3D/cell_track.o ../../CT3D/palette.o ../../CT3D/bipartite.o coseg.o

all: coseg

coseg:$(OBJ_COSEG)
	$(CXX) $(CXXFLAGS) $(OBJ_COSEG) -o coseg

(%.o):(%.cpp)
	$(CXX) $(CXXFLAGS) -c $< -o $*.o

clean:
	rm -f *.o
	rm -f ../../*.o
	rm -f ../../CT3D/*.o
