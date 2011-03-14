CC = gcc
CXX = g++ -g -Wall

CXXFLAGS = -ltiff

OBJ_COSEG_TWO = src/myalgorithms.o src/component_tree.o src/CT3D/cell_track.o src/CT3D/palette.o coseg_two.o
OBJ_COSEG_MORE = src/myalgorithms.o src/component_tree.o src/CT3D/cell_track.o src/CT3D/palette.o coseg_more.o

all: $(OBJ_COSEG_MORE) $(OBJ_COSEG_TWO)
	$(CXX) $(CXXFLAGS) -llpsolve55 $(OBJ_COSEG_MORE) -o coseg_more
	$(CXX) $(CXXFLAGS) -llpsolve55 $(OBJ_COSEG_TWO) -o coseg_two

coseg_more:$(OBJ_COSEG_MORE)
	$(CXX) $(CXXFLAGS) -llpsolve55 $(OBJ_COSEG_MORE) -o coseg_more
coseg_two: $(OBJ_COSEG_TWO)
	$(CXX) $(CXXFLAGS) -llpsolve55 $(OBJ_COSEG_TWO) -o coseg_two

(%.o):(%.cpp)
	$(CXX) $(CXXFLAGS) -c $< -o $*.o

clean:
	rm *.o
	rm src/*.o
	rm src/CT3D/*.o
